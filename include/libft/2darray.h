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

void		ft_2darray_del(void **arr, int arr_len);
void 		ft_2darray_del_null_terminated(void **arr);
int			ft_2darray_len_null_terminated(void **arr);
char		*ft_2darray_strjoin(char **str_arr, size_t str_arr_len, const char *delim);

#endif
