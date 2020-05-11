#include <ft_ssl.h>
#include <ssl_hash.h>

static const t_sha1_word	HASH_INIT_VECT[] = {
	0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0
};

static t_sha1_word		var[SHA1_HASH_LEN];
static t_sha1_word		hash[SHA1_HASH_LEN];
static uint64_t			msize;

static t_hash	sha1 = {
	.var = &var,
	.hash = &hash,
	.size = SHA1_HASH_LEN * sizeof(t_sha1_word),
	.msize = &msize
};

t_hash	*hash_sha1_init(void)
{
	ft_memcpy(sha1.var, HASH_INIT_VECT, sizeof(hash));
	ft_memcpy(sha1.hash, HASH_INIT_VECT, sizeof(hash));
	msize = 0;

	return (&sha1);
}
