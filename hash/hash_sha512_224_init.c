#include <ft_ssl.h>
#include <ssl_hash.h>

static const t_sha512_224_word	HASH_INIT_VECT[] = {
	0x8c3d37c819544da2, 0x73e1996689dcd4d6,
	0x1dfab7ae32ff9c82, 0x679dd514582f9fcf,
	0x0f6d2b697bd44da8, 0x77e36f7304c48942,
	0x3f9d85a86a1d36c8, 0x1112e6ad91d692a1
};

static t_sha512_224_word		var[SHA512_HASH_LEN];
static t_sha512_224_word		hash[SHA512_HASH_LEN];
static uint128_t				msize;

static t_hash	sha512_224 = {
	.var = &var,
	.hash = &hash,
	.size = SHA512_224_HASH_LEN * sizeof(t_sha512_224_word) - 4,
	.msize = &msize
};

t_hash	*hash_sha512_224_init(void)
{
	ft_memcpy(sha512_224.var, HASH_INIT_VECT, sizeof(hash));
	ft_memcpy(sha512_224.hash, HASH_INIT_VECT, sizeof(hash));
	msize = 0;

	return (&sha512_224);
}
