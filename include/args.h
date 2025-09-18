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

#ifndef FT_ARGS_H
# define FT_ARGS_H

# include <common.h>
# include <alloc.h>
# include <libft/htable.h>
# include <libft/logger.h>

# define ARGP_LOG(LEVEL, MES, ...)	args_logger_log(__func__, __FILE__, __LINE__, LIBFT_LOG_LEVEL_##LEVEL, MES __VA_OPT__(,) __VA_ARGS__)

# define DEFAULT_CMD	"default"

typedef enum	e_args_type
{
	AP_ARG_TYPE_FLAG = 0,
	AP_ARG_TYPE_STRING,
	AP_ARG_TYPE_NUMBER,
}				t_args_type;

typedef struct	s_args
{
	t_htbl		*htbl;
}				t_args;

typedef struct	s_args_def
{
	char		*name;
	char		*desc;
	t_args_type	type;
}				t_args_def;

typedef struct	s_args_opt
{
	t_args_type	type;
	char		*name;
	char		*desc;
	char		*value;
	int			pos;
	int			set;
}				t_args_opt;

typedef struct	s_args_cmd
{
	char		*name;
	char		*desc;
	void		*func;
	t_htbl		*opts;
}				t_args_cmd;

int		args_logger_log(const char *func_name, const char *file_name, int line_number, uint8_t level, const char *fmt, ...);

int     	args_init(t_args *args);
t_args_cmd	*args_new_cmd(const char *name, const char *desc, void *func);
t_args_opt	*args_new_opt(const char *name, const char *desc, t_args_type type);
int     	args_add_cmd(t_args *args, const t_args_cmd *cmd);
int     	args_cmd_add_opt(t_args_cmd *cmd, const t_args_opt *opt);
int     	args_parse(t_args *args, const char **sargs);

t_args_cmd	*args_get_cmd(const t_args *args, const char *cmd_name);
t_args_opt	*args_cmd_get_opt(const t_args_cmd *cmd, const char *opt_name);
int     	args_cmd_opt_is_set(const t_args_cmd *cmd, const char *opt_name);
int     	args_cmd_opt_is_eq(const t_args_cmd *cmd, const char *opt_name, const char *opt_value);
const char *args_cmd_opt_get_val(const t_args_cmd *cmd, const char *opt_name);

char	*args_dump_cmd_helps(const t_args_cmd *cmd);
char	*args_dump_helps(const t_args *args);
void	args_dump_help(const t_args *args);

#endif
