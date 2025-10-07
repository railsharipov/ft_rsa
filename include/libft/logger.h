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

typedef int	(*t_func_log)(const char *mes);

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

typedef struct	s_logger
{
	t_func_log f_logger;
	uint8_t 	log_level;
	uint8_t 	is_ansi_colored;
	uint8_t 	debug_info_thres;
} 				t_logger;

int	ft_logger_f_default_logger(const char *mes);
int	ft_logger_log(const char *func_name, const char *file_name, int line_number, t_logger *logger, uint8_t level, const char *fmt, ...);
int	ft_logger_va_log(const char *func_name, const char *file_name, int line_number, t_logger *logger, uint8_t level, const char *fmt, va_list va_arg);

#endif
