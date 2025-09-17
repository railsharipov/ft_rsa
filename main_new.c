/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/06 11:09:10 by rsharipo          #+#    #+#             */
/*   Updated: 2018/10/13 10:41:35 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <common.h>
#include <base64.h>
#include <des.h>
#include <hash.h>
#include <rsa.h>
#include <cmd.h>
#include <argp.h>
#include <libft/string.h>

static const char *B64_CMD_NAME = "base64";
static const t_ap_opt_def B64_CMD_OPTS[] = {
	/*	KEY	PTR	TFLAG	GFLAG	OFLAG	VAL	*/
	{ .key = "-b", .desc = "line length", .type = AP_ARG_TYPE_NUMBER },
	{ .key = "-e", .desc = "encode", .type = AP_ARG_TYPE_FLAG },
	{ .key = "-d", .desc = "decode", .type = AP_ARG_TYPE_FLAG },
	{ .key = "-i", .desc = "input file", .type = AP_ARG_TYPE_STRING },
	{ .key = "-o", .desc = "output file", .type = AP_ARG_TYPE_STRING },
};

static struct {
	char			*name_comm;
	FUNC_COM		func_comm;
	t_ap_opt_def	*options;
	size_t			num_options;
} COMMAND[] = {
	{ "md5", cmd_hash },
	{ "sha1", cmd_hash },
	{ "sha224", cmd_hash },
	{ "sha256", cmd_hash },
	{ "sha384", cmd_hash },
	{ "sha512", cmd_hash },
	{ "sha512/224", cmd_hash },
	{ "sha512/256", cmd_hash },
	{ B64_CMD_NAME, cmd_base64 },
	{ "des-ecb", cmd_des_ecb },
	{ "des-cbc", cmd_des_cbc },
	{ "genrsa", cmd_rsa_gen },
	{ "rsa", cmd_rsa },
	{ "rsautl", cmd_rsa_utl },
	{ "test", cmd_test },
	{ NULL, NULL }
};

static void __get_command(FUNC_COM *func_comm, char **name_comm, const char *sarg);
static void __print_usage(void);

int	main(int ac, const char **av)
{
	t_ap_parser	parser;

	if (ac < 2) {
		__print_usage();
		exit(1);
	}

	argp_parser_init(&parser);
	argp_new_cmd("base64", "base64 command", NULL, 0, NULL);

	return (0);
}

static void	__get_command(FUNC_COM *func_comm, char **name_comm, const char *sarg)
{
	int	ix;

	*func_comm = NULL;
	*name_comm = NULL;

	ix = 0;
	while (NULL != COMMAND[ix].name_comm) {
		if (!ft_strcmp(COMMAND[ix].name_comm, sarg)) {
			*func_comm = COMMAND[ix].func_comm;
			*name_comm = COMMAND[ix].name_comm;
			break ;
		}
		ix++;
	}
}

static void __print_usage(void) {
	cli_logger_print_file("./docs/usage.txt");
}
