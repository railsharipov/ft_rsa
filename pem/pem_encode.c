#include <common.h>
#include <logger.h>
#include <rand.h>
#include <pem.h>
#include <base64.h>
#include <des.h>
#include <textutil.h>
#include <libft.h>

// Encrypted PEM format:
//
// -----BEGIN X-----
// Proc-Type: 4,ENCRYPTED
// DEK-Info: CIPHER,SALT
// [ ... BASE64 ENCODING ... ]
// -----END X-----
//
// Old versions of OpenSSL's `enc` command uses a proprietary password-based encryption format.
// The "SALT" from the DEK-Info header is used as the salt for a key derivation
// function (KDF). The KDF then generates the actual cipher key and IV.

int	pem_encode(t_pem *pem, t_ostring *data, t_ostring *enc, const char *pass)
{
	t_ostring	pemenc, cipher, b64flat, b64formatted;
	char		buf[1024], *salthex;
	uint8_t		key[8], iv[8], salt[8];
	int			ret;
	t_des_ctx		des;

	SSL_LOG(TRACE, "encoding pem: content: %p, size: %d", data->content, data->size);

	if (NULL == pem || NULL == data || NULL == enc) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	ret = SSL_ERR;

	ft_ostr_init(enc);
	ft_ostr_init(&pemenc);
	ft_ostr_init(&cipher);
	ft_ostr_init(&b64flat);
	ft_ostr_init(&b64formatted);

	SSL_LOG(TRACE, "writing pem label begin: %s", pem->label);
	ft_snprintf(buf, sizeof(buf), "%s%s%s", "-----BEGIN ", pem->label, "-----\n");
	ft_ostr_append(&pemenc, buf, ft_strlen(buf));

	if (pem->proc == PEM_PROC_TYPE_NONE) {
		SSL_LOG(TRACE, "unencrypted pem");
		SSL_LOG(TRACE, "base64 encoding data: content: %p, size: %d", data->content, data->size);
		if (SSL_OK != base64_encode_all(data->content, data->size, &b64flat.content, &b64flat.size)) {
			SSL_LOG(ERROR, "base64 encode failed");
			goto label_exit;
		}
	}
	else {
		SSL_LOG(TRACE, "encrypted pem: legacy format");
		if (pem->proc == PEM_PROC_TYPE_ENCRYPTED) {
			SSL_LOG(TRACE, "encrypted pem: writing proc type header");
			ft_strcpy(buf, "Proc-Type: 4,ENCRYPTED\n");
			ft_ostr_append(&pemenc, buf, ft_strlen(buf));
		} else {
			SSL_LOG(ERROR, "invalid pem proc type: %d", pem->proc);
			goto label_exit;
		}

		if (pem->cipher == PEM_CIPHER_DES_CBC) {
			if (pem->has_salt == 0) {
				SSL_LOG(TRACE, "no salt provided: generating random salt for des cipher");
				if ((SSL_OK != rand_useed((uint64_t *)salt, 8))) {
					SSL_LOG(ERROR, "rand useed failed");
					goto label_exit;
				}
			} else {
				SSL_LOG(TRACE, "using provided salt for des cipher");
				ft_memcpy(salt, pem->salt, 8);
			}
			SSL_LOG(TRACE, "generating key and iv from password for des cipher");
			if (SSL_OK != rand_openssl_kdf(key, salt, iv, pass)) {
				SSL_LOG(ERROR, "rand pbkdf2 failed");
				goto label_exit;
			}
			SSL_LOG(TRACE, "converting des salt to hex");
			salthex = ft_bytes_to_hex_upper(salt, 8);

			SSL_LOG(TRACE, "writing dek info header: salt: %s", salthex);
			ft_ostr_appendf(&pemenc, "DEK-Info: DES-CBC,%s\n\n", salthex);
			SSL_FREE(salthex);

			SSL_LOG(TRACE, "encrypting data: content: %p, size: %zu", data->content, data->size);

			if (SSL_OK != des_init(&des, key, iv, DES_CRYPT_CBC, DES_MODE_ENCRYPT)) {
				SSL_LOG(ERROR, "des init error");
				goto label_exit;
			}
			// Allocate output buffer (input size + one block for padding)
			size_t max_cipher_size = data->size + DES_BLOCK_SIZE;
			SSL_ALLOC(cipher.content, max_cipher_size);

			ssize_t update_written = des_update(&des, (char *)data->content, (char *)cipher.content, data->size);
			if (update_written < 0) {
				SSL_LOG(ERROR, "bad des encrypt");
				goto label_exit;
			}
			ssize_t final_written = des_final(&des, (char *)cipher.content + update_written, max_cipher_size - update_written);
			if (final_written < 0) {
				SSL_LOG(ERROR, "bad des encrypt");
				goto label_exit;
			}
			cipher.size = update_written + final_written;
		}
		else {
			SSL_LOG(ERROR, "unsupported pem cipher type: %d", pem->cipher);
			goto label_exit;
		}

		SSL_LOG(TRACE, "base64 encoding cipher: content: %p, size: %d", cipher.content, cipher.size);
		if (SSL_OK != base64_encode_all(cipher.content, cipher.size, &b64flat.content, &b64flat.size)) {
			SSL_LOG(ERROR, "base64 encode failed");
			goto label_exit;
		}
	}

	SSL_LOG(TRACE, "formatting base64 encoding: content: %p, size: %d", b64flat.content, b64flat.size);
	if (SSL_OK != textutil_insert_delim((char *)b64flat.content, b64flat.size, (char **)&b64formatted.content, &b64formatted.size, '\n', 64)) {
		SSL_LOG(ERROR, "base64 format failed");
		goto label_exit;
	}
	ft_ostr_append_ostr(&pemenc, &b64formatted);
	ft_ostr_append_cstr(&pemenc, "\n");

	SSL_LOG(TRACE, "writing pem label end: %s", pem->label);
	ft_ostr_appendf(&pemenc, "%s%s%s", "-----END ", pem->label, "-----\n");

	ft_ostr_append_ostr(enc, &pemenc);
	SSL_LOG(TRACE, "pem encode complete: content: %p, size: %zu", enc->content, enc->size);
	ret = SSL_OK;

label_exit:
	ft_ostr_clear(&pemenc);
	ft_ostr_clear(&cipher);
	ft_ostr_clear(&b64flat);
	ft_ostr_clear(&b64formatted);
	return (ret);
}
