#ifndef SSL_TEST_H
# define SSL_TEST_H

# include <assert.h>
# include <printnl.h>

# define TEST_ENABLE_VERBOSE
# define MAX_NUM_OF_TESTS_PER_MODULE	256

# ifdef TEST_ENABLE_VERBOSE
#  define TEST_INFO(MODULE_ID)				test_info(MODULE_ID, SSL_TRUE);
#  define TEST_SUMMARY(RES_ARR, ARR_SIZE)	test_summary(RES_ARR, ARR_SIZE, SSL_TRUE);
#  define TEST_RESULT(RES, __FUN, __FIL)	test_result(RES, SSL_TRUE, __FUN, __FIL)
#  define TEST_ASSERT(EXPR)					test_assert((EXPR), __func__, __FILE__, __LINE__)
# else
#  define TEST_INFO(MODULE_ID)				test_info(MODULE_ID, SSL_FALSE);
#  define TEST_SUMMARY(RES_ARR, ARR_SIZE)	test_summary(RES_ARR, ARR_SIZE, SSL_FALSE);
#  define TEST_RESULT(RES, __FUN, __FIL)	test_result(RES, SSL_FALSE, __FUN, __FIL)
#  define TEST_ASSERT(EXPR)					test_assert((EXPR), __func__, NULL, NULL)
# endif

# define TEST_PASS()		TEST_RESULT(SSL_OK, __func__, __FILE__)
# define TEST_FAIL()		TEST_RESULT(SSL_FAIL, __func__, __FILE__)

typedef int	(*FUNC_TEST)(void);

enum	e_test_modules
{
	BNUM_MODULE = 0,
	BASE64_MODULE,
	DER_MODULE,
	IO_MODULE,
	LIBFT_MODULE,
	NUMBER_OF_TEST_MODULES
};

extern const FUNC_TEST	TESTS[];
extern const int		NUM_TESTS;
extern const char		*TEST_DESC_ARR[];
extern const size_t		TEST_DESC_ARR_SIZE;

int		test_info(int module_id, int verbose);
int		test_assert(int bool, const char *func, const char *file, int line);
int		test_get_testfile_content(const char *testfile_path, t_ostring *ostring);
int		test_result(int res, int verbose, const char *func_name, const char *file_name);
int		test_summary(int *result_arr, size_t arr_size, int verbose);

int		test_libft(void);
int		test_io(void);
int		test_base64(void);
int		test_der(void);
int		test_bnum(void);

#endif
