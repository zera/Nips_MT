/* *********************************************************
 * Different similarity sketches, etc.
 * *********************************************************/

#ifndef _SKETCHES_H_
#define _SKETCHES_H_

#include <vector>
#include <cstdint>
#include <limits>
#include <cassert>
#include <queue>

// TODO: If you have a file with random bytes from e.g. random.org, place it
// in the seed folder and use randomgen.h instead.
#include <random>  // TODO: Replace with #include "randomgen.h"
#include "hashing.h"


using namespace std;


/* *******************************************************
 * k-partition a la one permutation of Li et al.
 * *******************************************************/

template <class F>
class k_partition
{
    uint32_t m_k;
    vector<uint32_t> m_copy; // Shrivastava&Li left/right densification

    F h; // The hash function to be used.

    void sketch_core(const vector<uint32_t>& input, vector<uint32_t>& output);

    public:
    k_partition();
    k_partition(uint32_t k);
    k_partition(uint32_t k, uint32_t hparam);

    void sketch(const vector<uint32_t>& input, vector<uint32_t>& output);
    void bbit_sketch(const vector<uint32_t>& input, vector<uint32_t>& output, uint32_t b);

    double estimate(const vector<uint32_t>& A, const vector<uint32_t>& B);

};

template <class F>
k_partition<F>::k_partition()
{
    k_partition(200); // default value
}

template <class F>
k_partition<F>::k_partition(uint32_t k)
{
    // TODO: Remove below 3 lines if using randomgen.h
    mt19937 rng;
    rng.seed(random_device()());
    uniform_int_distribution<uint32_t> dist(0,1);

    m_k = k;
    m_copy.resize(m_k, 0);
    for (int i = 0; i < m_k; ++i)
        m_copy[i] = dist(rng);
        // TODO: Replace with line below if using randomgen.h
        //m_copy[i] = getRandomBool();

    h.init(); // Initialize the hash function
}

template <class F>
k_partition<F>::k_partition(uint32_t k, uint32_t hparam)
{
    // TODO: Remove below 3 lines if using randomgen.h
    mt19937 rng;
    rng.seed(random_device()());
    uniform_int_distribution<uint32_t> dist(0,1);

    m_k = k;
    m_copy.resize(m_k, 0);
    for (int i = 0; i < m_k; ++i)
        m_copy[i] = dist(rng);
        // TODO: Replace with line below if using randomgen.h
        //m_copy[i] = getRandomBool();

    h.init(hparam); // Initialize the hash function
}

// The actual k-partition part.
template <class F>
void k_partition<F>::sketch_core(const vector<uint32_t>& input, vector<uint32_t>& output)
{
    output.resize(m_k, -1); // Prepare k-partition sketch (initialize to max)
    // Note that -1 = max value is too large to be an actual value
    for (auto it = input.begin(); it != input.end(); ++it) {
        uint32_t v = h(*it);
        uint32_t bin = v % m_k;
        uint32_t val = v / m_k;
        output[bin] = min(output[bin],val);
    }
}

// Call the core and do densification
template <class F>
void k_partition<F>::sketch(const vector<uint32_t>& input, vector<uint32_t>& output)
{
    sketch_core(input, output);

    uint32_t thr = numeric_limits<uint32_t>::max() / m_k + 1;

    uint32_t sl, sr;
    uint32_t jl = 0, jr = 0;
    for (int i = m_k-1; i >= 0; --i) {
        ++jl;
        if (output[i] < thr) {
            sl = output[i];
            break;
        }
    }
    for (int i = 0; i < m_k; ++i) {
        ++jr;
        if (output[i] < thr) {
            sr = output[i];
            break;
        }
    }

    for (int i = 0; i < m_k; ++i) {
        ++jl;
        if (output[i] < thr) {
            sl = output[i];
            jl = 0;
        }
        if (output[i] >= thr && m_copy[i] == 0)
            output[i] = sl + jl*thr;
    }
    for (int i = m_k-1; i >= 0; --i) {
        ++jr;
        if (output[i] < thr) {
            sr = output[i];
            jr = 0;
        }
        if (output[i] >= thr && m_copy[i] == 1)
            output[i] = sr + jr * thr;
    }
}

template <class F>
void k_partition<F>::bbit_sketch(const vector<uint32_t>&input, vector<uint32_t>& output, uint32_t b)
{
    sketch(input, output);

    uint32_t mask = (1 << b) - 1;
    for (auto it = output.begin(); it != output.end(); ++it)
        *it &= mask;
}

template <class F>
double k_partition<F>::estimate(const vector<uint32_t>& A, const vector<uint32_t>& B)
{
    assert(A.size() == B.size());
    assert(A.size() == m_k);

    uint32_t match = 0;
    for (uint32_t i = 0; i < m_k; ++i)
        match += (A[i] == B[i]);
    return (double)match/(double)m_k;
}

/* *******************************************************************
 * Feature hashing (Weinberger et al.) -- similar to countsketch
 * Inputs to this sketch are high-dimensional vectors represented as
 * (index, value)-pairs.
 * *******************************************************************/

template <class F>
class f_hash
{
    uint32_t m_d;

    F h1;
    F h2; // The hash functions to be used.

    public:
    f_hash();
    f_hash(uint32_t d);
    f_hash(uint32_t d, uint32_t hparam);

    void sketch(const vector<pair<uint32_t,double>>& input, vector<double>& output);
    double dotprod(const vector<double>& A, const vector<double>& B);
};

template <class F>
f_hash<F>::f_hash()
{
    f_hash(100);
}

template <class F>
f_hash<F>::f_hash(uint32_t d)
{
    m_d = d;
    h1.init();
    h2.init();
}

template <class F>
f_hash<F>::f_hash(uint32_t d, uint32_t hparam)
{
    m_d = d;
    h1.init(hparam);
    h2.init(hparam);
}

template <class F>
void f_hash<F>::sketch(const vector<pair<uint32_t,double>>&input, vector<double>& output)
{
    // We assumption is that the input is a set (i.e. no weighted elements!)
    output.resize(m_d,0.0);
    for (auto it = input.begin(); it != input.end(); ++it) {
        uint32_t idx = it->first;
        double val = it->second;
        uint32_t bin = h1(idx) % m_d;
        int32_t sgn = h2(idx) % 2;
        output[bin] += (double)(sgn*2 - 1) * val; // {0,1} -> {-1,1}
    }
}

template <class F>
double f_hash<F>::dotprod(const vector<double>& A, const vector<double>& B)
{
    assert(A.size() == B.size());
    assert(A.size() == m_d);
    double res = 0.0;
    for (uint32_t i = 0; i < m_d; ++i)
        res += (A[i] * B[i]);
    return res;
}


/* *******************************************************************
 * Bottom-k hashing
 * *******************************************************************/

template <class F>
class bottom_k
{
    uint32_t m_k;

    F h; // The hash function to be used.

    public:
    bottom_k();
    bottom_k(uint32_t k);

    void sketch(const vector<uint32_t>& input, vector<uint32_t>& output);

    double estimate(const vector<uint32_t>& A, const vector<uint32_t>& B);
};

template <class F>
bottom_k<F>::bottom_k()
{
    bottom_k(200); // default value
}

template <class F>
bottom_k<F>::bottom_k(uint32_t k)
{
    m_k = k;

    h.init(); // Initialize the hash function
}

// Create the bottom-k sketch. We assume that the input set has at least k
// elements. Otherwise the sketch is not good.
template <class F>
void bottom_k<F>::sketch(const vector<uint32_t>& input, vector<uint32_t>& output)
{
    assert(input.size() >= m_k);

    priority_queue<uint32_t> pq;

    for (auto it = input.begin(); it != input.end(); ++it) {
        uint32_t v = h(*it);
        if (pq.size() < m_k)
            pq.push(v);
        else if (pq.top() > v) {
            pq.pop();
            pq.push(v);
        }
    }

    // Create the sketch in sorted order.
    output.resize(m_k);
    for (int i = m_k-1; i >= 0; --i) {
        output[i] = pq.top();
        pq.pop();
    }
}

template <class F>
double bottom_k<F>::estimate(const vector<uint32_t>& A, const vector<uint32_t>& B)
{
    assert(A.size() == B.size());
    assert(A.size() == m_k);

    // Count intersection among k smallest in total
    uint32_t cInt = 0;
    uint32_t seen = 0;
    for (auto it1 = A.begin(), it2 = B.begin(); it1 != A.end(), it2 != B.end();) {
        if (*it1 == *it2) {
            ++cInt;
            ++it1;
            ++it2;
        }
        else if (*it1 < *it2)
            ++it1;
        else
            ++it2;
        if (++seen == m_k)
            break;
    }
    return (double)cInt/(double)m_k;
}



#endif // _SKETCHES_H_
