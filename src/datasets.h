/* ***************************************************************
 * Code for generating synthetic data sets.
 * ***************************************************************/

#include <vector>
#include <cmath>

typedef pair<uint32_t, double> pid;

using namespace std;

/* Generate sets for similarity estimation detection.
 * The two sets are created as follows:
 * - Intersection is a dense subset of expected size intSize from [1,...]
 * - Symmetric Difference has size sdSize and consists of random numbers
 * The density of the subset is specified by the frac parameter.
 * */
uint32_t genBinarySets(vector<uint32_t>& A, vector<uint32_t>& B,
        uint32_t sdSize, uint32_t intSize, double frac)
{
    // Use C++ random for symmetric difference. It just needs to be somewhat
    // random.
    mt19937 rng;
    rng.seed(random_device()());
    uniform_int_distribution<uint32_t> dist;
    uniform_real_distribution<double> rdist(0.0,1.0);

    // Reset the set vectors
    A.resize(0);
    B.resize(0);

    // Generate symmetric difference separately
    for (uint32_t i = 0; i < sdSize/2; ++i)
        A.push_back(dist(rng));
    for (uint32_t i = 0; i < sdSize/2; ++i)
        B.push_back(dist(rng));

    uint32_t cnt=0;
    // Add intersection to the sets
    for (uint32_t i = 0; i < ceil((double)intSize/frac); ++i) {
        if (rdist(rng) < frac) {
            ++cnt;
            A.push_back(i);
            B.push_back(i);
        }
    }

    // Sort the sets for good measure
    sort(A.begin(), A.end());
    sort(B.begin(), B.end());
    A.erase(unique(A.begin(), A.end()), A.end());
    B.erase(unique(B.begin(), B.end()), B.end());

    return cnt;
}
    
uint32_t genRealSets(vector<pid>& A, vector<pid>& B,
        uint32_t sdSize, uint32_t intSize, double frac)
{
    A.resize(0);
    B.resize(0);

    vector<uint32_t> AA, BB;
    uint32_t cnt = genBinarySets(AA, BB, sdSize, intSize, frac);
    // Normalize vector lengths
    double dASize = sqrt(1.0/(double)AA.size());
    double dBSize = sqrt(1.0/(double)BB.size());
    for (auto it = AA.begin(); it != AA.end(); ++it)
        A.push_back(make_pair(*it, dASize));
    for (auto it = BB.begin(); it != BB.end(); ++it)
        B.push_back(make_pair(*it, dBSize));

    // Return the actual intersection size
    return cnt;
}

uint32_t genSplitSets(vector<uint32_t>& A, vector<uint32_t>& B,
        uint32_t sdSize, uint32_t intSize)
{
    // Use C++ random for symmetric difference. It just needs to be somewhat
    // random.
    uint32_t n = 2*(intSize+sdSize);

    // Reset the set vectors
    A.resize(0);
    B.resize(0);
    
    mt19937 rng;
    rng.seed(random_device()());
    uniform_real_distribution<double> rdist(0.0,1.0);

    uint32_t cnt=0;
    // Generate symmetric difference separately
    for (uint32_t i = 0; i < n; ++i) {
        if (rdist(rng) < 0.5)
            continue;
        if (i < sdSize)
            A.push_back(i);
        else if (i > sdSize + 2*intSize)
            B.push_back(i);
        else {
            ++cnt;
            A.push_back(i);
            B.push_back(i);
        }
    }

    return cnt;
}
 
uint32_t genSplitRealSets(vector<pid>& A, vector<pid>& B,
        uint32_t sdSize, uint32_t intSize)
{
    A.resize(0);
    B.resize(0);

    vector<uint32_t> AA, BB;
    uint32_t cnt = genSplitSets(AA, BB, sdSize, intSize);
    // Normalize vector lengths
    double dASize = sqrt(1.0/(double)AA.size());
    double dBSize = sqrt(1.0/(double)BB.size());
    for (auto it = AA.begin(); it != AA.end(); ++it)
        A.push_back(make_pair(*it, dASize));
    for (auto it = BB.begin(); it != BB.end(); ++it)
        B.push_back(make_pair(*it, dBSize));

    // Return the actual intersection size
    return cnt;
}

