/*
이 파일이 어떤 일을 하나요(요약):
esp_wifi_set_csi(1) → esp_wifi_set_csi(true) 변경
wifi_csi_config_t 초기화 방식 변경 (= {} 로 전체 초기화)
stringstream 기반 → snprintf 기반으로 출력 로직 교체
Queue + 별도 태스크 구조로 콜백 경량화
#endif 누락 수정

What does this file do(Summary):
Updated esp_wifi_set_csi API usage: Changed from 1 to true for better readability.
Improved wifi_csi_config_t initialization: Switched to uniform initialization (= {}) to ensure all fields are zero-initialized.
Optimized output logic: Replaced stringstream with snprintf to reduce memory overhead and improve performance.
Refactored callback structure: Moved data processing to a dedicated FreeRTOS task using a Queue to keep the callback execution lightweight.
Fixed missing #endif: Added the missing preprocessor directive to correct compilation errors.
*/

#ifndef ESP32_CSI_CSI_COMPONENT_H
#define ESP32_CSI_CSI_COMPONENT_H

#include "time_component.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_wifi.h"

char *project_type = NULL;

#define CSI_RAW 1
#define CSI_AMPLITUDE 0
#define CSI_PHASE 0

#define MAX_CSI_LEN 256

typedef struct {
    wifi_csi_info_t info;
    int8_t buf[MAX_CSI_LEN];
    long long real_time;
    long long steady_time;
} csi_data_t;

QueueHandle_t csi_queue;

void _wifi_csi_cb(void *ctx, wifi_csi_info_t *data) {
    if (data == NULL || data->buf == NULL || data->len == 0) return;

    csi_data_t packet;
    packet.info = *data;
    packet.real_time = real_time_set;
    packet.steady_time = get_steady_clock_timestamp();

    int copy_len = data->len > MAX_CSI_LEN ? MAX_CSI_LEN : data->len;
    memcpy(packet.buf, data->buf, copy_len);
    packet.info.len = copy_len;

    xQueueSend(csi_queue, &packet, 0);
}

void csi_process_task(void *pvParameters) {
    csi_data_t d;
    char print_buf[2048];

    while (1) {
        if (xQueueReceive(csi_queue, &d, portMAX_DELAY)) {
            int offset = 0;

            offset += snprintf(print_buf + offset, sizeof(print_buf) - offset,
                "CSI_DATA,%s,%02X:%02X:%02X:%02X:%02X:%02X,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%u,%d,%d,%d,%lld,%lld,%d,[",
                project_type,
                d.info.mac[0], d.info.mac[1],
                d.info.mac[2], d.info.mac[3],
                d.info.mac[4], d.info.mac[5],
                (int)d.info.rx_ctrl.rssi,
                (int)d.info.rx_ctrl.rate,
                (int)d.info.rx_ctrl.sig_mode,
                (int)d.info.rx_ctrl.mcs,
                (int)d.info.rx_ctrl.cwb,
                (int)d.info.rx_ctrl.smoothing,
                (int)d.info.rx_ctrl.not_sounding,
                (int)d.info.rx_ctrl.aggregation,
                (int)d.info.rx_ctrl.stbc,
                (int)d.info.rx_ctrl.fec_coding,
                (int)d.info.rx_ctrl.sgi,
                (int)d.info.rx_ctrl.noise_floor,
                (int)d.info.rx_ctrl.ampdu_cnt,
                (int)d.info.rx_ctrl.channel,
                (int)d.info.rx_ctrl.secondary_channel,
                (unsigned int)d.info.rx_ctrl.timestamp,
                (int)d.info.rx_ctrl.ant,
                (int)d.info.rx_ctrl.sig_len,
                (int)d.info.rx_ctrl.rx_state,
                (long long)d.real_time,
                (long long)d.steady_time,
                (int)d.info.len
            );

            int data_len = d.info.len;

#if CSI_RAW
            for (int i = 0; i < data_len; i++) {
                offset += snprintf(print_buf + offset, sizeof(print_buf) - offset, "%d ", d.buf[i]);
            }
#endif
#if CSI_AMPLITUDE
            for (int i = 0; i < data_len / 2; i++) {
                int amp = (int)sqrt(pow(d.buf[i * 2], 2) + pow(d.buf[(i * 2) + 1], 2));
                offset += snprintf(print_buf + offset, sizeof(print_buf) - offset, "%d ", amp);
            }
#endif
#if CSI_PHASE
            for (int i = 0; i < data_len / 2; i++) {
                int phase = (int)atan2((double)d.buf[i*2], (double)d.buf[(i*2)+1]);
                offset += snprintf(print_buf + offset, sizeof(print_buf) - offset, "%d ", phase);
            }
#endif
            snprintf(print_buf + offset, sizeof(print_buf) - offset, "]\n");
            printf("%s", print_buf);
        }
    }
}

void _print_csi_csv_header() {
    printf("type,role,mac,rssi,rate,sig_mode,mcs,bandwidth,smoothing,not_sounding,aggregation,stbc,fec_coding,sgi,noise_floor,ampdu_cnt,channel,secondary_channel,local_timestamp,ant,sig_len,rx_state,real_time_set,real_timestamp,len,CSI_DATA\n");
}

void csi_init(char *type) {
    project_type = type;

    csi_queue = xQueueCreate(10, sizeof(csi_data_t));
    xTaskCreate(csi_process_task, "csi_process", 4096, NULL, 5, NULL);

#ifdef CONFIG_SHOULD_COLLECT_CSI
    ESP_ERROR_CHECK(esp_wifi_set_csi(true));

    wifi_csi_config_t configuration_csi = {};

    configuration_csi.lltf_en = 1;
    configuration_csi.htltf_en = 1;
    configuration_csi.stbc_htltf2_en = 1;
    configuration_csi.ltf_merge_en = 1;
    configuration_csi.channel_filter_en = 0;
    configuration_csi.manu_scale = 0;

    ESP_ERROR_CHECK(esp_wifi_set_csi_config(&configuration_csi));
    ESP_ERROR_CHECK(esp_wifi_set_csi_rx_cb(&_wifi_csi_cb, NULL));
    _print_csi_csv_header();
#endif
}

#endif //ESP32_CSI_CSI_COMPONENT_H
