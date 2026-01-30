#ifndef BNUM_H
# define BNUM_H

# include <common.h>
# include <inttypes.h>
# include <stddef.h>
# include <sys/errno.h>
# include <libft/std.h>
# include <libft/alloc.h>
# include <libft/logger.h>
# include <printnl.h>

# define SSL_PRIME_TEST

# define BNUM_WORD_BIT	128
# define BNUM_INT_BIT	64
# define BNUM_DIGIT_BIT	60
# define BNUM_PTAB_SIZE	256

# define BNUM_MAX_64	0xFFFFFFFFFFFFFFFF
# define BNUM_MSB_64	0x8000000000000000
# define BNUM_MAX_128	(~((__uint128_t)0))
# define BNUM_MSB_128	((__uint128_t)1 << 127)

# define BNUM_BASE		((uint64_t)1 << (BNUM_DIGIT_BIT))
# define BNUM_MAX_VAL	((BNUM_BASE) - 1)
# define BNUM_MSB_VAL	((BNUM_BASE) >> 1)

# define BNUM_MAX_DIG_COUNT  		((INT_MAX - 2) / BNUM_DIGIT_BIT)
# define BNUM_MIN_DIG_COUNT  		((BNUM_INT_BIT + BNUM_DIGIT_BIT) - 1) / BNUM_DIGIT_BIT
# define BNUM_MAX_WDIG_COUNT		(1 << (BNUM_WORD_BIT - 2*BNUM_DIGIT_BIT + 1))
# define BNUM_MAX_DIG_COUNT_COMBA	(1 << (BNUM_WORD_BIT - 2*BNUM_DIGIT_BIT))

# define BNUM_NEG		-1
# define BNUM_POS		1
# define BNUM_TRUE		1
# define BNUM_FALSE		0
# define BNUM_KAR_THRES	80


# define BNUM_MAX(A,B)	((A)>(B)?(A):(B))
# define BNUM_MIN(A,B)	((A)<(B)?(A):(B))
# define BNUM_ZERO(X)	(((X)->len == 1u) && ((X)->val[0] == 0u))
# define BNUM_ONE(X)	(((X)->len == 1u) && ((X)->val[0] == 1u))
# define BNUM_EVEN(X)	(((X)->val[0] & 1u) == 0u)
# define BNUM_ODD(X)	(((X)->val[0] & 1u) == 1u)
# define BNUM_SIGN(X)	(((X)->sign == BNUM_NEG)?(BNUM_NEG):(BNUM_POS))

# define BNUM_ALLOC(PTR, SZ)		LIBFT_ALLOC(PTR, SZ)
# define BNUM_REALLOC(PTR, SZ, NSZ)	LIBFT_REALLOC(PTR, SZ, NSZ)
# define BNUM_FREE(PTR)				LIBFT_FREE(PTR)

typedef __uint128_t		uint128_t;
typedef __int128_t		int128_t;

typedef struct			s_num
{
	uint64_t		*val;
	size_t			size;
	int 			len;
	int				sign;
}					t_num;

#define BNUM_SWAP_PTR(X, Y) \
	do                      \
	{                       \
		const t_num *T = X; \
		X = Y;              \
		Y = T;              \
	} while (0)

void    bnum_abs(const t_num *num, t_num *res);
void    bnum_add(const t_num *a, const t_num *b, t_num *res);
void    bnum_add_dig(const t_num *a, uint64_t b, t_num *res);
void    bnum_add_dig_u(const t_num *a, uint64_t b, t_num *res);
void    bnum_add_u(const t_num *a, const t_num *b, t_num *res);
void    bnum_clear(t_num *num);
void    bnum_clear_multi(t_num *num, ...);
int     bnum_cmp(const t_num *a, const t_num *b);
int     bnum_cmp_dig(const t_num *a, uint64_t b);
int     bnum_cmp_u(const t_num *a, const t_num *b);
void    bnum_copy(const t_num *src, t_num *dst);
t_num   *bnum_clone(const t_num *src);
t_num   *bnum_create(void);
void    bnum_decrem_u(t_num *num);
void    bnum_del(t_num *num);
void    bnum_div2_inpl(t_num *num);
void    bnum_divmod(const t_num *a, const t_num *b, t_num *res, t_num *mod);
void    bnum_divmod_dig(const t_num *a, uint64_t b, t_num *res, uint64_t *mod);
void    bnum_exp(const t_num *a, uint64_t b, t_num *res);
void    bnum_exp2(t_num *num, int bits);
void    bnum_from_bytes_u(t_num *num, const char *bytes, int len);
t_num   *bnum_from_dec(const char *dec);
t_num   *bnum_from_hex_u(const char *hex);
void    bnum_gcd(const t_num *a, const t_num *b, t_num *res);
void    bnum_increase_size(t_num *num, size_t newsize);
void    bnum_increm_u(t_num *num);
void    bnum_init(t_num *num);
void    bnum_init_multi(t_num *num, ...);
void    bnum_init_with_size(t_num *num, int size);
void    bnum_invmod(const t_num *a, const t_num *b, t_num *res);
void    bnum_lcm(const t_num *a, const t_num *b, t_num *res);
int     bnum_lmbit(const t_num *num);
void    bnum_lsh_bit_inpl(t_num *num, int bits);
void    bnum_lsh_dig_inpl(t_num *num, int shifts);
void    bnum_m_powmod(const t_num *a, const t_num *b, const t_num *c, t_num *res);
int     bnum_miller_rabin(const t_num *a, const t_num *b);
void    bnum_montgomery_fast_reduce(t_num *num, const t_num *mod, uint64_t m);
void    bnum_montgomery_norm(const t_num *a, t_num *res);
void    bnum_montgomery_reduce(t_num *num, const t_num *mod, uint64_t m);
void    bnum_montgomery_setup(const t_num *mod, uint64_t *m);
void    bnum_mul(const t_num *a, const t_num *b, t_num *res);
void    bnum_mul_comba(const t_num *a, const t_num *b, t_num *res);
void    bnum_mul_comba_hi(const t_num *a, const t_num *b, t_num *res, int dig);
void    bnum_mul_comba_lo(const t_num *a, const t_num *b, t_num *res, int dig);
void    bnum_mul_dig(const t_num *a, uint64_t b, t_num *res);
void    bnum_mul_karatsuba(const t_num *a, const t_num *b, t_num *res);
void    bnum_powmod(const t_num *a, const t_num *b, const t_num *c, t_num *res);
int     bnum_prime_test(const t_num *a, int test_count, int test_size, int test_type);
void    bnum_print(const char *func_name, const t_num *num);
void    bnum_print_bits(const char *func_name, const t_num *num);
void    bnum_print_raw(const t_num *num);
void    bnum_bzero(t_num *num);
int     bnum_rmbit(const t_num *num);
void    bnum_rsh_bit_inpl(t_num *num, int bits);
void    bnum_rsh_dig_inpl(t_num *num, int shifts);
void    bnum_set_bit(t_num *num, int bit);
void    bnum_set_dig_u(t_num *num, uint64_t dig);
void    bnum_set_rand(t_num *num, int rand);
void    bnum_skip_zeros(t_num *num);
void    bnum_sqr(const t_num *num, t_num *res);
void    bnum_sqr_comba(const t_num *num, t_num *res);
void    bnum_sqr_karatsuba(const t_num *num, t_num *res);
void    bnum_sub(const t_num *a, const t_num *b, t_num *res);
void    bnum_sub_dig(const t_num *a, uint64_t b, t_num *res);
void    bnum_sub_dig_u(const t_num *a, uint64_t b, t_num *res);
void    bnum_sub_u(const t_num *a, const t_num *b, t_num *res);
void    bnum_swap_values(t_num *a, t_num *b);
void    bnum_to_bytes_u(const t_num *num, char **bytes, size_t *len);
char    *bnum_to_dec(const t_num *num);
char    *bnum_to_hex_u(const t_num *num);

#endif
