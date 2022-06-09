#include <ft_ssl.h>
#include <ssl_test.h>

int	test_summary(int *result_arr, size_t arr_size, int verbose)
{
	int	num_passed;
	int	idx;

	num_passed = 0;

	idx = 0;
	while (idx < arr_size)
	{
		if (SSL_OK == result_arr[idx])
			num_passed++;
		idx++;
	}

	if (verbose)
	{
		ft_printf(
			"%@tested modules: %d, pass: %d, fail: %d\n",
			arr_size, num_passed, arr_size - num_passed);
	}

	return (num_passed);
}
