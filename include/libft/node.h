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
	void			(*f_del)(void *); /* FUNC_CONTENT_DEL */
}					t_node;

typedef	void (*FUNC_CONTENT_DEL)(void *content);

void 		(ft_node_init)(t_node *);
t_node		*(ft_node_create)(void);
t_node		*(ft_node_new)(const char *, void *, size_t);
void 		(ft_node_del)(t_node *);
void 		ft_node_init_with_f_del(t_node *, FUNC_CONTENT_DEL);
t_node		*ft_node_create_with_f_del(FUNC_CONTENT_DEL);
t_node		*ft_node_new_with_f_del(const char *, void *, size_t, FUNC_CONTENT_DEL);
void 		ft_node_del_with_f_del(t_node *, FUNC_CONTENT_DEL);
int			ft_node_is_parent(t_node *);

# define 	ft_node_init2(...) ft_node_init_with_f_del(__VA_ARGS__)
# define 	ft_node_init1(...) ft_node_init(__VA_ARGS__)
# define 	ft_node_create1(...) ft_node_create_with_f_del(__VA_ARGS__)
# define 	ft_node_create0(...) ft_node_create(__VA_ARGS__)
# define 	ft_node_new4(...) ft_node_new_with_f_del(__VA_ARGS__)
# define 	ft_node_new3(...) ft_node_new(__VA_ARGS__)
# define 	ft_node_del2(...) ft_node_del_with_f_del(__VA_ARGS__)
# define 	ft_node_del1(...) ft_node_del(__VA_ARGS__)

# define	ft_node_init(...) VFUNC(ft_node_init, __VA_ARGS__)
# define	ft_node_create(...) VFUNC(ft_node_create, __VA_ARGS__)
# define 	ft_node_new(...) VFUNC(ft_node_new, __VA_ARGS__)
# define 	ft_node_del(...) VFUNC(ft_node_del, __VA_ARGS__)

#endif
