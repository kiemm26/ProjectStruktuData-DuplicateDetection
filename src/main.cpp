#include <iostream>
#include <vector>
#include <chrono>

#include "../include/data.h"
#include "../include/fileHandler.h"

// ── Toggle this define to benchmark either structure ──────────────────────────
#define USE_HASH_SYSTEM // Aktifkan jika mau menggunakan hash table, default linked list

#ifdef USE_HASH_SYSTEM
#include "../include/hashSystem.h"
using DataStructure = HashSystem;
using DataNode = HashNode;
#else
#include "../include/linkedList.h"
using DataStructure = LinkedList;
using DataNode = Node;
#endif

using namespace std;

void showMenu()
{
      cout << endl;
      cout << "===== DUPLICATE DETECTION SYSTEM =====" << endl;
#ifdef USE_HASH_SYSTEM
      cout << "[Structure: Hash Table]" << endl;
#else
      cout << "[Structure: Linked List]" << endl;
#endif
      cout << "1. Insert Data" << endl;
      cout << "2. Search Data" << endl;
      cout << "3. Detect Duplicate" << endl;
      cout << "4. Show Data" << endl;
      cout << "5. Delete Data" << endl;
      cout << "6. Statistics" << endl;
      cout << "7. Exit" << endl;
      cout << "Choose menu: ";
}

int main()
{
      DataStructure ds;

      long insertTime = 0;
      long searchTime = 0;
      long deleteTime = 0;
      long showTime = 0;
      long detectTime = 0;

      vector<Data> dataset = readCSV("public/dataset.csv");
      for (Data d : dataset) ds.insert(d);
            
      int choice;
      while (true)
      {
            showMenu();
            cin >> choice;

            switch (choice)
            {
            case 1:
            {
                  Data d;
                  cout << "ID: ";
                  cin >> d.id;
                  cout << "Name: ";
                  cin >> d.name;
                  cout << "Size (KB): ";
                  cin >> d.size;
                  cout << "Upload date (yyyy-mm-dd): ";
                  cin >> d.upload_date;
                  cout << "Source: ";
                  cin >> d.source;
                  cout << "Content: ";
                  cin >> d.content;

                  auto start = chrono::high_resolution_clock::now();

                  ds.insert(d);

                  auto end = chrono::high_resolution_clock::now();
                  insertTime = chrono::duration_cast<chrono::milliseconds>(end - start).count();

                  cout << "Data inserted successfully!" << endl;
                  break;
            }
            // SEARCH DATA
            case 2:
            {
                  int option;
                  cout << "\nSEARCH DATA\n"
                       << "1. Search by ID\n"
                       << "2. Search by Name\n"
                       << "3. Search by ID and Name\n"
                       << "Choose option: ";
                  cin >> option;

                  auto start = chrono::high_resolution_clock::now();
                  DataNode *result = nullptr;

                  if (option == 1)
                  {
                        string id;
                        cout << "Enter ID: ";
                        cin >> id;
                        result = ds.searchByID(id);
                  }
                  else if (option == 2)
                  {
                        string name;
                        cout << "Enter name: ";
                        cin >> name;
                        result = ds.searchByName(name);
                  }
                  else if (option == 3)
                  {
                        string id, name;
                        cout << "Enter ID: ";
                        cin >> id;
                        cout << "Enter Name: ";
                        cin >> name;
                        result = ds.searchByIDAndName(id, name);
                  }

                  auto end = chrono::high_resolution_clock::now();
                  searchTime = chrono::duration_cast<chrono::milliseconds>(end - start).count();

                  if (result != nullptr)
                        cout << result->data.id << " | " << result->data.name << " | "
                             << result->data.size << " | " << result->data.upload_date << " | "
                             << result->data.source << " | " << result->data.content << endl;
                  else
                        cout << "Data not found" << endl;
                  break;
            }
            // DETECT DUPLICATE
            case 3:
            {
                  int option;
                  cout << "\nDETECT DUPLICATE\n"
                       << "1. Based on Content\n"
                       << "2. Based on Metadata (Name + Size)\n"
                       << "3. Based on Full Data\n"
                       << "Choose option: ";
                  cin >> option;

                  auto start = chrono::high_resolution_clock::now();

                  if (option == 1)
                        ds.detectDuplicateByContent();
                  else if (option == 2)
                        ds.detectDuplicateByMetadata();
                  else if (option == 3)
                        ds.detectDuplicateByFullData();

                  cout << "Duplicate detection completed!" << endl;

                  auto end = chrono::high_resolution_clock::now();
                  detectTime = chrono::duration_cast<chrono::milliseconds>(end - start).count();
                  break;
            }
            // SHOW DATA
            case 4:
            {
                  int option;
                  cout << "\nSHOW DATA\n"
                       << "1. Show All Data\n"
                       << "2. Show Duplicate Data\n"
                       << "Choose option: ";
                  cin >> option;

                  auto start = chrono::high_resolution_clock::now();

                  if (option == 1) ds.printAll();
                  else if (option == 2) ds.printDuplicates();

                  auto end = chrono::high_resolution_clock::now();
                  showTime = chrono::duration_cast<chrono::milliseconds>(end - start).count();
                  break;
            }
            // DELETE DATA
            case 5:
            {
                  string id;
                  cout << "Enter ID to delete: ";
                  cin >> id;

                  auto start = chrono::high_resolution_clock::now();

                  ds.deleteByID(id);

                  auto end = chrono::high_resolution_clock::now();
                  deleteTime = chrono::duration_cast<chrono::milliseconds>(end - start).count();
                  break;
            }
            // STATISTICS
            case 6:
            {
                  ds.showStatistics(insertTime, searchTime, deleteTime, showTime, detectTime);
                  break;
            }
            // EXIT
            case 7:
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