/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ssl.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/06 10:47:08 by rsharipo          #+#    #+#             */
/*   Updated: 2018/10/05 08:45:31 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SSL_COMMON_H
# define SSL_COMMON_H

# include <stdint.h>
# include <stddef.h>
# include <alloc.h>
# include <libft/common.h>

# define SSL_GLOBAL_LOG_LEVEL		LIBFT_LOG_LEVEL_DEBUG

# define SSL_FLAG(FLAG, FLAGS)			((int)(((FLAGS)&(FLAG))==(FLAG)))

# define INVALID_INPUT_ERROR	"invalid input"
# define UNSPECIFIED_ERROR		"unspecified error"
# define UNEXPECTED_ERROR		"unexpected error"
# define NOT_IMPLEMENTED_ERROR	"not implemented"

# define NONE	0

enum e_ssl_boolean {
	SSL_FALSE 	= 0,
	SSL_TRUE 	= 1,
};

enum	e_ssl_status
{
	SSL_OK		= 0,
	SSL_ERR		= 1,
	SSL_STATUS_COUNT
};

typedef uint32_t	t_bool;
typedef __uint128_t	uint128_t;

#endif
