#include <ft_ssl.h>
#include <ssl_rand.h>
#include <bnum.h>
#include <ssl_test.h>
#include <printnl.h>

static t_num	__zero_num;

static int	__test_bnum_setup(void);
static void	__test_bnum_cleanup(void);
static int	__test_bnum_num_to_hex(void);
static int	__test_bnum_hex_to_num(void);
static int	__test_bnum_abs_num(void);
static int	__test_bnum_add_num_ud(void);
static int	__test_bnum_sub_num_ud(void);
static int	__test_bnum_add_num_d(void);
static int	__test_bnum_sub_num_d(void);
static int	__test_bnum_add_num_u(void);
static int	__test_bnum_sub_num_u(void);
static int	__test_bnum_add_num(void);
static int	__test_bnum_sub_num(void);
static int	__test_bnum_increm_num_u(void);
static int	__test_bnum_decrem_num_u(void);
static int	__test_bnum_bytes_to_num(void);
static int	__test_bnum_num_to_bytes(void);
static int	__test_bnum_skip_zeros(void);
static int	__test_bnum_lsh_num_d_inpl(void);
static int	__test_bnum_rsh_num_d_inpl(void);
static int	__test_bnum_lsh_num_b_inpl(void);
static int	__test_bnum_rsh_num_b_inpl(void);
static int	__test_bnum_compare_num_d(void);
static int	__test_bnum_compare_num_u(void);
static int	__test_bnum_compare_num(void);
static int	__test_bnum_copy_num(void);
static int	__test_bnum_div_num_2d_inpl(void);
static int	__test_bnum_divmod_num(void);
static int	__test_bnum_divmod_num_d(void);

int	test_bnum(void)
{
	int	res;

	if (SSL_OK != __test_bnum_setup())
		return (SSL_ERROR(UNSPECIFIED_ERROR));

	res = __test_bnum_num_to_hex();
	res |= __test_bnum_hex_to_num();
	res |= __test_bnum_abs_num();
	res |= __test_bnum_add_num_ud();
	res |= __test_bnum_sub_num_ud();
	res |= __test_bnum_add_num_d();
	res |= __test_bnum_sub_num_d();
	res |= __test_bnum_add_num();
	res |= __test_bnum_sub_num();
	res |= __test_bnum_increm_num_u();
	res |= __test_bnum_decrem_num_u();
	res |= __test_bnum_bytes_to_num();
	res |= __test_bnum_num_to_bytes();
	res |= __test_bnum_skip_zeros();
	res |= __test_bnum_lsh_num_d_inpl();
	res |= __test_bnum_rsh_num_d_inpl();
	res |= __test_bnum_lsh_num_b_inpl();
	res |= __test_bnum_rsh_num_b_inpl();
	res |= __test_bnum_compare_num_d();
	res |= __test_bnum_compare_num_u();
	res |= __test_bnum_compare_num();
	res |= __test_bnum_copy_num();
	res |= __test_bnum_div_num_2d_inpl();
	res |= __test_bnum_divmod_num();
	res |= __test_bnum_divmod_num_d();

	__test_bnum_cleanup();

	return (res);
}

static int	__test_bnum_setup(void)
{
	t_num		n1;
	t_num		n2;
	uint64_t	digit;

	// init_num(&n1);
	// init_num(&n2);
	// set_randnum(&n1, 137);
	// set_randnum(&n2, 142);
	// rand_useed(&digit, sizeof(digit));
	// print_num(NULL, &n1);
	// print_num(NULL, &n2);
	// printf("%#llx\n", digit);
	// clear_num(&n1);
	// clear_num(&n2);

	init_num(&__zero_num);
	set_num_d(&__zero_num, 0);

	return (SSL_OK);
}

static void	__test_bnum_cleanup(void)
{
	clear_num(&__zero_num);
}

static int	__test_bnum_num_to_hex(void)
{
	t_num		test_num;
	char		digit_hex[16];
	uint64_t	digit;
	char		*test_hex, *zero_test_hex, *ref_hex, *ref_hex_ptr;
	size_t		hexsize, digit_hexsize;
	int			idx, pass;

	pass = SSL_OK;

	init_num(&test_num);
	set_randnum(&test_num, 1709);

	hexsize = 2 * NBITS_TO_NBYTES(test_num.len * BNUM_DIGIT_BIT);
	SSL_ALLOC(ref_hex, hexsize+1);
	ref_hex_ptr = ref_hex;

	idx = test_num.len - 1;

	digit = test_num.val[idx--];
	digit_hexsize = sprintf(digit_hex, "%llx", digit);
	ft_strncpy(ref_hex_ptr, digit_hex, digit_hexsize);
	ref_hex_ptr += digit_hexsize;

	while (idx >= 0)
	{
		digit = test_num.val[idx--];
		digit_hexsize = sprintf(digit_hex, "%.15llx", digit);
		ft_strncpy(ref_hex_ptr, digit_hex, digit_hexsize);
		ref_hex_ptr += digit_hexsize;
	}

	test_hex = num_to_hex(&test_num);
	zero_test_hex = num_to_hex(&__zero_num);

	pass = TEST_ASSERT(ft_strcmp(ref_hex, test_hex) == 0);
	pass |= TEST_ASSERT(ft_strcmp("0", zero_test_hex) == 0);

	clear_num(&test_num);
	SSL_FREE(ref_hex);
	SSL_FREE(test_hex);
	SSL_FREE(zero_test_hex);

	if (SSL_OK != pass)
		return (TEST_FAIL());

	return (TEST_PASS());
}

static int	__test_bnum_hex_to_num(void)
{
	t_num	test_num, tmp_num;
	char	*test_hex, *zero_test_hex, *ref_hex;
	int		pass;

	pass = SSL_OK;

	init_num(&test_num);
	init_num(&tmp_num);

	ref_hex = num_to_hex(&test_num);

	hex_to_num(&tmp_num, ref_hex);
	test_hex = num_to_hex(&tmp_num);

	hex_to_num(&tmp_num, "0");
	zero_test_hex = num_to_hex(&tmp_num);

	pass = TEST_ASSERT(ft_strcmp(test_hex, ref_hex) == 0);
	pass |= TEST_ASSERT(ft_strcmp(zero_test_hex, "0") == 0);

	clear_num(&test_num);
	clear_num(&tmp_num);
	SSL_FREE(ref_hex);
	SSL_FREE(test_hex);
	SSL_FREE(zero_test_hex);

	if (SSL_OK != pass)
		return (TEST_FAIL());

	return (TEST_PASS());
}

static int	__test_bnum_abs_num(void)
{
	char	*hex;
	t_num	test_num, res_num;
	int		pass;

	pass = SSL_OK;

	init_num(&test_num);
	init_num(&res_num);

	test_num.sign = BNUM_NEG;
	abs_num(&test_num, &res_num);

	pass = TEST_ASSERT(test_num.len == res_num.len);
	pass |= TEST_ASSERT(res_num.sign == BNUM_POS);
	pass |= TEST_ASSERT(
		ft_memcmp(test_num.val, res_num.val, res_num.len * sizeof(uint64_t)) == 0);

	clear_num(&test_num);
	clear_num(&res_num);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_add_num_ud(void)
{
	const char	*n1_hex = "179672f4b9ce4ff1ce809d56dbdc138ec5c";
	const char	*ref_hex = "179672f4b9ce4ff1ce86302d024b7dcf569";
	char		*test_hex;
	uint64_t	digit = 0x592d6266f6a4090d;
	t_num		n1, test_num;
	int			pass;

	pass = SSL_OK;

	init_num(&n1);
	hex_to_num(&n1, n1_hex);

	init_num(&test_num);
	add_num_ud(&n1, digit, &test_num);
	test_hex = num_to_hex(&test_num);

	pass = TEST_ASSERT(ft_strcmp(test_hex, ref_hex) == 0);

	clear_num(&n1);
	clear_num(&test_num);
	SSL_FREE(test_hex);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_sub_num_ud(void)
{
	const char	*n1_hex = "179672f4b9ce4ff1ce80000000000000000";
	const char	*ref_hex = "179672f4b9ce4ff1ce7a6d29d99095bf6f3";
	char		*test_hex;
	uint64_t	digit = 0x592d6266f6a4090d;
	t_num		n1, test_num;
	int			pass;

	pass = SSL_OK;

	init_num(&n1);
	hex_to_num(&n1, n1_hex);

	init_num(&test_num);
	sub_num_ud(&n1, digit, &test_num);
	test_hex = num_to_hex(&test_num);

	pass = TEST_ASSERT(ft_strcmp(test_hex, ref_hex) == 0);

	clear_num(&n1);
	clear_num(&test_num);
	SSL_FREE(test_hex);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_add_num_d(void)
{
	const char	*n1_hex = "179672f4b9ce4ff1ce809d56dbdc138ec5c";
	const char	*ref_hex = "179672f4b9ce4ff1ce86302d024b7dcf569";
	const char	*ref_hex2 = "179672f4b9ce4ff1ce7b0a80b56ca94e34f";
	char		*test_hex, *test_hex2;
	uint64_t	digit = 0x592d6266f6a4090d;
	t_num		n1, test_num, test_num2;
	int			pass;

	pass = SSL_OK;

	init_num(&n1);
	hex_to_num(&n1, n1_hex);

	init_num(&test_num);
	add_num_d(&n1, digit, &test_num);
	test_hex = num_to_hex(&test_num);

	init_num(&test_num2);
	n1.sign = BNUM_NEG;
	add_num_d(&n1, digit, &test_num2);
	test_hex2 = num_to_hex(&test_num2);

	pass = TEST_ASSERT(ft_strcmp(test_hex, ref_hex) == 0);
	pass |= TEST_ASSERT(ft_strcmp(test_hex2, ref_hex2) == 0);

	clear_num(&n1);
	clear_num(&test_num);
	SSL_FREE(test_hex);
	SSL_FREE(test_hex2);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_sub_num_d(void)
{
	const char	*n1_hex = "179672f4b9ce4ff1ce809d56dbdc138ec5c";
	const char	*ref_hex = "179672f4b9ce4ff1ce7b0a80b56ca94e34f";
	const char	*ref_hex2 = "179672f4b9ce4ff1ce86302d024b7dcf569";
	char		*test_hex, *test_hex2;
	uint64_t	digit = 0x592d6266f6a4090d;
	t_num		n1, test_num, test_num2;
	int			pass;

	pass = SSL_OK;

	init_num(&n1);
	hex_to_num(&n1, n1_hex);

	init_num(&test_num);
	sub_num_d(&n1, digit, &test_num);
	test_hex = num_to_hex(&test_num);

	init_num(&test_num2);
	n1.sign = BNUM_NEG;
	sub_num_d(&n1, digit, &test_num2);
	test_hex2 = num_to_hex(&test_num2);

	pass = TEST_ASSERT(ft_strcmp(test_hex, ref_hex) == 0);
	pass |= TEST_ASSERT(ft_strcmp(test_hex2, ref_hex2) == 0);

	clear_num(&n1);
	clear_num(&test_num);
	SSL_FREE(test_hex);
	SSL_FREE(test_hex2);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_add_num(void)
{
	const char	*n1_hex = "179672f4b9ce4ff1ce809d56dbdc138ec5c";
	const char	*n2_hex = "2a01e004af52dce303c7b6959facb5191493";
	const char	*ref_hex = "2b7b4733faefc1e220afc06b0d6a765200ef";
	const char	*ref_hex2 = "288878d563b5f7e3e6dfacc031eef3e02837";
	char		*test_hex, *test_hex2;
	t_num		n1, n2, test_num, test_num2;
	int			pass;

	pass = SSL_OK;

	init_num_multi(&n1, &n2, &test_num, &test_num2, NULL);

	hex_to_num(&n1, n1_hex);
	hex_to_num(&n2, n2_hex);

	add_num(&n1, &n2, &test_num);
	test_hex = num_to_hex(&test_num);

	n1.sign = BNUM_NEG;
	add_num(&n1, &n2, &test_num2);
	test_hex2 = num_to_hex(&test_num2);

	pass = TEST_ASSERT(ft_strcmp(test_hex, ref_hex) == 0);
	pass |= TEST_ASSERT(ft_strcmp(test_hex2, ref_hex2) == 0);
	pass |= TEST_ASSERT(test_num.sign == BNUM_POS);
	pass |= TEST_ASSERT(test_num2.sign == BNUM_POS);

	clear_num_multi(&n1, &n2, &test_num, &test_num2, NULL);

	SSL_FREE(test_hex);
	SSL_FREE(test_hex2);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_sub_num(void)
{
	const char	*n1_hex = "179672f4b9ce4ff1ce809d56dbdc138ec5c";
	const char	*n2_hex = "2a01e004af52dce303c7b6959facb5191493";
	const char	*ref_hex = "288878d563b5f7e3e6dfacc031eef3e02837";
	const char	*ref_hex2 = "2b7b4733faefc1e220afc06b0d6a765200ef";
	char		*test_hex, *test_hex2;
	t_num		n1, n2, test_num, test_num2;
	int			pass;

	pass = SSL_OK;

	init_num_multi(&n1, &n2, &test_num, &test_num2, NULL);

	hex_to_num(&n1, n1_hex);
	hex_to_num(&n2, n2_hex);

	sub_num(&n1, &n2, &test_num);
	test_hex = num_to_hex(&test_num);

	n1.sign = BNUM_NEG;
	sub_num(&n1, &n2, &test_num2);
	test_hex2 = num_to_hex(&test_num2);

	pass = TEST_ASSERT(ft_strcmp(test_hex, ref_hex) == 0);
	pass |= TEST_ASSERT(ft_strcmp(test_hex2, ref_hex2) == 0);
	pass |= TEST_ASSERT(test_num.sign == BNUM_NEG);
	pass |= TEST_ASSERT(test_num2.sign == BNUM_NEG);

	clear_num_multi(&n1, &n2, &test_num, &test_num2, NULL);

	SSL_FREE(test_hex);
	SSL_FREE(test_hex2);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_increm_num_u(void)
{
	const char	*hex = "179672f4b9ce4ff1ce809d56dbdc138ec5c";
	const char	*ref_hex = "179672f4b9ce4ff1ce809d56dbdc138ec5d";
	char		*test_hex;
	t_num		test_num;
	int			pass;

	pass = SSL_OK;

	init_num(&test_num);
	hex_to_num(&test_num, hex);

	increm_num_u(&test_num);
	test_hex = num_to_hex(&test_num);

	pass = TEST_ASSERT(ft_strcmp(test_hex, ref_hex) == 0);

	clear_num(&test_num);
	SSL_FREE(test_hex);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_decrem_num_u(void)
{
	const char	*hex = "179672f4b9ce4ff1ce809d56dbdc138ec5c";
	const char	*ref_hex = "179672f4b9ce4ff1ce809d56dbdc138ec5b";
	char		*test_hex;
	t_num		test_num;
	int			pass;

	pass = SSL_OK;

	init_num(&test_num);
	hex_to_num(&test_num, hex);

	decrem_num_u(&test_num);
	test_hex = num_to_hex(&test_num);

	pass = TEST_ASSERT(ft_strcmp(test_hex, ref_hex) == 0);

	clear_num(&test_num);
	SSL_FREE(test_hex);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_bytes_to_num(void)
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

	init_num(&test_num);
	bytes_to_num(&test_num, bin, binsize);
	test_hex = num_to_hex(&test_num);

	pass = TEST_ASSERT(ft_strcmp(hex, test_hex) == 0);

	clear_num(&test_num);
	SSL_FREE(test_hex);
	SSL_FREE(bin);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_num_to_bytes(void)
{
	const char	*hex = "179672f4b9ce4ff1ce809d56dbdc138ec5c";
	char		*bin, *test_hex;
	size_t		binsize;
	t_num		test_num;
	int			pass;

	pass = SSL_OK;

	init_num(&test_num);
	hex_to_num(&test_num, hex);
	num_to_bytes(&test_num, &bin, &binsize);

	test_hex = ft_binhex(bin, binsize);

	pass = TEST_ASSERT(ft_strcmp(hex, test_hex) == 0);

	clear_num(&test_num);
	SSL_FREE(test_hex);
	SSL_FREE(bin);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_skip_zeros(void)
{
	const char	*hex = "179672f4b9ce4ff1ce809d56dbdc138ec5c";
	char		*test_hex;
	t_num		test_num;
	int			pass;

	pass = SSL_OK;

	init_num(&test_num);
	hex_to_num(&test_num, hex);
	test_num.len += 1;
	skip_zeros(&test_num);
	test_hex = num_to_hex(&test_num);

	pass = TEST_ASSERT(ft_strcmp(hex, test_hex) == 0);

	clear_num(&test_num);
	SSL_FREE(test_hex);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_lsh_num_d_inpl(void)
{
	const char	*hex = "179672f4b9ce4ff1ce809d56dbdc138ec5c";
	const char	*refhex = "179672f4b9ce4ff1ce809d56dbdc138ec5c000000000000000";
	char		*test_hex;
	t_num		test_num;
	int			pass;

	pass = SSL_OK;

	init_num(&test_num);
	hex_to_num(&test_num, hex);

	lsh_num_d_inpl(&test_num, 1);
	test_hex = num_to_hex(&test_num);

	pass = TEST_ASSERT(ft_strcmp(refhex, test_hex) == 0);

	clear_num(&test_num);
	SSL_FREE(test_hex);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_rsh_num_d_inpl(void)
{
	const char	*hex = "179672f4b9ce4ff1ce809d56dbdc138ec5c";
	const char	*refhex = "179672f4b9ce4ff1ce80";
	char		*test_hex;
	t_num		test_num;
	int			pass;

	pass = SSL_OK;

	init_num(&test_num);
	hex_to_num(&test_num, hex);

	rsh_num_d_inpl(&test_num, 1);
	test_hex = num_to_hex(&test_num);

	pass = TEST_ASSERT(ft_strcmp(refhex, test_hex) == 0);

	clear_num(&test_num);
	SSL_FREE(test_hex);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_lsh_num_b_inpl(void)
{
	const char	*hex = "179672f4b9ce4ff1ce809d56dbdc138ec5c";
	const char	*refhex = "bcb397a5ce727f8e7404eab6dee09c762e00000000000000000";
	char		*test_hex;
	t_num		test_num;
	int			pass;

	pass = SSL_OK;

	init_num(&test_num);
	hex_to_num(&test_num, hex);

	lsh_num_b_inpl(&test_num, 67);
	test_hex = num_to_hex(&test_num);

	pass = TEST_ASSERT(ft_strcmp(refhex, test_hex) == 0);

	clear_num(&test_num);
	SSL_FREE(test_hex);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_rsh_num_b_inpl(void)
{
	const char	*hex = "179672f4b9ce4ff1ce809d56dbdc138ec5c";
	const char	*refhex = "2f2ce5e9739c9fe39d";
	char		*test_hex;
	t_num		test_num;
	int			pass;

	pass = SSL_OK;

	init_num(&test_num);
	hex_to_num(&test_num, hex);

	rsh_num_b_inpl(&test_num, 67);
	test_hex = num_to_hex(&test_num);

	pass = TEST_ASSERT(ft_strcmp(refhex, test_hex) == 0);

	clear_num(&test_num);
	SSL_FREE(test_hex);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_compare_num_d(void)
{
	const char	*hex = "179672f4b9ce4ff1ce809d56dbdc138ec5caaa123123";
	const char	*hex2 = "179672f4b9ce4ff1ce809d56dbd";
	const char	*hex3 = "4b9ce4ff1ce9";
	t_num		test_num, test_num2, test_num3;
	int			res, pass;

	pass = SSL_OK;

	init_num_multi(&test_num, &test_num2, &test_num3, NULL);
	hex_to_num(&test_num, hex);
	hex_to_num(&test_num2, hex2);
	hex_to_num(&test_num3, hex3);

	res = compare_num_d(&test_num, 0x972f4b9ce4ff1ce8);
	pass = TEST_ASSERT(res > 0);

	res = compare_num_d(&test_num, 0x4b9ce4ff1ce7);
	pass |= TEST_ASSERT(res > 0);

	test_num.sign = BNUM_NEG;
	res = compare_num_d(&test_num, 0x72f4b9ce4ff1ce8);
	pass |= TEST_ASSERT(res < 0);

	res = compare_num_d(&test_num2, 0x972f4b9ce4ff1ce8);
	pass |= TEST_ASSERT(res > 0);

	res = compare_num_d(&test_num2, 0x4b9ce4ff1ce7);
	pass |= TEST_ASSERT(res > 0);

	res = compare_num_d(&test_num3, 0x972f4b9ce4ff1ce8);
	pass |= TEST_ASSERT(res < 0);

	res = compare_num_d(&test_num3, 0x4b9ce4ff1ce7);
	pass |= TEST_ASSERT(res > 0);

	res = compare_num_d(&test_num3, 0x4b9ce4ff1cff);
	pass |= TEST_ASSERT(res < 0);

	res = compare_num_d(&test_num3, 0x4b9ce4ff1ce9);
	pass |= TEST_ASSERT(res == 0);

	clear_num_multi(&test_num, &test_num2, &test_num3, NULL);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_compare_num_u(void)
{
	const char	*hex = "179672f4b9ce4ff1ce809d56dbdc138ec5caaa123123";
	const char	*hex2 = "179672f4b9ce4ff1ce809d56dbd";
	t_num		test_num, test_num2;
	int			res, pass;

	pass = SSL_OK;

	init_num_multi(&test_num, &test_num2, NULL);
	hex_to_num(&test_num, hex);
	hex_to_num(&test_num2, hex2);

	res = compare_num_u(&test_num, &test_num2);
	pass = TEST_ASSERT(res > 0);

	res = compare_num_u(&test_num2, &test_num);
	pass |= TEST_ASSERT(res < 0);

	res = compare_num_u(&test_num, &test_num);
	pass |= TEST_ASSERT(res == 0);

	clear_num_multi(&test_num, &test_num2, NULL);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_compare_num(void)
{
	const char	*hex = "179672f4b9ce4ff1ce809d56dbdc138ec5caaa123123";
	const char	*hex2 = "179672f4b9ce4ff1ce809d56dbd";
	t_num		test_num, test_num2;
	int			res, pass;

	pass = SSL_OK;

	init_num_multi(&test_num, &test_num2, NULL);
	hex_to_num(&test_num, hex);
	hex_to_num(&test_num2, hex2);

	test_num.sign = BNUM_POS;
	test_num2.sign = BNUM_POS;
	res = compare_num(&test_num, &test_num2);
	pass = TEST_ASSERT(res > 0);

	test_num.sign = BNUM_NEG;
	test_num2.sign = BNUM_NEG;
	res = compare_num(&test_num, &test_num2);
	pass |= TEST_ASSERT(res < 0);

	test_num.sign = BNUM_NEG;
	test_num2.sign = BNUM_POS;
	res = compare_num(&test_num, &test_num2);
	pass |= TEST_ASSERT(res < 0);

	test_num.sign = BNUM_POS;
	test_num2.sign = BNUM_NEG;
	res = compare_num(&test_num, &test_num2);
	pass |= TEST_ASSERT(res > 0);

	test_num.sign = BNUM_NEG;
	res = compare_num(&test_num, &test_num);
	pass |= TEST_ASSERT(res == 0);

	test_num.sign = BNUM_POS;
	res = compare_num(&test_num, &test_num);
	pass |= TEST_ASSERT(res == 0);

	clear_num_multi(&test_num, &test_num2, NULL);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_copy_num(void)
{
	const char	*hex = "179672f4b9ce4ff1ce809d56dbdc138ec5caaa123123";
	t_num	num, copy;
	int		pass;

	pass = SSL_OK;

	init_num(&num);
	init_num(&copy);

	hex_to_num(&num, hex);
	copy_num(&num, &copy);

	pass = TEST_ASSERT(compare_num(&num, &copy) == 0);

	clear_num(&num);
	clear_num(&copy);

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

	init_num(&num);
	init_num(&ref_num);

	hex_to_num(&num, hex);
	hex_to_num(&ref_num, ref_hex);
	div_num_2d_inpl(&num);

	pass = TEST_ASSERT(compare_num(&num, &ref_num) == 0);

	set_num_d(&num, 0);
	div_num_2d_inpl(&num);

	pass |= TEST_ASSERT(compare_num(&num, &__zero_num) == 0);

	clear_num(&num);
	clear_num(&ref_num);


	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_divmod_num(void)
{
	const char	*num_hex = "179672f4b9ce4ff1ce809d56dbdc138ec5caaa123123";
	const char	*div_hex = "21b40e90bfd2b6c5af67adaeb529f5ab837c";
	const char	*ref_quot_hex = "b32a27bb";
	const char	*ref_rem_hex = "132fd09e5fa8b7c3485487add0a3ce69418f";
	t_num		num, div_num, quot_num, rem_num, ref_quot_num, ref_rem_num;
	int			pass;

	pass = SSL_OK;

	init_num_multi(
		&num, &div_num, &quot_num, &rem_num, &ref_quot_num, &ref_rem_num, NULL);

	hex_to_num(&num, num_hex);
	hex_to_num(&div_num, div_hex);
	hex_to_num(&ref_quot_num, ref_quot_hex);
	hex_to_num(&ref_rem_num, ref_rem_hex);

	divmod_num(&num, &div_num, &quot_num, &rem_num);

	pass = TEST_ASSERT(compare_num(&quot_num, &ref_quot_num) == 0);
	pass |= TEST_ASSERT(compare_num(&rem_num, &ref_rem_num) == 0);

	set_num_d(&num, 0);
	divmod_num(&num, &div_num, &quot_num, &rem_num);

	pass |= TEST_ASSERT(compare_num(&quot_num, &__zero_num) == 0);
	pass |= TEST_ASSERT(compare_num(&rem_num, &__zero_num) == 0);

	clear_num_multi(
		&num, &div_num, &quot_num, &rem_num, &ref_quot_num, &ref_rem_num, NULL);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_bnum_divmod_num_d(void)
{
	const char		*num_hex = "179672f4b9ce4ff1ce809d56dbdc138ec5caaa123123";
	const char		*ref_quot_hex = "27f0dc4afffdef09c46e401389f3";
	const uint64_t	div_digit = 0x972f4b9ce4ff1ce8;
	const uint64_t	ref_rem_digit = 0x18663777a03998eb;
	t_num			num, quot_num, ref_quot_num;
	uint64_t		rem_digit;
	int				pass;

	pass = SSL_OK;

	init_num_multi(&num, &quot_num, &ref_quot_num, NULL);

	hex_to_num(&num, num_hex);
	hex_to_num(&ref_quot_num, ref_quot_hex);

	divmod_num_d(&num, div_digit, &quot_num, &rem_digit);

	pass = TEST_ASSERT(compare_num(&quot_num, &ref_quot_num) == 0);
	pass |= TEST_ASSERT(rem_digit == ref_rem_digit);

	set_num_d(&num, 0);
	divmod_num_d(&num, div_digit, &quot_num, &rem_digit);

	pass |= TEST_ASSERT(compare_num(&quot_num, &__zero_num) == 0);
	pass |= TEST_ASSERT(rem_digit == 0);

	clear_num_multi(&num, &quot_num, &ref_quot_num, NULL);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}
