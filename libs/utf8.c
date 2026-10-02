#include <common.h>
#include <libft.h>
#include <libft_v2.h>

static const uint8_t rng_ascii[2]		= { 0x00, 0x7F };	/* ASCII character / one-byte UTF-8 sequence */
static const uint8_t rng_2_byte[2]		= { 0xC2, 0xDF };	/* Start of a 2-byte sequence */
static const uint8_t rng_3_byte[2]		= { 0xE0, 0xEF };	/* Start of a 3-byte sequence */
static const uint8_t rng_4_byte[2]		= { 0xF0, 0xF4 };	/* Start of a 4-byte sequence */
static const uint8_t rng_cont[2]		= { 0x80, 0xBF };	/* Continuation byte */
static const uint8_t rng_unused_1[2]	= { 0xC0, 0xC1 };	/* Unused */
static const uint8_t rng_unused_2[2]	= { 0xF5, 0xFF };	/* Unused */

bool utf8_next(const uint8_t *bytes, size_t size, size_t *next)
{
	if (NULL == bytes || NULL == next) return false;
	if (size == 0) return false;
	if (*next >= size) return false;

	uint8_t octet = bytes[*next];
	size_t nbytes = 0;

	if (octet >= rng_ascii[0] && octet <= rng_ascii[1])			nbytes = 1;
	else if (octet >= rng_2_byte[0] && octet <= rng_2_byte[1])	nbytes = 2;
	else if (octet >= rng_3_byte[0] && octet <= rng_3_byte[1])	nbytes = 3;
	else if (octet >= rng_4_byte[0] && octet <= rng_4_byte[1])	nbytes = 4;
	else return false;

	if (*next + nbytes > size) return false;

	*next += nbytes;
	return true;
}

bool utf8_is_valid(const uint8_t *bytes, size_t size)
{
	if (NULL == bytes) return false;

	size_t next = 0;
	for (size_t cur = 0; utf8_next(bytes, size, &next); cur = next) {
		const uint8_t *p = bytes + cur;
		size_t nbytes = next - cur;

		if (p[0] >= rng_unused_1[0] && p[0] <= rng_unused_1[1]) return false;
		if (p[0] >= rng_unused_2[0] && p[0] <= rng_unused_2[1]) return false;
		for (size_t i = 1; i < nbytes; i++) {
			if (p[i] < rng_cont[0] || p[i] > rng_cont[1]) return false;
		}
	}
	if (next != size) return false; // incomplete sequence
	return true;
}
