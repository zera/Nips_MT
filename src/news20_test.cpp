#include <algorithm>
#include <string>
#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>

#include "framework/sketches.h"
#include "framework/hashing.h"
#include "framework/hashing_more.h"

typedef pair<uint32_t, double> pid;

using namespace std;

const string strFile = "data/news20-fast.txt";

void testSketches(vector<double>& skMS, vector<double>& skMT, vector<double>& skPOLY, vector<double>& skP20, vector<double>& skMur)
{
    f_hash<mixedtab> fh_mt(256);
    f_hash<multishift> fh_ms(256);
    f_hash<polyhash> fh_poly(256);
    f_hash<murmurwrap> fh_mur(256);
    f_hash<polyhash> fh_p20(256,20);

    skMS.resize(0);
    skMT.resize(0);
    skPOLY.resize(0);
    skP20.resize(0);
    skMur.resize(0);

    ifstream in(strFile.c_str());
    uint32_t x;
    in >> x; // read N

    vector<pid> cur; // To store the current data point
    // Each line is c entry_1, ..., entry_c
    uint32_t cnt;
    uint32_t item;
    while (in >> cnt) {
        double d = 1.0/sqrt(cnt); // Each element has equal weight
        double norm = 0.0;
        for (uint32_t i = 0; i < cnt; ++i) {
            in >> item;
            cur.push_back(make_pair(item,d));
            norm += d*d;
        }
        // We Read an entire item. Now create a sketch
        vector<double> S_MS, S_MT, S_POLY, S_P20, S_MUR;
        fh_mt.sketch(cur, S_MT);
        fh_ms.sketch(cur, S_MS);
        fh_poly.sketch(cur, S_POLY);
        fh_mur.sketch(cur, S_MUR);
        fh_p20.sketch(cur, S_P20);
        // Calculate the dot product and push to results
        skMS.push_back(fh_ms.dotprod(S_MS,S_MS));
        skMT.push_back(fh_mt.dotprod(S_MT,S_MT));
        skPOLY.push_back(fh_poly.dotprod(S_POLY,S_POLY));
        skP20.push_back(fh_p20.dotprod(S_P20,S_P20));
        skMur.push_back(fh_mur.dotprod(S_MUR,S_MUR));
        // Reset the current data point
        cur.resize(0);
    }
    in.close();
}

void printResults(vector<double>& res, string file)
{
    sort(res.begin(), res.end());
    ofstream fout;
    fout.open(file.c_str());
    for (auto it = res.begin(); it != res.end(); ++it)
        fout << *it << endl;
    fout.close();
}

int main()
{
    vector<double> res_ms, res_mt, res_poly, res_p20, res_mur;
    vector<double> res_ms2, res_mt2, res_poly2, tmp_p20, tmp_mur;

    // Perform the test
    for (int i = 0 ; i < 100; ++i) {
        cout << "Running experiment " << i << endl;
        testSketches(res_ms2, res_mt2, res_poly2, tmp_p20, tmp_mur);
        res_ms.insert(res_ms.begin(), res_ms2.begin(), res_ms2.end());
        res_mt.insert(res_mt.begin(), res_mt2.begin(), res_mt2.end());
        res_poly.insert(res_poly.begin(), res_poly2.begin(), res_poly2.end());
        res_p20.insert(res_p20.end(), tmp_p20.begin(), tmp_p20.end());
        res_mur.insert(res_mur.end(), tmp_mur.begin(), tmp_mur.end());
    }

    // Store the results
    printResults(res_ms, "output/news20_ms.txt");
    printResults(res_mt, "output/news20_mt.txt");
    printResults(res_poly, "output/news20_poly.txt");
    printResults(res_p20, "output/news20_p20.txt");
    printResults(res_mur, "output/news20_mur.txt");

    cout << "Experiment done." << endl;
}
