#include <ft_ssl.h>
#include <parser.h>
#include <ssl_rand.h>
#include <ssl_base64.h>
#include <ssl_pem.h>
#include <ssl_des.h>

static const char	*PEM_PROC = "Proc-Type: ";
static const char	*DEK_INFO = "DEK-Info: ";

static char			*__content;
static size_t		__consize;
static t_bool		__encrypted;
static char			__vecthex[16];

// Encrypted PEM format:
//
// -----BEGIN X-----
// Proc-Type: 4,ENCRYPTED
// DEK-Info: CIPHER,PIV
// [ ... BASE64 ENCODING ... ]
// -----END X-----
//
// OpenSSL uses a non-standard ecryption format for PEM encoding.
// PEM initial vector (PIV) is used for both as CIPHER salt and
// CIPHER initial vector. Thus, CIPHER key is generated using PIV as a salt.
// Generated key and PIV is then passed to CIPHER.
// Initial vector generated by PBKDF function is ignored.

static int __decrypt(void)
{
	t_des		*des;
	t_ostring	cipher;
	t_ostring	message;
	uint8_t		vect[8];
	uint8_t		key[8];

	ft_hexbin(vect, __vecthex, 16);

	if (SSL_OK != rand_pbkdf2(key, vect, NULL))
	{
		return (SSL_ERROR("rand pbkdf error"));
	}
	des = des_init(key, NULL, vect);
	cipher.content = __content;
	cipher.size = __consize;

	if (SSL_OK != des_cbc_decrypt(des, &cipher, &message))
	{
		return (SSL_ERROR("des cbc decrypt error"));
	}
	SSL_FREE(__content);
	__content = (char *)message.content;
	__consize = message.size;

	return (SSL_OK);
}

static int __parse_proc(const char *proc)
{
	char	**proc_info;
	char	**proc_type;
	int		check;

	proc_info = ft_strsplit(proc, ' ');

	if (ft_2darray_len(proc_info) < 2)
	{
		return (SSL_ERR);
	}
	proc_type = ft_strsplit(proc_info[1], ',');

	check = (ft_2darray_len(proc_type) < 2);
	check += ft_strcmp("4", proc_type[0]);
	check += ft_strcmp("ENCRYPTED", proc_type[1]);

	ft_2darray_del(proc_type, -1);
	ft_2darray_del(proc_info, -1);

	if (check != 0)
	{
		return (SSL_ERR);
	}
	return (SSL_OK);
}

static int __parse_dek(const char *dek)
{
	char	**dek_info;
	char	**dek_cipher;
	int		check;

	dek_info = ft_strsplit(dek, ' ');

	if (ft_2darray_len(dek_info) < 2)
	{
		return (SSL_ERR);
	}
	dek_cipher = ft_strsplit(dek_info[1], ',');

	check = (ft_2darray_len(dek_cipher) < 2);
	check += ft_strcmp("DES-CBC", dek_cipher[0]);
	check += (ft_strlen(dek_cipher[1]) != 16);
	check += !ft_str_ishex(dek_cipher[1]);

	ft_memcpy(__vecthex, dek_cipher[1], MIN(16, ft_strlen(dek_cipher[1])));
	ft_2darray_del(dek_cipher, -1);
	ft_2darray_del(dek_info, -1);

	if (check != 0)
	{
		return (SSL_ERR);
	}
	return (SSL_OK);
}

static int	__check_crypt_header(const char *proc, const char *dek)
{
	int	procidx;
	int	dekidx;
	int	proclen;
	int	deklen;

	if ((NULL == proc) || (NULL == dek))
	{
		return (SSL_ERROR("invalid crypt header"));
	}
	procidx = parser_find(__content, __consize, PEM_PROC, ft_strlen(PEM_PROC));
	dekidx = parser_find(__content, __consize, DEK_INFO, ft_strlen(DEK_INFO));
	proclen = ft_strlen(proc);
	deklen = ft_strlen(dek);

	if (procidx > dekidx)
	{
		return (SSL_ERROR("bad crypt header"));
	}
	if (dekidx + deklen + 1 >= __consize) // include newline character
	{
		return (SSL_ERROR("bad crypt header"));
	}
	return (SSL_OK);
}

static void	__remove_crypt_header(void)
{
	char	*dek;
	int		dekidx;
	int		idx;

	dekidx = parser_find(__content, __consize, DEK_INFO, ft_strlen(DEK_INFO));
	dek = parser_line(__content, __consize, DEK_INFO, ft_strlen(DEK_INFO));
	idx = dekidx + ft_strlen(dek);

	ft_memcpy(__content, __content + idx, __consize - idx);
	__consize -= idx;
}

static int	__parse_crypt_header(void)
{
	char	*proc;
	char	*dek;
	int		ret;

	ret = SSL_OK;
	dek = parser_line(__content, __consize, DEK_INFO, ft_strlen(DEK_INFO));
	proc = parser_line(__content, __consize, PEM_PROC, ft_strlen(PEM_PROC));

	if (SSL_OK != __check_crypt_header(proc, dek))
	{
		ret = SSL_ERR;
	}
	else if (SSL_OK != __parse_proc(proc))
	{
		ret = SSL_ERROR("invalid proc line");
	}
	else if (SSL_OK != __parse_dek(dek))
	{
		ret = SSL_ERROR("invalid dek line");
	}
	SSL_FREE(proc);
	SSL_FREE(dek);

	return (ret);
}

static int __decode(void)
{
	char	*b64enc;
	int		b64len;
	int		ret;

	ret = SSL_OK;

	if (parser_find(__content, __consize, PEM_PROC, ft_strlen(PEM_PROC)) >= 0)
	{
		__encrypted = SSL_TRUE;

		if (SSL_OK != __parse_crypt_header())
		{
			return (SSL_ERR);
		}
		__remove_crypt_header();
	}
	parser_del_eolws(__content, __consize, &b64enc, &b64len);
	SSL_FREE(__content);

	if (SSL_OK != base64_decode(b64enc, b64len, &__content, &__consize))
	{
		ret = SSL_ERROR("base64 error");
	}
	else if (SSL_TRUE == __encrypted)
	{
		ret = __decrypt();
	}
	SSL_FREE(b64enc);

	return (ret);
}

int pem_decode(t_pem *pem, const char *type, t_ostring **content)
{
	SSL_CHECK(NULL != pem);
	SSL_CHECK(NULL != content);
	__encrypted = SSL_FALSE;
	*content = NULL;

	if (SSL_OK != pem_remove_encap(pem, type, &__content, &__consize))
	{
		return (SSL_ERR);
	}
	if (SSL_OK != __decode())
	{
		return (SSL_ERROR("bad pem encoding"));
	}
	SSL_ALLOC(*content, sizeof(t_ostring));

	(*content)->content = __content;
	(*content)->size = __consize;

	return (SSL_OK);
}
