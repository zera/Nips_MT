#include <fstream>
#include <iostream>

#include <algorithm>
#include <cmath>

#include "framework/hashing.h"
#include "framework/hashing_more.h"
#include "framework/sketches.h"
#include "datasets.h"

using namespace std;

typedef pair<uint32_t, double> pid;

// Dot product of vectors represented as (idx,val) pairs that are sorted by
// idx.
double dotprod(const vector<pid>& A, const vector<pid>& B)
{
    // Assume A and B are sorted.
    double res = 0.0;
    for (uint32_t i = 0, j = 0; i != A.size() && j != B.size();)
    {
        if (A[i].first == B[j].first)
            res += (A[i++].second * B[j++].second);
        else if (A[i].first > B[j].first)
            ++j;
        else
            ++i;
    }
    return res;
}

/* Test code for similarity estimation with different hash functions
 * */
void testNorm(uint32_t sdSize, uint32_t intSize,
        uint32_t trials, uint32_t k)
{
    // Create sets for estimation
    vector<pid> A,B;
    intSize = genRealSets(A,B,sdSize,intSize,0.5);
//    intSize = genSplitRealSets(A,B,sdSize,intSize);

    cout << "Generated sets with the following properties: " << endl;
    cout << "<A,A> = " << dotprod(A,A) << endl;
    cout << "<A,B> = " << dotprod(A,B) << endl;
    cout << "<B,B> = " << dotprod(B,B) << endl;

    vector<double> results_ms, results_mt, results_poly, results_poly20, results_mur;

    // Run the trials with multiply-shift
    for (uint32_t i = 0; i < trials; ++i) {
        f_hash<multishift> s_ms(k);
        f_hash<mixedtab> s_mt(k);
        f_hash<polyhash2> s_poly(k);
        f_hash<murmurwrap> s_mur(k);
        f_hash<polyhash> s_poly20(k,20);

        vector<double> A_ms, A_mt, A_p, A_p20, A_mur;
        s_ms.sketch(A, A_ms);
        s_mt.sketch(A, A_mt);
        s_poly.sketch(A, A_p);
        s_mur.sketch(A, A_mur);
        s_poly20.sketch(A, A_p20);
        results_ms.push_back(s_ms.dotprod(A_ms, A_ms));
        results_mt.push_back(s_mt.dotprod(A_mt, A_mt));
        results_poly.push_back(s_poly.dotprod(A_p, A_p));
        results_mur.push_back(s_mur.dotprod(A_mur, A_mur));
        results_poly20.push_back(s_poly20.dotprod(A_p20, A_p20));
    }
    
    sort(results_ms.begin(), results_ms.end());
    sort(results_mt.begin(), results_mt.end());
    sort(results_poly.begin(), results_poly.end());
    sort(results_mur.begin(), results_mur.end());
    sort(results_poly20.begin(), results_poly20.end());

    // Output results
    ofstream fout;
    double avg = 0.0;
    fout.open("output/fhash_ms.txt");
    for (auto it = results_ms.begin(); it != results_ms.end(); ++it) {
        fout << *it << endl;
        avg += *it;
    }
    fout.close();
    cout << "Average <A',A'> for Multiply-Shift: " << avg/(double)trials << endl;

    fout.open("output/fhash_mt.txt");
    avg = 0.0;
    for (auto it = results_mt.begin(); it != results_mt.end(); ++it) {
        fout << *it << endl;
        avg += *it;
    }
    fout.close();
    cout << "Average <A',A'> for Mixed tab: " << avg/(double)trials << endl;

    fout.open("output/fhash_poly20.txt");
    avg = 0.0;
    for (auto it = results_poly20.begin(); it != results_poly20.end(); ++it) {
        fout << *it << endl;
        avg += *it;
    }
    fout.close();
    cout << "Average <A',A'> for Polyhash (20-indep): " << avg/(double)trials << endl;

    fout.open("output/fhash_mur.txt");
    avg = 0.0;
    for (auto it = results_mur.begin(); it != results_mur.end(); ++it) {
        fout << *it << endl;
        avg += *it;
    }
    fout.close();
    cout << "Average <A',A'> for Murmur: " << avg/(double)trials << endl;

    fout.open("output/fhash_poly.txt");
    avg = 0.0;
    for (auto it = results_poly.begin(); it != results_poly.end(); ++it) {
        fout << *it << endl;
        avg += *it;
    }
    fout.close();
    cout << "Average <A',A'> for Polyhash (2-indep): " << avg/(double)trials << endl;
}

int main()
{
    testNorm(2000,2000,2000,500);
}
