#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include "../include/fileHandler.h"

using namespace std;

vector<Data> readCSV(string filename)
{
      vector<Data> dataset;
      ifstream file(filename);

      if (!file.is_open())
      {
            cout << "Failed to open file!" << endl;
            return dataset;
      }

      string line;
      getline(file, line); // skip header

      int limit = 1000;
      int count = 0;
      while (getline(file, line))
      {

            if (count >= limit)
                  break;

            if (line.empty())
                  continue;

            stringstream ss(line);
            string temp;
            Data d;

            getline(ss, d.id, ',');
            getline(ss, d.name, ',');
            getline(ss, temp, ',');

            try
            {
                  d.size = stoi(temp);
            }
            catch (...)
            {
                  d.size = 0;
            }

            getline(ss, d.upload_date, ',');
            getline(ss, d.source, ',');
            getline(ss, d.content, ',');

            dataset.push_back(d);
            count++;
      }
      file.close();
      return dataset;
}