#ifndef SSL_BASE64_H
# define SSL_BASE64_H

# include <common.h>
# include <io.h>
# include <libft/logger.h>

# define B64_A(X)	(((X)&(0xfc))>>(2))
# define B64_B(X)	((((X)<<(4))&(0x30))|(((X)>>(12))&(0xf)))
# define B64_C(X)	((((X)>>(6))&(0x3c))|(((X)>>(22))&(0x3)))
# define B64_D(X)	(((X)>>(16))&(0x3f))

# define B64_LOG(LEVEL, MES, ...)	base64_logger_log(__func__, __FILE__, __LINE__, LIBFT_LOG_LEVEL_##LEVEL, "base64: ", MES __VA_OPT__(,) __VA_ARGS__)

enum	e_base64_flag
{
	B64_D	= 1 << 1
};

int		base64_logger_log(const char *func_name, const char *file_name, int line_number, uint8_t level, const char *fmt, ...);

int		base64_encode(const unsigned char *mes, size_t messize, unsigned char **enc, size_t *encsize);
int		base64_decode(const unsigned char *enc, size_t encsize, unsigned char **mes, size_t *messize);
int		base64_check(const unsigned char *b64enc, size_t size);

#endif
