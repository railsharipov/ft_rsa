/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_lst_func.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/30 19:20:09 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/27 10:10:15 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <libft/std.h>

void	pf_l_arg_zero(t_pf *l)
{
	if (l)
	{
		l->flag = 0;
		l->len = 0;
		l->fwid = 0;
		l->prec = -1;
		l->type = -1;
		l->size = 0;
		l->str_size = 0;
		l->space = 0;
		l->base = 10;
	}
}

t_pf	*pf_l_arg_new(int fd)
{
	t_pf	*new;

	if (NULL == (new = malloc(sizeof(t_pf))))
		return (NULL);

	ft_bzero(new, sizeof(t_pf));

	new->fd = fd;
	new->prec = -1;
	new->type = -1;
	new->base = 10;

	return (new);
}
