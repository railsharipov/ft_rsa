#include <ft_ssl.h>
#include <ssl_rand.h>

int	rand_bytes(uint64_t seed, void *buf, size_t nbytes)
{
	uint64_t	tmp;

	if (nbytes <= 0)
	{
		return (SSL_ERROR("invalid number of bytes"));
	}
	rand_mtw_init(seed);

	while (nbytes > 8)
	{
		*(uint64_t *)(buf) = rand_mtw_extract();
		buf += 8;
		nbytes -= 8;
	}
	tmp = rand_mtw_extract();

	while (nbytes-- > 0)
	{
		*(uint8_t *)(buf) = tmp & 0xFF;
		buf++;
		tmp >>= 8;
	}

	return (SSL_OK);
}
