/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_malloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/27 08:50:55 by rsharipo          #+#    #+#             */
/*   Updated: 2018/10/13 10:00:53 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>

static t_node	*__memlist;
int				global_ft_malloc_error;

t_node	*__lookup(void *memptr)
{
	t_node *node;

	node = __memlist;
	while (node)
	{
		if (node->content == memptr)
		{
			return (node);
		}
		node = node->next;
	}
	return (NULL);
}

int	__push(const char *memkey, void *memptr, size_t memsize)
{
	t_node	*node;

	if (NULL == (node = malloc(sizeof(t_node))))
	{
		return (LIBFT_MEM_FATAL);
	}
	node->key = (char *)memkey;
	node->content = memptr;
	node->size = memsize;
	node->next = __memlist;
	__memlist = node;
	return (LIBFT_OK);
}

void	__clear(void)
{
	t_node	*node;

	while (__memlist)
	{
		node = __memlist;
		__memlist = __memlist->next;
		free(node);
	}
}

void	*ft_malloc(const char *memkey, size_t memsize)
{
	void	*memptr;
	t_node	*lookup_node;

	global_ft_malloc_error = LIBFT_OK;

	memptr = malloc(memsize);
	ft_bzero(memptr, memsize);

	if (ENOMEM == errno)
	{
		global_ft_malloc_error = LIBFT_MEM_FATAL;
		return (NULL);
	}
	if (NULL != (lookup_node = __lookup(memptr)))
	{
		global_ft_malloc_error = LIBFT_MEM_LEAK;
		return (NULL);
	}
	if (LIBFT_OK != __push(memkey, memptr, memsize))
	{
		global_ft_malloc_error = LIBFT_MEM_FATAL;
		return (NULL);
	}
	return (memptr);
}

void	ft_free(const char *memkey, void *memptr)
{
	t_node	*node;
	t_node	*prevnode;

	global_ft_malloc_error = LIBFT_OK;

	node = __memlist;
	prevnode = NULL;
	while (node)
	{
		if (node->content == memptr)
			break ;
		prevnode = node;
		node = node->next;
	}
	if (NULL == node)
	{
		global_ft_malloc_error = LIBFT_MEM_DOUBLE_FREE;
		return ;
	}
	if (NULL == prevnode)
	{
		__memlist = node->next;
	}
	else
	{
		prevnode->next = node->next;
	}
	free(node->content);
	free(node);
}

void	ft_free_all(void)
{
	__clear();
}
