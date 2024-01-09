#include <ft_ssl.h>
#include <ssl_test.h>

#define ARR_SIZE(ARR)	sizeof(ARR) / sizeof(ARR[0])

const FUNC_TEST	TESTS[] = {
	test_libft,
	test_io,
	test_bnum,
	test_base64,
	test_der,
	test_json,
};

const int	NUM_TESTS = sizeof(TESTS)/sizeof(FUNC_TEST);

const char	*TEST_DESC_ARR[] = {
	"libft",
	"io",
	"bnum",
	"base64",
	"der",
	"json",
};

const size_t	TEST_DESC_ARR_SIZE = ARR_SIZE(TEST_DESC_ARR);
