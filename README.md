# Sistem Deteksi Duplikasi Data Arsip

## Deskripsi Proyek

Proyek ini mengimplementasikan sistem sederhana untuk **mengelola arsip
digital serta mendeteksi data duplikat secara otomatis**. Sistem ini
memungkinkan pengguna untuk menambahkan data arsip, mencari data,
menghapus data, menampilkan seluruh data, serta mendeteksi data duplikat
berdasarkan beberapa kriteria.

Pada implementasi ini, data arsip disimpan menggunakan **struktur data
Linked List**. Struktur ini dipilih karena fleksibel untuk operasi
penyisipan data secara dinamis serta mudah diimplementasikan untuk
kebutuhan eksplorasi struktur data dasar.

Selain itu, sistem juga mampu melakukan **deteksi duplikasi data
menggunakan beberapa metode**, yaitu:

- Berdasarkan **content (isi data)**
- Berdasarkan **metadata (name + size)**
- Berdasarkan **kesamaan seluruh atribut data**

Sistem juga dilengkapi dengan fitur **pengukuran waktu eksekusi
(execution time)** untuk setiap operasi utama seperti insert, search,
delete, show, dan duplicate detection.

---

# Arsitektur Sistem

Sistem menggunakan pendekatan **in-memory data management** dengan
dataset awal yang dimuat dari file CSV.

Alur data sistem:

```
dataset.csv
      ↓
readCSV()
      ↓
Linked List (runtime storage)
      ↓
Operasi sistem (insert, search, delete, show, detect duplicate)
      ↓
Hasil duplicate disimpan ke output/result.txt
```

---

# Struktur Folder

```
duplicate-detection/
├── include/
|     ├── fileHandler.h              - deklarasi fungsi pembaca dataset
|     ├── data.h                     - struktur data arsip
|     ├── linkedList.h               - deklarasi Linked List
|     └── hashSystem.h               - deklarasi Hash Table
├── src/
|     ├── main.cpp                   - program utama dan menu sistem
|     ├── fileHandler.cpp            - membaca dataset CSV
|     ├── linkedList.cpp             - implementasi Linked List
|     └── hashSystem.cpp             - implementasi Hash Table
├── public/
|     └── dataset.csv          - dataset arsip
└── output/
      └── result.txt           - hasil deteksi duplikasi
```
---

# Struktur Data

## Data Arsip

Setiap arsip memiliki atribut berikut:

```
Data
├── id          -> ID unik file
├── name        -> nama file
├── size        -> ukuran file
├── upload_date -> tanggal upload
├── source      -> sumber dokumen
└── content     -> representasi isi file
```

---

## Node Linked List
Setiap node pada Linked List mempunyai satu data arsip.

```
Node
├── Data            -> informasi arsip
├── isDuplicate     -> penanda apakah data merupakan duplikat
└── next            -> pointer ke node berikutnya
```

# Dataset

Format CSV:

```
id,name,size,upload_date,source,content
```

Contoh:

```
004901,ClimateRepo.pdf,2048,2004-07-24,JohnSmith,a81f3c92d11a0e5b\
004912,ClimateRepo_copy.pdf,2048,2004-07-24,JohnSmith,a81f3c92d11a0e5b
```
---
# Mekanisme Load Dataset
Dataset dimuat menggunakan fungsi `readCSV()`.
Prosesnya:

1.  Membaca file CSV
2.  Parsing setiap baris menjadi objek `Data`
3.  Memasukkan data ke dalam `vector`
4.  Menyimpan data ke dalam Linked List menggunakan fungsi `insert()`

---
# Mekanisme Deteksi Duplikasi

Sistem mendukung tiga metode deteksi duplikasi:

1.  **Content**
Dua data dianggap duplikat jika memiliki nilai content yang sama.

```
if(dataA.content == dataB.content)
```

Metode ini mensimulasikan deteksi file identik berdasarkan hash isi file.

2.  **Metadata (name + size)**
Duplikasi dideteksi menggunakan kombinasi:
```
name + size
```
```
if(dataA.name == dataB.name && dataA.size == dataB.size)
```
Metode ini sering digunakan pada sistem penyimpanan untuk mendeteksi kemungkinan file duplikat.

3.  **Full data comparison**

Semua atribut dibandingkan.

```
id
name
size
upload_date
source
content
```
Jika semua atribut sama maka data dianggap duplikat.

---

# Algoritma Duplicate Detection

Pseudo code:

```
for each node A\
 for each node B setelah A\
  if data sama → tandai duplicate
```

Kompleksitas:
```
O(n²)
```
dimana `n` adalah jumlah data dalam sistem.

---

# Fitur Sistem

1.  Insert Data
Menambahkan data arsip baru ke dalam sistem.

2.  Search Data
Pencarian data berdasarkan:
 - ID
 - Nama file
 - Kombinasi ID dan nama
   
3.  Detect Duplicate
Mendeteksi data duplikat menggunakan tiga metode:
 - Based on Content
 - Based on Metadata
 - Based on Full Data
Hasil deteksi disimpan ke dalam file:
```
output/result.txt
```
4.  Show Data
Menampilkan data yang tersimpan dalam sistem.
Pilihan tampilan:
 - seluruh data
 - hanya data duplikat
   
5.  Delete Data
Menghapus data arsip berdasarkan ID.

6.  Statistics
Menampilkan statistik sistem:

```
Total Data
Duplicate Data
Unique Data
Execution Time
```
---

# Pengukuran Execution Time

Program menggunakan library **chrono** dari C++ untuk mengukur waktu
eksekusi setiap operasi.
Contoh penggunaan:
```
auto start = chrono::high_resolution_clock::now();
list.detectDuplicateByContent();
auto end = chrono::high_resolution_clock::now();
```
Hasil waktu eksekusi dihitung menggunakan:
```
chrono::duration_cast
```
Operasi yang diukur:
 - Insert
 - Search
 - Delete
 - Show

Duplicate Detection
---

# Kompleksitas Operasi

Operasi Kompleksitas

| Operasi | Kompleksitas |
| ------ | ------ |
| Insert | O(1) |
| Search | O(n) |
| Delete | O(n) |
| Show Data | O(n) |
| Detect Duplicate | O(n²) |

Dimana:
```
n = jumlah data arsip
```
Operasi duplicate detection memiliki kompleksitas terbesar karena setiap data harus dibandingkan dengan seluruh data lainnya.

---

# Cara Menjalankan Program

Compile:

```
g++ *.cpp -o program1
```

Run:

```
./program1
```

---

# Teknologi yang Digunakan

- C++17
- STL (`vector`,`string`)
- Linked List
- chrono library
- CSV file handling

---

# Pengembangan Selanjutnya

- Implementasi Hash Table
- Perbandingan performa Linked List vs Hash Table
- Grafik execution time
- Analisis performa struktur data
- Penambahan fitur Update dan Perbaikan Bug
