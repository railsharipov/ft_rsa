#include <ssl/ssl.h>
#include <ssl/error.h>

# define	SIZE_OF_TABLE(TABLE)	sizeof(TABLE) / sizeof(void *)
# define	ERROR_CTX(TABLE)		{TABLE, SIZE_OF_TABLE(TABLE)}

static char	*__SSL_ERROR_MESSAGES[] = {
	"unspecified error",
	"unexpected error",
	"not implemented error",
	"invalid command line argument",
	"memory allocation error",
	"invalid argument",
	"invalid option flag",
	"expection option flag",
	"invalid password size",
	"passwords not identical",
};

static char	*__ASN_ERROR_MESSAGES[] = {
	"invalid asn tree",
	"invalid der encoding",
};

static char	*__BASE64_ERROR_MESSAGES[] = {
	"invalid base64 encoding",
};

static char	*__DER_ERROR_MESSAGES[] = {
	"invalid asn object id",
	"invalid asn type tag",
	"invalid asn len tag",
	"invalid der encoding",
	"unknown asn object id",
};

static char	*__DES_ERROR_MESSAGES[] = {
	"invalid des encoding",
	"key derivation error",
};

static char	*__HASH_ERROR_MESSAGES[] = {
};

static char	*__IO_ERROR_MESSAGES[] = {
	"invalid io",
	"invalid io parameters",
};

static char	*__PEM_ERROR_MESSAGES[] = {
	"invalid pem encoding",
	"invalid pem encapsulation",
};

static char	*__RAND_ERROR_MESSAGES[] = {
	"expected password input",
	"invalid number of bytes read",
};

static char	*__RSA_ERROR_MESSAGES[] = {
	"invalid rsa key type",
	"invalid rsa key",
	"invalid rsa key size",
	"failed rsa key generation",
	"invalid octet string size",
	"invalid integer size",
};

static struct s_error_ctx __ERROR_CTX_TABLE[] = {
	ERROR_CTX(__SSL_ERROR_MESSAGES),
	ERROR_CTX(__ASN_ERROR_MESSAGES),
	ERROR_CTX(__BASE64_ERROR_MESSAGES),
	ERROR_CTX(__DER_ERROR_MESSAGES),
	ERROR_CTX(__DES_ERROR_MESSAGES),
	ERROR_CTX(__HASH_ERROR_MESSAGES),
	ERROR_CTX(__IO_ERROR_MESSAGES),
	ERROR_CTX(__PEM_ERROR_MESSAGES),
	ERROR_CTX(__RAND_ERROR_MESSAGES),
	ERROR_CTX(__RSA_ERROR_MESSAGES),
};

static size_t	__ERROR_CTX_TABLE_SIZE =
	sizeof(__ERROR_CTX_TABLE) / sizeof(struct s_error_ctx);

const char	*error_to_string(int error)
{
	struct s_error_ctx	error_ctx;
	uint32_t			error_ctx_idx;
	uint32_t			error_idx;

	if (error == UNSPECIFIED_ERROR)
		return (NULL);

	error_ctx_idx = GET_ERROR_CTX(error);
	error_idx = GET_ERROR(error) - 1;

	if (error_ctx_idx >= __ERROR_CTX_TABLE_SIZE)
		return (__INVALID_ERROR_ID_MESSAGE);

	error_ctx = __ERROR_CTX_TABLE[error_ctx_idx];

	if (error_idx >= error_ctx.size)
		return (__INVALID_ERROR_ID_MESSAGE);

	return error_ctx.table[error_idx];
}
