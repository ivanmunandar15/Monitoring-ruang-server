#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoJson.h>
#include <WebSocketsClient.h>
#include <SocketIOclient.h>
#include <Hash.h>

// Library untuk sensor DHT11
#include <DHT.h>

// Konfigurasi DHT11
#define DHTPIN 13     // Pin data sensor DHT11 (sesuaikan dengan pin yang Anda gunakan)
#define DHTTYPE DHT11 // Tipe sensor DHT (DHT11)

// Inisialisasi sensor DHT
DHT dht(DHTPIN, DHTTYPE);

// Konfigurasi WiFi dan Socket.IO
const char* ssid = "Delvi Wifi 2.4Ghz";
const char* password = "Delvi3107";
const char* socketServer = "192.168.1.9";
const int socketPort = 3000;

ESP8266WiFiMulti WiFiMulti;
SocketIOclient socketIO;

#define USE_SERIAL Serial

// Variabel untuk menyimpan data sensor
float temperature = 0;
float humidity = 0;

void socketIOEvent(socketIOmessageType_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case sIOtype_DISCONNECT:
            USE_SERIAL.printf("[IOc] Disconnected!\n");
            break;
        case sIOtype_CONNECT:
            USE_SERIAL.printf("[IOc] Connected to url: %s\n", payload);

            // join default namespace (no auto join in Socket.IO V3)
            socketIO.send(sIOtype_CONNECT, "/");
            break;
        case sIOtype_EVENT:
            USE_SERIAL.printf("[IOc] get event: %s\n", payload);
            break;
        case sIOtype_ACK:
            USE_SERIAL.printf("[IOc] get ack: %u\n", length);
            hexdump(payload, length);
            break;
        case sIOtype_ERROR:
            USE_SERIAL.printf("[IOc] get error: %u\n", length);
            hexdump(payload, length);
            break;
        case sIOtype_BINARY_EVENT:
            USE_SERIAL.printf("[IOc] get binary: %u\n", length);
            hexdump(payload, length);
            break;
        case sIOtype_BINARY_ACK:
            USE_SERIAL.printf("[IOc] get binary ack: %u\n", length);
            hexdump(payload, length);
            break;
    }
}

void setup() {
    USE_SERIAL.begin(115200);
    USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    // Inisialisasi sensor DHT11
    dht.begin();
    USE_SERIAL.println("DHT11 sensor initialized");

    // Countdown sebelum memulai
    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    // Nonaktifkan mode AP jika aktif
    if(WiFi.getMode() & WIFI_AP) {
        WiFi.softAPdisconnect(true);
    }

    // Koneksi ke WiFi dengan kredensial yang diberikan
    WiFiMulti.addAP(ssid, password);

    while(WiFiMulti.run() != WL_CONNECTED) {
        delay(100);
    }

    String ip = WiFi.localIP().toString();
    USE_SERIAL.printf("[SETUP] WiFi Connected %s\n", ip.c_str());

    // Konfigurasi Socket.IO server
    socketIO.begin(socketServer, socketPort, "/socket.io/?EIO=4");
    
    // Set callback untuk event Socket.IO
    socketIO.onEvent(socketIOEvent);
}

unsigned long sensorTimestamp = 0;    // create_at untuk membaca sensor
unsigned long messageTimestamp = 0;   // create_at untuk mengirim pesan

void loop() {
    socketIO.loop();

    uint64_t now = millis();

    // Baca sensor DHT11 setiap 2 detik
    if(now - sensorTimestamp > 2000) {
        sensorTimestamp = now;

        // Baca nilai suhu dan kelembaban
        humidity = dht.readHumidity();
        temperature = dht.readTemperature();

        // Cek apakah pembacaan berhasil
        if (isnan(humidity) || isnan(temperature)) {
            USE_SERIAL.println("Gagal membaca data dari sensor DHT11!");
            return;
        }

        USE_SERIAL.printf("Suhu: %.1f°C, Kelembaban: %.1f%%\n", temperature, humidity);
    }

    // Kirim data ke server setiap 5 detik
    if(now - messageTimestamp > 5000) {
        messageTimestamp = now;

        // Buat JSON untuk data sensor
        DynamicJsonDocument doc(1024);
        JsonArray array = doc.to<JsonArray>();
        
        // Nama event: "dht11_data"
        array.add("dht11_data");
        
        // Parameter event (data sensor)
        JsonObject sensorData = array.createNestedObject();
        sensorData["temperature"] = temperature;
        sensorData["humidity"] = humidity;
        sensorData["create_at"] = (uint32_t) now;
        sensorData["device_id"] = WiFi.macAddress(); // Gunakan MAC address sebagai ID perangkat
        
        // JSON ke String
        String output;
        serializeJson(doc, output);
        
        // Kirim event
        socketIO.sendEVENT(output);
        
        // Debug
        USE_SERIAL.println(output);
    }
}