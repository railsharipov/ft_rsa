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

#ifndef FT_SSL_H
# define FT_SSL_H

# include <string.h>
# include <alloc.h>
# include <libft/htable.h>
# include <libft/logger.h>

# define SSL_ERROR(MES, ...)	util_logger_log(__func__, __FILE__, __LINE__, LIBFT_LOG_LEVEL_ERROR, "ssl ", MES __VA_OPT__(,) __VA_ARGS__)
# define SSL_INFO(MES, ...) 	util_logger_log(__func__, __FILE__, __LINE__, LIBFT_LOG_LEVEL_INFO, "ssl ", MES __VA_OPT__(,) __VA_ARGS__)
# define SSL_DEBUG(MES, ...)	util_logger_log(__func__, __FILE__, __LINE__, LIBFT_LOG_LEVEL_DEBUG, "ssl ", MES __VA_OPT__(,) __VA_ARGS__)

# define SSL_FLAG(F,X)		((int)(((X)&(F))==(F)))
# define NONE	0

# define INVALID_INPUT_ERROR	"invalid input"
# define UNSPECIFIED_ERROR		"unspecified error"
# define UNEXPECTED_ERROR		"unexpected error"
# define NOT_IMPLEMENTED_ERROR	"not implemented"

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

typedef int		(*FUNC_COM)(const char **, const char *);

typedef uint32_t	t_bool;
typedef __uint128_t	uint128_t;

typedef struct	s_task
{
	char		*key;
	void		*ptr;
	uint32_t	tflag;
	uint32_t	gflag;
	uint32_t	oflag;
	uint32_t	val;
}				t_task;

FUNC_LOGGER		util_logger_get_logger(void);
void			util_logger_set_logger(FUNC_LOGGER f_logger);
void			util_logger_set_level(uint8_t level);
void			util_logger_enable_ansi_color(void);
void			util_logger_disable_ansi_color(void);
int				util_logger_log(const char *func_name, const char *file_name, int line_number, uint8_t level, const char *fmt_prefix, const char *fmt, ...);

void			cli_print_usage(void);
char			*util_getpass(void);
int				util_setpass(const char *);
void			util_unsetpass(void);

t_htbl  		*cli_task_htable(const t_task *, int);
void			cli_task_htable_del(const t_htbl *);

#endif
