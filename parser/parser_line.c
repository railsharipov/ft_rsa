#include <util/parser.h>
#include <libft/string.h>

char  *parser_line(const char *octets, int olen, const char *pattern, int patlen)
{
	int	idx;
	int	bidx;
	int	eidx;

	if ((idx = parser_find(octets, olen, pattern, patlen)) < 0) {
		return (NULL);
	}
	bidx = idx;
	while (bidx > 0) {
		if (ft_iseol(octets[bidx])) {
			break ;
		}
		bidx--;
	}
	if (ft_iseol(octets[bidx])) {
		bidx++;
	}
	eidx = idx;
	while (eidx < olen) {
		if (ft_iseol(octets[eidx])) {
			break ;
		}
		eidx++;
	}
	return (ft_strsub(octets, bidx, eidx-bidx));
}
