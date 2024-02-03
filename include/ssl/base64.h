#ifndef SSL_BASE64_H
# define SSL_BASE64_H

# include <ssl/ssl.h>
# include <util/io.h>

# define B64_A(X)	(((X)&(0xfc))>>(2))
# define B64_B(X)	((((X)<<(4))&(0x30))|(((X)>>(12))&(0xf)))
# define B64_C(X)	((((X)>>(6))&(0x3c))|(((X)>>(22))&(0x3)))
# define B64_D(X)	(((X)>>(16))&(0x3f))

# define BASE64_ERROR(MES, ...)	ssl_error_log(__func__, __FILE__, __LINE__, "base64 error: ", MES __VA_OPT__(,) __VA_ARGS__)

enum	e_base64_flag
{
	B64_D	= 1 << 1
};

int		base64_encode(const char *, size_t, char **, size_t *);
int		base64_decode(const char *, size_t, char **, size_t *);
int		base64_check(const char *, size_t);

#endif
