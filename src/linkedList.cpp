#include <iostream>
#include <fstream>
#include "../include/linkedList.h"
#include <iomanip>

using namespace std;

LinkedList::LinkedList()
{
      head = NULL;
      duplicateMode = 0;
}

void LinkedList::insert(Data d)
{
      Node *newNode = new Node;

      newNode->data = d;
      newNode->isDuplicate = false;
      newNode->next = head;

      head = newNode;
}

Node *LinkedList::searchByID(string id)
{
      Node *curr = head;

      while (curr != NULL)
      {
            if (curr->data.id == id)
            {
                  return curr;
            }

            curr = curr->next;
      }

      return NULL;
}

Node *LinkedList::searchByName(string name)
{
      Node *curr = head;

      while (curr != NULL)
      {
            if (curr->data.name == name)
            {
                  return curr;
            }

            curr = curr->next;
      }

      return NULL;
}

Node *LinkedList::searchByIDAndName(string id, string name)
{
      Node *curr = head;

      while (curr != NULL)
      {
            if (curr->data.id == id && curr->data.name == name)
            {
                  return curr;
            }
            curr = curr->next;
      }

      return NULL;
}

bool LinkedList::existsContent(string content)
{
      Node *curr = head;

      while (curr != NULL)
      {
            if (curr->data.content == content)
            {
                  return true;
            }

            curr = curr->next;
      }

      return false;
}

void LinkedList::resetDuplicate()
{
      Node *curr = head;

      while (curr != NULL)
      {
            curr->isDuplicate = false;
            curr = curr->next;
      }
}

void LinkedList::detectDuplicateByContent()
{
      duplicateMode = 1;
      resetDuplicate();
      ofstream out("output/result.txt");

      Node *curr1 = head;
      int group = 1;

      while (curr1 != NULL)
      {
            if (!curr1->isDuplicate)
            {
                  Node *curr2 = curr1->next;
                  bool found = false;

                  while (curr2 != NULL)
                  {
                        if (curr1->data.content == curr2->data.content)
                        {
                              if (!found)
                              {
                                    out << "Duplicate Group " << group++ << " (Content)\n" << endl;
                                    out << curr1->data.id << " | " << curr1->data.name << " | " << curr1->data.content << endl;

                                    curr1->isDuplicate = true;
                                    found = true;
                              }

                              out << curr2->data.id << " | " << curr2->data.name << " | " << curr2->data.content << endl;

                              curr2->isDuplicate = true;
                        }
                        curr2 = curr2->next;
                  }
                  if (found)
                        out << "\n";
            }
            curr1 = curr1->next;
      }
      out.close();
      cout << "Duplicate detection finished. Result saved to output/result.txt\n";
}

void LinkedList::detectDuplicateByMetadata()
{
      duplicateMode = 2;
      resetDuplicate();

      ofstream out("output/result.txt", ios::trunc);

      Node *curr1 = head;
      int group = 1;

      while (curr1 != NULL)
      {
            if (!curr1->isDuplicate)
            {
                  Node *curr2 = curr1->next;
                  bool found = false;

                  while (curr2 != NULL)
                  {
                        if (curr1->data.name == curr2->data.name && curr1->data.size == curr2->data.size)
                        {
                              if (!found)
                              {
                                    out << "Duplicate Group " << group++ << " (Metadata)\n\n";
                                    out << curr1->data.id << " | " << curr1->data.name << " | " << curr1->data.size << endl;

                                    curr1->isDuplicate = true;
                                    found = true;
                              }
                              out << curr2->data.id << " | " << curr2->data.name << " | " << curr2->data.size << endl;

                              curr2->isDuplicate = true;
                        }
                        curr2 = curr2->next;
                  }
                  if (found)
                        out << "\n";
            }
            curr1 = curr1->next;
      }

      out.close();
}

void LinkedList::detectDuplicateByFullData()
{
      duplicateMode = 3;
      resetDuplicate();

      ofstream out("output/result.txt", ios::trunc);

      Node *curr1 = head;

      int group = 1;

      while (curr1 != NULL)
      {
            if (!curr1->isDuplicate)
            {
                  Node *curr2 = curr1->next;

                  bool found = false;

                  while (curr2 != NULL)
                  {
                        if (curr1->data.id == curr2->data.id && curr1->data.name == curr2->data.name && curr1->data.size == curr2->data.size && curr1->data.upload_date == curr2->data.upload_date && curr1->data.source == curr2->data.source && curr1->data.content == curr2->data.content)
                        {
                              if (!found)
                              {
                                    out << "Duplicate Group " << group++ << " (Full Data)\n\n";

                                    out << curr1->data.id << " | " << curr1->data.name << " | " << curr1->data.size << " | " << curr1->data.upload_date << " | " << curr1->data.source << " | " << curr1->data.content << endl;

                                    curr1->isDuplicate = true;
                                    found = true;
                              }
                              out << curr2->data.id << " | " << curr2->data.name << " | " << curr2->data.size << " | " << curr2->data.upload_date << " | " << curr2->data.source << " | " << curr2->data.content << endl;
                              curr2->isDuplicate = true;
                        }
                        curr2 = curr2->next;
                  }
                  if (found)
                        out << "\n";
            }
            curr1 = curr1->next;
      }
      out.close();
}

string truncateText(string text, int width)
{
      if (text.length() <= width)
      {
            return text;
      }

      return text.substr(0, width - 3) + "...";
}

void LinkedList::printAll()
{
      vector<Node *> rows;
      Node *curr = head;

      while (curr != NULL)
      {
            rows.push_back(curr);
            curr = curr->next;
      }

      if (rows.empty())
      {
            cout << "No data available." << endl;
            return;
      }

      const int pageSize = 20;
      int page = 0;
      int total = rows.size();

      while (true)
      {
            int start = page * pageSize;
            if (start >= total)
            {
                  break;
            }

            int end = start + pageSize;
            if (end > total)
            {
                  end = total;
            }

            cout << "\n+-----------------------------------------------------------------------------------------------------------------------------+" << endl;
            cout << "| #  | ID         | Name                 | Size      | Upload Date  | Source                     | Content                    |" << endl;
            cout << "+-----------------------------------------------------------------------------------------------------------------------------+" << endl;

            for (int i = start; i < end; i++)
            {
                  Data d = rows[i]->data;
                  cout  << "| " << left 
                        << setw(2) << (i + 1) << " | " 
                        << setw(10) << d.id << " | " 
                        << setw(20) << truncateText(d.name, 18) << " | " 
                        << setw(9) << d.size << " | " 
                        << setw(12) << d.upload_date << " | " 
                        << setw(26) << truncateText(d.source, 26) << " | " 
                        << setw(27) << truncateText(d.content, 26) << "|" 
                        << endl;
            }

            cout << "+-----------------------------------------------------------------------------------------------------------------------------+" << endl;
            cout << "Showing data " << (start + 1) << "-" << end << " of " << total << endl;

            if (end >= total)
            {
                  break;
            }

            cout << "Next page? (y/n): ";
            char next;
            cin >> next;

            if (next != 'y' && next != 'Y')
            {
                  break;
            }
            page++;
      }
}

void LinkedList::printDuplicates()
{
      vector<vector<Node *>> groups;
      vector<Node *> processed;

      Node *curr1 = head;
      while (curr1 != NULL)
      {
            if (curr1->isDuplicate)
            {
                  bool alreadyProcessed = false;
                  for (Node *p : processed)
                  {
                        if (p == curr1)
                        {
                              alreadyProcessed = true;
                              break;
                        }
                  }

                  if (!alreadyProcessed)
                  {
                        vector<Node *> group;
                        group.push_back(curr1);
                        processed.push_back(curr1);

                        Node *curr2 = curr1->next;
                        while (curr2 != NULL)
                        {
                              bool match = false;
                              if (curr2->isDuplicate)
                              {
                                    if (duplicateMode == 1)
                                    {
                                          match = (curr1->data.content == curr2->data.content);
                                    }
                                    else if (duplicateMode == 2)
                                    {
                                          match = (curr1->data.name == curr2->data.name && curr1->data.size == curr2->data.size);
                                    }
                                    else if (duplicateMode == 3)
                                    {
                                          match = (curr1->data.id == curr2->data.id && curr1->data.name == curr2->data.name && curr1->data.size == curr2->data.size && curr1->data.upload_date == curr2->data.upload_date && curr1->data.source == curr2->data.source && curr1->data.content == curr2->data.content);
                                    }

                                    if (match)
                                    {
                                          group.push_back(curr2);
                                          processed.push_back(curr2);
                                    }
                              }
                              curr2 = curr2->next;
                        }

                        if (group.size() > 1)
                        {
                              groups.push_back(group);
                        }
                  }
            }
            curr1 = curr1->next;
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
            {
                  cout << " (Content)";
            }
            else if (duplicateMode == 2)
            {
                  cout << " (Metadata)";
            }
            else if (duplicateMode == 3)
            {
                  cout << " (Full Data)";
            }
            cout << endl;

            cout << "\n+-----------------------------------------------------------------------------------------------------------------------------+" << endl;
            cout << "| #  | ID         | Name                 | Size      | Upload Date  | Source                     | Content                    |" << endl;
            cout << "+-----------------------------------------------------------------------------------------------------------------------------+" << endl;

            for (size_t i = 0; i < groups[g].size(); i++)
            {
                  Data d = groups[g][i]->data;
                  cout  << "| " << left
                        << setw(2) << (shown + 1) << " | "
                        << setw(10) << d.id << " | "
                        << setw(20) << truncateText(d.name, 20) << " | "
                        << setw(9) << d.size << " | "
                        << setw(12) << d.upload_date << " | "
                        << setw(26) << truncateText(d.source, 26) << " | "
                        << setw(27) << truncateText(d.content, 26) << "|"
                        << endl;

                  shown++;
                  if (shown % pageSize == 0)
                  {
                        cout << "+-----------------------------------------------------------------------------------------------------------------------------+" << endl;
                        cout << "Showing duplicate data " << shown << " so far. Next page? (y/n): ";
                        char next;
                        cin >> next;
                        if (next != 'y' && next != 'Y')
                        {
                              return;
                        }
                        cout << "\n+-----------------------------------------------------------------------------------------------------------------------------+" << endl;
                        cout << "| #  | ID         | Name                 | Size      | Upload Date  | Source                     | Content                    |" << endl;
                        cout << "+-----------------------------------------------------------------------------------------------------------------------------+" << endl;
                  }
            }

            cout << "+-----------------------------------------------------------------------------------------------------------------------------+" << endl;
      }
}

void LinkedList::showStatistics(long insertTime, long searchTime, long deleteTime, long showTime, long detectTime)
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

void LinkedList::deleteByID(string id)
{
      Node *curr = head;
      Node *prev = NULL;

      while (curr != NULL)
      {
            if (curr->data.id == id)
            {
                  if (prev == NULL)
                  {
                        head = curr->next;
                  }
                  else
                  {
                        prev->next = curr->next;
                  }

                  delete curr;

                  cout << "Data deleted" << endl;

                  return;
            }

            prev = curr;
            curr = curr->next;
      }

      cout << "Data not found" << endl;
}

int LinkedList::count()
{

      int total = 0;
      Node *curr = head;

      while (curr != NULL)
      {
            total++;
            curr = curr->next;
      }

      return total;
}

int LinkedList::countDuplicate()
{
      int total = 0;
      Node *curr = head;

      while (curr != NULL)
      {
            if (curr->isDuplicate)
            {
                  total++;
            }

            curr = curr->next;
      }

      return total;
}