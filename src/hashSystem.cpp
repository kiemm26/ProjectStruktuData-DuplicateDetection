#include "../include/hashSystem.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>

using namespace std;

int HashSystem::hashFunction(const string &key) const
{
      const int BASE = 31;
      long long hash = 0;
      long long power = 1;

      for (char c : key)
      {
            hash = (hash + (c - 'a' + 1) * power) % TABLE_SIZE;
            power = (power * BASE) % TABLE_SIZE;
      }

      // Ensure non-negative index (c is unsigned-char safe, but guard anyway)
      return (int)((hash % TABLE_SIZE + TABLE_SIZE) % TABLE_SIZE);
}

//  CONSTRUCTOR / DESTRUCTOR
HashSystem::HashSystem()
{
      for (int i = 0; i < TABLE_SIZE; i++)
            table[i] = nullptr;

      totalNodes = 0;
      duplicateMode = 0;
}

HashSystem::~HashSystem()
{
      for (int i = 0; i < TABLE_SIZE; i++)
      {
            HashNode *curr = table[i];
            while (curr != nullptr)
            {
                  HashNode *next = curr->next;
                  delete curr;
                  curr = next;
            }
            table[i] = nullptr;
      }
}

//  INSERT
void HashSystem::insert(Data d)
{
      int idx = hashFunction(d.id);

      HashNode *newNode = new HashNode;
      newNode->data = d;
      newNode->isDuplicate = false;
      newNode->next = table[idx]; // prepend (same as LinkedList)
      table[idx] = newNode;

      totalNodes++;
}

//  SEARCH
HashNode *HashSystem::searchByID(const string &id)
{
      int idx = hashFunction(id);
      HashNode *curr = table[idx];

      while (curr != nullptr)
      {
            if (curr->data.id == id)
                  return curr;
            curr = curr->next;
      }
      return nullptr;
}

HashNode *HashSystem::searchByName(const string &name)
{
      // Full-table scan – no secondary index on name
      for (int i = 0; i < TABLE_SIZE; i++)
      {
            HashNode *curr = table[i];
            while (curr != nullptr)
            {
                  if (curr->data.name == name)
                        return curr;
                  curr = curr->next;
            }
      }
      return nullptr;
}

HashNode *HashSystem::searchByIDAndName(const string &id, const string &name)
{
      int idx = hashFunction(id);
      HashNode *curr = table[idx];

      while (curr != nullptr)
      {
            if (curr->data.id == id && curr->data.name == name)
                  return curr;
            curr = curr->next;
      }
      return nullptr;
}

//  DELETE
void HashSystem::deleteByID(const string &id)
{
      int idx = hashFunction(id);
      HashNode *curr = table[idx];
      HashNode *prev = nullptr;

      while (curr != nullptr)
      {
            if (curr->data.id == id)
            {
                  if (prev == nullptr)
                        table[idx] = curr->next;
                  else
                        prev->next = curr->next;

                  delete curr;
                  totalNodes--;

                  cout << "Data deleted" << endl;
                  return;
            }
            prev = curr;
            curr = curr->next;
      }

      cout << "Data not found" << endl;
}

//  HELPER – collect all nodes into a flat vector (preserves insertion order
//  per bucket; bucket order follows hash index)

static vector<HashNode *> collectAll(HashNode **table, int tableSize)
{
      vector<HashNode *> rows;
      for (int i = 0; i < tableSize; i++)
      {
            HashNode *curr = table[i];
            while (curr != nullptr)
            {
                  rows.push_back(curr);
                  curr = curr->next;
            }
      }
      return rows;
}

//  PRINT HELPERS  (identical formatting to LinkedList)
static string truncate(const string &text, int width)
{
      if ((int)text.length() <= width)
            return text;
      return text.substr(0, width - 3) + "...";
}

void HashSystem::printTableHeader() const
{
      cout << "\n+-----------------------------------------------------------------------------------------------------------------------------+" << endl;
      cout << "| #  | ID         | Name                 | Size      | Upload Date  | Source                     | Content                    |" << endl;
      cout << "+-----------------------------------------------------------------------------------------------------------------------------+" << endl;
}

void HashSystem::printRow(int rowNum, const Data &d) const
{
      cout << "| " << left
           << setw(2) << rowNum << " | "
           << setw(10) << truncate(d.id, 10) << " | "
           << setw(20) << truncate(d.name, 18) << " | "
           << setw(9) << d.size << " | "
           << setw(12) << d.upload_date << " | "
           << setw(26) << truncate(d.source, 26) << " | "
           << setw(27) << truncate(d.content, 26) << "|"
           << endl;
}

//  PRINT ALL  – paginated, 20 rows per page
void HashSystem::printAll()
{
      vector<HashNode *> rows = collectAll(table, TABLE_SIZE);

      if (rows.empty())
      {
            cout << "No data available." << endl;
            return;
      }

      const int pageSize = 20;
      int page = 0;
      int total = (int)rows.size();

      while (true)
      {
            int start = page * pageSize;
            if (start >= total)
                  break;

            int end = min(start + pageSize, total);

            printTableHeader();
            for (int i = start; i < end; i++)
                  printRow(i + 1, rows[i]->data);

            cout << "+-----------------------------------------------------------------------------------------------------------------------------+" << endl;
            cout << "Showing data " << (start + 1) << "-" << end << " of " << total << endl;

            if (end >= total)
                  break;

            cout << "Next page? (y/n): ";
            char next;
            cin >> next;
            if (next != 'y' && next != 'Y')
                  break;
            page++;
      }
}

//  RESET DUPLICATE FLAGS
void HashSystem::resetDuplicate()
{
      for (int i = 0; i < TABLE_SIZE; i++)
      {
            HashNode *curr = table[i];
            while (curr != nullptr)
            {
                  curr->isDuplicate = false;
                  curr = curr->next;
            }
      }
}

//  DETECT DUPLICATE – BY CONTENT
void HashSystem::detectDuplicateByContent()
{
      duplicateMode = 1;
      resetDuplicate();

      ofstream out("output/result.txt", ios::trunc);
      if (!out.is_open())
      {
            cout << "Failed to open output file\n";
            return;
      }

      const int CIDX_SIZE = 2003; // nilai prima lebih baik
      struct CNode
      {
            string key;
            vector<HashNode *> nodes;
            CNode *next;
      };
      vector<CNode *> cidx(CIDX_SIZE, nullptr);

      auto cidxHash = [&](const string &s) -> int
      {
            const int BASE = 31;
            long long h = 0, p = 1;
            for (char c : s)
            {
                  h = (h + (c - 'a' + 1) * p) % CIDX_SIZE;
                  p = (p * BASE) % CIDX_SIZE;
            }
            return (int)((h % CIDX_SIZE + CIDX_SIZE) % CIDX_SIZE);
      };

      for (int i = 0; i < TABLE_SIZE; i++)
      {
            HashNode *curr = table[i];
            while (curr != nullptr)
            {
                  int ci = cidxHash(curr->data.content);
                  CNode *cn = cidx[ci];
                  bool found = false;
                  while (cn)
                  {
                        if (cn->key == curr->data.content)
                        {
                              cn->nodes.push_back(curr);
                              found = true;
                              break;
                        }
                        cn = cn->next;
                  }
                  if (!found)
                  {
                        CNode *newCN = new CNode{curr->data.content, {curr}, cidx[ci]};
                        cidx[ci] = newCN;
                  }
                  curr = curr->next;
            }
      }

      // Output duplicate groups & mark flags  O(n)
      int group = 1;
      for (int i = 0; i < CIDX_SIZE; i++)
      {
            CNode *cn = cidx[i];
            while (cn)
            {
                  if (cn->nodes.size() > 1)
                  {
                        out << "Duplicate Group " << group++ << " (Content)\n\n";
                        for (HashNode *hn : cn->nodes)
                        {
                              out << hn->data.id << " | " << hn->data.name
                                  << " | " << hn->data.content << "\n";
                              hn->isDuplicate = true;
                        }
                        out << "\n";
                  }
                  CNode *tmp = cn;
                  cn = cn->next;
                  delete tmp;
            }
            cidx[i] = nullptr;
      }

      out.close();
      cout << "Duplicate detection finished. Result saved to output/result.txt\n";
}

//  DETECT DUPLICATE – BY METADATA  (name + size)
void HashSystem::detectDuplicateByMetadata()
{
      duplicateMode = 2;
      resetDuplicate();

      ofstream out("output/result.txt", ios::trunc);
      if (!out.is_open())
      {
            cout << "Failed to open output file\n";
            return;
      }

      const int MIDX_SIZE = 2003;
      struct MNode
      {
            string key;
            vector<HashNode *> nodes;
            MNode *next;
      };
      vector<MNode *> midx(MIDX_SIZE, nullptr);

      auto midxHash = [&](const string &s) -> int
      {
            const int BASE = 31;
            long long h = 0, p = 1;
            for (char c : s)
            {
                  h = (h + (c - 'a' + 1) * p) % MIDX_SIZE;
                  p = (p * BASE) % MIDX_SIZE;
            }
            return (int)((h % MIDX_SIZE + MIDX_SIZE) % MIDX_SIZE);
      };

      for (int i = 0; i < TABLE_SIZE; i++)
      {
            HashNode *curr = table[i];
            while (curr != nullptr)
            {
                  string compositeKey = curr->data.name + "|" + to_string(curr->data.size);
                  int ci = midxHash(compositeKey);
                  MNode *mn = midx[ci];
                  bool found = false;
                  while (mn)
                  {
                        if (mn->key == compositeKey)
                        {
                              mn->nodes.push_back(curr);
                              found = true;
                              break;
                        }
                        mn = mn->next;
                  }
                  if (!found)
                  {
                        MNode *newMN = new MNode{compositeKey, {curr}, midx[ci]};
                        midx[ci] = newMN;
                  }
                  curr = curr->next;
            }
      }

      int group = 1;
      for (int i = 0; i < MIDX_SIZE; i++)
      {
            MNode *mn = midx[i];
            while (mn)
            {
                  if (mn->nodes.size() > 1)
                  {
                        out << "Duplicate Group " << group++ << " (Metadata)\n\n";
                        for (HashNode *hn : mn->nodes)
                        {
                              out << hn->data.id << " | " << hn->data.name
                                  << " | " << hn->data.size << "\n";
                              hn->isDuplicate = true;
                        }
                        out << "\n";
                  }
                  MNode *tmp = mn;
                  mn = mn->next;
                  delete tmp;
            }
            midx[i] = nullptr;
      }

      out.close();
      cout << "Duplicate detection finished. Result saved to output/result.txt\n";
}

//  DETECT DUPLICATE – BY FULL DATA
void HashSystem::detectDuplicateByFullData()
{
      duplicateMode = 3;
      resetDuplicate();

      ofstream out("output/result.txt", ios::trunc);
      if (!out.is_open())
      {
            cout << "Failed to open output file\n";
            return;
      }

      const int FIDX_SIZE = 2003;
      struct FNode
      {
            string key;
            vector<HashNode *> nodes;
            FNode *next;
      };
      vector<FNode *> fidx(FIDX_SIZE, nullptr);

      auto fidxHash = [&](const string &s) -> int
      {
            const int BASE = 31;
            long long h = 0, p = 1;
            for (char c : s)
            {
                  h = (h + (c - 'a' + 1) * p) % FIDX_SIZE;
                  p = (p * BASE) % FIDX_SIZE;
            }
            return (int)((h % FIDX_SIZE + FIDX_SIZE) % FIDX_SIZE);
      };

      // Composite key: all 6 fields joined by "|"
      for (int i = 0; i < TABLE_SIZE; i++)
      {
            HashNode *curr = table[i];
            while (curr != nullptr)
            {
                  const Data &d = curr->data;
                  string compositeKey = d.id + "|" + d.name + "|" +
                                        to_string(d.size) + "|" +
                                        d.upload_date + "|" +
                                        d.source + "|" + d.content;
                  int ci = fidxHash(compositeKey);
                  FNode *fn = fidx[ci];
                  bool found = false;
                  while (fn)
                  {
                        if (fn->key == compositeKey)
                        {
                              fn->nodes.push_back(curr);
                              found = true;
                              break;
                        }
                        fn = fn->next;
                  }
                  if (!found)
                  {
                        FNode *newFN = new FNode{compositeKey, {curr}, fidx[ci]};
                        fidx[ci] = newFN;
                  }
                  curr = curr->next;
            }
      }

      int group = 1;
      for (int i = 0; i < FIDX_SIZE; i++)
      {
            FNode *fn = fidx[i];
            while (fn)
            {
                  if (fn->nodes.size() > 1)
                  {
                        out << "Duplicate Group " << group++ << " (Full Data)\n\n";
                        for (HashNode *hn : fn->nodes)
                        {
                              const Data &d = hn->data;
                              out << d.id << " | " << d.name << " | "
                                  << d.size << " | " << d.upload_date << " | "
                                  << d.source << " | " << d.content << "\n";
                              hn->isDuplicate = true;
                        }
                        out << "\n";
                  }
                  FNode *tmp = fn;
                  fn = fn->next;
                  delete tmp;
            }
            fidx[i] = nullptr;
      }

      out.close();
      cout << "Duplicate detection finished. Result saved to output/result.txt\n";
}

//  PRINT DUPLICATES  – paginated, groups by duplicateMode
void HashSystem::printDuplicates()
{
      if (duplicateMode == 0)
      {
            cout << "No duplicate data found. Run duplicate detection first." << endl;
            return;
      }

      // Collect flagged nodes and group them
      vector<HashNode *> all = collectAll(table, TABLE_SIZE);

      // Build groups: for each unmarked duplicate, find its siblings
      vector<vector<HashNode *>> groups;
      vector<HashNode *> processed;

      for (HashNode *hn : all)
      {
            if (!hn->isDuplicate)
                  continue;

            // Already in a group?
            bool seen = false;
            for (HashNode *p : processed)
                  if (p == hn)
                  {
                        seen = true;
                        break;
                  }
            if (seen)
                  continue;

            vector<HashNode *> group;
            group.push_back(hn);
            processed.push_back(hn);

            for (HashNode *other : all)
            {
                  if (!other->isDuplicate || other == hn)
                        continue;
                  bool alreadyIn = false;
                  for (HashNode *p : processed)
                        if (p == other)
                        {
                              alreadyIn = true;
                              break;
                        }
                  if (alreadyIn)
                        continue;

                  bool match = false;
                  if (duplicateMode == 1)
                        match = (hn->data.content == other->data.content);
                  else if (duplicateMode == 2)
                        match = (hn->data.name == other->data.name &&
                                 hn->data.size == other->data.size);
                  else if (duplicateMode == 3)
                        match = (hn->data.id == other->data.id &&
                                 hn->data.name == other->data.name &&
                                 hn->data.size == other->data.size &&
                                 hn->data.upload_date == other->data.upload_date &&
                                 hn->data.source == other->data.source &&
                                 hn->data.content == other->data.content);

                  if (match)
                  {
                        group.push_back(other);
                        processed.push_back(other);
                  }
            }

            if (group.size() > 1)
                  groups.push_back(group);
      }

      if (groups.empty())
      {
            cout << "No duplicate data found. Run duplicate detection first." << endl;
            return;
      }

      const int pageSize = 20;
      int shown = 0;
      int groupNumber = 1;

      for (size_t g = 0; g < groups.size(); g++)
      {
            cout << "\nDuplicate Group " << groupNumber++;
            if (duplicateMode == 1)
                  cout << " (Content)";
            else if (duplicateMode == 2)
                  cout << " (Metadata)";
            else if (duplicateMode == 3)
                  cout << " (Full Data)";
            cout << endl;

            printTableHeader();

            for (size_t i = 0; i < groups[g].size(); i++)
            {
                  printRow(shown + 1, groups[g][i]->data);
                  shown++;

                  if (shown % pageSize == 0)
                  {
                        cout << "+-----------------------------------------------------------------------------------------------------------------------------+" << endl;
                        cout << "Showing duplicate data " << shown << " so far. Next page? (y/n): ";
                        char next;
                        cin >> next;
                        if (next != 'y' && next != 'Y')
                              return;
                        printTableHeader();
                  }
            }
            cout << "+-----------------------------------------------------------------------------------------------------------------------------+" << endl;
      }
}

//  COUNT HELPERS
int HashSystem::count() const
{
      return totalNodes;
}

int HashSystem::countDuplicate() const
{
      int total = 0;
      for (int i = 0; i < TABLE_SIZE; i++)
      {
            HashNode *curr = table[i];
            while (curr != nullptr)
            {
                  if (curr->isDuplicate)
                        total++;
                  curr = curr->next;
            }
      }
      return total;
}

//  STATISTICS  – identical output format to LinkedList::showStatistics
void HashSystem::showStatistics(long insertTime, long searchTime, long deleteTime, long showTime, long detectTime)
{
      int total = count();
      int duplicate = countDuplicate();

      cout << endl;
      cout << "Total Data : " << total << endl;
      cout << "Duplicate Data : " << duplicate << endl;
      cout << "Unique Data : " << total - duplicate << endl;
      cout << endl;
      cout << "Execution Time Statistics (ms)" << endl;
      cout << "Insert Time : " << insertTime << endl;
      cout << "Search Time : " << searchTime << endl;
      cout << "Delete Time : " << deleteTime << endl;
      cout << "Show Time : " << showTime << endl;
      cout << "Duplicate Detection Time : " << detectTime << endl;
}