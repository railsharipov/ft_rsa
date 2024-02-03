/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/11 10:33:59 by rsharipo          #+#    #+#             */
/*   Updated: 2018/07/12 15:34:03 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdarg.h>
#include <libft/string.h>
#include <libft/alloc.h>

char	*ft_strjoin_multi(int n, ...)
{
	char	*res;
	char	*s;
	size_t	slen;
	va_list arg;
	int		idx;

	if (n <= 0) {
		return (NULL);
	}
	va_start(arg, n);
	slen = 0;

	idx = 0;
	while (idx < n) {
		s = va_arg(arg, char *);
		slen += ft_strlen(s);
		idx++;
	}
	va_end(arg);

	LIBFT_ALLOC(res, slen+1);

	va_start(arg, n);
	slen = 0;

	idx = 0;
	while (idx < n) {
		s = va_arg(arg, char *);
		res = ft_strcat(res, s);
		idx++;
	}
	va_end(arg);

	return (res);
}
