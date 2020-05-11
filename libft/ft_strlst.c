/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlst.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/10 10:59:03 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/10 11:07:19 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>
#include <stdarg.h>

int	ft_strlst(const char *s, int num, ...)
{
	va_list	arg;
	int		check;

	va_start(arg, num);
	check = 0;
	while (--num >= 0)
		if (!ft_strcmp(s, va_arg(arg, char *)))
			check = 1;
	va_end(arg);
	return (check);
}
