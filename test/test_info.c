#include <ssl.h>
#include <test.h>

int	test_info(int module_id, int verbose)
{
	if (!verbose) {
		return (SSL_OK);
	}

	if (module_id < 0 || module_id >= TEST_DESC_ARR_SIZE) {
		TEST_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	ft_printf("%@\nTEST MODULE: " TXT_YELL("%s\n"), TEST_DESC_ARR[module_id]);

	return (SSL_OK);
}
