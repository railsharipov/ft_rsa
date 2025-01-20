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

#include <libft/node.h>
#include <libft/stack.h>
#include <libft/list.h>
#include <libft/alloc.h>

t_stack	*ft_stack_create(void)
{
	t_stack	*stack;

	LIBFT_ALLOC(stack, sizeof(t_stack));

	return (stack);
}

void	*ft_stack_pop(t_stack *stack)
{
	t_node	*node;
	void	*content;

	if ((NULL == stack) || (NULL == stack->top)) {
		return (NULL);
	}
	node = stack->top;
	content = node->content;

	stack->top = stack->top->next;

	LIBFT_FREE(node->key);
	LIBFT_FREE(node);

	return (content);
}

t_node	*ft_stack_pop_node(t_stack *stack)
{
	t_node	*node;

	if ((NULL == stack) || (NULL == stack->top)) {
		return (NULL);
	}
	node = stack->top;
	stack->top = stack->top->next;

	return (node);
}

void	ft_stack_push(t_stack *stack, const char *key, void *content, size_t size)
{
	if (NULL == stack) {
		return ;
	}
	ft_lst_prepend(&stack->top, ft_node_new(key, content, size));
}

void	ft_stack_push_node(t_stack *stack, t_node *node)
{
	if (NULL == stack) {
		return ;
	}
	ft_lst_prepend(&stack->top, node);
}

t_node	*ft_stack_peek(t_stack *stack)
{
	if (NULL == stack) {
		return (NULL);
	}
	return (stack->top);
}

int		ft_stack_is_empty(t_stack *stack)
{
	if ((NULL == stack) || (NULL == stack->top)) {
		return (1);
	}
	return (0);
}

int		ft_stack_size(t_stack *stack)
{
	if ((NULL == stack) || (NULL == stack->top)) {
		return (0);
	}
	return (ft_lst_size(stack->top));
}

void	ft_stack_del(t_stack *stack)
{
	if (NULL == stack) {
		return ;
	}
	ft_lst_del(stack->top);
	LIBFT_FREE(stack);
}

void	ft_stack_del_with_f_del(t_stack *stack, FUNC_CONTENT_DEL f_del)
{
	if (NULL == stack) {
		return ;
	}
	ft_lst_del_with_f_del(stack->top, f_del);
	LIBFT_FREE(stack);
}

t_htbl	*ft_stack_to_htable(t_stack *stack)
{
	if ((NULL == stack) || (NULL == stack->top)) {
		return (NULL);
	}
	return (ft_lst_to_htable(stack->top));
}

t_node	*ft_stack_pop_list(t_stack *stack)
{
	t_node	*node;

	node = ft_stack_peek(stack);
	stack->top = NULL;

	return (node);
}
