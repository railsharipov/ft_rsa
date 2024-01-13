#include <util/parser.h>
#include <libft/string.h>
#include <libft/std.h>

// Find ostring 'pattern' in ostring 'octets'
int parser_find(const char *octets, int olen, const char *pattern, int patlen)
{
	unsigned int pathash, winhash;
	unsigned char *oct, *pat;
	int mod;
	int ix, iy;

	if (NULL == pattern || NULL == octets) {
		return (-1);
	}
	pat = (unsigned char *)pattern;
	oct = (unsigned char *)octets;

	if (patlen > olen) {
		return (-1);
	}
	if (patlen == 0) {
		return (0);
	}
	mod = 0;
	pathash = 0;
	winhash = 0;

	for (ix = 0; ix < patlen; ix++) {
		mod = (mod != 0) ? ((mod * RK_ALPHA) % RK_PRIME) : (1);
		pathash = (pathash * RK_ALPHA + pat[ix]) % RK_PRIME;
		winhash = (winhash * RK_ALPHA + oct[ix]) % RK_PRIME;
	}
	for (ix = 0; ix < olen-patlen; ix++) {
		if (winhash == pathash) {
			if (!ft_memcmp(oct + ix, pat, patlen)) {
				return (ix);
			}
		}
		winhash = (winhash + RK_PRIME) - (oct[ix] * mod) % RK_PRIME;
		winhash = (RK_ALPHA * winhash + oct[ix+patlen]) % RK_PRIME;
	}
	if (winhash == pathash) {
		if (!ft_memcmp(oct + ix, pat, patlen)) {
			return (ix);
		}
	}
	return (-1);
}
