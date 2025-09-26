#include <iostream>
#include <vector>
#include <numeric>
#include <random>
#include <chrono>
#include <algorithm>
#include <cmath>

using namespace std;
using int64 = long long;

// --- Utility and Polynomial Functions (Unchanged) ---

int64 modq(int64 x, int64 q) {
	int64 r = x % q;
	if (r < 0) r += q;
	return r;
}

int64 center_rep(int64 x, int64 q) {
	int64 v = modq(x, q);
	int64 half = q / 2;
	if (v > half) v -= q;
	return v;
}

using poly = vector<int64>;

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

poly poly_add(const poly &a, const poly &b, int64 q) {
	size_t n = a.size();
	poly r(n);
	for (size_t i = 0; i < n; i++) r[i] = modq(a[i] + b[i], q);
	return r;
}

poly poly_sub(const poly &a, const poly &b, int64 q) {
	size_t n = a.size();
	poly r(n);
	for (size_t i = 0; i < n; i++) r[i] = modq(a[i] - b[i], q);
	return r;
}

// Helper to negate a polynomial
poly poly_neg(const poly &a, int64 q) {
    size_t n = a.size();
    poly r(n);
    for (size_t i = 0; i < n; i++) r[i] = modq(-a[i], q);
    return r;
}


poly poly_scalar_mul (const poly &a, int64 scalar, int64 q) {
    size_t n = a.size();
    poly r(n);
    for (size_t i = 0; i < n; i++) {
        __int128 tmp = (__int128)a[i] * scalar;
        r[i] = modq((int64)(tmp % q), q);
    }
    return r;
}


std::mt19937_64 rng((uint64_t)chrono::high_resolution_clock::now().time_since_epoch().count());

// --- Data Structures (GLWE, GLev, and new GGSW) ---

struct GLWECiphertext {
    poly B;
    vector<poly> D_tilde;
};

struct GLevCiphertext {
    vector<GLWECiphertext> levels;
};

// A GGSW ciphertext is a collection of k+1 GLev ciphertexts
struct GGSWCiphertext {
    vector<GLevCiphertext> glev_rows;
};

// --- GLev Functions (with Level-0 Bug Fix) ---

GLevCiphertext encrypt_glev(
    const poly& PK1, const vector<poly>& PK2, const poly& M,
    int l, int64 beta, int64 q, size_t n, int k, int64 noise_bound)
{
    GLevCiphertext glev_ct;
    glev_ct.levels.resize(l + 1);
    uniform_int_distribution<int> udU(0, 1);
    uniform_int_distribution<int64> udNoise(-noise_bound, noise_bound);
    int64 beta_pow_j = 1;

    for (int j = 0; j <= l; ++j) {
        // 🐞 FIX: Ensure scaling factor is never `q` by starting the division from `beta`.
        int64 delta_j = q / (beta * beta_pow_j);
        if (delta_j == 0) delta_j = 1; // prevent division by zero if q is small
        poly scaled_M = poly_scalar_mul(M, delta_j, q); // Using poly mul for scalar

        poly U(n, 0); for (size_t i = 0; i < n; i++) U[i] = udU(rng);
        poly E1(n, 0); for (size_t i = 0; i < n; i++) E1[i] = modq(udNoise(rng), q);
        vector<poly> E2(k, poly(n, 0));
        for (int i = 0; i < k; i++) for (size_t co = 0; co < n; co++) E2[i][co] = modq(udNoise(rng), q);

        poly PK1U = poly_mul_negacyclic(PK1, U, q);
        poly B = poly_add(PK1U, scaled_M, q);
        B = poly_add(B, E1, q);

        vector<poly> D_tilde(k, poly(n, 0));
        for (int i = 0; i < k; i++) {
            poly tmp = poly_mul_negacyclic(PK2[i], U, q);
            D_tilde[i] = poly_add(tmp, E2[i], q);
        }
        glev_ct.levels[j] = {B, D_tilde};
        beta_pow_j *= beta;
    }
    return glev_ct;
}

poly decrypt_glev_level(
    const GLevCiphertext& glev_ct, const vector<poly>& S, int level_idx,
    int64 beta, int64 q, int64 t, size_t n)
{
    if (level_idx >= glev_ct.levels.size()) {
        cerr << "Error: Decryption level index out of bounds." << endl;
        return poly(n, 0);
    }
    const GLWECiphertext& ct_i = glev_ct.levels[level_idx];
    const int k = S.size();
    int64 beta_pow_i = 1;
    for(int i = 0; i < level_idx; ++i) beta_pow_i *= beta;
    // 🐞 FIX: Match the scaling factor used during encryption.
    int64 delta_i = q / (beta * beta_pow_i);
    if (delta_i == 0) delta_i = 1;

    poly DtimesS(n, 0);
    for (int j = 0; j < k; j++) {
        poly prod = poly_mul_negacyclic(ct_i.D_tilde[j], S[j], q);
        DtimesS = poly_add(DtimesS, prod, q);
    }
    poly dec_intermediate = poly_sub(ct_i.B, DtimesS, q);
    poly M_rec(n, 0);
    for (size_t i = 0; i < n; i++) {
        int64 centered = center_rep(dec_intermediate[i], q);
        int64 rounded = (centered >= 0) ? (centered + delta_i / 2) / delta_i : (centered - delta_i / 2) / delta_i;
        M_rec[i] = modq(rounded, t);
    }
    return M_rec;
}

// --- New GGSW Encryption and Decryption Functions ---

/**
 * @brief Encrypts a message M into a GGSW ciphertext.
 * Note: GGSW encryption requires the secret key S.
 */
GGSWCiphertext encrypt_ggsw(
    const poly& PK1, const vector<poly>& PK2, const vector<poly>& S, const poly& M,
    int l, int64 beta, int64 q, size_t n, int k, int64 noise_bound)
{
    GGSWCiphertext ggsw_ct;
    ggsw_ct.glev_rows.resize(k + 1);

    // Encrypt the first k rows: GLev(-S_i * M)
    for (int i = 0; i < k; ++i) {
        poly Si_M = poly_mul_negacyclic(S[i], M, q);
        poly neg_Si_M = poly_neg(Si_M, q);
        ggsw_ct.glev_rows[i] = encrypt_glev(PK1, PK2, neg_Si_M, l, beta, q, n, k, noise_bound);
    }

    // Encrypt the final row: GLev(M)
    ggsw_ct.glev_rows[k] = encrypt_glev(PK1, PK2, M, l, beta, q, n, k, noise_bound);

    return ggsw_ct;
}

/**
 * @brief Decrypts a GGSW ciphertext to recover the message M.
 * This is done by decrypting a level from the final GLev ciphertext.
 */
poly decrypt_ggsw(
    const GGSWCiphertext& ggsw_ct, const vector<poly>& S, int level_idx,
    int64 beta, int64 q, int64 t, size_t n)
{
    // Decryption uses the last GLev row, which encrypts M.
    const GLevCiphertext& final_glev_row = ggsw_ct.glev_rows.back();
    return decrypt_glev_level(final_glev_row, S, level_idx, beta, q, t, n);
}


int main(void) {
	// --- Parameters ---
	int64 q = (1LL << 32);
	size_t n = 1024;
	int k = 2;
	int64 t = 256;
	int64 noise_bound = 8;
	int l = 4;
	int64 beta = 16;

	// --- Key Generation (Same as before) ---
	vector<poly> S;
	for (int i = 0; i < k; i++) {
		poly s_i(n, 0); uniform_int_distribution<int> ud01(0, 1);
		for (size_t j = 0; j < n; j++) s_i[j] = ud01(rng);
		S.push_back(s_i);
	}
	vector<poly> A;
	for (int i = 0; i < k; i++) {
		poly a_i(n, 0); uniform_int_distribution<int64> udA(0, q - 1);
		for (size_t j = 0; j < n; j++) a_i[j] = udA(rng);
		A.push_back(a_i);
	}
	poly E(n, 0); uniform_int_distribution<int64> udE(-noise_bound, noise_bound);
	for (size_t i = 0; i < n; i++) E[i] = modq(udE(rng), q);
	poly AS(n, 0);
	for (int j = 0; j < k; j++) AS = poly_add(poly_mul_negacyclic(A[j], S[j], q), AS, q);
	poly PK1 = poly_add(AS, E, q);
	vector<poly> PK2 = A;

	// --- Message Preparation ---
	poly M(n, 0);
	for (size_t i = 0; i < n; i++) M[i] = (int64)(i % t);
	cout << "Original message (first 10 coeffs): ";
	for (size_t i = 0; i < 10; i++) cout << M[i] << " ";
	cout << endl << "------------------------------------------" << endl;

	// --- GGSW Encryption ---
	cout << "Encrypting message into a GGSW ciphertext..." << endl;
    // Note: GGSW encryption needs the secret key S to compute the first k rows.
	GGSWCiphertext ggsw_ciphertext = encrypt_ggsw(PK1, PK2, S, M, l, beta, q, n, k, noise_bound);
	cout << "Encryption complete." << endl << "------------------------------------------" << endl;

	// --- GGSW Decryption (at different levels of the final row) ---
    for (int level_to_decrypt : {0, 3}) {
        if (level_to_decrypt > l) continue;

        cout << "Attempting to decrypt GGSW ciphertext using level " << level_to_decrypt << "..." << endl;
        poly M_rec = decrypt_ggsw(ggsw_ciphertext, S, level_to_decrypt, beta, q, t, n);

        cout << "Recovered message (first 10 coeffs): ";
        for (size_t i = 0; i < 10; i++) cout << M_rec[i] << " ";
        cout << endl;

        size_t matches = 0;
        for (size_t i = 0; i < n; i++) if (M_rec[i] == M[i]) matches++;
        cout << "Matches: " << matches << " / " << n << endl;
        cout << "------------------------------------------" << endl;
    }

	return 0;
}
