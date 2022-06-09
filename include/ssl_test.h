#ifndef SSL_TEST_H
# define SSL_TEST_H

# include <assert.h>

# define TEST_ENABLE_VERBOSE
# define MAX_NUM_OF_TESTS_PER_MODULE	256

# ifdef TEST_ENABLE_VERBOSE
#  define TEST_RESULT(RES, __FUN, __FIL)	test_result(RES, SSL_TRUE, __FUN, __FIL)
#  define TEST_ASSERT(EXPR)					test_assert((EXPR), #EXPR, __func__, __FILE__)
#  define TEST_SUMMARY(RES_ARR, ARR_SIZE)	test_summary(RES_ARR, ARR_SIZE, SSL_TRUE);
# else
#  define TEST_RESULT(RES, __FUN, __FIL)	test_result(RES, SSL_FALSE, __FUN, __FIL)
#  define TEST_ASSERT(EXPR)					test_assert((EXPR), #EXPR, NULL, NULL)
#  define TEST_SUMMARY(RES_ARR, ARR_SIZE)	test_summary(RES_ARR, ARR_SIZE, SSL_FALSE);
# endif

# define TEST_PASS()		TEST_RESULT(SSL_OK, __func__, __FILE__)
# define TEST_FAIL()		TEST_RESULT(SSL_FAIL, __func__, __FILE__)

enum	e_test_modules
{
	TEST_MODULE = 0,
	BASE64_MODULE,
	DER_MODULE,
	NUMBER_OF_TEST_MODULES
};

int		test_assert(int bool, const char *expr, const char *func, const char *file);
int		test_get_testfile_content(const char *testfile_path, t_ostring *ostring);
int		test_result(int res, int verbose, const char *func_name, const char *file_name);
int		test_summary(int *result_arr, size_t arr_size, int verbose);

int		test_base64(void);
int		test_der(void);

#endif
