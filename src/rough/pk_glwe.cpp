#include <bits/stdc++.h>
using namespace std;
using int64 = long long;

int64 modq(int64 x, int64 q) {
	int64 r = x % q;
	if (r < 0) {
		r += q;
	}
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
				// ensure positive mod
				res[idx2] = (int64)(((tmp % q) + q) % q);
			}
		}
	}
	for (size_t i = 0; i < n; i++) res[i] = modq(res[i], q);
	return res;
}

poly poly_add (const poly &a, const poly &b, int64 q) {
	size_t n = a.size();
	poly r(n);
	for (size_t i = 0; i < n; i++) r[i] = modq(a[i] + b[i], q);
	return r;
}

poly poly_sub (const poly &a, const poly &b, int64 q) {
	size_t n = a.size();
	poly r(n);
	for (size_t i = 0; i < n; i++) r[i] = modq(a[i] - b[i], q);
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

vector<int64> poly_centred(const poly &a, int64 q) {
	size_t n = a.size();
	poly r(n);
	for (size_t i = 0; i < n; i++) r[i] = center_rep(a[i], q);
	return r;
}

std::mt19937_64 rng((uint64_t)chrono::high_resolution_clock::now().time_since_epoch().count());

int main(void) {
	// Parameters (small example-ish values; change as needed)
	int64 q = (1LL << 26);
	size_t n = 1024;
	int k = 4;            // reduced k for runtime; original code used 256 but heavy
	int64 t = 256;
	int64 delta = q / t;  // scaling factor
	int64 noise_bound = 3; // small noise

	// 1. Secret key S = {S_i}_{i=0..k-1} in R_{n,2} (binary polys)
	vector<poly> S;
	for (int i = 0; i < k; i++) {
		poly s_i(n, 0);
		uniform_int_distribution<int> ud01(0, 1);
		for (size_t j = 0; j < n; j++) s_i[j] = ud01(rng);
		S.push_back(s_i);
	}

	// 2. Public randomness A = {A_i} with coefficients uniform in Z_q
	vector<poly> A;
	for (int i = 0; i < k; i++) {
		poly a_i(n, 0);
		uniform_int_distribution<int64> udA(0, q-1);
		for (size_t j = 0; j < n; j++) a_i[j] = udA(rng);
		A.push_back(a_i);
	}

	// 3. Small error polynomial E
	poly E(n, 0);
	uniform_int_distribution<int64> udNoise(-noise_bound, noise_bound);
	for (size_t i = 0; i < n; i++) E[i] = modq(udNoise(rng), q);

	// 4. Compute AS = sum_j A_j * S_j (negacyclic multiplication)
	poly AS(n, 0);
	for (int j = 0; j < k; j++) {
		poly AS_j = poly_mul_negacyclic(A[j], S[j], q);
		AS = poly_add(AS_j, AS, q);
	}

	// 5. Public key PK1 = AS + E  (in R_{n,q})
	poly PK1 = poly_add(AS, E, q);

	// PK2 is just the vector A
	vector<poly> PK2 = A;

	// ---- Prepare message M in R_{n,t} (coeffs < t) ----
	poly M(n, 0);
	for (size_t i = 0; i < n; i++) {
		M[i] = (int64)(i % t); // example message
	}
	cout << "original message (first 10 coeffs): ";
	for (size_t i = 0; i < min(n, (size_t)10); i ++) {
		cout << M[i] << " ";
	}
	cout << endl;

	// ---- Encryption ----
	// 1. Scale up M -> delta * M in R_{n,q}
	poly deltaM = poly_scalar_mul(M, delta, q);

	// 2. Choose U in R_{n,2} (binary poly)
	poly U(n, 0);
	uniform_int_distribution<int> udU(0, 1);
	for (size_t i = 0; i < n; i++) U[i] = udU(rng);

	// 3. E1 small noise poly and E2 small noise vector (one per A_j)
	poly E1(n, 0);
	for (size_t i = 0; i < n; i++) E1[i] = modq(udNoise(rng), q);

	vector<poly> E2(k, poly(n,0));
	for (int j = 0; j < k; j++) {
		for (size_t i = 0; i < n; i++) E2[j][i] = modq(udNoise(rng), q);
	}

	// Compute B = PK1 * U + deltaM + E1
	poly PK1U = poly_mul_negacyclic(PK1, U, q);
	poly B = poly_add(PK1U, deltaM, q);
	B = poly_add(B, E1, q);

	// Compute D_tilde = PK2 * U + E2 -> that's a vector of k polys
	vector<poly> D_tilde(k, poly(n,0));
	for (int j = 0; j < k; j++) {
		poly tmp = poly_mul_negacyclic(PK2[j], U, q);
		D_tilde[j] = poly_add(tmp, E2[j], q);
	}

	// ---- Decryption ----
	// Compute D_tilde ⋅ S = sum_j D_tilde[j] * S[j]
	poly DtimesS(n, 0);
	for (int j = 0; j < k; j++) {
		poly prod = poly_mul_negacyclic(D_tilde[j], S[j], q);
		DtimesS = poly_add(DtimesS, prod, q);
	}

	// Compute B - DtimesS = Delta*M + E_all
	poly dec_intermediate = poly_sub(B, DtimesS, q);

	// Now scale down: recover M by rounding dec_intermediate / delta
	poly M_rec(n, 0);
	for (size_t i = 0; i < n; i++) {
		int64 centered = center_rep(dec_intermediate[i], q); // in (-q/2, q/2]
		// integer rounding: if centered >= 0: (centered + delta/2)/delta else (centered - delta/2)/delta
		int64 rounded;
		if (centered >= 0) rounded = (centered + delta/2) / delta;
		else rounded = (centered - delta/2) / delta;
		// reduce modulo t to get message coefficient
		int64 mm = rounded % t;
		if (mm < 0) mm += t;
		M_rec[i] = mm;
	}

	cout << "recovered message (first 10 coeffs): ";
	for (size_t i = 0; i < min(n, (size_t)10); i ++) {
		cout << M_rec[i] << " ";
	}
	cout << endl;

	// Quick check: how many coefficients match
	size_t matches = 0;
	for (size_t i = 0; i < n; i++) if (M_rec[i] == M[i]) matches++;
	cout << "matches: " << matches << " / " << n << endl;

	// Print some stats about max noise (centered values) to get intuition
	int64 max_noise = 0;
	for (size_t i = 0; i < n; i++) {
		int64 centered = llabs(center_rep(dec_intermediate[i] - deltaM[i], q));
		if (centered > max_noise) max_noise = centered;
	}
	cout << "max |E_all coeff| (approx): " << max_noise << " (should be < delta/2 = " << (delta/2) << " for correct decryption)" << endl;

	return 0;
}

