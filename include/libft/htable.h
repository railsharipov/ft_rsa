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

#ifndef LIBFT_HTABLE_H
# define LIBFT_HTABLE_H

# include <libft/common.h>
# include <libft/node.h>
# include <libft/vfunc.h>

# define LIBFT_HT_SIZE	512

typedef struct		s_htbl
{
	void 			**arr;
	int				size;
}					t_htbl;

uint32_t	ft_hash(const unsigned char *key, size_t keysize);

void		*ft_htbl_create(int size);

t_node		*ft_htbl_node_next(t_htbl *htbl, t_node *node);

int			ft_htbl_has(t_htbl *htbl, const char *key);
void		*ft_htbl_get(t_htbl *htbl, const char *key);
void		*ft_htbl_get_rawkey(t_htbl *htbl, const void *rawkey, size_t rksize);
void		ft_htbl_dump(t_htbl *htbl);

void 		ft_htbl_resize(t_htbl *htbl, int size);

void		ft_htbl_add(t_htbl *htbl, void *content, const char *key);
void		ft_htbl_add_with_f_del(t_htbl *htbl, void *content, const char *key, t_func_content_del f_del);

t_htbl		*ft_htbl_copy(t_htbl *htbl);
t_htbl		*ft_htbl_copy_with_f_copy(t_htbl *htbl, t_func_content_copy f_copy);

void		ft_htbl_merge(t_htbl *htbl_to, t_htbl *htbl_from);
void		ft_htbl_merge_with_f_copy(t_htbl *htbl_to, t_htbl *htbl_from, t_func_content_copy f_copy);

void		ft_htbl_add_rawkey(t_htbl *htbl, void *content, const void *rawkey, size_t rksize);
void		ft_htbl_add_rawkey_with_f_del(t_htbl *htbl, void *content, const void *rawkey, size_t rksize, t_func_content_del f_del);

void		ft_htbl_assign(t_htbl *htbl, void *content, const char *key);
void		ft_htbl_assign_with_f_del(t_htbl *htbl, void *content, const char *key, t_func_content_del f_del);

void		ft_htbl_assign_rawkey(t_htbl *htbl, void *content, const void *rawkey, size_t rksize);
void		ft_htbl_assign_rawkey_with_f_del(t_htbl *htbl, void *content, const void *rawkey, size_t rksize, t_func_content_del f_del);

void		ft_htbl_del(t_htbl *htbl);
void		ft_htbl_del_with_f_del(t_htbl *htbl, t_func_content_del f_del);

void		ft_htbl_del_key(t_htbl *htbl, const char *key);
void		ft_htbl_del_key_with_f_del(t_htbl *htbl, const char *key, t_func_content_del f_del);

void		ft_htbl_del_rawkey(t_htbl *htbl, const char *key, const void *rawkey, size_t rksize);
void		ft_htbl_del_rawkey_with_f_del(t_htbl *htbl, const char *key, const void *rawkey, size_t rksize, t_func_content_del f_del);

#endif