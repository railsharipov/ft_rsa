/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putendl_fd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/11 14:04:51 by rsharipo          #+#    #+#             */
/*   Updated: 2018/07/14 20:32:37 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>
#include <unistd.h>

int	ft_putendl_fd(int fd, char const *content, size_t size)
{
	if ((NULL != content) && (size > 0))
	{
		if (write(fd, content, size) < 0)
			return (-1);
		if (write(fd, "\n", 1) < 0)
			return (-1);
	}
	return (0);
}
