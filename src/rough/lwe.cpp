// lwe.cpp
// Single-file LWE toy implementation (C++17)
// Compile: g++ -std=c++17 lwe.cpp -O2 -o lwe
// Run: ./lwe
//
// Implements the LWE scheme as in the provided screenshots:
//  - secret S in {0,1}^k
//  - public one-time vector A in Z_q^k
//  - ciphertext (A, b) where b = A·S + Delta*m + e (mod q)
//  - decryption: recover m by rounding (b - A·S)/Delta mod t
//
// Note: Noise sampled uniformly from [-B, B]. For correct decryption, ensure B < Delta/2.

#include <bits/stdc++.h>
using namespace std;
using int64 = long long;

// Utility: positive mod in range [0, q-1]
int64 modq(int64 x, int64 q) {
    int64 r = x % q;
    if (r < 0) r += q;
    return r;
}

// Dot product (mod q)
int64 dot_modq(const vector<int64>& a, const vector<int64>& b, int64 q) {
    if (a.size() != b.size()) throw runtime_error("dot: size mismatch");
    __int128 acc = 0;
    for (size_t i = 0; i < a.size(); ++i) {
        acc += (__int128)a[i] * (__int128)b[i];
    }
    // reduce to int64 mod q
    int64 acc64 = (int64)(acc % q);
    if (acc64 < 0) acc64 += q;
    return acc64;
}

// Key structures
struct SecretKey {
    vector<int64> S; // binary vector length k
};

struct PublicKey {
    vector<int64> A; // random vector length k in Z_q
};

struct Ciphertext {
    vector<int64> A; // same A used (we store it)
    int64 b;         // b in Z_q
};

// RNG utilities
std::mt19937_64 rng((uint64_t)chrono::high_resolution_clock::now().time_since_epoch().count());

vector<int64> sample_uniform_vector(size_t k, int64 q) {
    vector<int64> v(k);
    uniform_int_distribution<int64> ud(0, q-1);
    for (size_t i = 0; i < k; ++i) v[i] = ud(rng);
    return v;
}

vector<int64> sample_binary_secret(size_t k) {
    vector<int64> s(k);
    uniform_int_distribution<int> ud(0, 1);
    for (size_t i = 0; i < k; ++i) s[i] = ud(rng);
    return s;
}

// sample noise uniform in [-B, B]
int64 sample_noise(int64 B) {
    if (B <= 0) return 0;
    uniform_int_distribution<int64> ud(-B, B);
    return ud(rng);
}

// Keygen: produce secret key (binary S). Public key is not a fixed global here because A is
// chosen as a one-time public vector per encryption in the textbook description.
SecretKey keygen(size_t k) {
    SecretKey sk;
    sk.S = sample_binary_secret(k);
    return sk;
}

// Encrypt message m in Z_t using secret key sk and parameters (q, t, Delta, noiseBound B).
// Produces a ciphertext (A, b) where A is sampled uniformly in Z_q^k.
Ciphertext encrypt(int64 m, const SecretKey& sk, int64 q, int64 t, int64 Delta, int64 noiseBound) {
    size_t k = sk.S.size();
    if (m < 0 || m >= t) throw runtime_error("plaintext m out of range");
    Ciphertext ct;
    ct.A = sample_uniform_vector(k, q);
    int64 inner = dot_modq(ct.A, sk.S, q); // A·S (mod q)
    // Compute Delta*m + e (as integer, not mod q yet)
    int64 scaled_m = Delta * m;
    int64 e = sample_noise(noiseBound);
    // compute b = inner + scaled_m + e (mod q)
    int64 sum = inner;
    // sum may be large, so use int128 style accumulation via builtin
    __int128 s128 = (__int128)inner + (__int128)scaled_m + (__int128)e;
    int64 s64 = (int64)(s128 % q);
    if (s64 < 0) s64 += q;
    ct.b = s64;
    return ct;
}

// Decrypt ciphertext ct using secret key sk and parameters (q, t, Delta).
// Returns recovered message m_hat in [0, t-1].
// This uses rounding to nearest integer of (b - A·S)/Delta (taking correct representative modulo q).
int64 decrypt(const Ciphertext& ct, const SecretKey& sk, int64 q, int64 t, int64 Delta) {
    size_t k = sk.S.size();
    if (ct.A.size() != k) throw runtime_error("ciphertext A length mismatch with secret key");
    // compute b - A·S (mod q)
    int64 inner = dot_modq(ct.A, sk.S, q);
    int64 diff = modq((int64)ct.b - inner, q); // value in [0, q-1]
    // choose centered representative in (-q/2, q/2]
    int64 halfq = q / 2;
    int64 centered = diff;
    if (centered > halfq) centered -= q; // now in (-q/2, q/2]
    // Now centered = Delta*m + e as an integer (assuming noise small)
    // Recover m_hat = round(centered / Delta) mod t
    // use double for rounding; Delta fits in int64
    double val = (double)centered / (double)Delta;
    int64 m_hat = (int64)llround(val); // nearest integer
    // reduce modulo t to be in [0, t-1]
    m_hat %= t;
    if (m_hat < 0) m_hat += t;
    return m_hat;
}

// =========================
// Demo / small test harness
// =========================
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Example parameters (you can change these)
    // k: dimension of secret vector
    // q: ciphertext modulus
    // t: message modulus (plaintext space 0..t-1)
    // Delta = q / t (must be integer)
    size_t k = 256;
    int64 q = 1LL << 30; // large q (approx 1e9). Must be >= t*something.
    int64 t = 16;        // plaintext space size
    if (q % t != 0) {
        cerr << "require q % t == 0. adjust parameters\n";
        return 1;
    }
    int64 Delta = q / t;

    // Choose noise bound B (we'll use Delta/4 by default)
    int64 noiseBound = max<int64>(1, Delta / 4);

    cout << "LWE toy demo\n";
    cout << "parameters: k=" << k << " q=" << q << " t=" << t << " Delta=" << Delta << " noiseBound=" << noiseBound << "\n";

    // Keygen
    SecretKey sk = keygen(k);
    cout << "Secret key generated (binary vector length " << k << ")\n";

    // Test encrypt/decrypt for some sample messages
    vector<int64> test_msgs = {0, 1, 2, 3, 5, 7, 10, 15};
    int successes = 0;
    for (int64 m : test_msgs) {
        Ciphertext ct = encrypt(m, sk, q, t, Delta, noiseBound);
        int64 rec = decrypt(ct, sk, q, t, Delta);
        bool ok = (rec == m);
        cout << "m=" << m << " -> decrypt=" << rec << " " << (ok ? "OK" : "FAIL") << "\n";
        if (ok) successes++;
    }
    cout << successes << " / " << test_msgs.size() << " messages recovered correctly.\n";

    // Quick randomized test
    int trials = 50;
    int okcount = 0;
    uniform_int_distribution<int64> udmsg(0, t-1);
    for (int i = 0; i < trials; ++i) {
        int64 m = udmsg(rng);
        Ciphertext ct = encrypt(m, sk, q, t, Delta, noiseBound);
        int64 r = decrypt(ct, sk, q, t, Delta);
        if (r == m) okcount++;
    }
    cout << "Randomized test: " << okcount << " / " << trials << " successful\n";

    cout << "\nNotes:\n";
    cout << "- If noiseBound >= Delta/2, decryption will likely fail.\n";
    cout << "- To use discrete Gaussian noise, replace sample_noise() accordingly.\n";
    cout << "- For real cryptographic use, parameter selection and sampling distribution must be secure.\n";

    return 0;
}

