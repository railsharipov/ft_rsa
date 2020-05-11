#include <ft_ssl.h>
#include <ssl_hash.h>

static const t_md5_word	HASH_INIT_VECT[] = {
	0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476
};

static t_md5_word		var[MD5_HASH_LEN];
static t_md5_word		hash[MD5_HASH_LEN];
static uint64_t			msize;

static t_hash	md5 = {
	.var = &var,
	.hash = &hash,
	.size = MD5_HASH_LEN * sizeof(t_md5_word),
	.msize = &msize
};

t_hash	*hash_md5_init(void)
{
	ft_memcpy(md5.var, HASH_INIT_VECT, sizeof(hash));
	ft_memcpy(md5.hash, HASH_INIT_VECT, sizeof(hash));
	msize = 0;

	return (&md5);
}
