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

uint32_t	ft_hash(const unsigned char *, size_t);

void		*ft_htbl_init(int);
t_node		*ft_htbl_iter(t_htbl *);
void		(ft_htbl_add)(t_htbl *, void *, const char *);
void		(ft_htbl_add_rawkey)(t_htbl *, void *, const void *, size_t);
void		(ft_htbl_assign)(t_htbl *, void *, const char *);
void		(ft_htbl_assign_rawkey)(t_htbl *, void *, const void *, size_t);
void		(ft_htbl_del)(t_htbl *);
void		(ft_htbl_del_key)(t_htbl *htbl, const char *key);
void		(ft_htbl_del_rawkey)(t_htbl *htbl, const char *key, const void *, size_t);
void		ft_htbl_add_with_f_del(t_htbl *, void *, const char *, FUNC_CONTENT_DEL);
void		ft_htbl_add_rawkey_with_f_del(t_htbl *, void *, const void *, size_t, FUNC_CONTENT_DEL);
void		ft_htbl_assign_with_f_del(t_htbl *, void *, const char *, FUNC_CONTENT_DEL);
void		ft_htbl_assign_rawkey_with_f_del(t_htbl *, void *, const void *, size_t, FUNC_CONTENT_DEL);
void		ft_htbl_del_with_f_del(t_htbl *, FUNC_CONTENT_DEL);
void		ft_htbl_del_key_with_f_del(t_htbl *htbl, const char *key, FUNC_CONTENT_DEL);
void		ft_htbl_del_rawkey_with_f_del(t_htbl *htbl, const char *key, const void *, size_t, FUNC_CONTENT_DEL);
void		*ft_htbl_get(t_htbl *, const char *);
void		*ft_htbl_get_rawkey(t_htbl *, const void *, size_t);
void 		ft_htbl_resize(t_htbl *htbl, int size);

# define 	ft_htbl_add4(...) ft_htbl_add_with_f_del(__VA_ARGS__)
# define 	ft_htbl_add3(...) ft_htbl_add(__VA_ARGS__)
# define 	ft_htbl_add_rawkey5(...) ft_htbl_add_rawkey_with_f_del(__VA_ARGS__)
# define 	ft_htbl_add_rawkey4(...) ft_htbl_add_rawkey(__VA_ARGS__)
# define 	ft_htbl_assign4(...) ft_htbl_assign_with_f_del(__VA_ARGS__)
# define 	ft_htbl_assign3(...) ft_htbl_assign(__VA_ARGS__)
# define 	ft_htbl_assign_rawkey5(...) ft_htbl_assign_rawkey_with_f_del(__VA_ARGS__)
# define 	ft_htbl_assign_rawkey4(...) ft_htbl_assign_rawkey(__VA_ARGS__)
# define 	ft_htbl_del2(...) ft_htbl_del_with_f_del(__VA_ARGS__)
# define 	ft_htbl_del1(...) ft_htbl_del(__VA_ARGS__)
# define 	ft_htbl_del_key3(...) ft_htbl_del_key_with_f_del(__VA_ARGS__)
# define 	ft_htbl_del_key2(...) ft_htbl_del_key(__VA_ARGS__)
# define 	ft_htbl_del_rawkey4(...) ft_htbl_del_rawkey_with_f_del(__VA_ARGS__)
# define 	ft_htbl_del_rawkey3(...) ft_htbl_del_rawkey(__VA_ARGS__)
# define	ft_htbl_add(...) VFUNC(ft_htbl_add, __VA_ARGS__)
# define	ft_htbl_add_rawkey(...) VFUNC(ft_htbl_add_rawkey, __VA_ARGS__)
# define	ft_htbl_assign(...) VFUNC(ft_htbl_assign, __VA_ARGS__)
# define	ft_htbl_assign_rawkey(...) VFUNC(ft_htbl_assign_rawkey, __VA_ARGS__)
# define	ft_htbl_del(...) VFUNC(ft_htbl_del, __VA_ARGS__)

#endif