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

#include <ft_ssl.h>

t_htbl  *util_task_htable(const t_task *task, int size)
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
		ft_htbl_add(htbl, task->key, (void *)task);
		task++;
	}

	return (htbl);
}
