#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include "anura.h"

extern const uint8_t f[8][256];
extern const anura_transformation t;

extern uint64_t anura_transform(uint64_t x, const anura_transformation *u)
{
	x += u->u[0][(uint8_t)x];
	x = (x<<8) | (x>>56);
	x += u->u[1][(uint8_t)x];
	x = (x<<8) | (x>>56);
	x += u->u[2][(uint8_t)x];
	x = (x<<8) | (x>>56);
	x += u->u[3][(uint8_t)x];
	x = (x<<8) | (x>>56);
	x += u->u[4][(uint8_t)x];
	x = (x<<8) | (x>>56);
	x += u->u[5][(uint8_t)x];
	x = (x<<8) | (x>>56);
	x += u->u[6][(uint8_t)x];
	x = (x<<8) | (x>>56);
	x += u->u[7][(uint8_t)x];
	x = (x<<8) | (x>>56);
	return x;
}

extern uint64_t anura_transform_inv(uint64_t x, const anura_transformation *u)
{
	x = (x<<56) | (x>>8);
	x -= u->u[7][(uint8_t)x];
	x = (x<<56) | (x>>8);
	x -= u->u[6][(uint8_t)x];
	x = (x<<56) | (x>>8);
	x -= u->u[5][(uint8_t)x];
	x = (x<<56) | (x>>8);
	x -= u->u[4][(uint8_t)x];
	x = (x<<56) | (x>>8);
	x -= u->u[3][(uint8_t)x];
	x = (x<<56) | (x>>8);
	x -= u->u[2][(uint8_t)x];
	x = (x<<56) | (x>>8);
	x -= u->u[1][(uint8_t)x];
	x = (x<<56) | (x>>8);
	x -= u->u[0][(uint8_t)x];
	return x;
}

static uint64_t next_subkey(uint64_t prev_subkey, uint64_t count,
		const uint64_t *key, size_t key_length, const anura_transformation *u)
{
	const uint64_t e = 0xb7e151628aed2a6a;
	size_t i;
	prev_subkey ^= e;

	for (i=0; i<key_length; i++) {
		prev_subkey = anura_transform(prev_subkey^key[i], u);
	}

	return anura_transform(prev_subkey ^ count, u);
}

extern anura_status anura_schedule(anura_algorithm_code algorithm_code,
		const uint64_t *key, size_t key_length, 
		uint64_t *key_schedule, size_t key_schedule_length,
		anura_transformation *s, anura_transformation *sinv)
{
	uint64_t subkey, count, z, subkeys[8];
	size_t i, j, k, l;
	uint8_t x;

	subkey = 0x243f6a8885a308d3;

	for (i=0; i<key_length; i++) {
		subkey ^= anura_transform(key[i]^i, &t);
	}
	subkey = anura_transform(subkey, &t);

	count = 0;
	for (i=0; i<8; i++) {
		for (k=0; k<8; k++) {
			subkeys[k] = next_subkey(subkey, ++count, key, key_length, &t);
		}
		for (j=0; j<256; j++) {
			z = 0;
			for (k=0; k<8; k++) {
				x = f[k][i^j];
				for (l=1; l<9; l++) {
					x = f[(k+l)%8][x^(uint8_t)(subkeys[k]>>(8*(l-1)))];
				}
				z |= ((uint64_t)x)<<(8*k);
			}
			s->u[i][j] = z-j;

			if (sinv != NULL) {
				sinv->u[i][(uint8_t)z] = z-j;
			}
		}
	}
	for (i=0; i<key_schedule_length; i++) {
		subkey = next_subkey(subkey, ++count, key, key_length, s);
		key_schedule[i] = subkey;
	}
	return OK;
}
