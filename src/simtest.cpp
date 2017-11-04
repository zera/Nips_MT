#include <fstream>
#include <iostream>

#include <algorithm>

#include "framework/hashing.h"
#include "framework/hashing_more.h"
#include "framework/sketches.h"
#include "datasets.h"

/* Test code for similarity estimation with different hash functions
 * */
void testSimilarity(uint32_t sdSize, uint32_t intSize,
        uint32_t trials, uint32_t k)
{
    // Create sets for estimation
    vector<uint32_t> A,B;
    intSize = genBinarySets(A,B,sdSize,intSize,0.5);

    // Experiment results
    double ground_truth = (double)intSize/(double)(intSize + sdSize);
    vector<double> results_ms, results_mt, results_poly, results_poly20, results_mur;

    // Run the trials with multiply-shift
    for (uint32_t i = 0; i < trials; ++i) {
        k_partition<multishift> sketch(k);

        vector<uint32_t> Ak, Bk;
        sketch.sketch(A, Ak);
        sketch.sketch(B, Bk);
        results_ms.push_back(sketch.estimate(Ak,Bk));
    }
    
    // Run the trials with mixed tabulation
    for (uint32_t i = 0; i < trials; ++i) {
        k_partition<mixedtab> sketch(k);

        // Create sketches
        vector<uint32_t> Ak, Bk;
        sketch.sketch(A,Ak);
        sketch.sketch(B,Bk);
        results_mt.push_back(sketch.estimate(Ak,Bk));
    }

    // Run the trials with 20-wise polynomial hashing
    for (uint32_t i = 0; i < trials; ++i) {
        k_partition<polyhash> sketch(k,20);

        // Create sketches
        vector<uint32_t> Ak, Bk;
        sketch.sketch(A,Ak);
        sketch.sketch(B,Bk);
        results_poly20.push_back(sketch.estimate(Ak,Bk));
    }

    // Run the trials with 2-wise polynomial hashing
    for (uint32_t i = 0; i < trials; ++i) {
        k_partition<polyhash2> sketch(k);

        // Create sketches
        vector<uint32_t> Ak, Bk;
        sketch.sketch(A,Ak);
        sketch.sketch(B,Bk);
        results_poly.push_back(sketch.estimate(Ak,Bk));
    }

    // Run the trials with 2-wise polynomial hashing
    for (uint32_t i = 0; i < trials; ++i) {
        k_partition<murmurwrap> sketch(k);

        // Create sketches
        vector<uint32_t> Ak, Bk;
        sketch.sketch(A,Ak);
        sketch.sketch(B,Bk);
        results_mur.push_back(sketch.estimate(Ak,Bk));
    }


    cout << "Ran trials on sets with actual similarity: " << ground_truth << endl;
    // Output the results sorted for convenience
    sort(results_ms.begin(), results_ms.end());
    ofstream fout;
    double err = 0.0;
    fout.open("output/sim_ms.txt");
    for (auto it = results_ms.begin(); it != results_ms.end(); ++it) {
        fout << *it << endl;
        err += abs(*it - ground_truth);
    }
    fout.close();
    cout << "Average error for Multiply-Shift: " << err/(double)trials << endl;

    sort(results_mt.begin(), results_mt.end());
    fout.open("output/sim_mt.txt");
    err = 0.0;
    for (auto it = results_mt.begin(); it != results_mt.end(); ++it) {
        fout << *it << endl;
        err += abs(*it - ground_truth);
    }
    fout.close();
    cout << "Average error for Mixed tab: " << err/(double)trials << endl;

    sort(results_poly.begin(), results_poly.end());
    fout.open("output/sim_poly.txt");
    err = 0.0;
    for (auto it = results_poly.begin(); it != results_poly.end(); ++it) {
        fout << *it << endl;
        err += abs(*it - ground_truth);
    }
    fout.close();
    cout << "Average error for polyhash 2: " << err/(double)trials << endl;

    sort(results_poly20.begin(), results_poly20.end());
    fout.open("output/sim_poly20.txt");
    err = 0.0;
    for (auto it = results_poly20.begin(); it != results_poly20.end(); ++it) {
        fout << *it << endl;
        err += abs(*it - ground_truth);
    }
    fout.close();
    cout << "Average error for polyhash 20: " << err/(double)trials << endl;

    sort(results_mur.begin(), results_mur.end());
    fout.open("output/sim_mur.txt");
    err = 0.0;
    for (auto it = results_mur.begin(); it != results_mur.end(); ++it) {
        fout << *it << endl;
        err += abs(*it - ground_truth);
    }
    fout.close();
    cout << "Average error for MurmurHash: " << err/(double)trials << endl;
}

int main()
{
    testSimilarity(100,100,2000,200);
}
