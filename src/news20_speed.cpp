#include <algorithm>
#include <string>
#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>

#include <chrono>
#include <ctime>

#include "framework/sketches.h"
#include "framework/hashing.h"
#include "framework/hashing_more.h"

typedef pair<uint32_t, double> pid;

using namespace std;

const string strFile = "data/news20-fast.txt";

void readData(vector<vector<pid>>& data)
{
    data.resize(0);
    
    ifstream in(strFile.c_str());
    uint32_t x;
    in >> x; // read N

    vector<pid> cur; // To store the current data point
    uint32_t cnt;
    uint32_t item;

    // Each line is c entry_1, ..., entry_c
    while (in >> cnt) {
        double d = 1.0/sqrt(cnt); // Each element has equal weight
        for (uint32_t i = 0; i < cnt; ++i) {
            in >> item;
            cur.push_back(make_pair(item,d));
        }
        data.push_back(cur);
        cur.resize(0);
    }
    in.close();
}

template <class T>
void testInner(const vector<vector<pid>>& data, string name)
{
    vector<double> sk;
    clock_t start, end;
    f_hash<T> fh(128);
    
    start = clock();
    for (auto it = data.begin(); it != data.end(); ++it) {
        fh.sketch(*it, sk);
    }
    end = clock();
    cout << name << " & " << (float)(end-start)/(CLOCKS_PER_SEC/1000) << "ms \\\\" << endl;
}

void testSketches(const vector<vector<pid>>& data)
{
    testInner<multishift>(data, "Multiply-shift");
    testInner<mixedtab>(data, "Mixed Tabulation");
    testInner<polyhash2>(data, "2-wise PolyHash");
    testInner<polyhash3>(data, "3-wise PolyHash");

    vector<double> sk;
    clock_t start, end;
    f_hash<polyhash> fh(128,20);
    
    start = clock();
    for (auto it = data.begin(); it != data.end(); ++it) {
        fh.sketch(*it, sk);
    }
    end = clock();
    cout << "20-wise PolyHash" << " & " << (float)(end-start)/(CLOCKS_PER_SEC/1000) << "ms \\\\" << endl;


    testInner<murmurwrap>(data, "MurmurHash3");
    testInner<citywrap>(data, "CityHash");
    testInner<blake2wrap>(data, "Blake2");
}

int main()
{
    vector<vector<pid>> data;
    cout << "Reading input: " << endl;
    readData(data);
    cout << "Performing speed test: " << endl << endl;
    testSketches(data);
}
