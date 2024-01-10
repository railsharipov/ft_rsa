#ifndef SSL_ERROR_H
# define SSL_ERROR_H

# include <ft_printf.h>
# include <sys/errno.h>

# define SSL_ENABLE_DEBUG_ERROR_MESSAGES

# define ERROR_BUF_SIZE		1024
# define SSL_INFO_FILE		"./docs/usage.txt"

# define SSL_ERROR_STACK_SIZE	256

# ifdef SSL_ENABLE_DEBUG_ERROR_MESSAGES
#  define SSL_ERROR(ERROR)	error_dump_error(ERROR, __func__, __FILE__, __LINE__)
# else
#  define SSL_ERROR(ERROR)	error_update_error(ERROR)
# endif

# define MASK_16_BIT				((1u<<16)-1u)

# define GET_ERROR_CTX(ERROR)		(((uint32_t)(ERROR))&(~MASK_16_BIT))>>16
# define GET_ERROR(ERROR)			(((uint32_t)(ERROR))&(MASK_16_BIT))
# define ADD_ERROR_CTX(ERROR, MOD)	(((uint32_t)(ERROR))|(MOD<<16))

# define IS_ERROR(FLAG)				((int)(FLAG > 0))

# define __INVALID_ERROR_ID_MESSAGE	"invalid error id"


struct s_error_ctx
{
	char	**table;
	size_t	size;
};

enum	e_ssl_error
{
	UNSPECIFIED_ERROR = 1,
	UNEXPECTED_ERROR,
	NOT_IMPLEMENTED_ERROR,
	USAGE_ERROR,
	MEMORY_ERROR,
	INVALID_INPUT,
	INVALID_OPTION_FLAG,
	EXPECTED_OPTION_FLAG,
	INVALID_PASSWORD_SIZE,
	PASSWORDS_NOT_IDENTICAL,
};

enum	e_error_ctx
{
	SSL_ERROR_CTX = 0,
	ASN_ERROR_CTX,
	BASE64_ERROR_CTX,
	DER_ERROR_CTX,
	DES_ERROR_CTX,
	HASH_ERROR_CTX,
	IO_ERROR_CTX,
	PEM_ERROR_CTX,
	RAND_ERROR_CTX,
	RSA_ERROR_CTX,
	JSON_ERROR_CTX,
	NUMBER_OF_ERROR_CATEGORIES
};

const char	*error_to_string(int error);
const char	*error_ctx_to_string(int error);
int	error_update_error(int error);
int error_dump_error(
	int error, const char *func_name, const char *file_name, int line_number);

#endif
