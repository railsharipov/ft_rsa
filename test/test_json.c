#include <ft_ssl.h>
#include <ssl_asn.h>
#include <ssl_der.h>
#include <ssl_rand.h>
#include <ssl_test.h>
#include <ssl_io.h>

static int	__test_json_setup(void);
static void	__test_json_cleanup(void);

static int	__test_json_parse(void);

static char	*__content;

int	test_json(void)
{
	int	res;

	if (SSL_OK != __test_json_setup())
		return (SSL_ERROR(UNSPECIFIED_ERROR));

	res = SSL_OK;

	res = __test_json_parse();

	__test_json_cleanup();

	return (res);
}

static int	__test_json_setup(void)
{
	return (SSL_OK);
}

static void	__test_json_cleanup(void)
{
	return ;
}

static int	__test_json_parse(void)
{
	int			ret;
	int			pass;

	ret = SSL_OK;

	pass = TEST_ASSERT(SSL_OK == ret);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}