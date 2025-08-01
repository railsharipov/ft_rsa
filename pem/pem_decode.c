#include <common.h>
#include <rand.h>
#include <base64.h>
#include <pem.h>
#include <des.h>
#include <textutil.h>
#include <libft/2darray.h>
#include <libft/bytes.h>

// Encrypted PEM format:
//
// -----BEGIN X-----
// Proc-Type: 4,ENCRYPTED
// DEK-Info: CIPHER,PIV
// [ ... BASE64 ENCODING ... ]
// -----END X-----
//
// OpenSSL's `enc` command uses a proprietary password-based encryption format.
// The "IV" from the DEK-Info header is used as the salt for a key derivation
// function (KDF). The KDF then generates the actual cipher key and IV.

static const char *__CIPHER_NAME_DES_CBC = "DES-CBC";

int pem_decode(t_pem *pem, t_ostring *enc, t_ostring *data, const char *label, const char *pass)
{
	t_ostring	b64enc, b64dec;
	char		vecthex[16], *cipher_name, *proc_type;
	int			pos, matches, seek, idx, ret;
	t_des		*des;

	ret = SSL_ERR;

	if (NULL == pem || NULL == enc || NULL == data) {
		PEM_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);	
	}
	ft_ostr_init(&b64enc);
	ft_ostr_init(&b64dec);
	pem_init(pem);

	pos = 0;
	seek = textutil_seekf((char *)enc->content + pos, enc->size - pos, "-----BEGIN %s-----", label);
	if (seek <= 0) {
		PEM_LOG(ERROR, "bad pem encoding: encapsulation label");
		goto label_exit;
	}
	pos += seek;
	
	matches = textutil_sscanf((char *)enc->content + pos, enc->size - pos, "Proc-Type: %s\n", &proc_type);
	if (matches == 1 && ft_streq(proc_type, "4,ENCRYPTED")) {
		pem->proc = PEM_PROC_TYPE_ENCRYPTED;
	} else {
		pem->proc = PEM_PROC_TYPE_NONE;
	}
	SSL_FREE(proc_type);
	
	if (pem->proc == PEM_PROC_TYPE_ENCRYPTED) {
		matches = textutil_sscanf((char *)enc->content + pos, enc->size - pos, "DEK-Info: %s,%s\n", &cipher_name, &vecthex);
		if (matches != 2) {
			PEM_LOG(ERROR, "invalid pem encoding: bad encryption header");
			goto label_exit;
		}
		pem->cipher = ft_streq(cipher_name, __CIPHER_NAME_DES_CBC) ? PEM_CIPHER_DES_CBC : PEM_CIPHER_NONE;
		SSL_FREE(cipher_name);
		
		if (pem->cipher != PEM_CIPHER_DES_CBC) {
			PEM_LOG(ERROR, "unsupported pem cipher type: %d", pem->cipher);
			goto label_exit;
		} else {
			pos += textutil_seekf((char *)enc->content + pos, enc->size - pos, "DEK-Info: DES-CBC,%s\n", vecthex);
		}
		ft_hex_to_bytes(pem->iv, vecthex, 16);
	}
	pos += textutil_seekf((char *)enc->content + pos, enc->size - pos, "\n");
	
	idx = textutil_findf((char *)enc->content + pos, enc->size - pos, "%s%s%s", "-----END ", label, "-----");
	if (idx < 0) {
		PEM_LOG(ERROR, "bad pem encoding: encapsulation end");
		goto label_exit;
	}

	ft_ostr_append(&b64enc, (char *)enc->content + pos, idx - pos);

	if (SSL_OK != base64_decode(b64enc.content, b64enc.size, &b64dec.content, &b64dec.size)) {
		PEM_LOG(ERROR, "bad pem encoding: bad base64 encoding");
		goto label_exit;
	}
	ft_ostr_clear(&b64enc);

	if (pem->proc == PEM_PROC_TYPE_ENCRYPTED) {
		if (SSL_OK != rand_openssl_kdf((unsigned char *)pem->key, (unsigned char *)pem->iv, (unsigned char *)pem->salt, pass)) {
			PEM_LOG(ERROR, "bad pem encoding: bad key derivation");
			goto label_exit;
		}
	
		des = des_init((unsigned char *)pem->key, NULL, (unsigned char *)pem->salt);
	
		if (SSL_OK != des_cbc_decrypt(des, &b64dec, data, pass)) {
			PEM_LOG(ERROR, "bad pem encoding: bad des cbc decrypt");
			goto label_exit;
		}
	}
	else {
		ft_ostr_append(data, b64dec.content, b64dec.size);
	}

	ret = SSL_OK;

label_exit:
	ft_ostr_clear(&b64dec);
	ft_ostr_clear(&b64enc);

	return (ret);
}
