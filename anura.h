#include <stdlib.h>
#include <stdint.h>

typedef struct {
	uint64_t u[8][256];
} anura_transformation;


typedef enum {
	OK = 0,
	INVALID_KEY_LENGTH = 1,
	INVALID_KEY_SCHEDULE_LENGTH = 2,
	INVALID_ALGORITHM_CODE = 3
} anura_status;

typedef enum {
	BUFO = 0x0001,
	ATELOPUS = 0x0001,
	RHINELLA = 0x0002,
	RANA = 0x0100
} anura_algorithm_code;

uint64_t anura_transform(uint64_t x, const anura_transformation *u);
uint64_t anura_transform_inv(uint64_t x, const anura_transformation *u);

anura_status anura_schedule(anura_algorithm_code algorithm_code,
		const uint64_t *key, size_t key_length,
		uint64_t *key_schedule, size_t key_schedule_length,
		anura_transformation *s, anura_transformation *sinv);

void bufo_encrypt(const uint64_t *key_schedule,
		const anura_transformation *s, const uint64_t input[4],
		uint64_t output[4]);

void bufo_decrypt(const uint64_t *key_schedule,
		const anura_transformation *sinv, const uint64_t input[4],
		uint64_t output[4]);

