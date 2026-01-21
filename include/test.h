#ifndef SSL_TEST_H
# define SSL_TEST_H

# include <common.h>
# include <assert.h>
# include <printnl.h>
# include <libft/string.h>
# include <libft/logger.h>

# define TEST_LOG(LEVEL, MES, ...)	test_logger_log(__func__, __FILE__, __LINE__, LIBFT_LOG_LEVEL_##LEVEL, MES __VA_OPT__(,) __VA_ARGS__)
# define TEST_LOG_SET_LEVEL(LEVEL)	test_logger_set_level(LIBFT_LOG_LEVEL_##LEVEL)

# define MAX_NUM_OF_TESTS_PER_MODULE	256

# define TEST_RESULT(RES)	do { if ((RES == SSL_OK)) { TEST_LOG(INFO, TXT_B_GREEN("TEST OK")); } else { TEST_LOG(ERROR, TXT_B_RED("TEST FAIL")); } } while (0)
# define TEST_PASS()		do { TEST_RESULT(SSL_OK); return (SSL_OK); } while (0)
# define TEST_FAIL()		do { TEST_RESULT(SSL_ERR); return (SSL_ERR); } while (0)

# define TEST_ASSERT(EXPR)										        \
	do {														        \
		if ((EXPR)) {											        \
			TEST_LOG(INFO, TXT_GREEN("ASSERT PASS") " (%s)", #EXPR);	\
		} else {												        \
			TEST_LOG(ERROR, TXT_RED("ASSERT FAIL") " (%s)", #EXPR);	    \
			TEST_FAIL();										        \
		}														        \
	} while (0)

# define TEST_ASSERT_W_MSG(EXPR, FMT, ...)								\
	do {																\
		if ((EXPR)) {													\
			TEST_LOG(INFO, TXT_GREEN("ASSERT PASS") " (%s)", #EXPR);	\
		} else {														\
			TEST_LOG(ERROR, TXT_RED("ASSERT FAIL") " (%s)", #EXPR);	    \
			TEST_LOG(ERROR, FMT __VA_OPT__(,) __VA_ARGS__);	            \
			TEST_FAIL();												\
		}																\
	} while (0)

typedef int	(*t_func_test)(void);

int		test_logger_log(const char *func_name, const char *file_name, int line_number, uint8_t level, const char *fmt, ...);
void	test_logger_set_level(uint8_t level);

int		test_info(int module_id, int verbose);
int		test_assert(int boolean, const char *expr);
int		test_result(int res, int verbose);
int		test_summary(int *result_arr, size_t arr_size, int verbose);

int		test_libft(void);
int		test_argp(void);
int		test_textutil(void);
int		test_io(void);
int		test_base64(void);
int		test_der(void);
int		test_des(void);
int		test_pem(void);
int		test_bnum(void);
int		test_json(void);
int		test_asn(void);
int		test_hash(void);
int		test_args(void);

#endif
