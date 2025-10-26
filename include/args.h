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

typedef enum	e_opt_type
{
	AP_OPT_TYPE_FLAG = 0,
	AP_OPT_TYPE_STRING,
	AP_OPT_TYPE_NUMBER,
}				t_opt_type;

typedef struct	s_arg_opt
{
	t_opt_type	type;
	char		*name;
	char		*desc;
	char		*value;
	int			pos;
	int			set;
}				t_arg_opt;

typedef struct	s_arg_cmd
{
	char		*name;
	char		*desc;
	void		*func;
	t_htbl		*sub_cmds;
	t_htbl		*opts;
	t_htbl		*global_opts;
}				t_arg_cmd;

typedef struct	s_cmd
{
	t_arg_cmd	*arg_ref;
	void		*func;
	t_htbl		*opts;
}				t_cmd;

int		args_logger_log(const char *func_name, const char *file_name, int line_number, uint8_t level, const char *fmt, ...);

t_arg_cmd	*args_new_cmd(const char *name, const char *desc, void *func);
t_arg_opt	*args_new_opt(const char *name, const char *desc, t_opt_type type);

t_cmd		*args_parse(t_arg_cmd *cmd_arg, const char **argv, int argc);

int     	args_add_cmd(t_arg_cmd *cmd_arg, const t_arg_cmd *sub_cmd);
t_arg_cmd	*args_get_cmd(const t_arg_cmd *cmd_arg, const char *cmd_name);

int     	args_add_opt(t_arg_cmd *cmd_arg, const t_arg_opt *opt_arg);
int     	args_add_global_opt(t_arg_cmd *cmd_arg, const t_arg_opt *opt_arg);

t_arg_cmd	*args_copy_cmd(const t_arg_cmd *cmd_arg);
t_arg_opt	*args_copy_opt(const t_arg_opt *opt_arg);

char	*args_dump_helps(const t_arg_cmd *cmd_arg);
void	args_dump_help(const t_arg_cmd *cmd_arg);

#endif
