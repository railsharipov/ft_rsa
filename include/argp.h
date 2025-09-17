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

#ifndef FT_ARGP_H
# define FT_ARGP_H

# include <common.h>
# include <alloc.h>
# include <libft/htable.h>
# include <libft/logger.h>

# define ARGP_LOG(LEVEL, MES, ...)	argp_logger_log(__func__, __FILE__, __LINE__, LIBFT_LOG_LEVEL_##LEVEL, MES __VA_OPT__(,) __VA_ARGS__)

# define DEFAULT_CMD	"default"

typedef enum	e_ap_type
{
	AP_ARG_TYPE_FLAG = 0,
	AP_ARG_TYPE_STRING,
	AP_ARG_TYPE_NUMBER,
}				t_ap_type;

typedef struct	s_ap_parser
{
	t_htbl		*htbl;
}				t_ap_parser;

typedef struct	s_ap_opt_def
{
	char		*key;
	char		*desc;
	t_ap_type	type;
}				t_ap_opt_def;

typedef struct	s_ap_opt
{
	t_ap_type	type;
	char		*name;
	char		*desc;
	char		*value;
	int			pos;
	int			set;
}				t_ap_opt;

typedef struct	s_ap_cmd
{
	char		*name;
	char		*desc;
	void		*func;
	t_htbl		*opts;
}				t_ap_cmd;

int		argp_logger_log(const char *func_name, const char *file_name, int line_number, uint8_t level, const char *fmt, ...);

int     	argp_parser_init(t_ap_parser *parser);
t_ap_cmd	*argp_new_cmd(const char *name, const char *desc, const t_ap_opt_def *opt_def_arr, size_t arr_size, void *func);
int     	argp_add_cmd(t_ap_parser *parser, const t_ap_cmd *cmd);
int     	argp_parse(t_ap_parser *parser, const char **sargs);

t_ap_cmd	*argp_get_cmd(t_ap_parser *parser, const char *cmd_name);
t_ap_opt	*argp_get_opt(t_ap_cmd *cmd, const char *opt_name);

#endif
