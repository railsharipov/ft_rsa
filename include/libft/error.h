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

#ifndef LIBFT_ERROR_H
# define LIBFT_ERROR_H

# include <string.h>
# include <stdarg.h>
# include <sys/errno.h>
# include <libft/alloc.h>
# include <libft/string.h>

typedef int	(*FUNC_ERR_LOGGER)(const char *mes);

enum	e_error_level
{
	LIBFT_ERROR_LEVEL_INFO = 0,
	LIBFT_ERROR_LEVEL_DEBUG,
	LIBFT_NUM_ERROR_LEVELS,
};

FUNC_ERR_LOGGER	ft_error_get_logger(void);
void			ft_error_set_logger(FUNC_ERR_LOGGER f_logger);
void			ft_error_set_level(uint8_t level);

int	ft_error_log(const char *func_name, const char *file_name, int line_number, const char *fmt, ...);
int	ft_verror_log(const char *func_name, const char *file_name, int line_number, const char *fmt, va_list va_arg);

int	ft_error_log_info(const char *fmt, ...);
int	ft_verror_log_info(const char *fmt, va_list va_arg);

int	ft_error_log_debug(const char *func_name, const char *file_name, int line_number, const char *fmt, ...);
int	ft_verror_log_debug(const char *func_name, const char *file_name, int line_number, const char *fmt, va_list va_arg);

#endif
