# Wifi_Sensing_Motion
Motion detection with wifi sensing
<br></br>
(This is just for my own college project, so who cares)
<br></br>

## KOR
### 사용된 기기
- ESP32-S3-N16R8
<br></br>
- Raspberry Pi 3 Model B
<br></br>

### 작동방식
라즈베리파이에 내장된 와이파이칩을 통해 핫스팟을 틀고, ESP32로 해당 와이파이에 접속해 CSI값을 읽어 그 값을 다시 라즈베리파이에 보낸 뒤에
<br></br>
라즈베리파이에서 받은 CSI값을 가공해 평균 분산 값을 지정하고 그 값을 크게 웃돌 경우 움직임이 있다고 판단하는 방식입니다
<br></br>

## ENG
### Used Devices
- ESP32-S3-N16R8
<br></br>
- Raspberry Pi 3 Model B
<br></br>

### How it works
This system works by setting up a WiFi hotspot using the Raspberry Pi's built in wireless chip.
<br></br>
The ESP32 connects to this hotspot and captures CSI data, which is then sent back to the Raspberry Pi.
<br></br>
The Raspberry Pi processes the incoming CSI data to calculate its mean and variance.
<br></br>
If these values exceed a predefined threshold, the system determines that motion has been detected.
<br></br>

## Screenshots
### Raspberry Pi
<img width="4000" height="3000" alt="KakaoTalk_20260624_215948739_01" src="https://github.com/user-attachments/assets/b7106b08-9b41-4fa9-85cc-f02a16feea9d" />
<br></br>
<img width="4000" height="3000" alt="KakaoTalk_20260624_215948739" src="https://github.com/user-attachments/assets/73ce494c-ba71-4942-affc-2dec4bf0b9a5" />
<br></br>
<img width="4000" height="3000" alt="KakaoTalk_20260624_215948739_02" src="https://github.com/user-attachments/assets/2808e8d4-86f0-47ed-b621-0d4450bdf60e" />
<br></br>

### ESP32-S3
<img width="4000" height="3000" alt="KakaoTalk_20260624_215948739_13" src="https://github.com/user-attachments/assets/74432031-c85f-4149-949f-b1da1223c722" />
<br></br>
<img width="4000" height="3000" alt="KakaoTalk_20260624_215948739_14" src="https://github.com/user-attachments/assets/698caafa-f524-4dbf-9931-ebd8ee582507" />
<br></br>

### Code
<img width="4000" height="3000" alt="KakaoTalk_20260624_215948739_03" src="https://github.com/user-attachments/assets/3ab31859-68a3-453e-9ed8-9e92693acc60" />
<br></br>
<img width="4000" height="3000" alt="KakaoTalk_20260624_215948739_04" src="https://github.com/user-attachments/assets/a05a6d19-1b3e-4141-812d-d87c1d7dc07d" />
<br></br>
<img width="4000" height="3000" alt="KakaoTalk_20260624_215948739_05" src="https://github.com/user-attachments/assets/d3d6e037-f078-4f79-8aa5-e4ace30dbab1" />
<br></br>

### Others
<img width="4000" height="3000" alt="KakaoTalk_20260624_215948739_12" src="https://github.com/user-attachments/assets/4fa666ef-8967-4cc3-9ae7-076ae54392ad" />
<br></br>
<img width="4000" height="3000" alt="KakaoTalk_20260624_215948739_07" src="https://github.com/user-attachments/assets/bd1c07ed-4856-4e95-9fae-e192a1ae6fd1" />
<br></br>
<img width="4000" height="3000" alt="KakaoTalk_20260624_215948739_09" src="https://github.com/user-attachments/assets/36afee65-1a31-4f6f-97bc-9b3e92f42909" />
<br></br>
<img width="4000" height="3000" alt="KakaoTalk_20260624_215948739_08" src="https://github.com/user-attachments/assets/d49c0782-0cf4-4463-8a66-f02c72e91ab9" />
<br></br>
<img width="4000" height="3000" alt="KakaoTalk_20260624_215948739_11" src="https://github.com/user-attachments/assets/c3a16299-3349-4886-a2f9-51306c35d419" />
<br></br>
<img width="4000" height="3000" alt="KakaoTalk_20260624_215948739_10" src="https://github.com/user-attachments/assets/95e1bfa3-97c8-429c-80de-46f1a2a72b24" />
<br></br>
