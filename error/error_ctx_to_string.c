#include <ft_ssl.h>
#include <ssl_error.h>

# define	SIZE_OF_ARRAY(ARRAY)	sizeof(ARRAY) / sizeof(void *)

static char	*__ERROR_CTX_NAMES[] = {
	"ssl",
	"asn",
	"base64",
	"der",
	"des",
	"hash",
	"io",
	"pem",
	"rand",
	"rsa",
};

const char	*error_ctx_to_string(int error)
{
	struct s_error_ctx	error_ctx;
	uint32_t						error_ctx_idx;

	error_ctx_idx = GET_ERROR_CTX(error);

	if (error_ctx_idx >= SIZE_OF_ARRAY(__ERROR_CTX_NAMES))
		return __INVALID_ERROR_ID_MESSAGE;

	return __ERROR_CTX_NAMES[error_ctx_idx];
}
