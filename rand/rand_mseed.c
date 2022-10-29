#include <ft_ssl.h>

int	rand_mseed(uint64_t *seed, int n)
{
	void		*ptr;
	uint64_t	num[MAX(128, MIN(1024, n))];
	int			idx;

	if ((n < 128) || (n > 1024))
		n = 128;
	
	SSL_ALLOC(ptr, 1024);
	num[0] = (uint64_t)ptr;

	for (idx = 1; idx < n; idx++)
	{
		SSL_ALLOC(ptr, num[idx-1]&((1ULL<<10)-1));
		num[idx] = (uint64_t)ptr;
	}

	for (idx = 0; idx < n; idx++)
	{
		ptr = (void *)(num[idx]);
		SSL_FREE(ptr);
	}
	
	*seed = (uint64_t)ft_hash((unsigned char *)(num), sizeof(uint64_t)*n);
	*seed |= ((uint64_t)ft_hash((unsigned char *)(seed), sizeof(uint64_t))) << 32;

	return (SSL_OK);
}
