# Running Text P10 ESP32 + RTC + Auto Sync NTP Server

**Dokumentasi Teknis Lengkap (DED) – Embedded System Engineer**

---

## 1. Tentang Sistem

<p align="justify">Sistem ini adalah **Running Text LED P10 berbasis ESP32** yang dirancang sebagai papan informasi digital cerdas. Keunggulan utamanya adalah ketahanan terhadap gangguan internet: display tetap menampilkan teks berjalan dan waktu dari **RTC (Real-Time Clock)** meskipun jaringan Wi-Fi tidak tersedia. Ketika koneksi internet kembali, sistem akan **secara otomatis menyinkronkan waktu dari server NTP (Network Time Protocol)** dan menyimpannya ke modul RTC, memastikan akurasi waktu yang tinggi tanpa campur tangan pengguna.

Sistem dibangun dengan pendekatan **non-blocking** pada seluruh operasi (Wi-Fi, NTP, display), sehingga running text tetap berjalan lancar tanpa jeda yang mengganggu. Arsitektur modular memisahkan logika display, RTC, sinkronisasi NTP, dan buzzer ke dalam file terpisah, memudahkan pengembangan dan pemeliharaan.</p>

**Target Aplikasi:**

- Papan informasi kampus, sekolah, atau institusi.
- Display jam digital di ruang publik (lobi, koridor).
- Papan pengumuman di bengkel, laboratorium, atau area produksi.
- Sistem signage mandiri dengan ketergantungan internet minimal.

---

## 2. Identitas Programmer

| **Atribut**         | **Detail**                                                                   |
|---------------------|------------------------------------------------------------------------------|
| **Nama**            | Muhammad Lutfi Nur Anendi                                                    |
| **GitHub**          | [lutfi-rpi5](https://github.com/lutfi-rpi5)                                  |
| **Board Target**    | ESP32 WROOM-32 (ESP32 Dev Module v1.0.6)                                     |
| **Tanggal Rilis**   | 3 April 2026                                                                 |
| **Versi Firmware**  | 1.0.0 (Stable)                                                               |
| **Lisensi**         | MIT                                                                          |

---

## 3. Fitur Sistem

| **Fitur**                           | **Deskripsi**                                                                                                  |
|-------------------------------------|----------------------------------------------------------------------------------------------------------------|
| **Running Text LED P10**            | Mendukung panel P10 *single-color* dengan konfigurasi dimensi yang dapat diatur (`DISPLAYS_ACROSS`, `DISPLAYS_DOWN`). |
| **RTC Offline Mode**                | Waktu tetap akurat menggunakan modul RTC DS1302/DS3231 saat tidak ada internet.                                |
| **Auto Sync NTP Server**            | Sinkronisasi waktu otomatis setiap interval tertentu (default 60 menit) ketika Wi-Fi tersedia.                 |
| **Non-blocking Architecture**       | Semua operasi (Wi-Fi, NTP, display) berjalan tanpa memblokir *main loop*, menjamin running text tetap halus.   |
| **Fallback Otomatis ke RTC**        | Jika NTP gagal atau internet putus, sistem langsung beralih ke RTC tanpa mengganggu tampilan.                  |
| **Scene Management**                | Mendukung dua *scene* utama: **Scene 1 (Running Text)** dan **Scene 2 (Display RTC)** dengan transisi otomatis.|
| **Durasi Tampil RTC Dapat Dikonfigurasi** | Waktu tampil scene RTC dapat diatur dalam milidetik (default 3 menit).                                         |
| **Buzzer Feedback**                 | Indikator suara untuk *boot*, sinkronisasi berhasil, dan *error*.                                              |
| **Konfigurasi Mudah via Kode**      | Semua parameter (SSID, password, zona waktu, kecepatan scroll, dll.) terpusat di file `.ino` dan `.cpp`.       |
| **Modular Code**                    | Pemisahan modul `RunningText`, `RTCAutoSync`, `Buzzer`, dan `virtuabotixRTC` untuk kemudahan *debugging* dan pengembangan. |

---

## 4. Arsitektur Sistem

Diagram blok berikut menggambarkan aliran data dan kontrol utama:

```
+----------------+      +----------------+      +----------------+
|   Internet     |      |   ESP32        |      |   LED Panel    |
|   (NTP Server) |<---->|   (Wi-Fi)      |<---->|   P10          |
+----------------+      +----------------+      +----------------+
                              ^   ^
                              |   |
                              v   v
                         +----------------+
                         |   RTC Module   |
                         |   (DS1302)     |
                         +----------------+
                              ^
                              |
                              v
                         +----------------+
                         |   Buzzer       |
                         |   (Feedback)   |
                         +----------------+
```

**Alur Kerja Sistem:**

1. **ESP32 Boot** – Inisialisasi Serial, Buzzer, RTC, dan Display.
2. **Coba Koneksi Wi-Fi** – Dilakukan secara *non-blocking* setelah *boot delay* (default 0 detik).
3. **Jika Berhasil → Sync NTP** – Waktu dari `pool.ntp.org` diambil, dikonversi ke zona waktu WIB (GMT+7), dan disimpan ke RTC.
4. **Jika Gagal → Gunakan RTC** – Sistem tetap beroperasi dengan waktu yang tersimpan di RTC.
5. **Running Text Berjalan** – Scene teks utama ditampilkan (berulang 1x), lalu beralih ke scene RTC selama durasi tertentu, dan kembali ke scene teks.
6. **Sinkronisasi Berkala** – Setiap interval `ntp_sync_interval` (default 60 menit), sistem kembali mencoba sinkronisasi NTP.

**Penanganan Kegagalan:**

- **Wi-Fi Timeout** – Setelah 60 detik tanpa koneksi, Wi-Fi dimatikan untuk menghemat daya.
- **NTP Failed** – RTC tetap digunakan, dan sinkronisasi dicoba lagi pada interval berikutnya.

---

## 5. Hardware Requirements

| **Komponen**               | **Spesifikasi / Catatan**                                                                   |
|----------------------------|---------------------------------------------------------------------------------------------|
| **ESP32**                  | ESP32 WROOM-32 (disarankan) atau varian ESP32 lain dengan pinout serupa.                    |
| **LED Panel P10**          | *Single-color* (merah/hijau/biru), resolusi 32x16 pixel per panel.                          |
| **RTC Module**             | DS1302 (diuji) atau DS3231 (kompatibel). Pastikan baterai CR2032 terpasang untuk backup.    |
| **Power Supply**           | 5V DC dengan arus cukup (min. 2A untuk 1-3 panel, lebih besar untuk panel lebih banyak).    |
| **Kabel Jumper**           | Male-to-female dan female-to-female untuk koneksi ESP32 ↔ P10 ↔ RTC.                        |
| **Buzzer (Opsional)**      | Buzzer aktif LOW 5V.                                                                        |
| **Konektor**               | Header pin 2.54mm untuk koneksi yang rapi.                                                  |

---

## 6. Software Requirements

| **Software**                | **Versi / Catatan**                                                                                     |
|-----------------------------|---------------------------------------------------------------------------------------------------------|
| **Arduino IDE**             | 1.8.19 atau lebih baru.                                                                                 |
| **ESP32 Board Package**     | **Versi 1.0.6** (wajib). Versi lain mungkin menyebabkan error kompilasi pada library DMD32.             |
| **Library DMD32 v1.0**      | Instal via Library Manager Arduino IDE.                                                                 |
| **Library Bawaan ESP32**    | `WiFi.h`, `time.h`, `SPI.h`, `Wire.h` (sudah termasuk dalam board package).                             |
| **Library Lokal (Include)** | `runningtext.h/cpp`, `rtc_autosync.h/cpp`, `virtuabotixRTC.h/cpp`, `buzzer.h/cpp` (sudah ada di repo).  |
| **Fonts**                   | `SystemFont5x7.h`, `Arial_black_16.h` (termasuk dalam library DMD32).                                   |

---

## 7. Installation

### 7.1 Clone Repository

```bash
git clone https://github.com/lutfi-rpi5/runningtext-p10-esp32-rtc-autosync-ntp-server.git
```
Atau unduh ZIP dari GitHub dan ekstrak.

### 7.2 Install ESP32 Board Package v1.0.6
1. Buka **Arduino IDE** → **File** → **Preferences**.
2. Tambahkan URL berikut ke **Additional Boards Manager URLs**:
```text
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```
3. Buka **Tools** → **Board** → **Boards Manager**.
4. Cari **ESP32** by Espressif Systems dan instal **versi 1.0.6**.

### 7.3 Install Library DMD32
1. Buka **Sketch** → **Include Library** → **Manage Libraries**.
2. Cari **DMD32** dan install **versi 1.0**.

### 7.4 Buka Project
1. Buka file `runningtext-p10-esp32-rtc-autosync-ntp-server.ino` di **Arduino IDE**.
2. Pastikan board yang dipilih adalah **ESP32 Dev Module** (atau sesuai board Anda).

### 7.5 Compile & Upload
1. Hubungkan ESP32 ke komputer via USB.
2. Pilih port yang sesuai di **Tools** → **Port**.
3. Klik tombol **Upload**.

---

## 8. Configurations
Seluruh konfigurasi penting terpusat di file utama `.ino` dan file modul terkait. Berikut panduan lengkapnya.

### 8a. Sinkronisasi RTC (NTP Sync Interval)
**Lokasi**: `rtc_autosync.cpp` baris 11-13.

```cpp
const unsigned long ntp_sync_interval = (60 * 60) * 1000;  // 60 menit
const unsigned long ntp_sync_starting_at = (0.1 * 60) * 1000; // 10 detik pertama
```

* `ntp_sync_interval` : Interval sinkronisasi NTP dalam milidetik. Default 1 jam.
* `ntp_sync_starting_at` : Delay awal sebelum sinkronisasi pertama setelah boot. Default 10 detik (untuk memberi waktu koneksi Wi-Fi stabil).

### 8b. Time Zone & Day Offset Setting
**Lokasi**: `rtc_autosync.cpp` baris 9-10.

```cpp
const long gmtOffset_sec = 25200;   // WIB (GMT+7) = 7 * 3600 detik
const int daylightOffset_sec = 0;   // Tidak ada DST di Indonesia
```

* `gmtOffset_sec` : Offset zona waktu dalam detik dari GMT.
    * WIB (GMT+7) = 25200
    * WITA (GMT+8) = 28800
    * WIT (GMT+9) = 32400
* `daylightOffset_sec` : Offset Daylight Saving Time (tidak digunakan di Indonesia).

### 8c. SSID & PASSWORD Setting
**Lokasi**: `rtc_autosync.cpp` baris 8.

```cpp
const char* ssid = "sync-elektro";
const char* password = "12345678";
```
Ganti dengan kredensial Wi-Fi yang tersedia.

### 8d. Jumlah Display Across & Display Down
**Lokasi**: `runningtext.cpp` baris 9.

```cpp
#define DISPLAYS_ACROSS 3   // Jumlah panel horizontal
#define DISPLAYS_DOWN 1     // Jumlah panel vertikal
```
Atur sesuai konfigurasi fisik panel P10 Anda. Sistem akan otomatis menyesuaikan lebar total display (`32 * DISPLAYS_ACROSS` pixel).

### 8e. Ganti Text Utama
**Lokasi**: `runningtext-p10-esp32-rtc-autosync-ntp-server.ino` baris 8-10.

```cpp
String text_utama = "Selamat Datang di Bengkel Jurusan Teknik Elektro | Prodi Teknik Elektronika Politeknik Negeri Sriwijaya Utamakan Keselamatan dan Kesehatan Kerja";
```
Ganti dengan teks yang diinginkan. Gunakan karakter `|` sebagai pemisah jika ingin beberapa bagian teks (tidak memengaruhi animasi, hanya teks biasa).

### 8f. Display Speed
**Lokasi**: `runningtext-p10-esp32-rtc-autosync-ntp-server.ino` baris 12.

```cpp
display.setSpeed(20);
```
* Nilai dalam milidetik, semakin kecil semakin cepat. Rentang tipikal 15–50.

### 8g. Durasi Tampil Scene RTC
**Lokasi**: `runningtext-p10-esp32-rtc-autosync-ntp-server.ino` baris 22-23.

```cpp
display.sceneRTC( tanggal, waktu, (3 * 60) * 1000 ); // 3 menit
```
* Ubah (3 * 60) * 1000 menjadi durasi yang diinginkan dalam milidetik. Contoh:
    * 1 menit = (1 * 60) * 1000
    * 5 menit = (5 * 60) * 1000

---

## 9. Pin Configuration

### 9.1 Panel P10 → ESP32

|**P10 Pin**  |	**ESP32 GPIO** |  **Keterangan**  |
|---------|------------|--------------|
|GND      | GND        |	Ground    |
|OE	      | GPIO 22    |Output Enable |
|A	      | GPIO 19    |Address Line A|
|B	      | GPIO 21	   |Address Line B|
|CLK	  | GPIO 18	   |Clock (Shift Register)|
|LAT (STB)|	GPIO 2	   |Latch / Strobe|
|DR (R1)  | GPIO 23	   |Data Red (untuk panel single-color)|
**Catatan**: Untuk panel *single-color*, hanya gunakan pin **R1** (DR). Pin **G1** (hijau) dan **B1** (biru) tidak digunakan.

### 9.2 RTC (DS1302) → ESP32
|**DS1302 Pin** |	**ESP32 GPIO**  |
|-----------|---------------|
|CLK	    |    GPIO 25    |
|DAT	    |    GPIO 26    |
|RST	    |    GPIO 27    |
|VCC	    |5V (atau 3.3V) |
|GND	    |    GND        |

### 9.3 Buzzer → ESP32
|Buzzer Pin	 | ESP32 GPIO  |
|------------|-------------|
|+ (Positif) |	GPIO 33    |
|- (Negatif) |	GND        |
**Catatan**: Buzzer dikonfigurasi active *LOW* (menyala saat pin diberi logika `LOW`). Jika menggunakan buzzer langsung tanpa relay, ubah logika di `buzzer.cpp` baris 4-5 sesuai komentar.

---

## 10. Struktur Proyek
```text
runningtext-p10-esp32-rtc-autosync-ntp-server/
│
├── runningtext-p10-esp32-rtc-autosync-ntp-server.ino   # Main sketch
├── runningtext.h / .cpp                                # Modul display & scene
├── rtc_autosync.h / .cpp                               # Modul RTC & NTP sync
├── virtuabotixRTC.h / .cpp                             # Library RTC DS1302
├── buzzer.h / .cpp                                     # Modul buzzer
├── fonts/
│   ├── SystemFont5x7.h
│   └── Arial_black_16.h
├── build/                                              # Compiled firmware (opsional)
└── README.md
```

---

## 11. Troubleshooting

## 11. Troubleshooting

| **Masalah**                              | **Solusi**                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 |
|------------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| **Display tidak menyala / blank**        | - Periksa koneksi kabel P10 (OE, CLK, LAT, DR).<br>- Pastikan power supply 5V mencukupi (gunakan adaptor eksternal, bukan USB ESP32).                                                                                                                                                                                                                                                                                                                                                                                                                                                                       |
| **Teks berjalan terlalu cepat/lambat**   | Ubah nilai `display.setSpeed()` di file `.ino`. Semakin kecil nilai, semakin cepat.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         |
| **Waktu RTC tidak akurat / reset**       | - Ganti baterai CR2032 pada modul RTC.<br>- Pastikan sinkronisasi NTP berhasil (lihat Serial Monitor).                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      |
| **Wi-Fi tidak terhubung**                | - Periksa SSID dan password di `rtc_autosync.cpp`.<br>- Pastikan sinyal Wi-Fi cukup kuat.<br>- Tambahkan `delay(5000)` setelah `WiFi.begin()`.                                                                                                                                                                                                                                                                                                                                                                                                                                                              |
| **ESP32 bootloop (restart terus pada saat di Serial Monitor)**       | **Solusi Umum:** <br>- Buka Serial Monitor (baud 115200) dan periksa pesan error terakhir.<br>- Coba *flash* ulang firmware sambil menekan tombol **BOOT**.<br>- Pastikan power supply stabil (3.3V/5V) dan tidak ada korsleting pada pin GPIO.<br>- Hapus flash sepenuhnya dengan `esptool.py erase_flash` lalu upload ulang.<br><br>**Solusi Alternatif (Khusus Windows User – Pembersihan Cache & Instal Ulang Board Package):** <br>1. Buka **Board Manager** di Arduino IDE, cari **ESP32** by Espressif Systems, lalu klik **Remove** untuk menghapus versi yang terpasang (misal v1.0.6).<br>2. Tutup **semua jendela** Arduino IDE yang sedang berjalan.<br>3. Tekan tombol <kbd>Win</kbd> + <kbd>R</kbd>, ketik `%TEMP%` lalu tekan **Enter**. Pilih semua file dan folder (<kbd>Ctrl</kbd> + <kbd>A</kbd>) kemudian hapus permanen dengan <kbd>Shift</kbd> + <kbd>Delete</kbd>.<br>4. Ulangi langkah 3 untuk direktori **`temp`** dan **`PREFETCH`** (ketik masing‑masing pada dialog **Run**).<br>5. **Restart** komputer/laptop.<br>6. Buka kembali Arduino IDE, instal ulang **ESP32 Board Package versi 1.0.6** melalui Board Manager.<br>7. Coba upload ulang sketch. |
| **Kompilasi error "DMD was not declared"** | Pastikan library **DMD32** sudah terinstal. Jika tetap error, coba hapus dan instal ulang library.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           |
| **ESP32 tidak terdeteksi di Arduino IDE** | - Tekan tombol BOOT saat uploading.<br>- Instal driver USB-to-Serial (CP210x atau CH340).                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    |

## 12. Pengembangan Lebih Lanjut
* **OTA Update**: Tambahkan fitur Over-the-Air untuk update firmware via Wi-Fi.
* **Multi-Scene**: Tambahkan lebih banyak scene (misal: suhu, kelembaban, atau informasi dari sensor).
* **Web Config**: Buat portal web sederhana untuk mengubah teks dan pengaturan tanpa recompile.
* **E-paper / OLED**: Integrasi display sekunder untuk informasi status sistem.

---

## 13. Kontribusi
Kontribusi dalam bentuk pull request atau issue sangat diterima. Pastikan kode mengikuti gaya penulisan yang konsisten dan terdokumentasi dengan baik.

---

## 14. Lisensi
Proyek ini dilisensikan di bawah **MIT License**. Silakan gunakan, modifikasi, dan distribusikan sesuai kebutuhan.

---

**Dokumentasi ini disusun berdasarkan analisis menyeluruh terhadap kode sumber, struktur proyek, dan best practice _embedded system engineering_. Untuk pertanyaan lebih lanjut, silakan buka _issue_ di repositori GitHub.**

