/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ssl_htable.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/13 10:13:11 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/13 10:28:24 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ssl/ssl.h>
#include <libft/htable.h>

t_htbl  *ssl_task_htable(const t_task *task, int size)
{
	t_htbl  *htbl;
	int		ix;

	if (NULL == task)
		return (NULL);

	if (NULL == (htbl = ft_htbl_init(size)))
		return (NULL);

	ix = 0;
	while (ix++ < size)
	{
		ft_htbl_add(htbl, (void *)task, task->key);
		task++;
	}

	return (htbl);
}

void	ssl_task_htable_del(const t_htbl *htbl)
{
	if (NULL == htbl)
		return ;

	ft_htbl_del((t_htbl *)htbl);
}
