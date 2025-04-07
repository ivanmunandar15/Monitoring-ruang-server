# 🛰️ IoT Server Room Monitoring System

Sistem monitoring suhu dan kelembaban ruang server berbasis IoT menggunakan **ESP8266**, **sensor DHT11**, dan **WebSocket**, dengan backend **Supabase** sebagai penyimpanan data dan **Next.js** untuk tampilan antarmuka.

## 🚀 Fitur

- Monitoring suhu dan kelembaban secara **real-time**
- Komunikasi data antara ESP8266 dan website menggunakan **WebSocket**
- Penyimpanan data historis menggunakan **Supabase (PostgreSQL)**
- Tampilan dashboard berbasis web menggunakan **Next.js**
- Mudah dikembangkan dan di-deploy

## 🛠️ Teknologi yang Digunakan

- **ESP8266**: Mikrokontroler untuk mengirim data sensor
- **DHT11**: Sensor suhu dan kelembaban
- **WebSocket**: Protokol komunikasi real-time antara ESP8266 dan website
- **Supabase**: Backend-as-a-Service untuk database dan autentikasi
- **Next.js**: Framework React untuk membangun antarmuka web


## 🔌 Instalasi & Penggunaan

### 1. Persiapan ESP8266

- Upload program ke ESP8266 menggunakan Arduino IDE atau PlatformIO.
- Pastikan sudah mengatur:
  - SSID & password WiFi
  - PIN Data DHT 11
  - Alamat server WebSocket (IP lokal atau domain Next.js app)

### 2. Jalankan Web App

```bash
# Clone repositori ini
git clone https://github.com/ivanmunandar15/web-server-monitoring.git

# Masuk ke folder project
cd web-monitoring

# Install dependency
npm install

# Jalankan server Next.js
npm run dev
