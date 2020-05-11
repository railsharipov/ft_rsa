/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putendl.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/11 13:38:05 by rsharipo          #+#    #+#             */
/*   Updated: 2018/07/14 20:30:31 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>
#include <unistd.h>

int	ft_putendl(char const *content, size_t size)
{
	if ((NULL != content) && (size > 0))
	{
		if (write(1, content, size) < 0)
			return (-1);
		if (write(1, "\n", 1) < 0)
			return (-1);
	}
	return (0);
}
