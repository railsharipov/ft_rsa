#include <ft_ssl.h>
#include <pwd.h>

static const int	__SSL_PASS_LEN = 128;
static char			__pass[__SSL_PASS_LEN + 1];
static int			__is_set;

int	__passin(void)
{
	char	*input;
	int		inlen;
	int		ret;

	ret = SSL_OK;
	input = getpass("enter password:");
	inlen = ft_strlen(input);

	if (inlen > __SSL_PASS_LEN)
	{
		ret = SSL_ERROR("password too long");
	}
	else
	{
		ft_memzcpy(__pass, input, sizeof(__pass), inlen);
		input = getpass("confirm password:");
		if (ft_strcmp(__pass, input))
			ret = SSL_ERROR("passwords don't match");
	}
	ft_bzero(input, _PASSWORD_LEN);

	return (ret);
}

char	*ssl_getpass(void)
{
	if (!__is_set)
	{
		ft_bzero(__pass, sizeof(__pass));

		if (SSL_OK != __passin())
			return (NULL);

		__is_set = 1;
	}
	return (&__pass);
}

int		ssl_setpass(const char *passin)
{
	int passin_len;

	ft_bzero(__pass, sizeof(__pass));
	passin_len = ft_strlen(passin);

	if (NULL != passin)
	{
		__is_set = 1;
	}
	if (passin_len > __SSL_PASS_LEN)
	{
		return (SSL_ERROR("password too long"));
	}
	ft_memcpy(__pass, passin, passin_len);

	return (SSL_OK);
}

void	ssl_unsetpass(void)
{
	ft_bzero(__pass, sizeof(__pass));
	__is_set = 0;
}
