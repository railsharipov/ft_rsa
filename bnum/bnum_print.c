#include <bnum.h>
#include <logger.h>
#include <libft.h>
#include <unistd.h>

static const char A[] = "0123456789abcdef";

char *__to_hex_string(const t_num *num)
{
	char *hexrev, *hptr, *hex;
	uint64_t digit;
	size_t hexsize;
	int idx, idy;

	if (NULL == num) {
		return (NULL);
	}
	if (BNUM_ZERO(num)) {
		return (strdup("0"));
	}
	hexsize = NBITS_TO_NWORDS(num->len * BNUM_DIGIT_BIT, 4);
	BNUM_ALLOC(hexrev, hexsize + 1);
	hptr = hexrev;

	idx = 0;
	while (idx < num->len) {
		digit = num->val[idx];
		idy = 0;
		while (idy < BNUM_DIGIT_BIT) {
			*hptr++ = A[(digit >> idy) & 0xF];
			idy += 4;
		}
		idx++;
	}
	while (hexsize > 0 && hexrev[hexsize - 1] == '0') {
		hexsize--;
	}
	BNUM_ALLOC(hex, hexsize + 1);

	idx = 0;
	while (hexsize > 0) {
		hex[idx++] = hexrev[hexsize-- - 1];
	}
	BNUM_FREE(hexrev);

	return (hex);
}

static int	__get_sign(const t_num *num) {
	if (num->sign == BNUM_NEG) {
		return ('-');
	} else if (num->sign == BNUM_POS) {
		return ('+');
	} else {
		return ('?');
	}
}

void	bnum_print(const char *prefix, const t_num *num)
{
	char	*hex;
	int		bits;
	char	sign;

	if (NULL == num || NULL == num->val || num->len == 0) {
		SSL_LOG(INFO, "[none]");
		return ;
	}

	hex = __to_hex_string(num);
	sign = __get_sign(num);

	bits = bnum_lmbit(num);
	if (prefix) {
		SSL_LOG(INFO, "%s: [len=%d, bits=%d, sign=(%c)] %s ", prefix, num->len, bits, sign, hex);
	} else {
		SSL_LOG(INFO, "[len=%d, bits=%d, sign=(%c)] %s", num->len, bits, sign, hex);
	}

	BNUM_FREE(hex);
}

void	bnum_print_raw(const t_num *num)
{
	char	*str;
	char 	sign;
	ssize_t	wbytes;
	ssize_t	tbytes;
	size_t	slen;

	if (NULL == num || NULL == num->val || num->len == 0) {
		SSL_LOG(INFO, "[none]");
		return ;
	}

	slen = num->len * (1 + BNUM_DIGIT_BIT);
	BNUM_ALLOC(str, slen + 1);
	sign = __get_sign(num);

	tbytes = 0;
	for (int i = num->len-1; i >= 0; i--) {
		wbytes = snprintf(str + tbytes, slen - tbytes + 1, "%llu ", num->val[i]);
		if (wbytes < 0) {
			SSL_LOG(ERROR, "print error");
			return ;
		}
		tbytes += wbytes;
	}
	str[tbytes] = 0;

	SSL_LOG(INFO, "%c%s", sign, str);
	BNUM_FREE(str);
}

void	bnum_print_bits(const char *prefix, const t_num *num)
{
	char *str;
	char sign;
	size_t	size;
	size_t 	bitcount;
	ssize_t wbytes;
	ssize_t tbytes;

	if (NULL == num || NULL == num->val || num->len == 0) {
		SSL_LOG(INFO, "[none]");
		return ;
	}
	size = 0;
	sign = __get_sign(num);
	size += 1; // sign
	size += num->len * BNUM_DIGIT_BIT; // bit count
	size += (num->len * BNUM_DIGIT_BIT) / 4; // spaces between every 4 bits
	size += num->len - 1; // newlines between every word
	size += 1; // null character
	BNUM_ALLOC(str, size);

	tbytes = 0;
	for (int i = num->len-1; i >= 0; i--) {
		for (int j = BNUM_DIGIT_BIT; j-- > 0;) {
			wbytes = snprintf(str + tbytes, 2, "%u", (unsigned int)((num->val[i] >> j) & 0x1));
			if (wbytes < 0) {
				SSL_LOG(ERROR, "print error");
				return ;
			}
			tbytes += wbytes;
			if ((BNUM_DIGIT_BIT-j) % 4 == 0 && j != 0) {
				wbytes = snprintf(str + tbytes, 2, " ");
				if (wbytes < 0) {
					SSL_LOG(ERROR, "print error");
					return ;
				}
				tbytes += wbytes;
			}
		}
		if (i != 0) {
			wbytes = snprintf(str + tbytes, 2, "\n");
			if (wbytes < 0) {
				SSL_LOG(ERROR, "print error");
				return ;
			}
			tbytes += wbytes;
		}
	}

	bitcount = bnum_lmbit(num);
	if (prefix) {
		SSL_LOG(INFO, "%s: [len=%d, bits=%d, sign=(%c)]\n%s", prefix, num->len, bitcount, sign, str);
	} else {
		SSL_LOG(INFO, "[len=%d, bits=%d, sign=(%c)]\n%s", num->len, bitcount, sign, str);
	}
	BNUM_FREE(str);
}
