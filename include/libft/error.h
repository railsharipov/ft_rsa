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
	LIBFT_ERROR_LEVEL_ERROR = 0,
	LIBFT_ERROR_LEVEL_WARN,
	LIBFT_ERROR_LEVEL_INFO,
	LIBFT_ERROR_LEVEL_DEBUG,
	LIBFT_ERROR_LEVEL_TRACE,
	LIBFT_NUM_ERROR_LEVELS,
};

FUNC_ERR_LOGGER	ft_logger_get_logger(void);
void			ft_logger_set_logger(FUNC_ERR_LOGGER f_logger);
void			ft_logger_set_level(uint8_t level);

int	ft_logger_log(const char *func_name, const char *file_name, int line_number, uint8_t level, const char *fmt, ...);
int	ft_logger_va_log(const char *func_name, const char *file_name, int line_number, uint8_t level, const char *fmt, va_list va_arg);

#endif
