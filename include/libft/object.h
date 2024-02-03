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

#ifndef LIBFT_OBJECT_H
# define LIBFT_OBJECT_H

# include <string.h>
# include <libft/common.h>
# include <libft/node.h>

typedef	t_node  t_object;

enum    e_libft_type
{
    LIBFT_OBJECT,
    LIBFT_ARRAY,
    LIBFT_LIST,
    LIBFT_NODE,
    LIBFT_STACK,
    LIBFT_QUEUE,
    LIBFT_HTABLE,
    LIBFT_CSTRING,
    LIBFT_STRING,
    LIBFT_OSTRING,
    LIBFT_BIGNUM,
    LIBFT_INT64,
    LIBFT_UINT64,
    LIBFT_BOOLEAN,
    LIBFT_NULL,
};

#endif
