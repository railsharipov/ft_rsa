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

int pem_decode(t_pem *pem, t_ostring *enc, t_ostring *data, const char *pass)
{
	t_ostring	b64enc_lines, b64enc, b64dec;
	char		vecthex[16], cipher_name[128], proc_type[128];
	int			pos, matches, idx, ret;
	t_des		*des;

	ret = SSL_ERR;

	if (NULL == pem || NULL == enc || NULL == data) {
		PEM_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);	
	}
	ft_ostr_init(&b64enc_lines);
	ft_ostr_init(&b64enc);
	ft_ostr_init(&b64dec);

	pos = 0;
	matches = textutil_sscanf((char *)enc->content + pos, enc->size - pos, "-----BEGIN %[^-]-----", &pem->label);
	if (matches != 1) {
		PEM_LOG(ERROR, "bad encapsulation label");
		goto label_exit;
	}
	
	pos += textutil_seekf((char *)enc->content + pos, enc->size - pos, "-----BEGIN %s-----", pem->label);
	
	matches = textutil_bnscanf((char *)enc->content + pos, enc->size - pos, "Proc-Type: %s\n", proc_type, sizeof(proc_type));
	if (matches == 1 && ft_streq(proc_type, "4,ENCRYPTED")) {
		pem->proc = PEM_PROC_TYPE_ENCRYPTED;
	} else {
		pem->proc = PEM_PROC_TYPE_NONE;
	}
	
	if (pem->proc == PEM_PROC_TYPE_ENCRYPTED) {
		matches = textutil_bnscanf((char *)enc->content + pos, enc->size - pos, "DEK-Info: %s,%s\n", cipher_name, sizeof(cipher_name), vecthex, sizeof(vecthex));
		if (matches != 2) {
			PEM_LOG(ERROR, "bad encryption header");
			goto label_exit;
		}
		pem->cipher = ft_streq(cipher_name, __CIPHER_NAME_DES_CBC) ? PEM_CIPHER_DES_CBC : PEM_CIPHER_NONE;
		
		if (pem->cipher != PEM_CIPHER_DES_CBC) {
			PEM_LOG(ERROR, "unsupported pem cipher type: %d", pem->cipher);
			goto label_exit;
		} else {
			pos += textutil_seekf((char *)enc->content + pos, enc->size - pos, "DEK-Info: DES-CBC,%s\n", vecthex);
		}
		ft_hex_to_bytes(pem->iv, vecthex, 16);
	}
	pos += textutil_seekf((char *)enc->content + pos, enc->size - pos, "\n");
	
	idx = textutil_findf((char *)enc->content, enc->size, "%s%s%s", "-----END ", pem->label, "-----");
	if (idx < 0) {
		PEM_LOG(ERROR, "bad encapsulation end");
		goto label_exit;
	}
	ft_ostr_append(&b64enc_lines, (char *)enc->content + pos, idx - pos);

	if (SSL_OK != textutil_del_eol((char *)b64enc_lines.content, b64enc_lines.size, (char **)&b64enc.content, &b64enc.size)) {
		PEM_LOG(ERROR, "bad base64 format");
		goto label_exit;
	}

	if (SSL_OK != base64_decode(b64enc.content, b64enc.size, &b64dec.content, &b64dec.size)) {
		PEM_LOG(ERROR, "bad base64 encoding");
		goto label_exit;
	}
	ft_ostr_clear(&b64enc);

	if (pem->proc == PEM_PROC_TYPE_ENCRYPTED) {
		if (SSL_OK != rand_openssl_kdf((unsigned char *)pem->key, (unsigned char *)pem->iv, (unsigned char *)pem->salt, pass)) {
			PEM_LOG(ERROR, "bad key derivation");
			goto label_exit;
		}
	
		des = des_init((unsigned char *)pem->key, NULL, (unsigned char *)pem->salt);
	
		if (SSL_OK != des_cbc_decrypt(des, &b64dec, data, pass)) {
			PEM_LOG(ERROR, "bad des cbc decrypt");
			goto label_exit;
		}
	}
	else {
		ft_ostr_append_ostr(data, &b64dec);
	}

	ret = SSL_OK;

label_exit:
	if (ret != SSL_OK) {
		pem_clear(pem);
	}
	ft_ostr_clear(&b64dec);
	ft_ostr_clear(&b64enc);

	return (ret);
}
