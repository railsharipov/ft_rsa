/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/03 09:02:36 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/26 10:39:00 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <libft/std.h>
#include <libft/string.h>
#include <libft/alloc.h>

static int __line_complete;

int	read_to_buf(char *buf, int fd, int size)
{
	char	c;
	int		rbytes;
	int		tbytes;

	tbytes = 0;
	while (tbytes < size)
	{
		c = 0;

		if (0 > (rbytes = read(fd, &c, 1)))
			return (-1);

		*buf++ = c;
		tbytes += rbytes;

		if (rbytes == 0 || c == '\n')
		{
			__line_complete = 1;
			break;
		}
	}

	*buf = 0;

	return (tbytes);
}

int	get_next_line(int fd, char **line)
{
	char	buf[LIBFT_BUFF + 1];
	int		rbytes;
	int		tbytes;

	__line_complete = 0;

	if ((NULL == line) || (fd < 0))
		return (-1);

	LIBFT_ALLOC(*line, 1);

	tbytes = 0;
	while ((rbytes = read_to_buf(buf, fd, LIBFT_BUFF)) > 0)
	{
		LIBFT_REALLOC(*line, tbytes+1, tbytes+rbytes+1);
		ft_strcat(*line, buf);
		tbytes += rbytes;

		if (__line_complete)
			break ;
	}

	if (rbytes < 0)
		return (-1);

	return (tbytes);
}
