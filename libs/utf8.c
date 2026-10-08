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

bool utf8_find(const uint8_t *bytes, size_t size, const uint8_t *pattern, size_t pattern_size, const uint8_t **res_ptr)
{
	if (NULL == bytes || NULL == pattern) return false;
	if (size == 0) return false;

	if (NULL != res_ptr) *res_ptr = NULL;
	if (pattern_size == 0) {
		if (NULL != res_ptr) *res_ptr = bytes;
		return true;
	}

	size_t next = 0;
	for (size_t cur = 0; utf8_next(bytes, size, &next); cur = next) {
		size_t char_size = next - cur;
		if (bytes[cur] == pattern[0]) {
			size_t i = 0;
			for (; i < char_size; i++) {
				if (bytes[cur+i] != pattern[i]) break;
			}
			if (i == char_size) {
				if (NULL != res_ptr) *res_ptr = bytes + cur+i;
				return true;
			}
		}
	}
	return false;
}


bool utf8_escape(const uint8_t *bytes, size_t size, uint8_t **escaped, size_t *escaped_size,
		const uint8_t *escape_prefix, size_t prefix_size, const uint8_t *escape_charset, size_t charset_size)
{
	if (NULL == bytes || NULL == escaped || NULL == escape_prefix || NULL == escape_charset) return false;
	if (size == 0 || prefix_size == 0 || charset_size == 0) {
		*escaped = ft_memdup(bytes, size);
		*escaped_size = size;
		return true;
	}

	t_ostring ostring = {0};
	ft_ostr_init_with_capacity(&ostring, size);

	size_t next = 0;
	for (size_t cur = 0; utf8_next(bytes, size, &next); cur = next) {
		if (utf8_find(escape_charset, charset_size, bytes+cur, next-cur, NULL)) {
			ft_ostr_append(&ostring, escape_prefix, prefix_size);
		}
		ft_ostr_append(&ostring, bytes+cur, next-cur);
	}
	void *content = NULL;
	size_t content_size = 0;
	ft_ostr_unwrap_content(&ostring, &content, &content_size);
	*escaped = content;
	*escaped_size = content_size;
	return true;
}

// Expects null-terminated escape prefix, produces null-terminated escape result.
bool utf8_escape_nt(const uint8_t *bytes, size_t size, char **escaped, const char *escape_prefix,
		const uint8_t *escape_charset, size_t charset_size)
{
	if (NULL == bytes || NULL == escaped || NULL == escape_prefix || NULL == escape_charset) return false;

	size_t prefix_size = ft_strlen(escape_prefix);
	if (size == 0 || prefix_size == 0 || charset_size == 0) {
		LIBFT_ALLOC(*escaped, size+1);
		ft_memzcpy(*escaped, bytes, size+1, size);
		return true;
	}

	t_ostring ostring = {0};
	ft_ostr_init_with_capacity(&ostring, size);

	size_t next = 0;
	for (size_t cur = 0; utf8_next(bytes, size, &next); cur = next) {
		if (utf8_find(escape_charset, charset_size, bytes+cur, next-cur, NULL)) {
			ft_ostr_append(&ostring, escape_prefix, prefix_size);
		}
		ft_ostr_append(&ostring, bytes+cur, next-cur);
	}
	*escaped = ft_ostr_to_cstr(&ostring, 0, ostring.size);
	return true;
}
