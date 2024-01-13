/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putstr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/11 14:01:01 by rsharipo          #+#    #+#             */
/*   Updated: 2018/07/14 20:30:50 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <libft/string.h>

void	ft_putstr_fd(int fd, char const *s)
{
	if (NULL != s && fd >= 0)
		write(fd, s, ft_strlen(s));
}
