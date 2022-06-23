#ifndef BNUM_H
# define BNUM_H

# include <inttypes.h>
# include <stddef.h>
# include <sys/errno.h>
# include <libft.h>
# include <ft_printf.h>

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
# define BNUM_MAX_DIG	(1 << (BNUM_WORD_BIT - 2*BNUM_DIGIT_BIT))
# define BNUM_MAX_WDIG	(1 << (BNUM_WORD_BIT - 2*BNUM_DIGIT_BIT + 1))

# define BNUM_NEG		-1
# define BNUM_POS		1
# define BNUM_TRUE		1
# define BNUM_FALSE		0
# define BNUM_KAR_THRES	80

# define BNUM_MAX_DIG_COMBA		(1 << (BNUM_WORD_BIT - 2*BNUM_DIGIT_BIT))

# define BNUM_MAX(A,B)	((A)>(B)?(A):(B))
# define BNUM_MIN(A,B)	((A)<(B)?(A):(B))
# define BNUM_ZERO(X)	(((X)->len == 1u) && ((X)->val[0] == 0u))
# define BNUM_ONE(X)	(((X)->len == 1u) && ((X)->val[0] == 1u))
# define BNUM_EVEN(X)	(((X)->val[0] & 1u) == 0u)
# define BNUM_ODD(X)	(((X)->val[0] & 1u) == 1u)
# define BNUM_SIGN(X)	(((X)->sign == BNUM_NEG)?(BNUM_NEG):(BNUM_POS))

# define BNUM_ERROR_EXIT(MES) ({ \
	ft_printf("bnum error in '%s', %s:%d\n", __func__, __FILE__, __LINE__); \
	if (errno) \
		perror(MES); \
	else \
		ft_printf("%@%s\n", MES); \
	exit(0); \
})

typedef __uint128_t		t_uint128;
typedef __int128_t		int128_t;

typedef struct			s_num
{
	uint64_t		*val;
	size_t			size;
	int 			len;
	int				sign;
}					t_num;

void	hex_to_num(t_num *num, const char *hex);
char	*num_to_hex(t_num *num);
void	error(char const *);
void	out_of_memory(void);
void	division_by_zero(void);
void	not_implemented(char const *);
void	size_error(void);
void	print_num(const char *, const t_num *);
void	print_num_raw(const t_num *);
void	print_num_bits(const t_num *);
void	num_to_bytes(const t_num *, char **, size_t *);
void	bytes_to_num(t_num *, const char *, int);
void	abs_num(const t_num *, t_num *);
void	init_num(t_num *);
void	init_num_multi(t_num *, ...);
void	init_num_with_size(t_num *num, int size);
void	reset_num(t_num *);
void	clear_num(t_num *);
void	clear_num_multi(t_num *, ...);
void	increase_num_size(t_num *, size_t newsize);
void	set_num_d(t_num *, uint64_t);
void	set_randnum(t_num *, int);
void	copy_num(const t_num *, t_num *);
int		compare_num(const t_num *, const t_num *);
int		compare_num_u(const t_num *, const t_num *);
int		compare_num_d(const t_num *, uint64_t);
void	skip_zeros(t_num *);
int		lmbit_num(const t_num *);
int		rmbit_num(const t_num *);
void	swap_num(t_num *, t_num *);
void	set_bit(t_num *, int);
void	lsh_num_d_inpl(t_num *, int shifts);
void	rsh_num_d_inpl(t_num *, int shifts);
void	lsh_num_b_inpl(t_num *, int bits);
void	rsh_num_b_inpl(t_num *, int bits);
void	decrem_num_u(t_num *);
void	increm_num_u(t_num *);
void	add_num(const t_num *, const t_num *, t_num *);
void	add_num_u(const t_num *, const t_num *, t_num *);
void	add_num_d(const t_num *, uint64_t, t_num *);
void	add_num_ud(const t_num *, uint64_t, t_num *);
void	sub_num(const t_num *, const t_num *, t_num *);
void	sub_num_u(const t_num *, const t_num *, t_num *);
void	sub_num_d(const t_num *, uint64_t, t_num *);
void	sub_num_ud(const t_num *, uint64_t, t_num *);
void	mul_num(const t_num *, const t_num *, t_num *);
void	mul_num_comba(const t_num *, const t_num *, t_num *);
void	mul_num_comba_lo(const t_num *, const t_num *, t_num *, int);
void	mul_num_comba_hi(const t_num *, const t_num *, t_num *, int);
void	mul_num_karatsuba(const t_num *, const t_num *, t_num *);
void	mul_num_d(const t_num *, uint64_t, t_num *);
void	sqr_num(const t_num *, t_num *);
void	sqr_num_comba(const t_num *, t_num *);
void	sqr_num_karatsuba(const t_num *, t_num *);
void	divmod_num(const t_num *, const t_num *, t_num *, t_num *);
void	divmod_num_d(const t_num *, uint64_t, t_num *, uint64_t *);
void	div_num_2d_inpl(t_num *num);
void	montgomery_setup(const t_num *, uint64_t *);
void	montgomery_norm(const t_num *, t_num *);
void	montgomery_reduce(t_num *, const t_num *, uint64_t);
void	montgomery_fast_reduce(t_num *, const t_num *, uint64_t);
void	exp_num(const t_num *, uint64_t, t_num *);
void	exp2_num(t_num *, int);
void	powmod_num(const t_num *, const t_num *, const t_num *, t_num *);
void	m_powmod_num(const t_num *, const t_num *, const t_num *, t_num *);
void	gcd_num(const t_num *, const t_num *, t_num *);
void	lcm_num(const t_num *, const t_num *, t_num *);
void	invmod_num(const t_num *a, const t_num *b, t_num *res);
int		miller_rabin(const t_num *, const t_num *);
int		prime_test(const t_num *, int, int, int);

#endif
