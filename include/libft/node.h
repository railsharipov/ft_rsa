/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/09 13:36:51 by rsharipo          #+#    #+#             */
/*   Updated: 2018/10/13 09:00:28 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_NODE_H
# define LIBFT_NODE_H

# include <stdint.h>
# include <string.h>
# include <libft/common.h>
# include <libft/vfunc.h>

typedef struct		s_node
{
	uint64_t		hash;
	uint8_t			type;
	char			*key;
	void			*content;
	size_t			size;
	struct s_node	*next;
	struct s_node	*nodes;
	void			(*f_del_content)(void *); /* t_func_content_del */
}					t_node;

typedef	void (*t_func_content_del)(void *content);

int 		ft_node_is_parent(t_node *);

void 		ft_node_init(t_node *);
void 		ft_node_init_with_f_del(t_node *, t_func_content_del);

t_node		*ft_node_new(const char *, void *, size_t);
t_node		*ft_node_new_with_f_del(const char *, void *, size_t, t_func_content_del);

void 		ft_node_del(t_node *);
void 		ft_node_del_with_f_del(t_node *, t_func_content_del);

t_node		*ft_node_create(void);
t_node		*ft_node_create_with_f_del(t_func_content_del);

#endif
