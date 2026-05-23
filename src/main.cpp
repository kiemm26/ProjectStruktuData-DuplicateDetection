#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>

#include "../include/avlSystem.h"
#include "../include/data.h"
#include "../include/fileHandler.h"
#include "../include/hashSystem.h"
#include "../include/linkedList.h"

using namespace std;

const string DEFAULT_DATASET_FILE = "dataset/dataset.csv";
const string FALLBACK_DATASET_FILE = "public/dataset.csv";
const string BENCHMARK_FILE = "output/benchmark.csv";
const string BENCHMARK_HEADER = "data_size,data_structure,insert_time_ms,search_time_ms,update_time_ms,delete_time_ms,duplicate_detection_time_ms,show_time_ms";

enum class StructureType
{
      LinkedList = 1,
      HashTable = 2,
      AVLTree = 3
};

struct BenchmarkStats
{
      int dataSize = 0;
      string dataStructure = "";
      long long insertTimeMs = 0;
      long long searchTimeMs = 0;
      long long updateTimeMs = 0;
      long long deleteTimeMs = 0;
      long long duplicateDetectionTimeMs = 0;
      long long showTimeMs = 0;
};

class DuplicateSystem
{
public:
      virtual ~DuplicateSystem() {}
      virtual void insert(const Data &d) = 0;
      virtual Data *searchByID(const string &id) = 0;
      virtual Data *searchByName(const string &name) = 0;
      virtual Data *searchByIDAndName(const string &id, const string &name) = 0;
      virtual void deleteByID(const string &id) = 0;
      virtual void detectDuplicateByContent() = 0;
      virtual void detectDuplicateByMetadata() = 0;
      virtual void detectDuplicateByFullData() = 0;
      virtual void printAll() = 0;
      virtual void printDuplicates() = 0;
      virtual int count() = 0;
};

template <typename SystemType, typename NodeType>
class SystemAdapter : public DuplicateSystem
{
private:
      SystemType system;

      Data *dataFromNode(NodeType *node)
      {
            if (node == nullptr)
                  return nullptr;
            return &node->data;
      }

public:
      void insert(const Data &d)
      {
            system.insert(d);
      }

      Data *searchByID(const string &id)
      {
            return dataFromNode(system.searchByID(id));
      }

      Data *searchByName(const string &name)
      {
            return dataFromNode(system.searchByName(name));
      }

      Data *searchByIDAndName(const string &id, const string &name)
      {
            return dataFromNode(system.searchByIDAndName(id, name));
      }

      void deleteByID(const string &id)
      {
            system.deleteByID(id);
      }

      void detectDuplicateByContent()
      {
            system.detectDuplicateByContent();
      }

      void detectDuplicateByMetadata()
      {
            system.detectDuplicateByMetadata();
      }

      void detectDuplicateByFullData()
      {
            system.detectDuplicateByFullData();
      }

      void printAll()
      {
            system.printAll();
      }

      void printDuplicates()
      {
            system.printDuplicates();
      }

      int count()
      {
            return system.count();
      }
};

template <typename Func>
long long measureMs(Func action)
{
      auto start = chrono::high_resolution_clock::now();
      action();
      auto end = chrono::high_resolution_clock::now();
      return chrono::duration_cast<chrono::milliseconds>(end - start).count();
}

bool fileExists(const string &filename)
{
      ifstream file(filename);
      return file.good();
}

string datasetFilename()
{
      if (fileExists(DEFAULT_DATASET_FILE))
            return DEFAULT_DATASET_FILE;
      return FALLBACK_DATASET_FILE;
}

string structureName(StructureType type)
{
      if (type == StructureType::LinkedList)
            return "Linked List";
      if (type == StructureType::HashTable)
            return "Hash Table";
      return "AVL Tree";
}

unique_ptr<DuplicateSystem> createDataStructure(StructureType type)
{
      if (type == StructureType::LinkedList)
            return unique_ptr<DuplicateSystem>(new SystemAdapter<LinkedList, Node>());
      if (type == StructureType::HashTable)
            return unique_ptr<DuplicateSystem>(new SystemAdapter<HashSystem, HashNode>());
      return unique_ptr<DuplicateSystem>(new SystemAdapter<AVLSystem, AVLNode>());
}

int readInt(const string &prompt)
{
      int value;
      while (true)
      {
            cout << prompt;
            if (cin >> value)
                  return value;

            cout << "Input harus berupa angka." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
      }
}

int readPositiveInt(const string &prompt)
{
      while (true)
      {
            int value = readInt(prompt);
            if (value > 0)
                  return value;
            cout << "Jumlah data harus lebih dari 0." << endl;
      }
}

string readText(const string &prompt)
{
      string value;
      cout << prompt;
      getline(cin >> ws, value);
      return value;
}

Data inputData()
{
      Data d;
      d.id = readText("ID: ");
      d.name = readText("Name: ");
      d.size = readInt("Size (KB): ");
      d.upload_date = readText("Upload date (yyyy-mm-dd): ");
      d.source = readText("Source: ");
      d.content = readText("Content: ");
      return d;
}

void printData(const Data &d)
{
      cout << d.id << " | " << d.name << " | " << d.size << " | "
           << d.upload_date << " | " << d.source << " | " << d.content << endl;
}

int chooseDataSize()
{
      return readPositiveInt("Masukkan jumlah data yang ingin digunakan: ");
}

StructureType chooseDataStructure()
{
      while (true)
      {
            cout << endl;
            cout << "Pilih struktur data:" << endl;
            cout << "1. Linked List" << endl;
            cout << "2. Hash Table" << endl;
            cout << "3. AVL Tree" << endl;

            int choice = readInt("Choose option: ");
            if (choice >= 1 && choice <= 3)
                  return static_cast<StructureType>(choice);

            cout << "Pilihan struktur data tidak valid." << endl;
      }
}

void resetBenchmarkFile()
{
      mkdir("output", 0777);

      ofstream out(BENCHMARK_FILE, ios::trunc);
      if (!out.is_open())
      {
            cout << "Gagal membuat file benchmark." << endl;
            return;
      }

      out << BENCHMARK_HEADER << endl;
      out.close();
}

vector<string> splitCSVLine(const string &line)
{
      vector<string> columns;
      string column;
      stringstream ss(line);

      while (getline(ss, column, ','))
      {
            columns.push_back(column);
      }

      return columns;
}

bool parseBenchmarkRow(const string &line, BenchmarkStats &row)
{
      vector<string> columns = splitCSVLine(line);
      if (columns.size() != 8)
            return false;

      try
      {
            row.dataSize = stoi(columns[0]);
            row.dataStructure = columns[1];
            row.insertTimeMs = stoll(columns[2]);
            row.searchTimeMs = stoll(columns[3]);
            row.updateTimeMs = stoll(columns[4]);
            row.deleteTimeMs = stoll(columns[5]);
            row.duplicateDetectionTimeMs = stoll(columns[6]);
            row.showTimeMs = stoll(columns[7]);
      }
      catch (...)
      {
            return false;
      }

      return true;
}

vector<BenchmarkStats> readBenchmarkRows()
{
      vector<BenchmarkStats> rows;
      ifstream in(BENCHMARK_FILE);

      if (!in.is_open())
            return rows;

      string line;
      getline(in, line);

      while (getline(in, line))
      {
            if (line.empty())
                  continue;

            BenchmarkStats row;
            if (parseBenchmarkRow(line, row))
                  rows.push_back(row);
      }

      in.close();
      return rows;
}

void writeBenchmarkRows(const vector<BenchmarkStats> &rows)
{
      ofstream out(BENCHMARK_FILE, ios::trunc);
      if (!out.is_open())
      {
            cout << "Gagal menulis file benchmark." << endl;
            return;
      }

      out << BENCHMARK_HEADER << endl;
      for (const BenchmarkStats &row : rows)
      {
            out << row.dataSize << ","
                << row.dataStructure << ","
                << row.insertTimeMs << ","
                << row.searchTimeMs << ","
                << row.updateTimeMs << ","
                << row.deleteTimeMs << ","
                << row.duplicateDetectionTimeMs << ","
                << row.showTimeMs << endl;
      }

      out.close();
}

void upsertBenchmarkRow(const BenchmarkStats &stats)
{
      vector<BenchmarkStats> rows = readBenchmarkRows();
      bool updated = false;

      for (BenchmarkStats &row : rows)
      {
            if (row.dataSize == stats.dataSize && row.dataStructure == stats.dataStructure)
            {
                  row = stats;
                  updated = true;
                  break;
            }
      }

      if (!updated)
            rows.push_back(stats);

      writeBenchmarkRows(rows);
}

void printBenchmarkSeparator()
{
      cout << "+-----------+---------------+-----------+-----------+-----------+-----------+------------+---------+" << endl;
}

void displayBenchmarkTable()
{
      vector<BenchmarkStats> rows = readBenchmarkRows();

      cout << endl;
      cout << "Benchmark Statistics" << endl;
      printBenchmarkSeparator();
      cout << "| " << left << setw(9) << "data_size"
           << " | " << setw(13) << "structure"
           << " | " << right << setw(9) << "insert"
           << " | " << setw(9) << "search"
           << " | " << setw(9) << "update"
           << " | " << setw(9) << "delete"
           << " | " << setw(10) << "duplicate"
           << " | " << setw(7) << "show"
           << " |" << endl;
      printBenchmarkSeparator();

      if (rows.empty())
      {
            cout << "| " << left << setw(99) << "Belum ada data benchmark."
                 << " |" << endl;
            printBenchmarkSeparator();
            return;
      }

      for (const BenchmarkStats &row : rows)
      {
            cout << "| " << right << setw(9) << row.dataSize
                 << " | " << left << setw(13) << row.dataStructure
                 << " | " << right << setw(9) << row.insertTimeMs
                 << " | " << setw(9) << row.searchTimeMs
                 << " | " << setw(9) << row.updateTimeMs
                 << " | " << setw(9) << row.deleteTimeMs
                 << " | " << setw(10) << row.duplicateDetectionTimeMs
                 << " | " << setw(7) << row.showTimeMs
                 << " |" << endl;
      }

      printBenchmarkSeparator();
}

void resetOperationTimes(BenchmarkStats &stats)
{
      stats.searchTimeMs = 0;
      stats.updateTimeMs = 0;
      stats.deleteTimeMs = 0;
      stats.duplicateDetectionTimeMs = 0;
      stats.showTimeMs = 0;
}

void reloadData(unique_ptr<DuplicateSystem> &system,
                vector<Data> &dataset,
                const string &filename,
                int limit,
                StructureType structureType,
                BenchmarkStats &stats)
{
      dataset = readCSV(filename, limit);
      system = createDataStructure(structureType);
      stats.dataStructure = structureName(structureType);
      stats.dataSize = static_cast<int>(dataset.size());
      resetOperationTimes(stats);

      stats.insertTimeMs = measureMs([&]()
                                     {
                                           for (const Data &d : dataset)
                                           {
                                                 system->insert(d);
                                           }
                                     });

      cout << "Loaded " << dataset.size() << " data using " << stats.dataStructure << "." << endl;
      cout << "Initial insert time: " << stats.insertTimeMs << " ms" << endl;
}

void rebuildCurrentData(unique_ptr<DuplicateSystem> &system,
                        const vector<Data> &dataset,
                        StructureType structureType,
                        BenchmarkStats &stats)
{
      system = createDataStructure(structureType);
      stats.dataStructure = structureName(structureType);
      stats.dataSize = static_cast<int>(dataset.size());
      resetOperationTimes(stats);

      stats.insertTimeMs = measureMs([&]()
                                     {
                                           for (const Data &d : dataset)
                                           {
                                                 system->insert(d);
                                           }
                                     });

      cout << "Data structure changed to " << stats.dataStructure << "." << endl;
      cout << "Reload insert time: " << stats.insertTimeMs << " ms" << endl;
}

void eraseRecordByID(vector<Data> &dataset, const string &id)
{
      for (vector<Data>::iterator it = dataset.begin(); it != dataset.end(); ++it)
      {
            if (it->id == id)
            {
                  dataset.erase(it);
                  return;
            }
      }
}

void updateRecordByID(vector<Data> &dataset, const string &id, const Data &newData)
{
      for (Data &d : dataset)
      {
            if (d.id == id)
            {
                  d = newData;
                  return;
            }
      }
}

void syncBenchmarkContext(BenchmarkStats &stats, DuplicateSystem *system, StructureType structureType)
{
      stats.dataSize = system->count();
      stats.dataStructure = structureName(structureType);
}

void showMenu(StructureType structureType, int dataSize)
{
      cout << endl;
      cout << "===== DUPLICATE DETECTION SYSTEM =====" << endl;
      cout << "[Structure: " << structureName(structureType) << "]" << endl;
      cout << "[Data Size: " << dataSize << "]" << endl;
      cout << "1. Insert Data" << endl;
      cout << "2. Search Data" << endl;
      cout << "3. Detect Duplicate" << endl;
      cout << "4. Show Data" << endl;
      cout << "5. Delete Data" << endl;
      cout << "6. Update Data" << endl;
      cout << "7. Show Statistics" << endl;
      cout << "8. Change Data Size" << endl;
      cout << "9. Change Data Structure" << endl;
      cout << "10. Exit" << endl;
}

int main()
{
      resetBenchmarkFile();

      string filename = datasetFilename();
      if (!fileExists(filename))
      {
            cout << "Dataset tidak ditemukan. Pastikan file dataset/dataset.csv tersedia." << endl;
            return 1;
      }

      int requestedDataSize = chooseDataSize();
      StructureType activeStructure = chooseDataStructure();

      unique_ptr<DuplicateSystem> system;
      vector<Data> currentDataset;
      BenchmarkStats stats;

      reloadData(system, currentDataset, filename, requestedDataSize, activeStructure, stats);

      while (true)
      {
            showMenu(activeStructure, system->count());
            int choice = readInt("Choose menu: ");

            switch (choice)
            {
            case 1:
            {
                  Data d = inputData();
                  stats.insertTimeMs = measureMs([&]()
                                                 { system->insert(d); });
                  currentDataset.push_back(d);
                  syncBenchmarkContext(stats, system.get(), activeStructure);
                  cout << "Data inserted successfully!" << endl;
                  break;
            }
            case 2:
            {
                  int option;
                  cout << endl;
                  cout << "SEARCH DATA" << endl;
                  cout << "1. Search by ID" << endl;
                  cout << "2. Search by Name" << endl;
                  cout << "3. Search by ID and Name" << endl;
                  option = readInt("Choose option: ");

                  Data *result = nullptr;
                  if (option == 1)
                  {
                        string id = readText("Enter ID: ");
                        stats.searchTimeMs = measureMs([&]()
                                                       { result = system->searchByID(id); });
                  }
                  else if (option == 2)
                  {
                        string name = readText("Enter name: ");
                        stats.searchTimeMs = measureMs([&]()
                                                       { result = system->searchByName(name); });
                  }
                  else if (option == 3)
                  {
                        string id = readText("Enter ID: ");
                        string name = readText("Enter Name: ");
                        stats.searchTimeMs = measureMs([&]()
                                                       { result = system->searchByIDAndName(id, name); });
                  }
                  else
                  {
                        cout << "Invalid search option!" << endl;
                        break;
                  }

                  if (result != nullptr)
                        printData(*result);
                  else
                        cout << "Data not found" << endl;
                  break;
            }
            case 3:
            {
                  int option;
                  cout << endl;
                  cout << "DETECT DUPLICATE" << endl;
                  cout << "1. Based on Content" << endl;
                  cout << "2. Based on Metadata (Name + Size)" << endl;
                  cout << "3. Based on Full Data" << endl;
                  option = readInt("Choose option: ");

                  if (option == 1)
                  {
                        stats.duplicateDetectionTimeMs = measureMs([&]()
                                                                   { system->detectDuplicateByContent(); });
                  }
                  else if (option == 2)
                  {
                        stats.duplicateDetectionTimeMs = measureMs([&]()
                                                                   { system->detectDuplicateByMetadata(); });
                  }
                  else if (option == 3)
                  {
                        stats.duplicateDetectionTimeMs = measureMs([&]()
                                                                   { system->detectDuplicateByFullData(); });
                  }
                  else
                  {
                        cout << "Invalid duplicate detection option!" << endl;
                        break;
                  }

                  cout << "Duplicate detection completed!" << endl;
                  break;
            }
            case 4:
            {
                  int option;
                  cout << endl;
                  cout << "SHOW DATA" << endl;
                  cout << "1. Show All Data" << endl;
                  cout << "2. Show Duplicate Data" << endl;
                  option = readInt("Choose option: ");

                  if (option == 1)
                  {
                        stats.showTimeMs = measureMs([&]()
                                                     { system->printAll(); });
                  }
                  else if (option == 2)
                  {
                        stats.showTimeMs = measureMs([&]()
                                                     { system->printDuplicates(); });
                  }
                  else
                  {
                        cout << "Invalid show option!" << endl;
                  }
                  break;
            }
            case 5:
            {
                  string id = readText("Enter ID to delete: ");
                  int before = system->count();
                  stats.deleteTimeMs = measureMs([&]()
                                                 { system->deleteByID(id); });

                  if (system->count() < before)
                  {
                        eraseRecordByID(currentDataset, id);
                        syncBenchmarkContext(stats, system.get(), activeStructure);
                  }
                  break;
            }
            case 6:
            {
                  string oldID = readText("Enter ID to update: ");
                  Data *existing = system->searchByID(oldID);

                  if (existing == nullptr)
                  {
                        cout << "Data not found" << endl;
                        break;
                  }

                  cout << "Current data: ";
                  printData(*existing);
                  cout << "Enter new data" << endl;
                  Data newData = inputData();

                  stats.updateTimeMs = measureMs([&]()
                                                 {
                                                       if (newData.id == oldID)
                                                       {
                                                             Data *target = system->searchByID(oldID);
                                                             if (target != nullptr)
                                                                   *target = newData;
                                                       }
                                                       else
                                                       {
                                                             system->deleteByID(oldID);
                                                             system->insert(newData);
                                                       }
                                                 });

                  updateRecordByID(currentDataset, oldID, newData);
                  cout << "Data updated successfully!" << endl;
                  break;
            }
            case 7:
            {
                  syncBenchmarkContext(stats, system.get(), activeStructure);
                  upsertBenchmarkRow(stats);
                  displayBenchmarkTable();
                  break;
            }
            case 8:
            {
                  requestedDataSize = chooseDataSize();
                  reloadData(system, currentDataset, filename, requestedDataSize, activeStructure, stats);
                  break;
            }
            case 9:
            {
                  activeStructure = chooseDataStructure();
                  rebuildCurrentData(system, currentDataset, activeStructure, stats);
                  break;
            }
            case 10:
            {
                  cout << "Program Ended" << endl;
                  return 0;
            }
            default:
            {
                  cout << "Invalid menu!" << endl;
                  break;
            }
            }
      }

      return 0;
}
