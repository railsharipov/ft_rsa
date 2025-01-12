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

typedef int	(*FUNC_LOGGER)(const char *mes);

enum	e_logger_level
{
	LIBFT_LOG_LEVEL_CRIT = 0,
	LIBFT_LOG_LEVEL_ERROR,
	LIBFT_LOG_LEVEL_WARN,
	LIBFT_LOG_LEVEL_INFO,
	LIBFT_LOG_LEVEL_DEBUG,
	LIBFT_LOG_LEVEL_TRACE,
	LIBFT_NUM_LOG_LEVELS,
};

FUNC_LOGGER		ft_logger_get_logger(void);
void			ft_logger_set_logger(FUNC_LOGGER f_logger);
void			ft_logger_set_level(uint8_t level);
uint8_t			ft_logger_get_level(void);
void 			ft_logger_enable_ansi_color(void);
void 			ft_logger_disable_ansi_color(void);

int ft_logger_log(const char *func_name, const char *file_name, int line_number, uint8_t level, const char *fmt, ...);
int	ft_logger_va_log(const char *func_name, const char *file_name, int line_number, uint8_t level, const char *fmt, va_list va_arg);

#endif
