#ifndef SSL_TEST_H
# define SSL_TEST_H

# include <assert.h>

# define TEST_ASSERT(EXPR)	test_assert((EXPR), #EXPR, __func__, __FILE__)
# define TEST_PASS()		ft_printf("%@%s, %s: OK\n", __func__, __FILE__)

void	test_get_testfile_content(const char *testfile_path, t_ostring *ostring);
void	test_assert(int bool, const char *expr, const char *func, const char *file);

void	test_base64(void);

#endif
