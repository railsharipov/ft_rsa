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

#include <ssl.h>
#include <base64.h>
#include <des.h>
#include <hash.h>
#include <rsa.h>
#include <comm.h>
#include <libft/string.h>

static const struct {
	char			*name_comm;
	FUNC_COM		func_comm;
} COMMAND[] = {
	{	"md5",			cli_hash		},
	{	"sha1",			cli_hash		},
	{	"sha224",		cli_hash		},
	{	"sha256",		cli_hash		},
	{	"sha384",		cli_hash		},
	{	"sha512",		cli_hash		},
	{	"sha512/224",	cli_hash		},
	{	"sha512/256",	cli_hash		},
	{	"base64", 		cli_base64		},
	{	"des-ecb", 		cli_des_ecb	},
	{	"des-cbc",		cli_des_cbc	},
	{	"genrsa",		cli_rsa_gen	},
	{	"rsa",			cli_rsa		},
	{	"rsautl",		cli_rsa_utl	},
	{	"test",			cli_test		},
	{	NULL,			NULL			}
};

static void	__get_command(
	FUNC_COM *func_comm, char **name_comm, const char *sarg)
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

static int __f_stderr_logger(const char *mes)
{
	ft_printf("%@%s\n", mes);
	return (SSL_ERR);
}

int		main(int ac, const char **av)
{
	FUNC_COM	func_comm;
	char		*name_comm;

	util_logger_set_logger(__f_stderr_logger);
	util_logger_set_level(LIBFT_LOG_LEVEL_DEBUG);
	util_logger_enable_ansi_color();

	if (ac < 2) {
		cli_print_usage();
		exit(1);
	}
	__get_command(&func_comm, &name_comm, av[1]);

	if (NULL == name_comm) {
		cli_print_usage();
		exit(1);
	}
	func_comm(av+2, name_comm);

	return (0);
}
