#include <string.h>
#include <libft/alloc.h>
#include <libft/std.h>

void	*ft_memdup(void *src, size_t srcsize)
{
	void	*dst;

	if (NULL == src)
		return (NULL);

	LIBFT_ALLOC(dst, srcsize);
	ft_memcpy(dst, src, srcsize);

	return (dst);
}

int	ft_memcmp(const void *p1, const void *p2, size_t n)
{
	unsigned char	*octets_1;
	unsigned char	*octets_2;
	size_t			i;

	octets_1 = (unsigned char *)p1;
	octets_2 = (unsigned char *)p2;

	i = 0;
	while (i < n)
	{
		if (octets_1[i] != octets_2[i])
			return (octets_1[i] - octets_2[i]);
		i++;
	}

	return (0);
}

void	*ft_memcpy(void *dst, const void *src, size_t srcsize)
{
	size_t			ix;
	unsigned char	*p1;
	unsigned char	*p2;

	ix = 0;
	p1 = (unsigned char *)dst;
	p2 = (unsigned char *)src;

	while (ix < srcsize)
	{
		p1[ix] = p2[ix];
		ix++;
	}

	return (dst);
}

// Copy destsize bytes from src to dest.
// If destsize is bigger than srcsize set extra bytes (destsize - srcsize) to zero
void	*ft_memzcpy(void *dst, const void *src, size_t dstsize, size_t srcsize)
{
	if (dstsize > srcsize)
	{
		ft_memcpy(dst, src, srcsize);
		ft_memset(dst + srcsize, 0, dstsize - srcsize);
	}
	else
	{
		ft_memcpy(dst, src, dstsize);
	}
	return (dst);
}

void	*ft_memset(void *buf, int c, size_t len)
{
	size_t	i;

	i = 0;
	while (i < len)
		((unsigned char *)buf)[i++] = (unsigned char)c;

	return (buf);
}

void	*ft_memmove(void *dst, const void *src, size_t size)
{
	unsigned char	*p1;
	unsigned char	*p2;
	size_t	ix;

	p1 = (unsigned char *)dst;
	p2 = (unsigned char *)src;
	ix = 0;

	// Check if destination address is "after" source address.
	// If so, copy in reverse order to avoid destructive copy
	// for overlapping strings
	if (dst >= src)
	{
		while (size--)
			p1[size] = p2[size];
	}
	else
	{
		while (ix < size)
		{
			p1[ix] = p2[ix];
			ix++;
		}
	}
	return (dst);
}
