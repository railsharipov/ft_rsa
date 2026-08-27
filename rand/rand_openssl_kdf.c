#include <common.h>
#include <logger.h>
#include <des.h>
#include <digest.h>
#include <rand.h>
#include <pwd.h>
#include <libft.h>

/*
** Implements OpenSSL's proprietary key derivation function (EVP_BytesToKey with MD5).
** This is NOT PBKDF2.
** It derives a key and optionally an IV from a password and a salt.
** The salt must be 8 bytes. The derived key is 8 bytes, and the IV is 8 bytes.
*/

int	rand_openssl_kdf(uint8_t key[8], uint8_t salt[8], uint8_t vect[8], const char *pass)
{
	t_digest_ctx	md5_ctx;
	char	buf[160] = {0};
	size_t	pass_len;

	if (NULL == salt) {
		SSL_LOG(ERROR, "salt is required");
		return (SSL_ERR);
	}
	if (NULL == pass) {
		SSL_LOG(ERROR, "password is required");
		return (SSL_ERR);
	}

	pass_len = MIN(sizeof(buf)-8, ft_strlen(pass));
	ft_memcpy(buf, pass, pass_len);
	ft_memcpy(buf + pass_len, salt, 8);

	md5_init(&md5_ctx);
	md5_update(&md5_ctx, (uint8_t *)buf, pass_len + 8);
	md5_final(&md5_ctx);

	ft_memcpy(key, md5_ctx.hash, 8);

	if (NULL != vect) {
		ft_memcpy(vect, md5_ctx.hash + 8, 8);
	}
	ft_bzero(md5_ctx.hash, md5_ctx.hashsize);

	return (SSL_OK);
}

/*
** Implements OpenSSL V3 proprietary key derivation function (EVP_BytesToKey with SHA256).
** This is NOT PBKDF2.
** It derives a key and optionally an IV from a password and a salt.
*/

int	rand_openssl_v3_kdf(
	uint8_t *key, size_t key_size,
	uint8_t *iv, size_t iv_size,
	const uint8_t *salt, size_t salt_size,
	const uint8_t *data, size_t data_size
)
{
	t_ostring octets;
	ft_ostr_init_with_capacity(&octets, key_size+iv_size+SHA256_HASH_SIZE);

	t_digest_ctx sha256_ctx;
	uint8_t *digest = NULL;
	size_t digest_size = 0;

	// SSL_LOG(TRACE, "key_size=%zu, data_size=%zu, salt_size=%zu", data_size, salt_size);

	// D_i = H(H(...H(D_(i-1) || data || salt)...))
	while (octets.size < key_size+iv_size) {
		sha256_init(&sha256_ctx);
		sha256_update(&sha256_ctx, digest, digest_size);
		if (NULL != data) {
			sha256_update(&sha256_ctx, data, data_size);
		}
		if (NULL != salt) {
			sha256_update(&sha256_ctx, salt, salt_size);
		}
		sha256_final(&sha256_ctx);

		digest = sha256_ctx.hash;
		digest_size = sha256_ctx.hashsize;

		ft_ostr_append(&octets, digest, digest_size);
	}
	ft_memcpy(key, octets.content, key_size);

	if (NULL != iv) {
		ft_memcpy(iv, octets.content + key_size, iv_size);
	}
	ft_ostr_clear(&octets);

	return (SSL_OK);
}
