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

#ifndef LIBFT_2DARRAY_H
# define LIBFT_2DARRAY_H

# include <string.h>
# include <libft/common.h>

void		ft_2darray_del(void **, int);
void 		ft_2darray_del_null_terminated(void **);
int			ft_2darray_len_null_terminated(void **);
char		*ft_2darray_strjoin(char **, size_t, const char *);

#endif
