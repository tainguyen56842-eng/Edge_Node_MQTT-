#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// Cấu hình WiFi
const char* ssid = "CHAO'S HOUSE 2";
const char* password = "hochanhtantoi1";
const char* mqtt_server = "broker.hivemq.com"; 
#define DHTPIN 15
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define RELAY_PIN 2 // Sử dụng LED onboard (thường là chân GPIO2 trên ESP32)

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;

// Hàm nhận lệnh từ MQTT (để điều khiển Relay/LED)
void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(message);

  // Kiểm tra topic điều khiển relay
  if (String(topic) == "esp32/relay/control") {
    if (message == "ON") {
      digitalWrite(RELAY_PIN, HIGH);
      Serial.println("Relay/LED ON");
    } else if (message == "OFF") {
      digitalWrite(RELAY_PIN, LOW);
      Serial.println("Relay/LED OFF");
    }
  }
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Tạo ID client ngẫu nhiên
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    
    if(client.connect(clientId.c_str())){ 
      Serial.println("connected");
      // Đăng ký nhận lệnh (Subscribe topic điều khiển)
      client.subscribe("esp32/relay/control");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  
  dht.begin();
  setup_wifi();
  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  // Gửi dữ liệu nhiệt độ lên MQTT mỗi 5 giây
  if (now - lastMsg > 5000) {
    lastMsg = now;
    
    float t = dht.readTemperature();
    if (isnan(t)) t = 25.0; // Giá trị dự phòng nếu lỗi cảm biến

    // Publish nhiệt độ lên broker
    String tempStr = String(t, 1);
    client.publish("esp32/sensor/temperature", tempStr.c_str());
    
    Serial.print("Published temperature: ");
    Serial.println(tempStr);
  }
}