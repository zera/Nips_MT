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

// Code for reading the MNIST data
uint32_t getInt(vector<char>& buffer, uint32_t pos) {
    uint32_t a(0);
    assert(pos+4 <= buffer.size());
    for(uint32_t i = 0;i < 4; ++i) {
        int32_t t = (int32_t)buffer[pos+i];
        if(t < 0) t += (1 << 8);
        a = (a << 8) + t;
    }
    return a;
}

void readImages(string fileName, vector<vector<uint8_t> >& output) {
    ifstream input(fileName, ios::binary|ios::in);
    vector<char> buffer((istreambuf_iterator<char>(input)), (istreambuf_iterator<char>()));
    assert(getInt(buffer,0) == 2051);
    uint32_t n = getInt(buffer,4);
    uint32_t rows = getInt(buffer, 8);
    uint32_t cols = getInt(buffer, 12);
    assert(rows == 28);
    assert(cols == 28);
    assert(n*rows*cols+16 == buffer.size());
    uint32_t size = rows*cols;
    output.resize(n,vector<uint8_t>(size));
    for(uint32_t i = 0;i < n; ++i) {
        for(uint32_t j = 0;j < size; ++j) {
            output[i][j] = buffer[(i*size)+16+j];
        }
    }
    input.close();
}

void imagesToSets(const vector<vector<uint8_t> >& images,
                    vector<vector<uint32_t> >& output, uint32_t n,
                    uint32_t imageSize, uint8_t thr) {
    assert(n == images.size());
    output.resize(n);
    for(uint32_t i = 0;i < n; ++i) {
        assert(images[i].size() == imageSize);
        for(uint32_t j = 0;j < imageSize; ++j) {
            if(images[i][j] >= thr) {
                output[i].push_back(j);
            }
        }
    }
}

void readSets(string fileName, vector<vector<uint32_t> >& output, uint8_t thr)     {
    vector<vector<uint8_t> > images;
    readImages(fileName, images);
    imagesToSets(images, output, images.size(), 28*28, thr);
}
// END: Code for reading MNIST data

const string strFile = "data/MNIST-train-images.idx3-ubyte";
vector<vector<uint32_t>> inputSets;

void testSketches(vector<double>& skMS, vector<double>& skMT, vector<double>& skPOLY, vector<double>& p20, vector<double>& skMur)
{
    f_hash<mixedtab> fh_mt(256);
    f_hash<multishift> fh_ms(256);
    f_hash<polyhash> fh_poly(256);
    f_hash<polyhash> fh_p20(256,20);
    f_hash<murmurwrap> fh_mur(256);

    skMS.resize(0);
    skMT.resize(0);
    skPOLY.resize(0);
    p20.resize(0);
    skMur.resize(0);

    for (auto vec = inputSets.begin(); vec != inputSets.end(); ++vec) {
        vector<pid> point;
        double d = 1.0/sqrt(vec->size());
        double norm = 0.0;
        for (auto it = vec->begin(); it != vec->end(); ++it)
            point.push_back(make_pair(*it, d));
        
        // We Read an entire item. Now create a sketch
        vector<double> S_MS, S_MT, S_POLY, S_P20, S_MUR;
        fh_mt.sketch(point, S_MT);
        fh_ms.sketch(point, S_MS);
        fh_poly.sketch(point, S_POLY);
        fh_p20.sketch(point, S_P20);
        fh_mur.sketch(point, S_MUR);
        // Calculate the dot product and push to results
        skMS.push_back(fh_ms.dotprod(S_MS,S_MS));
        skMT.push_back(fh_mt.dotprod(S_MT,S_MT));
        skPOLY.push_back(fh_poly.dotprod(S_POLY,S_POLY));
        p20.push_back(fh_p20.dotprod(S_P20,S_P20));
        skMur.push_back(fh_mur.dotprod(S_MUR,S_MUR));
        // Reset the current data point
    }
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
    // read the entire mnist dataset.
    readSets(strFile.c_str(), inputSets, 1);

    vector<double> res_ms, res_mt, res_poly, res_p20, res_mur;
    vector<double> res_ms2, res_mt2, res_poly2, res_p202, res_mur2;

    // Perform the test
    for (int i = 0; i < 100; ++i) {
        cout << "Running experiment " << i << endl;
        testSketches(res_ms2, res_mt2, res_poly2, res_p202, res_mur2);
        res_ms.insert(res_ms.end(), res_ms2.begin(), res_ms2.end());
        res_mt.insert(res_mt.end(), res_mt2.begin(), res_mt2.end());
        res_poly.insert(res_poly.end(), res_poly2.begin(), res_poly2.end());
        res_p20.insert(res_p20.end(), res_p202.begin(), res_p202.end());
        res_mur.insert(res_mur.end(), res_mur2.begin(), res_mur2.end());
    }

    // Store the results
    printResults(res_ms, "output/mnist_ms.txt");
    printResults(res_mt, "output/mnist_mt.txt");
    printResults(res_poly, "output/mnist_poly.txt");
    printResults(res_p20, "output/mnist_p20.txt");
    printResults(res_mur, "output/mnist_mur.txt");

    cout << "Experiment done." << endl;
}
