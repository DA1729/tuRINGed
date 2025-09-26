#include <iostream>
#include <vector>
#include <numeric>
#include <random>
#include <chrono>
#include <algorithm>
#include <cmath> // For std::pow, though we'll use iterative multiplication

using namespace std;
using int64 = long long;

// --- Utility and Polynomial Functions (from your code) ---

// Computes x mod q
int64 modq(int64 x, int64 q) {
	int64 r = x % q;
	if (r < 0) {
		r += q;
	}
	return r;
}

// Computes the centered representation of x in (-q/2, q/2]
int64 center_rep(int64 x, int64 q) {
	int64 v = modq(x, q);
	int64 half = q / 2;
	if (v > half) v -= q;
	return v;
}

using poly = vector<int64>;

// Negacyclic polynomial multiplication: a(x) * b(x) mod (x^n + 1)
poly poly_mul_negacyclic(const poly &a, const poly &b, int64 q) {
	size_t n = a.size();
	poly res(n, 0);
	for (size_t i = 0; i < n; i++) {
		for (size_t j = 0; j < n; j++) {
			size_t idx = j + i;
			__int128 prod = (__int128)a[i] * (__int128)b[j];
			if (idx < n) {
				__int128 tmp = (__int128)res[idx] + prod;
				res[idx] = (int64)(tmp % q);
			} else {
				size_t idx2 = idx - n;
				__int128 tmp = (__int128)res[idx2] - prod;
				res[idx2] = (int64)(((tmp % q) + q) % q);
			}
		}
	}
	for (size_t i = 0; i < n; i++) res[i] = modq(res[i], q);
	return res;
}

// Polynomial addition
poly poly_add (const poly &a, const poly &b, int64 q) {
	size_t n = a.size();
	poly r(n);
	for (size_t i = 0; i < n; i++) r[i] = modq(a[i] + b[i], q);
	return r;
}

// Polynomial subtraction
poly poly_sub (const poly &a, const poly &b, int64 q) {
	size_t n = a.size();
	poly r(n);
	for (size_t i = 0; i < n; i++) r[i] = modq(a[i] - b[i], q);
	return r;
}

// Scalar multiplication
poly poly_scalar_mul (const poly &a, int64 scalar, int64 q) {
	size_t n = a.size();
	poly r(n);
	for (size_t i = 0; i < n; i++) {
		__int128 tmp = (__int128)a[i] * scalar;
		r[i] = modq((int64)(tmp % q), q);
	}
	return r;
}

// Global random number generator
std::mt19937_64 rng((uint64_t)chrono::high_resolution_clock::now().time_since_epoch().count());

// --- New Data Structures for GLWE and GLev ---

// A GLWE ciphertext is a pair (B, D_tilde)
// B is a polynomial, D_tilde is a vector of k polynomials
struct GLWECiphertext {
    poly B;
    vector<poly> D_tilde;
};

// A GLev ciphertext is a collection of l+1 GLWE ciphertexts
struct GLevCiphertext {
    vector<GLWECiphertext> levels;
};


// --- GLev Encryption and Decryption Functions ---

/**
 * @brief Encrypts a message M into a GLev ciphertext.
 *
 * @param PK1 The first part of the public key (AS + E).
 * @param PK2 The second part of the public key (A).
 * @param M The message polynomial to encrypt.
 * @param l The number of levels in the GLev ciphertext (0 to l).
 * @param beta The base for the scaling factor.
 * @param q The ciphertext modulus.
 * @param n The polynomial degree.
 * @param k The number of polynomials in the secret key S.
 * @param noise_bound The bound for generating noise polynomials.
 * @return A GLevCiphertext containing l+1 GLWE ciphertexts.
 */
GLevCiphertext encrypt_glev(
    const poly& PK1,
    const vector<poly>& PK2,
    const poly& M,
    int l,
    int64 beta,
    int64 q,
    size_t n,
    int k,
    int64 noise_bound
) {
    GLevCiphertext glev_ct;
    glev_ct.levels.resize(l + 1);

    uniform_int_distribution<int> udU(0, 1);
    uniform_int_distribution<int64> udNoise(-noise_bound, noise_bound);

    int64 beta_pow_j = 1;

    for (int j = 0; j <= l; ++j) {
        // 1. Calculate the scaling factor for this level: Delta_j = q / (beta^j)
        int64 delta_j = q / beta_pow_j;
        poly scaled_M = poly_scalar_mul(M, delta_j, q);

        // 2. Generate fresh randomness (U, E1, E2) for each GLWE level
        poly U(n, 0);
        for (size_t i = 0; i < n; i++) U[i] = udU(rng);

        poly E1(n, 0);
        for (size_t i = 0; i < n; i++) E1[i] = modq(udNoise(rng), q);

        vector<poly> E2(k, poly(n, 0));
        for (int i = 0; i < k; i++) {
            for (size_t co = 0; co < n; co++) E2[i][co] = modq(udNoise(rng), q);
        }

        // 3. Encrypt using the GLWE formula
        // B = PK1 * U + scaled_M + E1
        poly PK1U = poly_mul_negacyclic(PK1, U, q);
        poly B = poly_add(PK1U, scaled_M, q);
        B = poly_add(B, E1, q);

        // D_tilde = PK2 * U + E2 (vector of k polynomials)
        vector<poly> D_tilde(k, poly(n, 0));
        for (int i = 0; i < k; i++) {
            poly tmp = poly_mul_negacyclic(PK2[i], U, q);
            D_tilde[i] = poly_add(tmp, E2[i], q);
        }

        glev_ct.levels[j] = {B, D_tilde};

        // Update beta^j for the next iteration
        beta_pow_j *= beta;
    }

    return glev_ct;
}

/**
 * @brief Decrypts a specific level of a GLev ciphertext.
 *
 * @param glev_ct The GLev ciphertext.
 * @param S The secret key.
 * @param level_idx The index 'i' of the GLWE ciphertext to decrypt.
 * @param beta The base for the scaling factor.
 * @param q The ciphertext modulus.
 * @param t The plaintext modulus.
 * @param n The polynomial degree.
 * @return The decrypted message polynomial.
 */
poly decrypt_glev_level(
    const GLevCiphertext& glev_ct,
    const vector<poly>& S,
    int level_idx,
    int64 beta,
    int64 q,
    int64 t,
    size_t n
) {
    if (level_idx >= glev_ct.levels.size()) {
        cerr << "Error: Decryption level index out of bounds." << endl;
        return poly(n, 0);
    }

    const GLWECiphertext& ct_i = glev_ct.levels[level_idx];
    const int k = S.size();

    // 1. Calculate the scaling factor Delta_i = q / (beta^i)
    int64 beta_pow_i = 1;
    for(int i = 0; i < level_idx; ++i) beta_pow_i *= beta;
    int64 delta_i = q / beta_pow_i;

    // 2. Compute D_tilde ⋅ S = sum_j D_tilde[j] * S[j]
    poly DtimesS(n, 0);
    for (int j = 0; j < k; j++) {
        poly prod = poly_mul_negacyclic(ct_i.D_tilde[j], S[j], q);
        DtimesS = poly_add(DtimesS, prod, q);
    }

    // 3. Compute B - DtimesS = Delta_i * M + E_all
    poly dec_intermediate = poly_sub(ct_i.B, DtimesS, q);

    // 4. Scale down and round to recover M
    poly M_rec(n, 0);
    for (size_t i = 0; i < n; i++) {
        int64 centered = center_rep(dec_intermediate[i], q);
        int64 rounded = (centered >= 0)
            ? (centered + delta_i / 2) / delta_i
            : (centered - delta_i / 2) / delta_i;

        M_rec[i] = modq(rounded, t);
    }

    return M_rec;
}


int main(void) {
	// --- Parameters ---
	int64 q = (1LL << 30);
	size_t n = 1024;
	int k = 2;              // Corresponds to k in the paper. k=4 is slow for demo.
	int64 t = 256;          // Plaintext modulus
	int64 noise_bound = 8;  // Small noise bound

	// --- GLev specific parameters ---
	int l = 3;              // GLev levels from 0 to l
	int64 beta = 16;         // Base for scaling factor

	// --- Key Generation (Same as GLWE) ---
	// 1. Secret key S
	vector<poly> S;
	for (int i = 0; i < k; i++) {
		poly s_i(n, 0);
		uniform_int_distribution<int> ud01(0, 1);
		for (size_t j = 0; j < n; j++) s_i[j] = ud01(rng);
		S.push_back(s_i);
	}

	// 2. Public randomness A
	vector<poly> A;
	for (int i = 0; i < k; i++) {
		poly a_i(n, 0);
		uniform_int_distribution<int64> udA(0, q - 1);
		for (size_t j = 0; j < n; j++) a_i[j] = udA(rng);
		A.push_back(a_i);
	}

	// 3. Small error polynomial E
	poly E(n, 0);
	uniform_int_distribution<int64> udE(-noise_bound, noise_bound);
	for (size_t i = 0; i < n; i++) E[i] = modq(udE(rng), q);

	// 4. Compute AS = sum_j A_j * S_j
	poly AS(n, 0);
	for (int j = 0; j < k; j++) {
		poly AS_j = poly_mul_negacyclic(A[j], S[j], q);
		AS = poly_add(AS_j, AS, q);
	}

	// 5. Public key (PK1, PK2)
	poly PK1 = poly_add(AS, E, q);
	vector<poly> PK2 = A;

	// --- Message Preparation ---
	poly M(n, 0);
	for (size_t i = 0; i < n; i++) {
		M[i] = (int64)(i % t); // Example message
	}
	cout << "Original message (first 10 coeffs): ";
	for (size_t i = 0; i < 10; i++) cout << M[i] << " ";
	cout << endl << "------------------------------------------" << endl;


	// --- GLev Encryption ---
	cout << "Encrypting message into a GLev ciphertext with l=" << l << " levels..." << endl;
	GLevCiphertext glev_ciphertext = encrypt_glev(PK1, PK2, M, l, beta, q, n, k, noise_bound);
	cout << "Encryption complete." << endl << "------------------------------------------" << endl;

	// --- GLev Decryption (at different levels) ---
	for (int level_to_decrypt : {0, 2}) {
        if (level_to_decrypt > l) continue;

        cout << "Attempting to decrypt level " << level_to_decrypt << "..." << endl;

        poly M_rec = decrypt_glev_level(glev_ciphertext, S, level_to_decrypt, beta, q, t, n);

        cout << "Recovered message (first 10 coeffs): ";
        for (size_t i = 0; i < 10; i++) cout << M_rec[i] << " ";
        cout << endl;

        // Verification
        size_t matches = 0;
        for (size_t i = 0; i < n; i++) if (M_rec[i] == M[i]) matches++;
        cout << "Matches: " << matches << " / " << n << endl;

        // Noise check
        int64 beta_pow = 1;
        for(int i = 0; i < level_to_decrypt; ++i) beta_pow *= beta;
        int64 delta = q / beta_pow;
        cout << "For correct decryption, noise should be < delta/2 = " << (delta / 2) << endl;
        cout << "------------------------------------------" << endl;
    }

	return 0;
}
