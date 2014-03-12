#include <stdint.h>
#include <stdlib.h>
#include "anura.h"

#define rotl(x, r) ((x << r) | (x >> (64-r)))

#define R(x, i) \
	x += s->u[i][(uint8_t)x]; \
	x = rotl(x, 8);

#define S(i) \
	R(a, i); R(b, i); R(c, i); R(d, i)

#define S2(i) \
	R(a, i); R(b, i); R(c, i); R(d, i); R(e, i); R(f, i); R(g, i); R(h, i)

#define T() \
	S(0); S(1); S(2); S(3); S(4); S(5); S(6); S(7)

#define T2() \
	S2(0); S2(1); S2(2); S2(3); S2(4); S2(5); S2(6); S2(7)

#define E(i) \
		a ^= key_schedule[4*i]; \
		b ^= key_schedule[4*i+1]; \
		c ^= key_schedule[4*i+2]; \
		d ^= key_schedule[4*i+3]; \
		T()

#define E2(i) \
		a ^= key_schedule[8*i]; \
		b ^= key_schedule[8*i+1]; \
		c ^= key_schedule[8*i+2]; \
		d ^= key_schedule[8*i+3]; \
		e ^= key_schedule[8*i+4]; \
		f ^= key_schedule[8*i+5]; \
		g ^= key_schedule[8*i+6]; \
		h ^= key_schedule[8*i+7]; \
		T2()

#define P() \
		a ^= rotl(b, 11) ^ rotl(c, 27) ^ rotl(d, 43); \
		b ^= rotl(a, 24); \
		c ^= rotl(a, 18); \
		d ^= rotl(a, 10)

#define P2() \
		a = rotl(a, 3) ^ rotl(b,  11) ^ rotl(c, 19) ^ rotl(d, 27) \
			^ rotl(e, 35) ^ rotl(f, 43) ^ rotl(g, 51) \
			^ rotl(h, 59); \
		b ^= rotl(a,  2); \
		c ^= rotl(a, 12); \
		d ^= rotl(a, 22); \
		e ^= rotl(a, 32); \
		f ^= rotl(a, 42); \
		g ^= rotl(a, 52); \
		h ^= rotl(a, 62)

#define Rinv(x, i) \
	x = rotl(x, 56); \
	x -= sinv->u[i][(uint8_t)x];
#define Sinv(i) \
	Rinv(a, i); Rinv(b, i); Rinv(c, i); Rinv(d, i)
#define Tinv() \
	Sinv(7); Sinv(6); Sinv(5); Sinv(4); Sinv(3); Sinv(2); Sinv(1); Sinv(0)

#define Einv(i) \
		Tinv(); \
		a ^= key_schedule[4*i]; \
		b ^= key_schedule[4*i+1]; \
		c ^= key_schedule[4*i+2]; \
		d ^= key_schedule[4*i+3]

#define Pinv() \
		b ^= rotl(a, 10); \
		c ^= rotl(a, 18); \
		d ^= rotl(a, 26); \
		a ^= rotl(b, 11) ^ rotl(c, 27) ^ rotl(d, 43)

void bufo_encrypt(const uint64_t *key_schedule,
		const anura_transformation *s, const uint64_t input[4],
		uint64_t output[4])
{
	uint64_t a, b, c, d;

	a = input[0];
	b = input[1];
	c = input[2];
	d = input[3];

	E(0); P();
	E(1); P();
	E(2); P();
	E(3); P();
	E(4); P();
	E(5); P();
	E(6); P();
	E(7);

	output[0] = a ^ key_schedule[32];
	output[1] = b ^ key_schedule[33];
	output[2] = c ^ key_schedule[34];
	output[3] = d ^ key_schedule[35];
}

void bufo_decrypt(const uint64_t *key_schedule,
		const anura_transformation *sinv, const uint64_t input[4],
		uint64_t output[4])
{
	uint64_t a, b, c, d;

	a = input[0] ^ key_schedule[32];
	b = input[1] ^ key_schedule[33];
	c = input[2] ^ key_schedule[34];
	d = input[3] ^ key_schedule[35];

	Einv(7); Pinv();
	Einv(6); Pinv();
	Einv(5); Pinv();
	Einv(4); Pinv();
	Einv(3); Pinv();
	Einv(2); Pinv();
	Einv(1); Pinv();
	Einv(0);

	output[0] = a;
	output[1] = b;
	output[2] = c;
	output[3] = d;
}
