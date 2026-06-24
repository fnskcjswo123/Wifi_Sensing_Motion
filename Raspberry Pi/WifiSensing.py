import serial
import time
import sys
import logging
import os
from datetime import datetime
import numpy as np
import matplotlib
matplotlib.use('TkAgg')
import matplotlib.pyplot as plt

#logging settings
os.makedirs('/home/pi/WifiSensing/logs', exist_ok = True)
today = datetime.now().strftime('%Y-%m-%d')
filename = f'/home/pi/WifiSensing/logs/csi_{today}.txt'
logging.basicConfig(
    level = logging.INFO,
    format = '%(asctime)s %(message)s',
    handlers = [
        logging.FileHandler(filename, mode = 'a'),
        logging.StreamHandler()
    ]
)

#csi value reading settings
ser = serial.Serial('/dev/ttyACM0', 115200, timeout = 0.1)
logging.info('Raspberrypi is waiting for Esp32s3 ready signal...')
while True:
    line = ser.readline().decode('utf-8', errors = 'ignore').strip()
    if line == 'Esp32s3_Ready':
        ser.write("Raspberrypi_Ready\n".encode('utf-8'))
        logging.info('Raspberrypi and Esp32s3 connection confirmed')
        logging.info('Start receiving CSI data')
        break

#values settings
last_graphupd = time.time()
graphupd_time = 0.5

last_logupd = time.time()
logupd_time = 0.5

last_bigmovupd = time.time()
bigmovupd_time = 0.5

csi_matrix = []
CSI_COLS = 128
temp = 500
table_limit = 100
variance_history = []
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8))
plt.ion()
plt.show()

while True:
    line = ser.readline().decode('utf-8', errors = 'ignore').strip()
    if line == 'Esp32s3_Ready':
        logging.info('Raspberrypi is already connected with Esp32s3!')
        continue
    elif line:
        if time.time() - last_logupd > logupd_time:
            logging.info('Data received from Esp32s3')
            #so laggy
            #logging.info(f'{line}')
            last_logupd = time.time()
        try:
            start = line.index('[') + 1
            end = line.index(']')
            values = list(map(int, line[start:end].split()))
            if values and len(values) >= CSI_COLS:
                csi_matrix.append(values[:CSI_COLS])
                if len(csi_matrix) > table_limit:
                    csi_matrix.pop(0)

            variance = np.var(values)
            variance_history.append(variance)
            if len(variance_history) > table_limit:
                variance_history.pop(0)

            if variance > temp:
                if time.time() - last_bigmovupd > bigmovupd_time:
                    logging.info('Big movement detected!')
                    logging.info(f'Variance: {variance:.1f}')
                    last_bigmovupd = time.time()

            if time.time() - last_graphupd > graphupd_time and len(csi_matrix) > 0:

                #heatmap update
                ax1.clear()
                ax1.imshow(
                    np.array(csi_matrix).T,
                    aspect='auto',
                    cmap='jet',
                    origin='lower'
                )
                ax1.set_xlabel('Time')
                ax1.set_ylabel('Subcarrier')
                ax1.set_title('CSI Heatmap')

                #variance update
                ax2.clear()
                ax2.plot(variance_history, color='b', label='variance')
                ax2.axhline(y=temp, color='r', linestyle='--', label=f'threshold: {temp}')
                ax2.set_title(f'Variance: {variance:.1f}')
                ax2.set_ylabel('Variance')
                ax2.legend()

                plt.tight_layout()
                plt.draw()
                plt.pause(0.01)
                last_graphupd = time.time()

        except (ValueError, IndexError) as e:
            logging.info(f'Error occured: {e}')
            continue
