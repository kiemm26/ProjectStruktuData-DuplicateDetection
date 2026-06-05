# Sistem Deteksi Duplikasi Data Arsip

## Deskripsi Proyek

Proyek ini mengimplementasikan sistem sederhana untuk **mengelola arsip
digital serta mendeteksi data duplikat secara otomatis**. Sistem ini
memungkinkan pengguna untuk menambahkan data arsip, mencari data,
menghapus data, menampilkan seluruh data, serta mendeteksi data duplikat
berdasarkan beberapa kriteria.

Pada implementasi ini, terdapat **tiga pilihan struktur data** yang dapat
digunakan dan dibandingkan performanya:

- **Linked List** — penyisipan O(1), fleksibel, implementasi paling sederhana
- **Hash Table** — pencarian & penyisipan O(1) amortised, paling cepat untuk lookup
- **AVL Tree** — BST self-balancing, pencarian O(log n), data selalu terurut by ID

Selain itu, sistem juga mampu melakukan **deteksi duplikasi data
menggunakan beberapa metode**, yaitu:

- Berdasarkan **content (isi data)**
- Berdasarkan **metadata (name + size)**
- Berdasarkan **kesamaan seluruh atribut data**

Sistem juga dilengkapi dengan fitur **benchmark runtime** untuk setiap
kombinasi ukuran dataset dan struktur data. Benchmark mencatat waktu
eksekusi operasi utama dalam microseconds serta penggunaan memori proses
dalam bytes.

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
Struktur Data Pilihan (Linked List / Hash Table / AVL Tree)
      ↓
Operasi sistem (insert, search, delete, show, detect duplicate)
      ↓
Hasil duplicate disimpan ke output/result.txt
Benchmark runtime disimpan ke output/benchmark.csv
```

---

# Struktur Folder

```
duplicate-detection/
├── include/
|     ├── fileHandler.h              - deklarasi fungsi pembaca dataset
|     ├── data.h                     - struktur data arsip
|     ├── linkedList.h               - deklarasi Linked List
|     ├── hashSystem.h               - deklarasi Hash Table
|     └── avlSystem.h                - deklarasi AVL Tree
├── src/
|     ├── main.cpp                   - program utama dan menu sistem
|     ├── fileHandler.cpp            - membaca dataset CSV
|     ├── linkedList.cpp             - implementasi Linked List
|     ├── hashSystem.cpp             - implementasi Hash Table
|     └── avlSystem.cpp              - implementasi AVL Tree
├── public/
|     └── dataset.csv          - dataset arsip
└── output/
      ├── result.txt           - hasil deteksi duplikasi
      └── benchmark.csv        - hasil benchmark waktu dan memori
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

## 1. Node Linked List
Setiap node pada Linked List mempunyai satu data arsip.

```
Node
├── Data            -> informasi arsip
├── isDuplicate     -> penanda apakah data merupakan duplikat
└── next            -> pointer ke node berikutnya
```

## 2. Node Hash Table

```
HashNode
├── Data            -> informasi arsip
├── isDuplicate     -> penanda apakah data merupakan duplikat
└── next            -> pointer untuk chaining tabrakan hash
```

## 3. Node AVL Tree

```
AVLNode
├── Data            -> informasi arsip
├── isDuplicate     -> penanda apakah data merupakan duplikat
├── height          -> tinggi subtree (untuk menghitung balance factor)
├── left            -> pointer ke child kiri  (key < node.id)
└── right           -> pointer ke child kanan (key >= node.id)
```

**Balance Factor (BF)** = `height(left) − height(right)`

AVL invariant: BF ∈ {−1, 0, 1} untuk setiap node. Jika BF dilanggar
setelah insert atau delete, sistem melakukan rotasi otomatis untuk
mengembalikan keseimbangan.

---

# Dataset

Format CSV:

```
id,name,size,upload_date,source,content
```

Contoh:

```
004901,ClimateRepo.pdf,2048,2004-07-24,JohnSmith,a81f3c92d11a0e5b
004912,ClimateRepo_copy.pdf,2048,2004-07-24,JohnSmith,a81f3c92d11a0e5b
```
---
# Mekanisme Load Dataset
Dataset dimuat menggunakan fungsi `readCSV()`.
Prosesnya:

1.  Membuka file CSV
2.  Parsing setiap baris menjadi objek `Data`
3.  Memasukkan data ke dalam `vector`
4.  Menyimpan data ke dalam struktur data aktif menggunakan fungsi `insert()`

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
id, name, size, upload_date, source, content
```
Jika semua atribut sama maka data dianggap duplikat.

---

# Algoritma Duplicate Detection

## Linked List — O(n²)

```
for each node A
  for each node B setelah A
    if data sama → tandai duplicate
```

## Hash Table & AVL Tree — O(n) / O(n log n)

Menggunakan mini hash bucket internal untuk grouping duplikat:

```
inorder traversal → kumpulkan semua node ke vector
for each node
  hitung composite key (content / name+size / all fields)
  masukkan ke bucket sesuai hash key
for each bucket
  jika ukuran bucket > 1 → semua node dalam bucket adalah duplikat
```

Pendekatan ini menghindari nested loop O(n²) dan menurunkan kompleksitas
deteksi menjadi O(n) untuk Hash Table dan O(n log n) untuk AVL Tree
(karena inorder traversal O(n) + insert ke bucket O(1) amortised).

---

# Mekanisme AVL Tree

## Rotasi

AVL Tree mempertahankan keseimbangan melalui empat jenis rotasi:

| Kasus | Kondisi | Rotasi |
|-------|---------|--------|
| LL (Left-Left) | BF(node)=+2, BF(left)≥0 | `rotateRight(node)` |
| RR (Right-Right) | BF(node)=−2, BF(right)≤0 | `rotateLeft(node)` |
| LR (Left-Right) | BF(node)=+2, BF(left)<0 | `rotateLeft(left)` → `rotateRight(node)` |
| RL (Right-Left) | BF(node)=−2, BF(right)>0 | `rotateRight(right)` → `rotateLeft(node)` |

## Insert Rebalancing

Setiap insert dilakukan secara rekursif. Setelah menyisipkan node baru,
fungsi `rebalance()` dipanggil pada perjalanan balik (post-order) rekursi
di setiap node sepanjang jalur dari leaf ke root. Dengan demikian seluruh
jalur ter-rebalance secara otomatis dalam satu pass.

## Delete Rebalancing

Delete menggunakan strategi **in-order successor**:

- Node dengan 0 atau 1 child → hapus langsung, ganti dengan child-nya.
- Node dengan 2 child → salin data dari in-order successor (node terkecil
  di subtree kanan), kemudian hapus successor tersebut.

`rebalance()` dipanggil pada perjalanan balik rekursi, menjamin AVL
invariant terjaga sepanjang jalur yang terpengaruh.

## Traversal

AVL Tree menggunakan **inorder traversal** (kiri → node → kanan)
sehingga data selalu tampil **terurut berdasarkan ID** secara otomatis
tanpa proses sorting tambahan.

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
Menampilkan tabel benchmark runtime yang tersimpan di `output/benchmark.csv`.

```
data_size
data_structure
insert_time_us
search_time_us
update_time_us
delete_time_us
duplicate_detection_time_us
show_time_us
memory_usage_bytes
```
---

# Pengukuran Benchmark

Program menggunakan library **chrono** dari C++ untuk mengukur waktu
eksekusi setiap operasi dalam microseconds.

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
 - Update
 - Delete
 - Show
 - Duplicate Detection

Selain waktu eksekusi, program juga mencatat penggunaan memori proses
runtime dengan `getrusage()` dari `<sys/resource.h>`.

- macOS: `ru_maxrss` sudah dalam bytes.
- Linux: `ru_maxrss` dalam KB, sehingga dikonversi ke bytes dengan `* 1024`.

Nilai `memory_usage_bytes` merepresentasikan penggunaan memori proses
saat benchmark berjalan, bukan memori murni dari struktur data saja.

File `output/benchmark.csv` di-reset setiap program dijalankan. Setiap
penulisan benchmark menggunakan mekanisme upsert berdasarkan kombinasi
`data_size + data_structure`, sehingga kombinasi yang sama diperbarui
tanpa membuat duplicate row.

Header CSV:

```csv
data_size,data_structure,insert_time_us,search_time_us,update_time_us,delete_time_us,duplicate_detection_time_us,show_time_us,memory_usage_bytes
```

---

# Kompleksitas Operasi

| Operasi | Linked List | Hash Table | AVL Tree |
|---------|-------------|------------|----------|
| Insert | O(1) | O(1) amortised | O(log n) |
| Search by ID | O(n) | O(1) amortised | O(log n) |
| Search by Name | O(n) | O(n) | O(n) |
| Delete | O(n) | O(1) amortised | O(log n) |
| Show Data | O(n) | O(n) | O(n) |
| Detect Duplicate | O(n²) | O(n) | O(n log n) |

Dimana `n` = jumlah data arsip dalam sistem.

**Catatan perbandingan:**
- **Linked List** paling sederhana, cocok untuk dataset kecil.
- **Hash Table** tercepat untuk insert/search/delete dan deteksi duplikat.
- **AVL Tree** unggul dibanding Linked List di semua operasi utama, dan
  memberikan keuntungan tambahan berupa data yang selalu **terurut by ID**
  tanpa biaya sorting tambahan — ideal untuk laporan yang memerlukan urutan.

---

# Cara Menjalankan Program

## Compile

```bash
g++ -Iinclude src/*.cpp -o main
```

## Run

```bash
./main
```

Saat program berjalan, pengguna memilih jumlah data awal dan struktur data
melalui menu. Struktur data dan ukuran dataset dapat diganti tanpa restart
melalui menu **Change Data Structure** dan **Change Data Size**.

---

# Teknologi yang Digunakan

- C++17
- STL (`vector`, `string`)
- Linked List
- Hash Table (separate chaining)
- AVL Tree (self-balancing BST)
- chrono library
- getrusage (`<sys/resource.h>`) untuk benchmark memori
- CSV file handling

---

# Pengembangan Selanjutnya

- Perbandingan performa ketiga struktur data secara visual (grafik execution time)
- Analisis performa dengan dataset berukuran bervariasi (1K, 10K, 100K)
- Secondary index untuk pencarian by Name di AVL Tree (AVL kedua dengan key = name)
- Visualisasi hasil `output/benchmark.csv` untuk analisis waktu dan memori
