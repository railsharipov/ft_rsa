#include <ft_ssl.h>
#include <ssl_hash.h>

static const t_sha512_256_word	HASH_INIT_VECT[] = {
	0x22312194fc2bf72c, 0x9f555fa3c84c64c2,
	0x2393b86b6f53b151, 0x963877195940eabd,
	0x96283ee2a88effe3, 0xbe5e1e2553863992,
	0x2b0199fc2c85b8aa, 0x0eb72ddc81c52ca2
};

static t_sha512_256_word		var[SHA512_HASH_LEN];
static t_sha512_256_word		hash[SHA512_HASH_LEN];
static t_uint128				msize;

static t_hash	sha512_256 = {
	.var = &var,
	.hash = &hash,
	.size = SHA512_256_HASH_LEN * sizeof(t_sha512_256_word),
	.msize = &msize
};

t_hash	*hash_sha512_256_init(void)
{
	ft_memcpy(sha512_256.var, HASH_INIT_VECT, sizeof(hash));
	ft_memcpy(sha512_256.hash, HASH_INIT_VECT, sizeof(hash));
	msize = 0;

	return (&sha512_256);
}
