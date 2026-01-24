#ifndef FT_ARGS_H
# define FT_ARGS_H

# include <common.h>
# include <alloc.h>
# include <libft/htable.h>
# include <libft/logger.h>

# define ARGP_LOGGER_NAME	"args"
# define ARGP_LOG(LEVEL, MES, ...)	ft_logger_log(__func__, __FILE__, __LINE__, NULL, ARGP_LOGGER_NAME, LIBFT_LOG_LEVEL_##LEVEL, MES __VA_OPT__(,) __VA_ARGS__)

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

t_arg_cmd	*args_new_cmd(const char *name, const char *desc, void *func);
t_arg_opt	*args_new_opt(const char *name, const char *desc, t_opt_type type);

int     	args_add_sub_cmd(t_arg_cmd *cmd_arg, const t_arg_cmd *sub_cmd_arg);
int     	args_add_cmd_opt(t_arg_cmd *cmd_arg, const t_arg_opt *opt_arg);
int     	args_add_global_cmd_opt(t_arg_cmd *cmd_arg, const t_arg_opt *opt_arg);

t_arg_cmd	*args_copy_cmd(const t_arg_cmd *cmd_arg);
t_arg_opt	*args_copy_opt(const t_arg_opt *opt_arg);

void		args_del_cmd(t_arg_cmd *cmd_arg);
void		args_del_opt(t_arg_opt *opt_arg);

char		*args_dump_helps(const t_arg_cmd *cmd_arg);
void		args_dump_help(const t_arg_cmd *cmd_arg);

int			args_parse(t_cmd *cmd, t_arg_cmd *cmd_arg, const char **argv, int argc);

#endif
