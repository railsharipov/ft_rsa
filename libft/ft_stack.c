/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_stack.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/13 09:33:08 by rsharipo          #+#    #+#             */
/*   Updated: 2018/08/22 11:25:55 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>

t_stack	*ft_stack_init(void)
{
	t_stack	*stack;

	LIBFT_ALLOC(stack, sizeof(t_stack));
	
	return (stack);
}

void	*ft_stack_pop(t_stack *stack)
{
	t_node	*node;
	void	*content;

	if ((NULL == stack) || (NULL == stack->top))
		return (NULL);

	node = stack->top;
	content = node->content;

	stack->top = stack->top->next;

	LIBFT_FREE(node->key);
	LIBFT_FREE(node);

	return (content);
}

void	ft_stack_push(
	t_stack *stack, const char *key, void *content, size_t size)
{
	t_node	*node;

	if (NULL == stack)
		return ;
	
	node = ft_node_new(key, content, size);

	ft_lst_prepend(&stack->top, node);
}

t_node	*ft_stack_peek(t_stack *stack)
{
	if (NULL == stack)
		return (NULL);
	
	return (stack->top);
}

int		ft_stack_is_empty(t_stack *stack)
{
	if ((NULL == stack) || (NULL == stack->top))
		return (1);
	
	return (0);
}

int		ft_stack_size(t_stack *stack)
{
	if ((NULL == stack) || (NULL == stack->top))
		return (0);
	
	return (ft_lst_size(stack->top));
}

int	ft_stack_del(t_stack *stack, int (*f_del)(t_node *))
{
	if (NULL == stack)
		return (LIBFT_ERR);
	

	if (LIBFT_OK != ft_lst_del(stack->top, f_del))
		return (LIBFT_ERR);
	
	LIBFT_FREE(stack);

	return (LIBFT_OK);
}

t_htbl	*ft_stack_htable(t_stack *stack)
{
	if ((NULL == stack) || (NULL == stack->top))
		return (NULL);
	
	return (ft_lst_htable(stack->top));
}
