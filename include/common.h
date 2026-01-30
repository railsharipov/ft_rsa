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
# include <libft/logger.h>

# define SSL_LOG(LEVEL, FMT, ...)	logger_log(__func__, __FILE__, __LINE__, LIBFT_LOG_LEVEL_##LEVEL, FMT __VA_OPT__(,) __VA_ARGS__)
# define SSL_FLAG(FLAG, FLAGS)		((int)(((FLAGS)&(FLAG))==(FLAG)))

# define INVALID_INPUT_ERROR	"invalid input"
# define FILE_READ_ERROR		"failed to read file"
# define FILE_WRITE_ERROR		"failed to write file"
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

int			logger_log(const char *func_name, const char *file_name, int line_number, uint8_t level, const char *fmt, ...);
t_logger	*logger_get_logger(void);

#endif
