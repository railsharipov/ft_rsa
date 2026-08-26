#include <common.h>
#include <logger.h>
#include <rand.h>
#include <base64.h>
#include <pem.h>
#include <des.h>
#include <textutil.h>
#include <libft.h>

// Encrypted PEM format:
//
// -----BEGIN X-----
// Proc-Type: 4,ENCRYPTED
// DEK-Info: CIPHER,PIV
// [ ... BASE64 ENCODING ... ]
// -----END X-----
//
// Old versions of OpenSSL's `enc` command uses a proprietary password-based encryption format.
// The "IV" from the DEK-Info header is used as the salt for a key derivation
// function (KDF). The KDF then generates the actual cipher key and IV.

static const char *__CIPHER_NAME_DES_CBC = "DES-CBC";

int pem_decode(t_pem *pem, t_ostring *enc, t_ostring *data, const char *pass)
{
	t_ostring	b64enc_lines, b64enc, b64dec;
	char		salthex[128], cipher_name[128], proc_type[128];
	uint8_t		key[8], iv[8], salt[8];
	int			pos, matches, idx, ret;
	t_des_ctx		des;

	SSL_LOG(TRACE, "decoding pem: content: %p, size: %d", enc->content, enc->size);
	ret = SSL_ERR;

	if (NULL == pem || NULL == enc || NULL == data) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	ft_ostr_init(&b64enc_lines);
	ft_ostr_init(&b64enc);
	ft_ostr_init(&b64dec);
	ft_ostr_init(data);

	pos = 0;

	SSL_LOG(TRACE, "scanning pem label begin: pos: %d", pos);
	matches = textutil_sscanf((char *)enc->content + pos, enc->size - pos, "-----BEGIN %[^-]-----\n", &pem->label);
	if (matches != 1) {
		SSL_LOG(ERROR, "bad encapsulation label");
		goto label_exit;
	}
	pos += textutil_seekf((char *)enc->content + pos, enc->size - pos, "-----BEGIN %s-----\n", pem->label);

	SSL_LOG(TRACE, "scanning proc type header: pos: %d", pos);
	matches = textutil_bnscanf((char *)enc->content + pos, enc->size - pos, "Proc-Type: %s\n", proc_type, sizeof(proc_type));

	if (matches == 1 && ft_streq(proc_type, "4,ENCRYPTED")) {
		pem->proc = PEM_PROC_TYPE_ENCRYPTED;
		pos += textutil_seekf((char *)enc->content + pos, enc->size - pos, "Proc-Type: %s\n", proc_type);
	} else {
		pem->proc = PEM_PROC_TYPE_NONE;
	}
    SSL_LOG(TRACE, "proc type: %#x", pem->proc);

	if (pem->proc == PEM_PROC_TYPE_ENCRYPTED) {
		SSL_LOG(TRACE, "scanning encryption header: pos: %d", pos);

        matches = textutil_bnscanf((char *)enc->content + pos, enc->size - pos, "DEK-Info: %[^,],%[a-zA-Z0-9]\n", cipher_name, sizeof(cipher_name), salthex, sizeof(salthex));
		if (matches != 2) {
			SSL_LOG(ERROR, "bad encryption header");
			goto label_exit;
		}
		pem->cipher = ft_streq(cipher_name, __CIPHER_NAME_DES_CBC) ? PEM_CIPHER_DES_CBC : PEM_CIPHER_NONE;

		if (pem->cipher != PEM_CIPHER_DES_CBC) {
			SSL_LOG(ERROR, "unsupported pem cipher type: %d", pem->cipher);
			goto label_exit;
		} else {
			pos += textutil_seekf((char *)enc->content + pos, enc->size - pos, "DEK-Info: DES-CBC,%s\n", salthex);
		}
        ft_hex_to_bytes(salt, salthex, 16);
        /* For PEM Proc-Type encryption, the DEK-Info value is the cipher IV and
         * is also used as the salt for openssl's EVP_BytesToKey. The actual cipher IV used
         * must be the header IV, not a derived IV. */
        ft_memcpy(iv, salt, 8);

		SSL_LOG(TRACE, "encryption: type=%#x, cipher=%s, iv=%s", pem->cipher, cipher_name, salthex);
	}
	pos += textutil_seekf((char *)enc->content + pos, enc->size - pos, "\n");

	SSL_LOG(TRACE, "searching pem label end: pos: %d", pos);
	idx = textutil_findf((char *)enc->content, enc->size, "%s%s%s", "-----END ", pem->label, "-----");
	if (idx < 0) {
		SSL_LOG(ERROR, "bad encapsulation end");
		goto label_exit;
	}
	ft_ostr_append(&b64enc_lines, (char *)enc->content + pos, idx - pos);
	SSL_LOG(TRACE, "copied base64 encoding: content: %p, size: %d", b64enc_lines.content, b64enc_lines.size);
	SSL_LOG(TRACE, "deleting whitespace from base64 encoding");

	if (SSL_OK != textutil_del_eol((char *)b64enc_lines.content, b64enc_lines.size, (char **)&b64enc.content, &b64enc.size)) {
		SSL_LOG(ERROR, "bad base64 format");
		goto label_exit;
	}

	SSL_LOG(TRACE, "decoding base64 encoding: content: %p, size: %d", b64enc.content, b64enc.size);
	if (SSL_OK != base64_decode_all(b64enc.content, b64enc.size, &b64dec.content, &b64dec.size)) {
		SSL_LOG(ERROR, "bad base64 encoding");
		goto label_exit;
	}
	SSL_LOG(TRACE, "decoded base64 encoding: content: %p, size: %d", b64dec.content, b64dec.size);

    if (pem->proc == PEM_PROC_TYPE_ENCRYPTED) {
		SSL_LOG(TRACE, "encrypted pem: generating key from password");
        /* Derive only the key using the header IV as the salt. Use the header IV
         * itself for the DES CBC initialization vector. */
        if (SSL_OK != rand_openssl_kdf(key, salt, NULL, pass)) {
			SSL_LOG(ERROR, "bad key derivation");
			goto label_exit;
		}
		SSL_LOG(TRACE, "decrypting data: content: %p, size: %d", b64dec.content, b64dec.size);

		if (SSL_OK != des_cbc_decrypt_init(&des, key, iv)) {
			SSL_LOG(ERROR, "des init error");
			goto label_exit;
		}
		// Allocate output buffer (at most same size as input)
		SSL_ALLOC(data->content, b64dec.size);

		ssize_t update_written = des_cbc_decrypt_update(&des, (char *)b64dec.content, (char *)data->content, b64dec.size);
		if (update_written < 0) {
			SSL_LOG(ERROR, "bad des decrypt");
			goto label_exit;
		}
		ssize_t final_written = des_cbc_decrypt_final(&des, (char *)data->content + update_written, b64dec.size - update_written);
		if (final_written < 0) {
			SSL_LOG(ERROR, "bad des decrypt");
			goto label_exit;
		}
		data->size = update_written + final_written;
	}
	else {
		SSL_LOG(TRACE, "unencrypted pem: copying data: content: %p, size: %d", b64dec.content, b64dec.size);
		ft_ostr_append_ostr(data, &b64dec);
	}

	ret = SSL_OK;
	SSL_LOG(TRACE, "pem decode complete: content: %p, size: %d", data->content, data->size);

label_exit:
	if (ret != SSL_OK) {
		pem_clear(pem);
	}
	ft_ostr_clear(&b64dec);
	ft_ostr_clear(&b64enc_lines);
	ft_ostr_clear(&b64enc);

	return (ret);
}
