# CRYPTEX — Crypto Currency Converter

> **Project Type:** Final Project — C Programming  
> **Programming Language:** C11  
> **Interface:** Terminal User Interface (TUI)  
> **Data Provider:** CoinPaprika API  
> **Difficulty:** Beginner–Intermediate  
> **Platform:** Linux

---

## 1. Project Overview

CRYPTEX adalah aplikasi konversi mata uang cryptocurrency berbasis terminal yang dikembangkan menggunakan bahasa pemrograman C.

Aplikasi ini memungkinkan pengguna melakukan konversi antara berbagai cryptocurrency dan mata uang fiat berdasarkan harga pasar yang diperoleh melalui CoinPaprika REST API.

CRYPTEX dirancang dengan antarmuka terminal yang sederhana, interaktif, dan mudah digunakan.

### Project Title

**Pengembangan Aplikasi Konversi Mata Uang Cryptocurrency Berbasis Terminal Menggunakan Bahasa Pemrograman C dengan Integrasi REST API.**

---

## 2. Project Objectives

Tujuan pengembangan CRYPTEX:

1. Mengimplementasikan bahasa pemrograman C dalam aplikasi yang memiliki kegunaan nyata.
2. Memahami cara mengambil dan mengolah data dari REST API.
3. Mengimplementasikan proses konversi berdasarkan nilai tukar cryptocurrency.
4. Menerapkan penggunaan struktur data dan fungsi dalam bahasa C.
5. Memahami pemrosesan data JSON.
6. Mengembangkan aplikasi terminal yang interaktif.

---

## 3. Technology Stack

| Komponen | Teknologi |
|---|---|
| Programming Language | C11 |
| Compiler | GCC |
| API Provider | CoinPaprika |
| HTTP Client | libcurl |
| JSON Parser | cJSON |
| Interface | Terminal + ANSI Escape Codes |
| Build System | Makefile |
| Data Storage | CSV |
| Operating System | Linux |

### External Dependencies

- **libcurl:** Mengirim HTTP request ke CoinPaprika API.
- **cJSON:** Membaca dan memproses respons JSON.

Antarmuka versi pertama menggunakan ANSI Escape Codes agar tidak membutuhkan library UI tambahan seperti ncurses.

---

## 4. API Provider

CRYPTEX menggunakan **CoinPaprika API** sebagai sumber data cryptocurrency.

### API Information

| Informasi | Detail |
|---|---|
| Provider | CoinPaprika |
| API Type | REST API |
| Data Format | JSON |
| Protocol | HTTPS |
| Authentication | Tidak diperlukan untuk Free Tier |
| Base URL | `https://api.coinpaprika.com/v1` |
| Free Quota | 20.000 calls/month per IP |
| Usage | Personal / Non-commercial |

API key tidak diperlukan untuk penggunaan Free Tier.

Dokumentasi:
- https://coinpaprika.com/developers
- https://coinpaprika.com/api

**Catatan:** Free Tier ditujukan untuk penggunaan personal dan nonkomersial. Aplikasi harus mencantumkan atribusi sumber data CoinPaprika. Ketentuan penggunaan perlu diperiksa kembali jika aplikasi akan didistribusikan atau digunakan secara komersial.

---

## 5. Core Features

### 5.1 Currency Converter

Fitur utama untuk mengonversi cryptocurrency dan mata uang fiat.

Contoh konversi:

- BTC → USD
- ETH → USD
- USDT → USD
- BTC → ETH
- ETH → USDT
- BTC → IDR (jika quote IDR tersedia)

Input pengguna:

- Mata uang asal.
- Mata uang tujuan.
- Jumlah yang ingin dikonversi.

Output:

- Jumlah awal.
- Mata uang asal dan tujuan.
- Nilai tukar.
- Hasil konversi.
- Waktu pembaruan data harga.

### 5.2 Cryptocurrency List

Menampilkan daftar cryptocurrency yang didukung aplikasi.

Daftar awal:

| Cryptocurrency | Symbol | CoinPaprika ID |
|---|---|---|
| Bitcoin | BTC | btc-bitcoin |
| Ethereum | ETH | eth-ethereum |
| Tether | USDT | usdt-tether |
| BNB | BNB | bnb-binance-coin |
| Solana | SOL | sol-solana |
| XRP | XRP | xrp-xrp |
| Dogecoin | DOGE | doge-dogecoin |
| Cardano | ADA | ada-cardano |

Daftar tersebut disimpan sebagai data konfigurasi lokal. CoinPaprika ID digunakan untuk mengidentifikasi aset secara lebih akurat daripada hanya menggunakan simbol.

### 5.3 Market Price

Menampilkan informasi harga cryptocurrency berdasarkan data CoinPaprika API.

Informasi yang ditampilkan:

- Nama cryptocurrency.
- Simbol.
- Harga dalam USD.
- Persentase perubahan harga 24 jam.
- Market capitalization jika tersedia.
- Waktu pembaruan harga.

### 5.4 Conversion History

Menyimpan riwayat konversi yang dilakukan pengguna.

Informasi riwayat:

- Tanggal dan waktu.
- Mata uang asal.
- Mata uang tujuan.
- Jumlah awal.
- Hasil konversi.
- Nilai tukar yang digunakan.

Riwayat disimpan secara lokal menggunakan CSV.

### 5.5 Refresh Market Data

Memungkinkan pengguna mengambil data harga terbaru dari API secara manual.

Aplikasi tidak melakukan polling terus-menerus agar penggunaan kuota API tetap efisien.

---

## 6. Application Interface

CRYPTEX menggunakan antarmuka terminal sederhana dengan sistem menu interaktif.

### Main Dashboard

```text
+------------------------------------------------------+
|                                                      |
|     ██████╗██████╗ ██╗   ██╗██████╗ ███████╗██╗  ██╗|
|    ██╔════╝██╔══██╗╚██╗ ██╔╝╚════██╗██╔════╝╚██╗██╔╝|
|    ██║     ██████╔╝ ╚████╔╝  █████╔╝█████╗   ╚███╔╝ |
|    ██║     ██╔══██╗  ╚██╔╝  ██╔═══╝ ██╔══╝   ██╔██╗ |
|    ╚██████╗██║  ██║   ██║   ███████╗███████╗██╔╝ ██╗|
|     ╚═════╝╚═╝  ╚═╝   ╚═╝   ╚══════╝╚══════╝╚═╝  ╚═╝|
|                                                      |
|             CRYPTO CURRENCY CONVERTER                |
+------------------------------------------------------+
| API STATUS : CONNECTED                               |
| DATA SOURCE: COINPAPRIKA                             |
+------------------------------------------------------+
|                                                      |
|  MAIN MENU                                           |
|                                                      |
|  1. Currency Converter                               |
|  2. Cryptocurrency List                              |
|  3. Market Price                                      |
|  4. Conversion History                                |
|  5. Refresh Market Data                               |
|  0. Exit                                              |
|                                                      |
+------------------------------------------------------+
|  Powered by CoinPaprika                              |
+------------------------------------------------------+
```

### Currency Converter Interface

```text
+------------------------------------------+
|           CURRENCY CONVERTER             |
+------------------------------------------+
|                                          |
|  FROM                                    |
|  Currency : BTC                          |
|  Amount   : 0.5                          |
|                                          |
|  TO                                      |
|  Currency : USD                          |
|                                          |
+------------------------------------------+
|                                          |
|  CONVERSION RESULT                       |
|                                          |
|  0.5 BTC                                 |
|  =                                       |
|  $47,500.00                              |
|                                          |
|  Exchange Rate: $95,000 / BTC            |
|                                          |
+------------------------------------------+
|  1. Convert Again                        |
|  0. Back to Main Menu                    |
+------------------------------------------+
```

*Angka harga hanya ilustrasi.*

---

## 7. API Integration

CRYPTEX menggunakan endpoint ticker CoinPaprika untuk mengambil harga cryptocurrency.

### API Base URL

```http
https://api.coinpaprika.com/v1
```

### 7.1 Get Cryptocurrency Price

Endpoint:

```http
GET /tickers/{coin_id}
```

Contoh request Bitcoin:

```bash
curl https://api.coinpaprika.com/v1/tickers/btc-bitcoin
```

Contoh request Ethereum:

```bash
curl https://api.coinpaprika.com/v1/tickers/eth-ethereum
```

### 7.2 Example JSON Response

```json
{
  "id": "btc-bitcoin",
  "name": "Bitcoin",
  "symbol": "BTC",
  "rank": 1,
  "last_updated": "2026-10-08T10:33:14Z",
  "quotes": {
    "USD": {
      "price": 82772.24,
      "volume_24h": 25739157978,
      "market_cap": 1663327588528,
      "percent_change_24h": -1.06
    }
  }
}
```

*Contoh respons menggunakan data ilustratif dari dokumentasi.*

### 7.3 Data Processing

Aplikasi mengambil beberapa informasi dari respons JSON:

- `id`: Identitas cryptocurrency.
- `name`: Nama cryptocurrency.
- `symbol`: Simbol cryptocurrency.
- `last_updated`: Waktu pembaruan harga.
- `quotes.USD.price`: Harga cryptocurrency dalam USD.
- `quotes.USD.percent_change_24h`: Perubahan harga 24 jam.

### 7.4 Fiat Currency Support

Konversi cryptocurrency ke mata uang fiat menggunakan quote currency yang didukung API.

Untuk USD, gunakan:

```text
quotes.USD.price
```

Untuk IDR, aplikasi dapat meminta quote IDR jika tersedia pada endpoint dan paket API yang digunakan.

Jika quote IDR tidak tersedia, konversi IDR tidak ditampilkan pada versi pertama. Dukungan IDR melalui penyedia nilai tukar fiat tambahan dapat dikembangkan pada versi berikutnya.

---

## 8. Conversion Algorithm

Konversi dilakukan berdasarkan harga cryptocurrency terhadap mata uang referensi yang sama.

### 8.1 Crypto to Crypto

Formula:

\[
Result = Amount \times \frac{Price_{From}}{Price_{To}}
\]

Contoh:

- Amount: 0.5 BTC
- Harga BTC: $95.000
- Harga ETH: $3.200

\[
Result = 0.5 \times \frac{95000}{3200}
\]

\[
Result = 14.84375\ ETH
\]

### 8.2 Crypto to Fiat

Formula:

\[
Result = Amount \times Price_{Crypto}
\]

Contoh:

- Amount: 0.5 BTC
- Harga BTC: $95.000

\[
Result = 0.5 \times 95000
\]

\[
Result = \$47.500
\]

### 8.3 Fiat to Crypto

Formula:

\[
Result = \frac{Amount}{Price_{Crypto}}
\]

Contoh:

- Amount: $1.000
- Harga BTC: $95.000

\[
Result = \frac{1000}{95000}
\]

\[
Result = 0.01052632\ BTC
\]

### Calculation Rules

1. Semua harga harus berasal dari data API yang valid.
2. Kedua harga harus menggunakan mata uang referensi yang sama.
3. Aplikasi harus menolak harga bernilai nol atau tidak valid.
4. Aplikasi harus membatasi jumlah angka desimal pada hasil tampilan.
5. Aplikasi harus menampilkan waktu pembaruan data.
6. Aplikasi tidak boleh menganggap harga API sebagai harga transaksi atau harga eksekusi trading.

---

## 9. System Architecture

Alur kerja aplikasi:

```text
+----------------------+
|        USER          |
+----------------------+
           |
           v
+----------------------+
|      MAIN MENU       |
+----------------------+
           |
           v
+----------------------+
|     USER INPUT       |
| Currency & Amount    |
+----------------------+
           |
           v
+----------------------+
|      API CLIENT      |
|       libcurl        |
+----------------------+
           |
           v
+----------------------+
|   COINPAPRIKA API    |
+----------------------+
           |
           v
+----------------------+
|      JSON PARSER     |
|        cJSON         |
+----------------------+
           |
           v
+----------------------+
|   CONVERSION ENGINE  |
+----------------------+
           |
           v
+----------------------+
|    DISPLAY RESULT    |
+----------------------+
           |
           v
+----------------------+
|    SAVE HISTORY      |
+----------------------+
```

---

## 10. Project Structure

```text
cryptex/
│
├── src/
│   ├── main.c
│   │
│   ├── api/
│   │   ├── api_client.c
│   │   └── api_client.h
│   │
│   ├── converter/
│   │   ├── converter.c
│   │   └── converter.h
│   │
│   ├── market/
│   │   ├── market.c
│   │   └── market.h
│   │
│   ├── history/
│   │   ├── history.c
│   │   └── history.h
│   │
│   ├── ui/
│   │   ├── menu.c
│   │   └── menu.h
│   │
│   └── utils/
│       ├── json_parser.c
│       └── json_parser.h
│
├── include/
│   └── cryptex.h
│
├── data/
│   └── history.csv
│
├── Makefile
├── README.md
├── PRD.md
└── .gitignore
```

---

## 11. Programming Concepts

Konsep bahasa C yang akan diterapkan:

| Konsep | Implementasi |
|---|---|
| Variables | Menyimpan jumlah dan harga |
| Data Types | double, int, char |
| Conditional | Validasi input dan menu |
| Looping | Menu utama dan daftar crypto |
| Functions | Pemisahan fungsi aplikasi |
| Struct | Menyimpan informasi cryptocurrency |
| Array | Menyimpan daftar cryptocurrency |
| Pointer | Pengelolaan data dan respons API |
| File Handling | Menyimpan riwayat konversi |
| Dynamic Memory | Menangani respons HTTP dan JSON |
| External Library | libcurl dan cJSON |

---

## 12. Error Handling

Aplikasi harus mampu menangani kondisi berikut:

| Error | Penanganan |
|---|---|
| Internet disconnected | Menampilkan pesan koneksi gagal |
| HTTP 404 | Cryptocurrency ID tidak ditemukan |
| HTTP 429 | API rate limit tercapai |
| HTTP 5xx | Server API mengalami gangguan |
| Invalid JSON | Menampilkan pesan respons tidak valid |
| Missing price | Data harga tidak tersedia |
| Invalid amount | Meminta input ulang |
| Unsupported currency | Menampilkan mata uang tidak didukung |

Aplikasi harus menggunakan timeout HTTP agar proses request tidak menggantung tanpa batas waktu.

---

## 13. Non-Functional Requirements

1. Aplikasi berjalan pada sistem operasi Linux.
2. Aplikasi dikembangkan menggunakan bahasa C11.
3. Aplikasi dapat dijalankan melalui terminal.
4. Data harga diperoleh dari CoinPaprika API.
5. Aplikasi menampilkan pesan kesalahan jika API tidak tersedia.
6. Aplikasi tidak melakukan request secara terus-menerus.
7. Proses konversi harus menghasilkan perhitungan yang akurat berdasarkan data harga yang diterima.
8. Riwayat konversi tersimpan secara lokal.
9. Aplikasi harus mencantumkan atribusi CoinPaprika.

---

## 14. Development Roadmap

### Phase 1 — Project Foundation

- Membuat struktur direktori.
- Menyiapkan compiler GCC.
- Menyiapkan Makefile.
- Memasang libcurl dan cJSON.
- Membuat program awal.

### Phase 2 — API Integration

- Membuat HTTP client.
- Menghubungkan aplikasi dengan CoinPaprika API.
- Mengambil data harga cryptocurrency.
- Memproses respons JSON.
- Menangani error HTTP.

### Phase 3 — Conversion Engine

- Membuat daftar cryptocurrency.
- Mengimplementasikan algoritma konversi.
- Membuat validasi input.
- Menguji hasil perhitungan.

### Phase 4 — User Interface

- Membuat menu utama.
- Membuat halaman converter.
- Membuat halaman market price.
- Membuat halaman cryptocurrency list.
- Menambahkan navigasi terminal.

### Phase 5 — History & Testing

- Menyimpan riwayat konversi.
- Membaca riwayat dari CSV.
- Menguji kegagalan koneksi API.
- Menguji validasi input.
- Menguji akurasi perhitungan.

### Phase 6 — Documentation

- Membuat README.md.
- Melengkapi dokumentasi instalasi.
- Mendokumentasikan API.
- Melakukan pengujian akhir.

---

## 15. Project Limitations

CRYPTEX memiliki batasan berikut:

1. Aplikasi hanya digunakan untuk konversi mata uang cryptocurrency.
2. Tidak menyediakan fitur jual beli cryptocurrency.
3. Tidak terhubung dengan wallet pengguna.
4. Tidak menyimpan private key atau informasi sensitif wallet.
5. Tidak melakukan transaksi blockchain.
6. Harga mengikuti data yang disediakan CoinPaprika API.
7. Aplikasi membutuhkan koneksi internet untuk mengambil harga terbaru.
8. Jumlah cryptocurrency yang didukung dibatasi pada daftar yang ditentukan.
9. Harga yang ditampilkan bukan jaminan harga transaksi aktual.
10. Dukungan mata uang fiat bergantung pada quote currency yang tersedia.

---

## 16. Acceptance Criteria

CRYPTEX dinyatakan berhasil apabila:

- [ ] Aplikasi berhasil dikompilasi menggunakan GCC.
- [ ] Aplikasi dapat dijalankan melalui terminal.
- [ ] Menu utama dapat digunakan.
- [ ] Aplikasi berhasil terhubung ke CoinPaprika API.
- [ ] Aplikasi dapat mengambil harga cryptocurrency.
- [ ] Aplikasi dapat menampilkan daftar cryptocurrency.
- [ ] Pengguna dapat melakukan konversi antar cryptocurrency.
- [ ] Pengguna dapat melakukan konversi crypto ke fiat yang didukung.
- [ ] Hasil konversi sesuai dengan data harga API.
- [ ] Aplikasi dapat menampilkan waktu pembaruan harga.
- [ ] Aplikasi dapat menyimpan riwayat konversi.
- [ ] Aplikasi dapat menangani kesalahan koneksi dan input.
- [ ] Aplikasi menampilkan atribusi CoinPaprika.

---

## 17. Conclusion

CRYPTEX merupakan aplikasi konversi cryptocurrency berbasis terminal yang mengimplementasikan bahasa pemrograman C dan integrasi REST API.

Project ini berfokus pada pengolahan data eksternal, pemrosesan JSON, algoritma perhitungan nilai tukar, serta pengelolaan file lokal.

Melalui project ini, pengguna dapat mempelajari penerapan bahasa C dalam pengembangan aplikasi yang berinteraksi dengan layanan API eksternal.
