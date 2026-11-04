# CRYPTEX

CRYPTEX adalah aplikasi konversi cryptocurrency berbasis terminal yang ditulis
dalam bahasa C. Aplikasi mengambil harga dari CoinPaprika API, menyimpan
riwayat konversi dalam CSV, dan menyediakan antarmuka menu interaktif.

## Fitur

- Konversi crypto ke crypto, crypto ke fiat, dan fiat ke crypto.
- Daftar cryptocurrency: BTC, ETH, USDT, BNB, SOL, XRP, DOGE, ADA, USDC, TRX,
  LINK, XLM, BCH, NEAR, UNI, LTC, SUI, AVAX, HBAR, dan ZEC.
- Mata uang fiat yang didukung: USD, IDR, dan EUR.
- Harga, perubahan 24 jam, market cap, dan waktu pembaruan dari CoinPaprika.
- Refresh data harga secara manual tanpa polling otomatis.
- Riwayat konversi lokal di `data/history.csv`.
- Hapus riwayat dari menu, dengan konfirmasi.
- Validasi input, timeout HTTP, dan penanganan kegagalan koneksi.

## Kebutuhan

- GCC dan `make`.
- libcurl.
- cJSON.
- `pkg-config` (opsional).

Contoh instalasi dependensi di Fedora:

```bash
sudo dnf install gcc make libcurl-devel cjson-devel pkg-config
```

Jika cJSON dipasang secara lokal, Makefile akan memakai fallback berikut:

```text
Include: $HOME/.local/usr/include
Library: $HOME/.local/usr/lib64
```

Makefile mencoba `pkg-config` terlebih dahulu. Jika tidak tersedia, Makefile
memakai path lokal tersebut.

## Build

```bash
make
```

Perintah lain:

```bash
make clean    # hapus build, binary, dan test runner
make debug    # build dengan simbol debug
make run      # build lalu jalankan aplikasi
```

## Menjalankan

```bash
./cryptex
```

Menu utama:

```text
1. Currency Converter
2. Cryptocurrency List
3. Market Price
4. Conversion History
5. Refresh Market Data
0. Exit
```

### Converter

Masukkan simbol mata uang asal, jumlah, lalu simbol mata uang tujuan. Contoh:

```text
FROM currency: BTC
Amount: 0.5
TO currency: USD
```

Simbol yang diterima: `BTC`, `ETH`, `USDT`, `BNB`, `SOL`, `XRP`, `DOGE`,
`ADA`, `USDC`, `TRX`, `LINK`, `XLM`, `BCH`, `NEAR`, `UNI`, `LTC`, `SUI`,
`AVAX`, `HBAR`, `ZEC`, `USD`, `IDR`, dan `EUR`. Input tidak membedakan
huruf besar/kecil.

### History

Menu `4` menampilkan riwayat, lalu menyediakan opsi:

```text
1. Clear History
0. Back to Main Menu
```

Riwayat yang ditampilkan dibatasi 20 entri terakhir. Total konversi tetap
ditampilkan. Penghapusan riwayat meminta konfirmasi.

## Data dan API

Sumber data: CoinPaprika.

```http
GET https://api.coinpaprika.com/v1/tickers/{coin_id}?quotes=USD,IDR,EUR
```

Aplikasi tidak membutuhkan API key. Timeout HTTP: 10 detik untuk koneksi dan
15 detik total. Data harga disimpan sementara di memori selama aplikasi
berjalan. Refresh manual mengambil ulang seluruh daftar. Waktu pembaruan harga
ditampilkan dalam zona WIB (UTC+7).

Riwayat disimpan di:

```text
data/history.csv
```

Format CSV:

```csv
timestamp,from_symbol,to_symbol,amount,exchange_rate,result
2026-11-04 11:25:43,BTC,USD,0.50000000,76572.13804996,38286.06902498
```

File dibuat otomatis saat aplikasi pertama kali dijalankan. Direktori `data/`
dipertahankan di repository melalui `.gitkeep`; file CSV diabaikan oleh Git.

## Testing

```bash
make test       # unit test, tanpa jaringan
make test-api   # integrasi CoinPaprika, butuh internet
make test-ui    # smoke test antarmuka, tanpa jaringan
```

Cakupan pengujian:

- `test_converter`: akurasi konversi dan penolakan nilai tidak valid.
- `test_validator`: validasi jumlah dan simbol.
- `test_json_parser`: parsing JSON, quote USD/IDR/EUR, dan data opsional.
- `test_history`: inisialisasi, simpan, baca, tampilkan, dan hapus riwayat.
- `test_api_client`: request live, HTTP 404, kegagalan koneksi, dan kurs fiat.
- `test_ui`: navigasi menu, format ribuan, batas tampilan riwayat, dan hapus
  riwayat.

## Struktur Proyek

```text
include/cryptex.h          tipe data dan konstanta bersama
src/main.c                 entry point dan lifecycle aplikasi
src/api/api_client.c       HTTP client libcurl
src/converter/converter.c  perhitungan konversi
src/market/market.c        registry, cache, harga, dan kurs fiat
src/history/history.c      penyimpanan dan pembacaan CSV
src/ui/menu.c              dashboard dan navigasi terminal
src/utils/json_parser.c    parsing response CoinPaprika
src/utils/validator.c      validasi input
tests/                     unit, integrasi, dan smoke test
data/                      penyimpanan riwayat runtime
```

## Penanganan Error

- Input tidak valid akan diminta ulang.
- HTTP 404, 429, 5xx, timeout, dan kegagalan koneksi ditampilkan sebagai pesan
  error.
- Response JSON yang tidak lengkap atau tidak valid ditolak.
- Harga nol, negatif, atau non-finite tidak dipakai untuk konversi.
- Jika sebagian data market gagal diambil, data yang berhasil tetap
  ditampilkan; entri tanpa data ditandai `N/A`.
- Kegagalan menulis riwayat ditampilkan sebagai peringatan tanpa menghentikan
  aplikasi.

## Batasan

- Hanya mendukung daftar cryptocurrency yang terdaftar di aplikasi.
- Fiat yang didukung hanya USD, IDR, dan EUR.
- Membutuhkan koneksi internet untuk mengambil harga terbaru.
- Harga yang ditampilkan bersifat informasi, bukan jaminan harga transaksi.
- Aplikasi tidak menyediakan jual beli, wallet, private key, atau transaksi
  blockchain.

## Atribusi

Powered by CoinPaprika.
