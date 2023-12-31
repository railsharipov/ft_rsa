#include <ft_ssl.h>
#include <ssl_rand.h>
#include <bnum.h>
#include <ssl_test.h>
#include <printnl.h>

static t_num	__zero_num;
static t_num	__one_num;

static int	__test_bnum_setup(void);
static void	__test_bnum_cleanup(void);
static int	__test_bbnum_to_hex_u(void);
static int	__test_bnum_from_hex(void);
static int	__test_bnum_abs(void);
static int	__test_bnum_add_dig_u(void);
static int	__test_bnum_sub_dig_u(void);
static int	__test_bnum_add_dig(void);
static int	__test_bnum_sub_dig(void);
static int	__test_bnum_add_u(void);
static int	__test_bnum_sub_u(void);
static int	__test_bnum_add(void);
static int	__test_bnum_sub(void);
static int	__test_bnum_increm_u(void);
static int	__test_bnum_decrem_u(void);
static int	__test_bnum_bytes_to(void);
static int	__test_bnum_to_bytes_u(void);
static int	__test_bnum_bnum_skip_zeros(void);
static int	__test_bnum_bnum_lsh_dig_inpl(void);
static int	__test_bnum_bnum_rsh_dig_inpl(void);
static int	__test_bnum_bnum_lsh_bit_inpl(void);
static int	__test_bnum_bnum_rsh_bit_inpl(void);
static int	__test_bnum_bnum_cmp_dig(void);
static int	__test_bnum_cmp_u(void);
static int	__test_bnum_cmp(void);
static int	__test_bnum_copy(void);
static int	__test_bnum_mul(void);
static int	__test_bnum_sqr(void);
static int	__test_bnum_div_num_2d_inpl(void);
static int	__test_bnum_divmod(void);
static int	__test_bnum_divmod_dig(void);
static int	__test_bnum_exp(void);
static int	__test_bnum_bnum_exp2(void);
static int	__test_bnum_gcd(void);
static int	__test_bnum_invmod(void);
static int	__test_bnum_lcm(void);
static int	__test_bnum_lmbit(void);
static int	__test_bnum_powmod(void);
static int	__test_bnum_m_powmod(void);

int	test_bnum(void)
{
	int	res;

	if (SSL_OK != __test_bnum_setup())
		return (SSL_ERROR(UNSPECIFIED_ERROR));

	res = __test_bbnum_to_hex_u();
	res |= __test_bnum_from_hex();
	res |= __test_bnum_abs();
	res |= __test_bnum_add_dig_u();
	res |= __test_bnum_sub_dig_u();
	res |= __test_bnum_add_dig();
	res |= __test_bnum_sub_dig();
	res |= __test_bnum_add();
	res |= __test_bnum_sub();
	res |= __test_bnum_increm_u();
	res |= __test_bnum_decrem_u();
	res |= __test_bnum_bytes_to();
	res |= __test_bnum_to_bytes_u();
	res |= __test_bnum_bnum_skip_zeros();
	res |= __test_bnum_bnum_lsh_dig_inpl();
	res |= __test_bnum_bnum_rsh_dig_inpl();
	res |= __test_bnum_bnum_lsh_bit_inpl();
	res |= __test_bnum_bnum_rsh_bit_inpl();
	res |= __test_bnum_bnum_cmp_dig();
	res |= __test_bnum_cmp_u();
	res |= __test_bnum_cmp();
	res |= __test_bnum_copy();
	res |= __test_bnum_mul();
	res |= __test_bnum_sqr();
	res |= __test_bnum_div_num_2d_inpl();
	res |= __test_bnum_divmod();
	res |= __test_bnum_divmod_dig();
	res |= __test_bnum_exp();
	res |= __test_bnum_bnum_exp2();
	res |= __test_bnum_gcd();
	res |= __test_bnum_invmod();
	res |= __test_bnum_lcm();
	res |= __test_bnum_lmbit();
	res |= __test_bnum_powmod();
	res |= __test_bnum_m_powmod();

	__test_bnum_cleanup();

	return (res);
}

static int	__test_bnum_setup(void)
{
	t_num		n1;
	t_num		n2;
	uint64_t	digit;

	bnum_init(&__zero_num);
	bnum_set_dig_u(&__zero_num, 0);

	bnum_init(&__one_num);
	bnum_set_dig_u(&__one_num, 1);

	return (SSL_OK);
}

static void	__test_bnum_cleanup(void)
{
	bnum_clear(&__zero_num);
	bnum_clear(&__one_num);
}

static int	__test_bbnum_to_hex_u(void)
{
	t_num		test_num;
	char		digit_hex[16+1];
	uint64_t	digit;
	char		*test_hex, *zero_test_hex, *ref_hex, *ref_hex_ptr;
	size_t		hexsize, digit_hexsize;
	int			idx, pass;

	pass = SSL_OK;

	bnum_init(&test_num);
	bnum_set_rand(&test_num, 1709);

	hexsize = 2 * NBITS_TO_NBYTES(test_num.len * BNUM_DIGIT_BIT);
	SSL_ALLOC(ref_hex, hexsize+1);
	ref_hex_ptr = ref_hex;

	idx = test_num.len - 1;

	digit = test_num.val[idx--];
	digit_hexsize = snprintf(digit_hex, sizeof(digit_hex), "%llx", digit);
	ft_strncpy(ref_hex_ptr, digit_hex, digit_hexsize);
	ref_hex_ptr += digit_hexsize;

	while (idx >= 0)
	{
		digit = test_num.val[idx--];
		digit_hexsize = snprintf(digit_hex, sizeof(digit_hex), "%.15llx", digit);
		ft_strncpy(ref_hex_ptr, digit_hex, digit_hexsize);
		ref_hex_ptr += digit_hexsize;
	}

	test_hex = bnum_to_hex_u(&test_num);
	zero_test_hex = bnum_to_hex_u(&__zero_num);

	pass = TEST_ASSERT(ft_strcmp(ref_hex, test_hex) == 0);
	pass |= TEST_ASSERT(ft_strcmp("0", zero_test_hex) == 0);

	bnum_clear(&test_num);
	SSL_FREE(ref_hex);
	SSL_FREE(test_hex);
	SSL_FREE(zero_test_hex);

	if (SSL_OK != pass)
		return (TEST_FAIL());

	return (TEST_PASS());
}

static int	__test_bnum_from_hex(void)
{
	t_num	test_num, tmp_num;
	char	*test_hex, *zero_test_hex, *ref_hex;
	int		pass;

	pass = SSL_OK;

	bnum_init(&test_num);
	bnum_init(&tmp_num);

	ref_hex = bnum_to_hex_u(&test_num);

	bnum_from_hex(&tmp_num, ref_hex);
	test_hex = bnum_to_hex_u(&tmp_num);

	bnum_from_hex(&tmp_num, "0");
	zero_test_hex = bnum_to_hex_u(&tmp_num);

	pass = TEST_ASSERT(ft_strcmp(test_hex, ref_hex) == 0);
	pass |= TEST_ASSERT(ft_strcmp(zero_test_hex, "0") == 0);

	bnum_clear(&test_num);
	bnum_clear(&tmp_num);
	SSL_FREE(ref_hex);
	SSL_FREE(test_hex);
	SSL_FREE(zero_test_hex);

	if (SSL_OK != pass)
		return (TEST_FAIL());

	return (TEST_PASS());
}

static int	__test_bnum_abs(void)
{
	char	*hex;
	t_num	test_num, res_num;
	int		pass;

	pass = SSL_OK;

	bnum_init(&test_num);
	bnum_init(&res_num);

	test_num.sign = BNUM_NEG;
	bnum_abs(&test_num, &res_num);

	pass = TEST_ASSERT(test_num.len == res_num.len);
	pass |= TEST_ASSERT(res_num.sign == BNUM_POS);
	pass |= TEST_ASSERT(
		ft_memcmp(test_num.val, res_num.val, res_num.len * sizeof(uint64_t)) == 0);

	bnum_clear(&test_num);
	bnum_clear(&res_num);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_add_dig_u(void)
{
	const char	*n1_hex = "179672f4b9ce4ff1ce809d56dbdc138ec5c";
	const char	*ref_hex = "179672f4b9ce4ff1ce86302d024b7dcf569";
	char		*test_hex;
	uint64_t	digit = 0x592d6266f6a4090d;
	t_num		n1, test_num;
	int			pass;

	pass = SSL_OK;

	bnum_init(&n1);
	bnum_from_hex(&n1, n1_hex);

	bnum_init(&test_num);
	bnum_add_dig_u(&n1, digit, &test_num);
	test_hex = bnum_to_hex_u(&test_num);

	pass = TEST_ASSERT(ft_strcmp(test_hex, ref_hex) == 0);

	bnum_clear(&n1);
	bnum_clear(&test_num);
	SSL_FREE(test_hex);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_sub_dig_u(void)
{
	const char	*n1_hex = "179672f4b9ce4ff1ce80000000000000000";
	const char	*ref_hex = "179672f4b9ce4ff1ce7a6d29d99095bf6f3";
	char		*test_hex;
	uint64_t	digit = 0x592d6266f6a4090d;
	t_num		n1, test_num;
	int			pass;

	pass = SSL_OK;

	bnum_init(&n1);
	bnum_from_hex(&n1, n1_hex);

	bnum_init(&test_num);
	bnum_sub_dig_u(&n1, digit, &test_num);
	test_hex = bnum_to_hex_u(&test_num);

	pass = TEST_ASSERT(ft_strcmp(test_hex, ref_hex) == 0);

	bnum_clear(&n1);
	bnum_clear(&test_num);
	SSL_FREE(test_hex);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_add_dig(void)
{
	const char	*n1_hex = "179672f4b9ce4ff1ce809d56dbdc138ec5c";
	const char	*ref_hex = "179672f4b9ce4ff1ce86302d024b7dcf569";
	const char	*ref_hex2 = "179672f4b9ce4ff1ce7b0a80b56ca94e34f";
	char		*test_hex, *test_hex2;
	uint64_t	digit = 0x592d6266f6a4090d;
	t_num		n1, test_num, test_num2;
	int			pass;

	pass = SSL_OK;

	bnum_init(&n1);
	bnum_from_hex(&n1, n1_hex);

	bnum_init(&test_num);
	bnum_add_dig(&n1, digit, &test_num);
	test_hex = bnum_to_hex_u(&test_num);

	bnum_init(&test_num2);
	n1.sign = BNUM_NEG;
	bnum_add_dig(&n1, digit, &test_num2);
	test_hex2 = bnum_to_hex_u(&test_num2);

	pass = TEST_ASSERT(ft_strcmp(test_hex, ref_hex) == 0);
	pass |= TEST_ASSERT(ft_strcmp(test_hex2, ref_hex2) == 0);

	bnum_clear(&n1);
	bnum_clear(&test_num);
	SSL_FREE(test_hex);
	SSL_FREE(test_hex2);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_sub_dig(void)
{
	const char	*n1_hex = "179672f4b9ce4ff1ce809d56dbdc138ec5c";
	const char	*ref_hex = "179672f4b9ce4ff1ce7b0a80b56ca94e34f";
	const char	*ref_hex2 = "179672f4b9ce4ff1ce86302d024b7dcf569";
	char		*test_hex, *test_hex2;
	uint64_t	digit = 0x592d6266f6a4090d;
	t_num		n1, test_num, test_num2;
	int			pass;

	pass = SSL_OK;

	bnum_init(&n1);
	bnum_from_hex(&n1, n1_hex);

	bnum_init(&test_num);
	bnum_sub_dig(&n1, digit, &test_num);
	test_hex = bnum_to_hex_u(&test_num);

	bnum_init(&test_num2);
	n1.sign = BNUM_NEG;
	bnum_sub_dig(&n1, digit, &test_num2);
	test_hex2 = bnum_to_hex_u(&test_num2);

	pass = TEST_ASSERT(ft_strcmp(test_hex, ref_hex) == 0);
	pass |= TEST_ASSERT(ft_strcmp(test_hex2, ref_hex2) == 0);

	bnum_clear(&n1);
	bnum_clear(&test_num);
	SSL_FREE(test_hex);
	SSL_FREE(test_hex2);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_add(void)
{
	const char	*n1_hex = "179672f4b9ce4ff1ce809d56dbdc138ec5c";
	const char	*n2_hex = "2a01e004af52dce303c7b6959facb5191493";
	const char	*ref_hex = "2b7b4733faefc1e220afc06b0d6a765200ef";
	const char	*ref_hex2 = "288878d563b5f7e3e6dfacc031eef3e02837";
	char		*test_hex, *test_hex2;
	t_num		n1, n2, test_num, test_num2;
	int			pass;

	pass = SSL_OK;

	bnum_init_multi(&n1, &n2, &test_num, &test_num2, NULL);

	bnum_from_hex(&n1, n1_hex);
	bnum_from_hex(&n2, n2_hex);

	bnum_add(&n1, &n2, &test_num);
	test_hex = bnum_to_hex_u(&test_num);

	n1.sign = BNUM_NEG;
	bnum_add(&n1, &n2, &test_num2);
	test_hex2 = bnum_to_hex_u(&test_num2);

	pass = TEST_ASSERT(ft_strcmp(test_hex, ref_hex) == 0);
	pass |= TEST_ASSERT(ft_strcmp(test_hex2, ref_hex2) == 0);
	pass |= TEST_ASSERT(test_num.sign == BNUM_POS);
	pass |= TEST_ASSERT(test_num2.sign == BNUM_POS);

	bnum_clear_multi(&n1, &n2, &test_num, &test_num2, NULL);

	SSL_FREE(test_hex);
	SSL_FREE(test_hex2);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_sub(void)
{
	const char	*n1_hex = "179672f4b9ce4ff1ce809d56dbdc138ec5c";
	const char	*n2_hex = "2a01e004af52dce303c7b6959facb5191493";
	const char	*ref_hex = "288878d563b5f7e3e6dfacc031eef3e02837";
	const char	*ref_hex2 = "2b7b4733faefc1e220afc06b0d6a765200ef";
	char		*test_hex, *test_hex2;
	t_num		n1, n2, test_num, test_num2;
	int			pass;

	pass = SSL_OK;

	bnum_init_multi(&n1, &n2, &test_num, &test_num2, NULL);

	bnum_from_hex(&n1, n1_hex);
	bnum_from_hex(&n2, n2_hex);

	bnum_sub(&n1, &n2, &test_num);
	test_hex = bnum_to_hex_u(&test_num);

	n1.sign = BNUM_NEG;
	bnum_sub(&n1, &n2, &test_num2);
	test_hex2 = bnum_to_hex_u(&test_num2);

	pass = TEST_ASSERT(ft_strcmp(test_hex, ref_hex) == 0);
	pass |= TEST_ASSERT(ft_strcmp(test_hex2, ref_hex2) == 0);
	pass |= TEST_ASSERT(test_num.sign == BNUM_NEG);
	pass |= TEST_ASSERT(test_num2.sign == BNUM_NEG);

	bnum_clear_multi(&n1, &n2, &test_num, &test_num2, NULL);

	SSL_FREE(test_hex);
	SSL_FREE(test_hex2);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_increm_u(void)
{
	const char	*hex = "179672f4b9ce4ff1ce809d56dbdc138ec5c";
	const char	*ref_hex = "179672f4b9ce4ff1ce809d56dbdc138ec5d";
	char		*test_hex;
	t_num		test_num;
	int			pass;

	pass = SSL_OK;

	bnum_init(&test_num);
	bnum_from_hex(&test_num, hex);

	bnum_increm_u(&test_num);
	test_hex = bnum_to_hex_u(&test_num);

	pass = TEST_ASSERT(ft_strcmp(test_hex, ref_hex) == 0);

	bnum_clear(&test_num);
	SSL_FREE(test_hex);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_decrem_u(void)
{
	const char	*hex = "179672f4b9ce4ff1ce809d56dbdc138ec5c";
	const char	*ref_hex = "179672f4b9ce4ff1ce809d56dbdc138ec5b";
	char		*test_hex;
	t_num		test_num;
	int			pass;

	pass = SSL_OK;

	bnum_init(&test_num);
	bnum_from_hex(&test_num, hex);

	bnum_decrem_u(&test_num);
	test_hex = bnum_to_hex_u(&test_num);

	pass = TEST_ASSERT(ft_strcmp(test_hex, ref_hex) == 0);

	bnum_clear(&test_num);
	SSL_FREE(test_hex);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_bytes_to(void)
{
	const char	*hex = "179672f4b9ce4ff1ce809d56dbdc138ec5c";
	char		*test_hex, *bin;
	size_t		binsize, hexsize;
	t_num		test_num;
	int			pass;

	pass = SSL_OK;

	hexsize = ft_strlen(hex);
	binsize = NBITS_TO_NBYTES(4 * hexsize);
	SSL_ALLOC(bin, binsize);

	ft_hexbin(bin, hex, hexsize);

	bnum_init(&test_num);
	bnum_bytes_to(&test_num, bin, binsize);
	test_hex = bnum_to_hex_u(&test_num);

	pass = TEST_ASSERT(ft_strcmp(hex, test_hex) == 0);

	bnum_clear(&test_num);
	SSL_FREE(test_hex);
	SSL_FREE(bin);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_to_bytes_u(void)
{
	const char	*hex = "179672f4b9ce4ff1ce809d56dbdc138ec5c";
	char		*bin, *test_hex;
	size_t		binsize;
	t_num		test_num;
	int			pass;

	pass = SSL_OK;

	bnum_init(&test_num);
	bnum_from_hex(&test_num, hex);
	bnum_to_bytes_u(&test_num, &bin, &binsize);

	test_hex = ft_binhex(bin, binsize);

	pass = TEST_ASSERT(ft_strcmp(hex, test_hex) == 0);

	bnum_clear(&test_num);
	SSL_FREE(test_hex);
	SSL_FREE(bin);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_bnum_skip_zeros(void)
{
	const char	*hex = "179672f4b9ce4ff1ce809d56dbdc138ec5c";
	char		*test_hex;
	t_num		test_num;
	int			pass;

	pass = SSL_OK;

	bnum_init(&test_num);
	bnum_from_hex(&test_num, hex);
	test_num.len += 1;
	bnum_skip_zeros(&test_num);
	test_hex = bnum_to_hex_u(&test_num);

	pass = TEST_ASSERT(ft_strcmp(hex, test_hex) == 0);

	bnum_clear(&test_num);
	SSL_FREE(test_hex);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_bnum_lsh_dig_inpl(void)
{
	const char	*hex = "179672f4b9ce4ff1ce809d56dbdc138ec5c";
	const char	*refhex = "179672f4b9ce4ff1ce809d56dbdc138ec5c000000000000000";
	char		*test_hex;
	t_num		test_num;
	int			pass;

	pass = SSL_OK;

	bnum_init(&test_num);
	bnum_from_hex(&test_num, hex);

	bnum_lsh_dig_inpl(&test_num, 1);
	test_hex = bnum_to_hex_u(&test_num);

	pass = TEST_ASSERT(ft_strcmp(refhex, test_hex) == 0);

	bnum_clear(&test_num);
	SSL_FREE(test_hex);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_bnum_rsh_dig_inpl(void)
{
	const char	*hex = "179672f4b9ce4ff1ce809d56dbdc138ec5c";
	const char	*refhex = "179672f4b9ce4ff1ce80";
	char		*test_hex;
	t_num		test_num;
	int			pass;

	pass = SSL_OK;

	bnum_init(&test_num);
	bnum_from_hex(&test_num, hex);

	bnum_rsh_dig_inpl(&test_num, 1);
	test_hex = bnum_to_hex_u(&test_num);

	pass = TEST_ASSERT(ft_strcmp(refhex, test_hex) == 0);

	bnum_clear(&test_num);
	SSL_FREE(test_hex);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_bnum_lsh_bit_inpl(void)
{
	const char	*hex = "179672f4b9ce4ff1ce809d56dbdc138ec5c";
	const char	*refhex = "bcb397a5ce727f8e7404eab6dee09c762e00000000000000000";
	char		*test_hex;
	t_num		test_num;
	int			pass;

	pass = SSL_OK;

	bnum_init(&test_num);
	bnum_from_hex(&test_num, hex);

	bnum_lsh_bit_inpl(&test_num, 67);
	test_hex = bnum_to_hex_u(&test_num);

	pass = TEST_ASSERT(ft_strcmp(refhex, test_hex) == 0);

	bnum_clear(&test_num);
	SSL_FREE(test_hex);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_bnum_rsh_bit_inpl(void)
{
	const char	*hex = "179672f4b9ce4ff1ce809d56dbdc138ec5c";
	const char	*refhex = "2f2ce5e9739c9fe39d";
	char		*test_hex;
	t_num		test_num;
	int			pass;

	pass = SSL_OK;

	bnum_init(&test_num);
	bnum_from_hex(&test_num, hex);

	bnum_rsh_bit_inpl(&test_num, 67);
	test_hex = bnum_to_hex_u(&test_num);

	pass = TEST_ASSERT(ft_strcmp(refhex, test_hex) == 0);

	bnum_clear(&test_num);
	SSL_FREE(test_hex);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_bnum_cmp_dig(void)
{
	const char	*hex = "179672f4b9ce4ff1ce809d56dbdc138ec5caaa123123";
	const char	*hex2 = "179672f4b9ce4ff1ce809d56dbd";
	const char	*hex3 = "4b9ce4ff1ce9";
	t_num		test_num, test_num2, test_num3;
	int			res, pass;

	pass = SSL_OK;

	bnum_init_multi(&test_num, &test_num2, &test_num3, NULL);
	bnum_from_hex(&test_num, hex);
	bnum_from_hex(&test_num2, hex2);
	bnum_from_hex(&test_num3, hex3);

	res = bnum_cmp_dig(&test_num, 0x972f4b9ce4ff1ce8);
	pass = TEST_ASSERT(res > 0);

	res = bnum_cmp_dig(&test_num, 0x4b9ce4ff1ce7);
	pass |= TEST_ASSERT(res > 0);

	test_num.sign = BNUM_NEG;
	res = bnum_cmp_dig(&test_num, 0x72f4b9ce4ff1ce8);
	pass |= TEST_ASSERT(res < 0);

	res = bnum_cmp_dig(&test_num2, 0x972f4b9ce4ff1ce8);
	pass |= TEST_ASSERT(res > 0);

	res = bnum_cmp_dig(&test_num2, 0x4b9ce4ff1ce7);
	pass |= TEST_ASSERT(res > 0);

	res = bnum_cmp_dig(&test_num3, 0x972f4b9ce4ff1ce8);
	pass |= TEST_ASSERT(res < 0);

	res = bnum_cmp_dig(&test_num3, 0x4b9ce4ff1ce7);
	pass |= TEST_ASSERT(res > 0);

	res = bnum_cmp_dig(&test_num3, 0x4b9ce4ff1cff);
	pass |= TEST_ASSERT(res < 0);

	res = bnum_cmp_dig(&test_num3, 0x4b9ce4ff1ce9);
	pass |= TEST_ASSERT(res == 0);

	bnum_clear_multi(&test_num, &test_num2, &test_num3, NULL);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_cmp_u(void)
{
	const char	*hex = "179672f4b9ce4ff1ce809d56dbdc138ec5caaa123123";
	const char	*hex2 = "179672f4b9ce4ff1ce809d56dbd";
	t_num		test_num, test_num2;
	int			res, pass;

	pass = SSL_OK;

	bnum_init_multi(&test_num, &test_num2, NULL);
	bnum_from_hex(&test_num, hex);
	bnum_from_hex(&test_num2, hex2);

	res = bnum_cmp_u(&test_num, &test_num2);
	pass = TEST_ASSERT(res > 0);

	res = bnum_cmp_u(&test_num2, &test_num);
	pass |= TEST_ASSERT(res < 0);

	res = bnum_cmp_u(&test_num, &test_num);
	pass |= TEST_ASSERT(res == 0);

	bnum_clear_multi(&test_num, &test_num2, NULL);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_cmp(void)
{
	const char	*hex = "179672f4b9ce4ff1ce809d56dbdc138ec5caaa123123";
	const char	*hex2 = "179672f4b9ce4ff1ce809d56dbd";
	t_num		test_num, test_num2;
	int			res, pass;

	pass = SSL_OK;

	bnum_init_multi(&test_num, &test_num2, NULL);
	bnum_from_hex(&test_num, hex);
	bnum_from_hex(&test_num2, hex2);

	test_num.sign = BNUM_POS;
	test_num2.sign = BNUM_POS;
	res = bnum_cmp(&test_num, &test_num2);
	pass = TEST_ASSERT(res > 0);

	test_num.sign = BNUM_NEG;
	test_num2.sign = BNUM_NEG;
	res = bnum_cmp(&test_num, &test_num2);
	pass |= TEST_ASSERT(res < 0);

	test_num.sign = BNUM_NEG;
	test_num2.sign = BNUM_POS;
	res = bnum_cmp(&test_num, &test_num2);
	pass |= TEST_ASSERT(res < 0);

	test_num.sign = BNUM_POS;
	test_num2.sign = BNUM_NEG;
	res = bnum_cmp(&test_num, &test_num2);
	pass |= TEST_ASSERT(res > 0);

	test_num.sign = BNUM_NEG;
	res = bnum_cmp(&test_num, &test_num);
	pass |= TEST_ASSERT(res == 0);

	test_num.sign = BNUM_POS;
	res = bnum_cmp(&test_num, &test_num);
	pass |= TEST_ASSERT(res == 0);

	bnum_clear_multi(&test_num, &test_num2, NULL);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_copy(void)
{
	const char	*hex = "179672f4b9ce4ff1ce809d56dbdc138ec5caaa123123";
	t_num	num, copy;
	int		pass;

	pass = SSL_OK;

	bnum_init(&num);
	bnum_init(&copy);

	bnum_from_hex(&num, hex);
	bnum_copy(&num, &copy);

	pass = TEST_ASSERT(bnum_cmp(&num, &copy) == 0);

	bnum_clear(&num);
	bnum_clear(&copy);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_mul(void)
{
	const char		*num1_hex =	"d9f2c6197c1610f216e06def0f65e4d7b34c737cb667fc"\
								"745f08e7f0270c4358e73cc2bf501b20bd4c8ca63659be"\
								"0347edc9c63275cf93c3267b25ce953d0da55647c3bc12"\
								"7cbba3abcedd08632253a5558c77acbf2989a975bb0b66"\
								"017f718cba54627855b848fd1ef695b764f6a86538dc67"\
								"5fdf208ace577ca087e49f433d";
	const char		*num2_hex =	"f937d6f312350b810bb40d0eaf9c7b2d77f084f455264a"\
								"b53b7e7ba204f9c4cc52cce6f844cc5bb3877a65efadb9"\
								"d6bd3f361b1b81de3db461d443926024705f";
	const char		*ref_hex =	"d42cacd73bb6b220712f8b53aff0919a37bcc5a11b280c"\
								"23ab413a2c65786bd60184199c9d94a11a7b6f6ab55b59"\
								"bfdf5d655acdad2c6e2cd5855b0861f825951418e63df4"\
								"22adf8118bc99922864cf23ebca0dad40249bbc97c81bc"\
								"91a25604f983b5247ba4d7a3aa853ef3bc3cdb041d12de"\
								"3a827c1b5bd8d623399a9f5187e83e1208c389fdb34a61"\
								"cdefc730bff1b95204c750cc73265d876344e562010961"\
								"71df2dade99e16b7445ccf764e5355ebc503e29fc86d2a"\
								"634435d9d918a3a3";
	t_num			num1, num2, test_num, ref_num;
	int				pass;

	pass = SSL_OK;

	bnum_init_multi(&num1, &num2, &test_num, &ref_num, NULL);

	bnum_from_hex(&num1, num1_hex);
	bnum_from_hex(&num2, num2_hex);
	bnum_from_hex(&ref_num, ref_hex);

	bnum_mul_comba(&num1, &num2, &test_num);
	pass = TEST_ASSERT(bnum_cmp(&test_num, &ref_num) == 0);

	bnum_mul(&num1, &num2, &test_num);
	pass = TEST_ASSERT(bnum_cmp(&test_num, &ref_num) == 0);

	bnum_exp(&num1, 10u, &num1);
	bnum_exp(&num2, 10u, &num2);
	bnum_mul_karatsuba(&num1, &num2, &test_num);
	bnum_mul_comba(&num1, &num2, &ref_num);
	pass |= TEST_ASSERT(bnum_cmp(&test_num, &ref_num) == 0);

	bnum_mul(&num1, &num2, &test_num);
	pass |= TEST_ASSERT(bnum_cmp(&test_num, &ref_num) == 0);

	bnum_clear_multi(&num1, &num2, &test_num, &ref_num, NULL);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_sqr(void)
{
	const char		*num_hex =	"d9f2c6197c1610f216e06def0f65e4d7b34c737cb667fc"\
								"745f08e7f0270c4358e73cc2bf501b20bd4c8ca63659be"\
								"0347edc9c63275cf93c3267b25ce953d0da55647c3bc12"\
								"7cbba3abcedd08632253a5558c77acbf2989a975bb0b66"\
								"017f718cba54627855b848fd1ef695b764f6a86538dc67"\
								"5fdf208ace577ca087e49f433d";
	const char		*ref_hex =	"b98d7a1255d770ee0f26e7ac32db9ffac1c5608ed8d306"\
								"58576b194109727d5174388c62912cd93c6c92bb592526"\
								"ff8f69d3acafc205be87c588fdf5fa6e4203c875d9f5fe"\
								"56930cd03f69ab9139b99095c563b9a65cfdf108f8e500"\
								"938517c338a7caa84f339d500eafdeae9b67a84f906fb5"\
								"15c5ea5b73c8f4026946711b2ecda750112ba99917322d"\
								"93c2faf691c7b8f7a261b7f939c60a846365e0ad623fe7"\
								"7c8d24da2428a392a78966e35176923d5dd71c97e9e8ac"\
								"26d50881e50f720038dd9bca487e4ab761b45952ac6349"\
								"ef860072f80211c2298de6d500220cab11d894b06faf03"\
								"7f837543a30f5fae579440f86b32ffc1da1299493d2f3f"\
								"6efc89";
	t_num			num, test_num, ref_num;
	int				pass;

	pass = SSL_OK;

	bnum_init_multi(&num, &test_num, &ref_num, NULL);

	bnum_from_hex(&num, num_hex);
	bnum_from_hex(&ref_num, ref_hex);

	bnum_sqr_comba(&num, &test_num);
	pass = TEST_ASSERT(bnum_cmp(&test_num, &ref_num) == 0);

	bnum_sqr(&num, &test_num);
	pass = TEST_ASSERT(bnum_cmp(&test_num, &ref_num) == 0);

	bnum_exp(&num, 10u, &num);
	bnum_sqr_karatsuba(&num, &test_num);
	bnum_sqr_comba(&num, &ref_num);
	pass |= TEST_ASSERT(bnum_cmp(&test_num, &ref_num) == 0);

	bnum_sqr(&num, &test_num);
	pass |= TEST_ASSERT(bnum_cmp(&test_num, &ref_num) == 0);

	bnum_clear_multi(&num, &test_num, &ref_num, NULL);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_div_num_2d_inpl(void)
{
	const char	*hex = "179672f4b9ce4ff1ce809d56dbdc138ec5caaa123123";
	const char	*ref_hex = "bcb397a5ce727f8e7404eab6dee09c762e555091891";
	t_num	num, ref_num;
	int		pass;

	pass = SSL_OK;

	bnum_init(&num);
	bnum_init(&ref_num);

	bnum_from_hex(&num, hex);
	bnum_from_hex(&ref_num, ref_hex);
	bnum_div2_inpl(&num);

	pass = TEST_ASSERT(bnum_cmp(&num, &ref_num) == 0);

	bnum_set_dig_u(&num, 0);
	bnum_div2_inpl(&num);

	pass |= TEST_ASSERT(bnum_cmp(&num, &__zero_num) == 0);

	bnum_clear(&num);
	bnum_clear(&ref_num);


	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_divmod(void)
{
	const char	*num_hex = "179672f4b9ce4ff1ce809d56dbdc138ec5caaa123123";
	const char	*div_hex = "21b40e90bfd2b6c5af67adaeb529f5ab837c";
	const char	*ref_quot_hex = "b32a27bb";
	const char	*ref_rem_hex = "132fd09e5fa8b7c3485487add0a3ce69418f";
	t_num		num, div_num, quot_num, rem_num, ref_quot_num, ref_rem_num;
	int			pass;

	pass = SSL_OK;

	bnum_init_multi(
		&num, &div_num, &quot_num, &rem_num, &ref_quot_num, &ref_rem_num, NULL);

	bnum_from_hex(&num, num_hex);
	bnum_from_hex(&div_num, div_hex);
	bnum_from_hex(&ref_quot_num, ref_quot_hex);
	bnum_from_hex(&ref_rem_num, ref_rem_hex);

	bnum_divmod(&num, &div_num, &quot_num, &rem_num);

	pass = TEST_ASSERT(bnum_cmp(&quot_num, &ref_quot_num) == 0);
	pass |= TEST_ASSERT(bnum_cmp(&rem_num, &ref_rem_num) == 0);

	bnum_set_dig_u(&num, 0);
	bnum_divmod(&num, &div_num, &quot_num, &rem_num);

	pass |= TEST_ASSERT(bnum_cmp(&quot_num, &__zero_num) == 0);
	pass |= TEST_ASSERT(bnum_cmp(&rem_num, &__zero_num) == 0);

	bnum_clear_multi(
		&num, &div_num, &quot_num, &rem_num, &ref_quot_num, &ref_rem_num, NULL);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_divmod_dig(void)
{
	const char		*num_hex = "179672f4b9ce4ff1ce809d56dbdc138ec5caaa123123";
	const char		*ref_quot_hex = "27f0dc4afffdef09c46e401389f3";
	const uint64_t	div_digit = 0x972f4b9ce4ff1ce8;
	const uint64_t	ref_rem_digit = 0x18663777a03998eb;
	t_num			num, quot_num, ref_quot_num;
	uint64_t		rem_digit;
	int				pass;

	pass = SSL_OK;

	bnum_init_multi(&num, &quot_num, &ref_quot_num, NULL);

	bnum_from_hex(&num, num_hex);
	bnum_from_hex(&ref_quot_num, ref_quot_hex);

	bnum_divmod_dig(&num, div_digit, &quot_num, &rem_digit);

	pass = TEST_ASSERT(bnum_cmp(&quot_num, &ref_quot_num) == 0);
	pass |= TEST_ASSERT(rem_digit == ref_rem_digit);

	bnum_set_dig_u(&num, 0);
	bnum_divmod_dig(&num, div_digit, &quot_num, &rem_digit);

	pass |= TEST_ASSERT(bnum_cmp(&quot_num, &__zero_num) == 0);
	pass |= TEST_ASSERT(rem_digit == 0);

	bnum_clear_multi(&num, &quot_num, &ref_quot_num, NULL);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_exp(void)
{
	const char		*num_hex = "179672f4b9ce4ff1ce809d56dbdc138ec5caaa123123";
	const char		*ref_hex = "307d822cbac6df486c0439fa7323e996bc8edc830ea826"\
		"f417f3e8cd1f36aae8a4e26881a00a5a53181ba5c067e078127e6c39038c34a758037"\
		"7ffa6a228757ce1ef74baa023c4022fbc4a8440bd89d9cd14033ff63964b3c856dcdd"\
		"dff9e4be2f4598511a160318f75d7fac3df6b71b2e80cf62958311d52daa2f29f78f1"\
		"c5e98777338b458885a22663d1a773a7a262fb9843be7c231bf6329c3c8f12fb01412"\
		"e5162ce266d000d32c9a5b50f804a3a6f0f55ea68b4a89cca964e1cd5dbb21afebcbf"\
		"5fab55fa96c1c70f81c8e94e745968ff88e5cf469";
	const uint64_t	exp_digit = 0xa;
	t_num			num, test_num, ref_num;
	int				pass;

	pass = SSL_OK;

	bnum_init_multi(&num, &test_num, &ref_num, NULL);

	bnum_from_hex(&num, num_hex);
	bnum_from_hex(&ref_num, ref_hex);

	bnum_exp(&num, exp_digit, &test_num);
	pass = TEST_ASSERT(bnum_cmp(&test_num, &ref_num) == 0);

	bnum_exp(&num, 0, &test_num);
	pass |= TEST_ASSERT(bnum_cmp(&test_num, &__one_num) == 0);

	bnum_exp(&__zero_num, exp_digit, &test_num);
	pass |= TEST_ASSERT(bnum_cmp(&test_num, &__zero_num) == 0);

	bnum_clear_multi(&num, &test_num, &ref_num, NULL);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_bnum_exp2(void)
{
	const char		*ref_hex =	"10000000000000000000000000000000000000"\
								"00000000000000000000000000000000000000"\
								"00000000000000000000000000000000000000"\
								"00000000000000000000000000000000000000"\
								"00000000000000000000000000000000000000"\
								"00000000000000000000000000000000000000"\
								"00000000000000000000000000000";
	const uint64_t	powers_of_two_digit = 0x400;
	t_num			test_num, ref_num;
	int				pass;

	pass = SSL_OK;

	bnum_init_multi(&test_num, &ref_num, NULL);

	bnum_from_hex(&ref_num, ref_hex);

	bnum_exp2(&test_num, powers_of_two_digit);
	pass = TEST_ASSERT(bnum_cmp(&test_num, &ref_num) == 0);

	bnum_exp2(&test_num, 0);
	pass |= TEST_ASSERT(bnum_cmp(&test_num, &__one_num) == 0);

	bnum_clear_multi(&test_num, &ref_num, NULL);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_gcd(void)
{
	const char		*num_hex =	"fb9a1f894f696eb8b5437f023eadf153fbf8f8"\
								"5028098415a4591";
	const char		*num2_hex =	"1aa092dcd526845fa1474d57154348d4c91581";
	const char		*ref_hex =	"aaaaaaabbb";
	t_num			num, num2, test_num, ref_num;
	int				pass;

	pass = SSL_OK;

	bnum_init_multi(&num, &num2, &test_num, &ref_num, NULL);

	bnum_from_hex(&num, num_hex);
	bnum_from_hex(&num2, num2_hex);
	bnum_from_hex(&ref_num, ref_hex);

	bnum_gcd(&num, &num2, &test_num);
	pass = TEST_ASSERT(bnum_cmp(&test_num, &ref_num) == 0);

	bnum_gcd(&num, &__one_num, &test_num);
	pass |= TEST_ASSERT(bnum_cmp(&test_num, &__one_num) == 0);

	bnum_gcd(&__one_num, &num, &test_num);
	pass |= TEST_ASSERT(bnum_cmp(&test_num, &__one_num) == 0);

	bnum_clear_multi(&num, &num2, &test_num, &ref_num, NULL);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_invmod(void)
{
	const char		*hex =		"ca49ba166dcaa428a46aaf1c8c913eae21f28f"\
								"c26ac3b7ff5026d0f30b1214f7";
	const char		*mod_hex =	"fe686c175338a780b1aa10807cd6383366fe05"\
								"88fb77262b7610775409c5bfdf";
	const char		*ref_hex =	"3ef2b6f98a5892c498eb07ef47728698428582"\
								"ddc016720f1a1743bb73705dc8";
	t_num			num, mod_num, test_num, ref_num;
	int				pass;

	pass = SSL_OK;

	bnum_init_multi(&num, &mod_num, &test_num, &ref_num, NULL);

	bnum_from_hex(&num, hex);
	bnum_from_hex(&mod_num, mod_hex);
	bnum_from_hex(&ref_num, ref_hex);

	bnum_invmod(&num, &mod_num, &test_num);

	pass = TEST_ASSERT(bnum_cmp(&test_num, &ref_num) == 0);

	bnum_clear_multi(&num, &mod_num, &test_num, &ref_num, NULL);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_lcm(void)
{
	const char		*num_hex =	"fb9a1f894f696eb8b5437f023eadf153fbf8f8"\
								"5028098415a4591";
	const char		*num2_hex =	"1aa092dcd526845fa1474d57154348d4c91581";
	const char		*ref_hex =	"274133cda512b84eee201d96dc306d9471e19d"\
								"8a1e0fd497502e692c7573fb8a036fadee4c1a"\
								"da1a3";
	t_num			num, num2, test_num, ref_num;
	int				pass;

	pass = SSL_OK;

	bnum_init_multi(&num, &num2, &test_num, &ref_num, NULL);

	bnum_from_hex(&num, num_hex);
	bnum_from_hex(&num2, num2_hex);
	bnum_from_hex(&ref_num, ref_hex);

	bnum_lcm(&num, &num2, &test_num);

	pass = TEST_ASSERT(bnum_cmp(&test_num, &ref_num) == 0);

	bnum_clear_multi(&num, &num2, &test_num, &ref_num, NULL);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_lmbit(void)
{
	const char	*hex =	"fb9a1f894f696eb8b5437f023eadf153fbf8f8"\
						"5028098415a4591";
	const int	ref_nbits = 212;
	int			nbits;
	t_num		num;
	int			pass;

	pass = SSL_OK;

	bnum_init(&num);
	bnum_from_hex(&num, hex);

	nbits = bnum_lmbit(&num);

	pass = TEST_ASSERT(nbits == ref_nbits);

	bnum_clear(&num);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_powmod(void)
{
	const char		*hex =		"d9f2c6197c1610f216e06def0f65e4d7b34c737cb667fc"\
								"745f08e7f0270c4358e73cc2bf501b20bd4c8ca63659be"\
								"0347edc9c63275cf93c3267b25ce953d0da55647c3bc12"\
								"7cbba3abcedd08632253a5558c77acbf2989a975bb0b66"\
								"017f718cba54627855b848fd1ef695b764f6a86538dc67"\
								"5fdf208ace577ca087e49f433d";
	const char		*exp_hex =	"f577d0a80d56a42fa7c4e204e3cb84d96f3108eba10620"\
								"be36b5ab58a7b5db77";
	const char		*mod_hex =	"f937d6f312350b810bb40d0eaf9c7b2d77f084f455264a"\
								"b53b7e7ba204f9c4cc52cce6f844cc5bb3877a65efadb9"\
								"d6bd3f361b1b81de3db461d443926024705f";
	const char		*ref_hex =	"f1100bb7f97d0c1609722f6798d886a449f33c793f735e"\
								"af53b1e944053dcd8f3475ac72ef832924fee76b494039"\
								"371bc72e301219e37409b64238e31018ca7b";
	t_num			num, exp_num, mod_num, test_num, ref_num;
	int				pass;

	pass = SSL_OK;

	bnum_init_multi(&num, &exp_num, &mod_num, &test_num, &ref_num, NULL);

	bnum_from_hex(&num, hex);
	bnum_from_hex(&exp_num, exp_hex);
	bnum_from_hex(&mod_num, mod_hex);
	bnum_from_hex(&ref_num, ref_hex);

	bnum_powmod(&num, &exp_num, &mod_num, &test_num);

	pass = TEST_ASSERT(bnum_cmp(&test_num, &ref_num) == 0);

	bnum_clear_multi(&num, &exp_num, &mod_num, &test_num, &ref_num, NULL);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_m_powmod(void)
{
	const char		*hex =		"d9f2c6197c1610f216e06def0f65e4d7b34c737cb667fc"\
								"745f08e7f0270c4358e73cc2bf501b20bd4c8ca63659be"\
								"0347edc9c63275cf93c3267b25ce953d0da55647c3bc12"\
								"7cbba3abcedd08632253a5558c77acbf2989a975bb0b66"\
								"017f718cba54627855b848fd1ef695b764f6a86538dc67"\
								"5fdf208ace577ca087e49f433d";
	const char		*exp_hex =	"f577d0a80d56a42fa7c4e204e3cb84d96f3108eba10620"\
								"be36b5ab58a7b5db77";
	const char		*mod_hex =	"f937d6f312350b810bb40d0eaf9c7b2d77f084f455264a"\
								"b53b7e7ba204f9c4cc52cce6f844cc5bb3877a65efadb9"\
								"d6bd3f361b1b81de3db461d443926024705f";
	const char		*ref_hex =	"f1100bb7f97d0c1609722f6798d886a449f33c793f735e"\
								"af53b1e944053dcd8f3475ac72ef832924fee76b494039"\
								"371bc72e301219e37409b64238e31018ca7b";
	t_num			num, exp_num, mod_num, test_num, ref_num;
	int				pass;

	pass = SSL_OK;

	bnum_init_multi(&num, &exp_num, &mod_num, &test_num, &ref_num, NULL);

	bnum_from_hex(&num, hex);
	bnum_from_hex(&exp_num, exp_hex);
	bnum_from_hex(&mod_num, mod_hex);
	bnum_from_hex(&ref_num, ref_hex);

	bnum_m_powmod(&num, &exp_num, &mod_num, &test_num);

	pass = TEST_ASSERT(bnum_cmp(&test_num, &ref_num) == 0);

	bnum_clear_multi(&num, &exp_num, &mod_num, &test_num, &ref_num, NULL);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}
