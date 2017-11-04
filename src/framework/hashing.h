/* ***********************************************
 * Hash functions:
 * This file define several hash functions as
 * classes.
 *
 * If DEBUG is defined it asserts that initialization
 * is done properly.
 * ***********************************************/

#ifndef _HASHING_H_
#define _HASHING_H_

#include <vector>
#include <cstdint>

#ifdef DEBUG
#include <cassert>
#endif

// TODO: If you have a seed of random bytes (from e.g. random.org) you can use
// the randomgen file instead to provide random numbers.
#include <random>  // TODO: Replace with: #include "randomgen.h"

/* ***********************************************
 * Multiply-shift hashing a la Dietzfelbinger
 * ***********************************************/
class multishift
{
#ifdef DEBUG
    bool hasInit;
#endif
    uint64_t m_a, m_b;

public:
    multishift();
    uint32_t operator()(uint32_t x);
    void init();
};

multishift::multishift()
{
#ifdef DEBUG
    hasInit=false;
#endif
}

void multishift::init()
{
    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<uint64_t> dist;
    m_a = dist(rng);
    m_b = dist(rng);
    // TODO: Replace with the lines below if using randomgen.h
    //m_a = getRandomUInt64();
    //m_b = getRandomUInt64();
#ifdef DEBUG
    hasInit=true;
#endif
}

uint32_t multishift::operator()(uint32_t x)
{
#ifdef DEBUG
    assert(hasInit);
#endif
    return (m_a * (uint64_t)x + m_b) >> 32;
}

/* ***************************************************
 * Poly hashing with 3-independence. Specialized
 * ***************************************************/

class polyhash3
{
#ifdef DEBUG
    bool hasInit;
#endif
    uint64_t m_a, m_b, m_c;
    // Large mersenne prime (2^61 - 1)
    const uint64_t m_p = 2305843009213693951;

public:
    polyhash3();
    void init(); 
    uint32_t operator()(uint32_t x);
};

polyhash3::polyhash3()
{
#ifdef DEBUG
    hasInit=false;
#endif
}

void polyhash3::init()
{
    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<uint64_t> dist;
    m_a = dist(rng);
    m_b = dist(rng);
    m_c = dist(rng);
    // TODO: Replace with the lines below if using randomgen.h
    //m_a = getRandomUInt64();
    //m_b = getRandomUInt64();
    //m_c = getRandomUInt64();
}

uint32_t polyhash3::operator()(uint32_t x)
{
#ifdef DEBUG
    assert(hasInit);
#endif
    __int128 h = m_a * x + m_b;
    h = (h & m_p) + (h >> 61);
    h = h*x + m_c;
    h = (h & m_p) + (h >> 61);
    h = (h & m_p) + (h >> 61);
    return (uint32_t)h;
}


/* ***************************************************
 * Poly hashing with 2-independence. Specialized
 * ***************************************************/

class polyhash2
{
#ifdef DEBUG
    bool hasInit;
#endif
    uint64_t m_a, m_b;
    // Large mersenne prime (2^61 - 1)
    const uint64_t m_p = 2305843009213693951;

public:
    polyhash2();
    void init(); 
    uint32_t operator()(uint32_t x);
};

polyhash2::polyhash2()
{
#ifdef DEBUG
    hasInit=false;
#endif
}

void polyhash2::init()
{
    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<uint64_t> dist;
    m_a = dist(rng);
    m_b = dist(rng);
    // TODO: Replace with below lines if using randomgen.h
    //m_a = getRandomUInt64();
    //m_b = getRandomUInt64();
}

uint32_t polyhash2::operator()(uint32_t x)
{
#ifdef DEBUG
    assert(hasInit);
#endif
    __int128 h = m_a * x + m_b;
    h = (h & m_p) + (h >> 61);
    h = (h & m_p) + (h >> 61);
    return (uint32_t)h;
}



/* ***************************************************
 * Poly hashing with large degree a la Carter & Wegman
 * ***************************************************/

class polyhash
{
#ifdef DEBUG
    bool hasInit;
#endif
    uint32_t m_deg;
    std::vector<uint64_t> m_coef;
    // Large mersenne prime (2^61 - 1)
    const uint64_t m_p = 2305843009213693951;

public:
    polyhash();
    void init(); // 2-indep
    void init(uint32_t deg);
    uint32_t operator()(uint32_t x);
};

polyhash::polyhash()
{
#ifdef DEBUG
    hasInit=false;
#endif
}

void polyhash::init()
{
    init(2);
}

void polyhash::init(uint32_t deg)
{
    // TODO: Remove the following 3 lines if using randomgen.h
    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<uint64_t> dist;

    m_deg = deg;
    m_coef.resize(m_deg,0);
    for (uint32_t i = 0; i < m_deg; ++i) {
		do {
            // TODO: Swap the two lines below if using randomgen.h
			//m_coef[i] = getRandomUInt64() >> 3;
			m_coef[i] = (dist(rng)) >> 3;
#ifdef DEBUG
			assert(m_coef[i] <= m_p); //Since poly_p = 2^61-1.
#endif
		} while(m_coef[i] >= m_p);
    }
#ifdef DEBUG
    hasInit=true;
#endif
}

uint32_t polyhash::operator()(uint32_t x)
{
#ifdef DEBUG
    assert(hasInit);
#endif
    __int128 h = 0;
    for (int32_t i = m_deg-1; i >= 0; --i) {
		h = h * x + m_coef[i];
		h = (h & m_p) + (h >> 61);
    }
	h = (h & m_p) + (h >> 61);
    return (uint32_t)h;
}

/* ***************************************************
 * Mixed Tabulation a la Dahlgaard et al.
 * ***************************************************/

class mixedtab
{
#ifdef DEBUG
    bool hasInit;
#endif
    // Use 4 characters + 4 derived characters.
    // TODO: Parameterize
    uint64_t mt_T1[256][4];
    uint32_t mt_T2[256][4];

public:
    mixedtab();
    void init();
    uint32_t operator()(uint32_t x);
};

mixedtab::mixedtab()
{
#ifdef DEBUG
    hasInit = false;
#endif
}

void mixedtab::init()
{
    // Use a degree-20 polynomial to fill out the entries.
    polyhash h;
    h.init(20);

	uint32_t x = 0;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 256; ++j) {
            mt_T1[j][i] = h(x++);
            mt_T1[j][i] <<= 32;
            mt_T1[j][i] += h(x++);
            mt_T2[j][i] = h(x++);
        }
    }
#ifdef DEBUG
    hasInit = true;
#endif
}

uint32_t mixedtab::operator()(uint32_t x)
{
#ifdef DEBUG
    assert(hasInit);
#endif
    uint64_t h=0; // Final hash value
    for (int i = 0; i < 4; ++i, x >>= 8)
        h ^= mt_T1[(uint8_t)x][i];
    uint32_t drv=h >> 32;
    for (int i = 0; i < 4; ++i, drv >>= 8)
        h ^= mt_T2[(uint8_t)drv][i];
    return (uint32_t)h;
}


/* ***************************************************
 * Simple Tabulation
 * ***************************************************/

class simpletab
{
    uint32_t m_T[256][4];

public:
    simpletab();
    void init();
    uint32_t operator()(uint32_t x);
};

simpletab::simpletab()
{
}

void simpletab::init()
{
    // Use a degree-20 polynomial to fill out the entries.
    polyhash h;
    h.init(20);

	uint32_t x = 0;
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 256; ++j)
            m_T[j][i] = h(x++);
}

uint32_t simpletab::operator()(uint32_t x)
{
    uint32_t h=0; // Final hash value
    for (int i = 0; i < 4; ++i, x >>= 8)
        h ^= m_T[(uint8_t)x][i];
    return h;
}

/* ***************************************************
 * Twisted Tabulation
 * ***************************************************/

class twisttab
{
    // Use 4 characters + 4 derived characters.
    uint64_t mt_T1[256][4];

public:
    twisttab();
    void init();
    uint32_t operator()(uint32_t x);
};

twisttab::twisttab()
{
}

void twisttab::init()
{
    // Use a degree-20 polynomial to fill out the entries.
    polyhash h;
    h.init(20);

	uint32_t x = 0;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 256; ++j) {
            mt_T1[j][i] = h(x++);
            mt_T1[j][i] <<= 32;
            mt_T1[j][i] += h(x++);
        }
    }
}

uint32_t twisttab::operator()(uint32_t x)
{
    uint64_t h=0; // Final hash value
    for (int i = 0; i < 3; ++i, x >>= 8)
        h ^= mt_T1[(uint8_t)x][i];
    uint32_t drv=h >> 32;
    h ^= mt_T1[(uint8_t)(x^drv)][3];
    return (uint32_t)h;
}

#endif // _HASHING_H_
