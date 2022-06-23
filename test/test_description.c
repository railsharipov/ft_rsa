#include <ft_ssl.h>
#include <ssl_test.h>

#define ARR_SIZE(ARR)	sizeof(ARR) / sizeof(ARR[0])

const char	*TEST_DESC_ARR[] = {
	"bnum",
	"base64",
	"der"
};

const size_t	TEST_DESC_ARR_SIZE = ARR_SIZE(TEST_DESC_ARR);
