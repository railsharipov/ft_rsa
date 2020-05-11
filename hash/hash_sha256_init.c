#include <ft_ssl.h>
#include <ssl_hash.h>

static const t_sha256_word	HASH_INIT_VECT[] = {
	0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
	0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};

static t_sha256_word		var[SHA256_HASH_LEN];
static t_sha256_word		hash[SHA256_HASH_LEN];
static uint64_t				msize;

static t_hash	sha256 = {
	.var = &var,
	.hash = &hash,
	.size = SHA256_HASH_LEN * sizeof(t_sha256_word),
	.msize = &msize
};

t_hash	*hash_sha256_init(void)
{
	ft_memcpy(sha256.var, HASH_INIT_VECT, sizeof(hash));
	ft_memcpy(sha256.hash, HASH_INIT_VECT, sizeof(hash));
	msize = 0;

	return (&sha256);
}
