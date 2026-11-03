# CRYPTEX — System Architecture

> **Project:** CRYPTEX — Crypto Currency Converter  
> **Document:** System Architecture  
> **Version:** 1.0.0  
> **Programming Language:** C11  
> **API Provider:** CoinPaprika  
> **HTTP Client:** libcurl  
> **JSON Parser:** cJSON  
> **Storage:** CSV  
> **Interface:** Terminal (ANSI Escape Codes)

---

## 1. Architecture Overview

CRYPTEX menggunakan pendekatan modular architecture, yaitu membagi aplikasi menjadi beberapa modul berdasarkan tanggung jawab masing-masing.

Setiap modul memiliki fungsi yang spesifik sehingga kode lebih mudah dikembangkan, diuji, dan dipelihara.

### Main Components

| Module | Responsibility |
|---|---|
| Main | Entry point aplikasi |
| UI | Menampilkan menu dan menerima input |
| API Client | Mengirim HTTP request ke CoinPaprika |
| JSON Parser | Memproses respons JSON |
| Market | Mengelola data cryptocurrency dan harga |
| Converter | Menghitung hasil konversi |
| History | Menyimpan dan membaca riwayat |
| Utils | Fungsi pendukung dan validasi |

### High-Level Architecture

```text
+------------------------------------------------+
|                    USER                        |
+------------------------------------------------+
                      |
                      v
+------------------------------------------------+
|                  UI MODULE                     |
|                                                |
|  Main Menu | Converter Menu | Market Menu      |
+------------------------------------------------+
                      |
                      v
+------------------------------------------------+
|                 MAIN MODULE                    |
|                                                |
|  Application Flow & Module Coordination        |
+------------------------------------------------+
          |                |              |
          v                v              v
+----------------+ +---------------+ +-------------+
| MARKET MODULE  | | CONVERTER     | | HISTORY     |
|                | | MODULE        | | MODULE      |
| Price Data     | | Calculation   | | CSV Storage|
+----------------+ +---------------+ +-------------+
          |
          v
+------------------------------------------------+
|                 API MODULE                     |
|                                                |
|  HTTP Request (libcurl)                        |
|  JSON Processing (cJSON)                       |
+------------------------------------------------+
                      |
                      v
+------------------------------------------------+
|               COINPAPRIKA API                  |
+------------------------------------------------+
```

---

## 2. Application Layer

### 2.1 Main Module

File:
- `src/main.c`

Tanggung jawab:

- Menjadi entry point aplikasi.
- Melakukan inisialisasi aplikasi.
- Menampilkan menu utama.
- Mengatur alur navigasi.
- Memanggil modul sesuai pilihan pengguna.
- Melakukan cleanup sebelum aplikasi ditutup.

Fungsi utama:

```c
int main(void);
```

Fungsi pendukung:

```c
void app_initialize(void);
void app_cleanup(void);
void main_menu(void);
```

### 2.2 UI Module

Files:

- `src/ui/menu.c`
- `src/ui/menu.h`

Tanggung jawab:

- Menampilkan dashboard terminal.
- Menampilkan menu utama.
- Menerima input pengguna.
- Menampilkan hasil konversi.
- Menampilkan informasi harga cryptocurrency.
- Menampilkan pesan kesalahan.

Fungsi utama:

```c
void display_dashboard(void);
void display_main_menu(void);
void display_converter_menu(void);
void display_market_menu(void);
void display_history_menu(void);
```

UI tidak boleh melakukan HTTP request atau menghitung nilai konversi secara langsung.

UI hanya bertanggung jawab terhadap interaksi dengan pengguna.

### 2.3 Market Module

Files:

- `src/market/market.c`
- `src/market/market.h`

Tanggung jawab:

- Mengelola daftar cryptocurrency.
- Mengambil informasi harga melalui API.
- Menyimpan data harga dalam struktur data.
- Menampilkan informasi market.
- Mengelola data harga yang berhasil diterima.

Fungsi utama:

```c
int market_fetch_prices(void);
void market_display_prices(void);
int market_find_coin(const char *symbol);
void market_refresh(void);
```

---

## 3. API Integration Layer

API Integration Layer bertanggung jawab untuk berkomunikasi dengan CoinPaprika API.

Layer ini terdiri dari HTTP Client dan JSON Parser.

### 3.1 API Client

Files:

- `src/api/api_client.c`
- `src/api/api_client.h`

Library:

- libcurl

Tanggung jawab:

- Membuat HTTP GET request.
- Mengatur URL endpoint.
- Mengatur connection timeout.
- Menerima HTTP response.
- Memeriksa HTTP status code.
- Mengembalikan response body kepada JSON Parser.

Fungsi utama:

```c
char *api_get(const char *endpoint);
```

Contoh penggunaan:

```c
char *response = api_get("/tickers/btc-bitcoin");
```

Alur kerja:

```text
API Client
    |
    v
Build Request URL
    |
    v
Initialize CURL
    |
    v
Send HTTP GET
    |
    v
Receive Response
    |
    v
Check HTTP Status
    |
    v
Return Response Body
```

### 3.2 JSON Parser

Files:

- `src/utils/json_parser.c`
- `src/utils/json_parser.h`

Library:

- cJSON

Tanggung jawab:

- Memproses response JSON.
- Mengambil informasi cryptocurrency.
- Mengambil harga berdasarkan quote currency.
- Mengambil persentase perubahan harga.
- Mengambil waktu pembaruan data.
- Memvalidasi field yang tersedia.

Fungsi utama:

```c
int parse_coin_data(
    const char *json,
    const char *currency,
    struct Coin *coin
);
```

Contoh data JSON:

```json
{
  "id": "btc-bitcoin",
  "name": "Bitcoin",
  "symbol": "BTC",
  "quotes": {
    "USD": {
      "price": 95000
    }
  }
}
```

Hasil parsing disimpan ke dalam struktur `Coin`.

---

## 4. Data Model

CRYPTEX menggunakan `struct` untuk mengelola informasi cryptocurrency.

### 4.1 Coin Structure

```c
typedef struct {
    char id[64];
    char name[64];
    char symbol[16];

    double price_usd;
    double percent_change_24h;

    char last_updated[32];

} Coin;
```

Penjelasan:

| Attribute | Description |
|---|---|
| id | CoinPaprika ID |
| name | Nama cryptocurrency |
| symbol | Simbol cryptocurrency |
| price_usd | Harga dalam USD |
| percent_change_24h | Perubahan harga 24 jam |
| last_updated | Waktu pembaruan harga |

### 4.2 Conversion Structure

```c
typedef struct {
    char from_symbol[16];
    char to_symbol[16];

    double amount;
    double exchange_rate;
    double result;

    char timestamp[32];

} Conversion;
```

Struktur ini digunakan untuk menyimpan informasi transaksi konversi.

### 4.3 Cryptocurrency Registry

Daftar cryptocurrency yang didukung disimpan dalam array konfigurasi.

```c
typedef struct {
    char id[64];
    char name[64];
    char symbol[16];
} CoinInfo;
```

Contoh:

```c
CoinInfo coins[] = {
    {"btc-bitcoin", "Bitcoin", "BTC"},
    {"eth-ethereum", "Ethereum", "ETH"},
    {"usdt-tether", "Tether", "USDT"},
    {"bnb-binance-coin", "BNB", "BNB"},
    {"sol-solana", "Solana", "SOL"},
    {"xrp-xrp", "XRP", "XRP"}
};
```

Registry ini merupakan daftar aset yang dikonfigurasi untuk aplikasi, bukan daftar lengkap seluruh aset CoinPaprika.

---

## 5. Conversion Engine

Files:

- `src/converter/converter.c`
- `src/converter/converter.h`

Conversion Engine merupakan modul yang bertanggung jawab terhadap seluruh proses perhitungan konversi.

### 5.1 Conversion Flow

```text
+------------------------+
| User Input             |
| From, To, Amount       |
+------------------------+
            |
            v
+------------------------+
| Validate Input         |
+------------------------+
            |
            v
+------------------------+
| Find Currency Data     |
+------------------------+
            |
            v
+------------------------+
| Get Price Data         |
+------------------------+
            |
            v
+------------------------+
| Calculate Exchange     |
+------------------------+
            |
            v
+------------------------+
| Display Result         |
+------------------------+
            |
            v
+------------------------+
| Save Conversion        |
+------------------------+
```

### 5.2 Conversion Formula

Konversi crypto ke crypto:

\[
Result = Amount \times \frac{Price_{From}}{Price_{To}}
\]

Konversi crypto ke fiat:

\[
Result = Amount \times Price_{Crypto}
\]

Konversi fiat ke crypto:

\[
Result = \frac{Amount}{Price_{Crypto}}
\]

### 5.3 Conversion Function

```c
double convert_currency(
    double amount,
    double from_price,
    double to_price
);
```

Implementasi:

```c
double convert_currency(
    double amount,
    double from_price,
    double to_price
) {
    if (amount <= 0 || from_price <= 0 || to_price <= 0) {
        return -1.0;
    }

    return amount * (from_price / to_price);
}
```

Catatan:

- Untuk crypto ke fiat, gunakan harga fiat per unit crypto.
- Untuk fiat ke crypto, gunakan pembagian jumlah fiat dengan harga crypto.
- Untuk crypto ke crypto, kedua harga harus menggunakan quote currency yang sama.
- Nilai `-1.0` digunakan sebagai penanda error dalam contoh sederhana; implementasi akhir sebaiknya menggunakan status return dan parameter output agar error tidak tercampur dengan hasil numerik.

---

## 6. History Management

Files:

- `src/history/history.c`
- `src/history/history.h`

Tanggung jawab:

- Menyimpan riwayat konversi.
- Membaca riwayat dari file CSV.
- Menampilkan riwayat konversi.
- Menangani file yang belum tersedia.

### CSV Structure

File:

```text
data/history.csv
```

Format:

```csv
timestamp,from_symbol,to_symbol,amount,exchange_rate,result
2026-11-03 10:00:00,BTC,USD,0.5,95000,47500
2026-11-03 10:05:00,ETH,USD,2,3200,6400
```

Fungsi utama:

```c
int history_save(const Conversion *conversion);
void history_display(void);
int history_initialize(void);
```

---

## 7. External API Communication

### 7.1 CoinPaprika Endpoint

Base URL:

```text
https://api.coinpaprika.com/v1
```

Endpoint ticker:

```http
GET /tickers/{coin_id}
```

Contoh:

```bash
curl https://api.coinpaprika.com/v1/tickers/btc-bitcoin
```

### 7.2 API Request Strategy

Untuk versi pertama, aplikasi menggunakan request berdasarkan kebutuhan pengguna.

Contoh:

- Pengguna membuka Market Price: mengambil data aset yang ditampilkan.
- Pengguna melakukan konversi: mengambil harga aset asal dan tujuan.
- Pengguna menekan Refresh: mengambil ulang data harga.

Aplikasi tidak melakukan polling otomatis secara terus-menerus.

### 7.3 Request Optimization

Untuk mengurangi penggunaan kuota API:

1. Mengambil data hanya ketika diperlukan.
2. Menghindari request berulang untuk aset yang sama dalam satu proses konversi.
3. Menggunakan cache harga sementara selama aplikasi berjalan.
4. Memungkinkan pengguna melakukan refresh manual.
5. Mengatur timeout HTTP.

### 7.4 Price Cache

Data harga dapat disimpan sementara di memori.

Contoh:

```c
typedef struct {
    Coin coin;
    int is_valid;
} PriceCache;
```

Cache hanya digunakan selama aplikasi berjalan.

Ketika pengguna melakukan refresh, aplikasi mengambil data terbaru dari API dan memperbarui cache.

Cache tidak boleh dianggap sebagai harga terkini tanpa memperhatikan `last_updated`.

---

## 8. Error Handling Architecture

CRYPTEX harus mampu menangani kesalahan pada setiap layer.

| Layer | Error | Handling |
|---|---|---|
| UI | Invalid input | Meminta input ulang |
| API Client | Connection failed | Menampilkan koneksi gagal |
| API Client | Timeout | Membatalkan request |
| API Client | HTTP 404 | Menampilkan data tidak ditemukan |
| API Client | HTTP 429 | Menampilkan rate limit |
| API Client | HTTP 5xx | Menampilkan server error |
| JSON Parser | Invalid JSON | Menolak response |
| JSON Parser | Missing field | Menampilkan data tidak lengkap |
| Converter | Price = 0 | Membatalkan konversi |
| History | File unavailable | Membuat file jika memungkinkan |

### Error Handling Flow

```text
API Request
    |
    v
HTTP Response
    |
    v
Check Status Code
    |
    +---- Failed ----> Display Error
    |
    v
Parse JSON
    |
    +---- Invalid ---> Display Error
    |
    v
Validate Price
    |
    +---- Invalid ---> Display Error
    |
    v
Conversion Engine
    |
    v
Display Result
```

---

## 9. Complete Project Structure

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
│       ├── json_parser.h
│       ├── validator.c
│       └── validator.h
│
├── include/
│   └── cryptex.h
│
├── data/
│   └── history.csv
│
├── tests/
│   ├── test_converter.c
│   └── test_validator.c
│
├── Makefile
├── README.md
├── PRD.md
├── ARCHITECTURE.md
└── .gitignore
```

---

## 10. Build Architecture

CRYPTEX menggunakan GCC dan Makefile untuk proses kompilasi.

### Dependencies

Ubuntu / Debian:

```bash
sudo apt update

sudo apt install build-essential libcurl4-openssl-dev
```

cJSON:

```bash
sudo apt install libcjson-dev
```

### Compilation Example

```bash
gcc -std=c11 \
    src/main.c \
    src/api/api_client.c \
    src/converter/converter.c \
    src/market/market.c \
    src/history/history.c \
    src/ui/menu.c \
    src/utils/json_parser.c \
    src/utils/validator.c \
    -o cryptex \
    -lcurl -lcjson -lm
```

### Makefile Commands

```bash
make
make run
make clean
```

---

## 11. Security Considerations

1. Semua komunikasi API menggunakan HTTPS.
2. Aplikasi tidak menyimpan API secret karena CoinPaprika Free Tier tidak memerlukan API key.
3. Respons API harus divalidasi sebelum digunakan.
4. Input pengguna harus divalidasi sebelum perhitungan.
5. Alokasi memori harus diperiksa untuk menghindari null pointer dereference.
6. Buffer HTTP harus memiliki batas ukuran yang wajar.
7. Semua alokasi memori harus dibebaskan setelah selesai digunakan.
8. Timeout koneksi harus diterapkan untuk mencegah aplikasi menggantung.

---

## 12. Development Priority

Pengembangan dilakukan secara bertahap.

| Priority | Module | Description |
|---|---|---|
| P0 | API Client | Menghubungkan aplikasi dengan CoinPaprika |
| P0 | JSON Parser | Mengambil data harga dari JSON |
| P0 | Converter | Menghitung konversi mata uang |
| P1 | Market | Menampilkan informasi harga |
| P1 | UI | Membuat menu terminal |
| P1 | History | Menyimpan riwayat konversi |
| P2 | Cache | Mengurangi request API berulang |
| P2 | Testing | Pengujian modul dan validasi |

---

## 13. Testing Strategy

### API Client Testing

- Menguji koneksi ke CoinPaprika.
- Menguji endpoint ticker.
- Menguji koneksi internet terputus.
- Menguji HTTP error.
- Menguji timeout.

### JSON Parser Testing

- Menguji JSON valid.
- Menguji JSON tidak valid.
- Menguji field yang hilang.
- Menguji nilai harga yang tidak valid.

### Converter Testing

- Menguji BTC ke USD.
- Menguji ETH ke USD.
- Menguji BTC ke ETH.
- Menguji input nol.
- Menguji input negatif.
- Menguji pembagian dengan nol.

### History Testing

- Menguji penyimpanan CSV.
- Menguji pembacaan CSV.
- Menguji file yang belum tersedia.
- Menguji riwayat kosong.

---

## 14. Architecture Principles

CRYPTEX dikembangkan dengan prinsip berikut:

1. **Modularity:** Setiap modul memiliki tanggung jawab yang jelas.
2. **Separation of Concerns:** UI, API, perhitungan, dan penyimpanan tidak dicampur.
3. **Reusability:** Fungsi dapat digunakan kembali oleh modul lain.
4. **Simplicity:** Menghindari kompleksitas yang tidak diperlukan.
5. **Memory Safety:** Mengelola alokasi dan pembebasan memori secara benar.
6. **Maintainability:** Struktur kode mudah dikembangkan dan dipelihara.

---

## 15. Conclusion

Arsitektur CRYPTEX dirancang menggunakan pendekatan modular untuk memisahkan proses interaksi pengguna, pengambilan data API, pemrosesan JSON, perhitungan konversi, dan penyimpanan riwayat.

Dengan menggunakan bahasa C, libcurl, cJSON, dan CoinPaprika API, CRYPTEX diharapkan menjadi aplikasi terminal yang sederhana, efisien, dan mudah dikembangkan.

Dokumen ini menjadi acuan utama dalam proses implementasi CRYPTEX.
