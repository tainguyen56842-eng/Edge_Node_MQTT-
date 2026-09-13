-ta sử dụng linux để giao tiếp với board 
sudo apt update
sudo apt install mosquitto-clients
-ESP32 gửi nhiệt độ lên linux
mosquitto_sub -h broker.hivemq.com -t "esp32/sensor/temperature"
-điều khiển đèn nội bộ (gpio2) trên esp32
mosquitto_pub -h broker.hivemq.com -t "esp32/relay/control" -m "ON"
mosquitto_pub -h broker.hivemq.com -t "esp32/relay/control" -m "OFF"
