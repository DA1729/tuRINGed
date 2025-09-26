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
	poly res(n);
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


poly sample_uniform_poly(size_t n, int64 q) {
	poly p(n);
	uniform_int_distribution<int64> ud(0, q-1);
	for (size_t i = 0; i < n; i++) p[i] = ud(rng);
	return p;
}


poly sample_binary_secret(size_t n) {
	poly s(n);
	uniform_int_distribution<int> ud(0, 1);
	for (size_t i = 0; i < n; i++) s[i] = ud(rng);
	return s;
}


poly sample_noise_poly(size_t n, int64 B, int64 q) {
	poly e(n);
	if (B <= 0) {
		for (size_t i = 0; i < n; i++) e[i] = 0;
		return e;
	}

	uniform_int_distribution<int64> ud(-B, B);
	for (size_t i = 0; i < n; i++) e[i] = modq(ud(rng), q);
	return e;
}


struct secret_key {
	poly S;
};

struct ciphertext {
	poly A;
	poly B;
};

secret_key key_gen(size_t n) {
	secret_key sk;
	sk.S = sample_binary_secret(n);
	return sk;
}



ciphertext encrypt(const poly &M, const secret_key &sk, int64 q, int64 t, int64 delta, int64 noise_bound) {
	size_t n = sk.S.size();
	if (M.size() != n) throw runtime_error("wrong, lmao, you are dumb");
	ciphertext ct;
	ct.A = sample_uniform_poly(n, q);
	poly E = sample_noise_poly(n, noise_bound, q);
	poly DM = poly_scalar_mul(M, delta, q);
	poly AS = poly_mul_negacyclic(ct.A, sk.S, q);
	ct.B = poly_add(poly_add(AS, DM, q), E, q);
	return ct;
}


poly decrypt(const ciphertext &ct, const secret_key &sk, int64 q, int64 t, int64 delta) {
	size_t n = sk.S.size();
	if (ct.A.size() != n || ct.B.size() != n) throw runtime_error("wrong, lmao, you are awfully bumb");
	poly AS = poly_mul_negacyclic(ct.A, sk.S, q);
	poly DF = poly_sub(ct.B, AS, q);

	vector<int64> centered = poly_centred(DF, q);

	poly Mh(n);

	for (size_t i = 0; i < n; i++) {
		double val = (double)centered[i] / (double)delta;
		int64 rounded = (int64)llround(val);
		int64 m_coeff = rounded % t;
		if (m_coeff < 0) m_coeff += t;
		Mh[i] = m_coeff;
	}

	return Mh;
}

int main() {
	size_t n = 1024;
	int64 q = 8192*2;
	int64 t = 256;
	int64 delta = q / t;
	int64 noise_bound = 3;

	cout << "RLWE Encryption Demo" << endl;
	cout << "Parameters: n=" << n << ", q=" << q << ", t=" << t << ", delta=" << delta << endl;

	secret_key sk = key_gen(n);
	cout << "Generated secret key" << endl;

	poly M(n);
	for (size_t i = 0; i < n; i++) {
		M[i] = i % t;
	}
	cout << "Original message (first 10 coeffs): ";
	for (size_t i = 0; i < min(n, (size_t)10); i++) {
		cout << M[i] << " ";
	}
	cout << endl;

	ciphertext ct = encrypt(M, sk, q, t, delta, noise_bound);
	cout << "Encrypted message" << endl;

	poly decrypted = decrypt(ct, sk, q, t, delta);
	cout << "Decrypted message (first 10 coeffs): ";
	for (size_t i = 0; i < min(n, (size_t)10); i++) {
		cout << decrypted[i] << " ";
	}
	cout << endl;

	bool success = true;
	for (size_t i = 0; i < n; i++) {
		if (M[i] != decrypted[i]) {
			success = false;
			break;
		}
	}

	if (success) {
		cout << "Decryption successful!" << endl;
	} else {
		cout << "Decryption failed!" << endl;
	}

	return 0;
}
