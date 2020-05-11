/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_bzero.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/09 14:54:22 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/06 11:08:30 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>

int	ft_error(const char *input, const char *mes)
{
	char	ferr[LIBFT_ERRBUF] = { 0 };
	char	*eptr;

	eptr = ft_strcat(ferr, "libft: ");

	ft_strncat(eptr, ((NULL == input) ? "" : input), 256);
	eptr = ft_strcat(eptr, ": ");

	ft_strncat(eptr, ((NULL == mes) ? "" : mes), 256);

	if (errno)
		perror(ferr);
	else
		ft_printf("%@%s\n", ferr);

	return (LIBFT_ERR);
}
