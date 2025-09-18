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
#include <cli.h>
#include <libft/string.h>

static const struct {
	char			*name_comm;
	FUNC_COMM		func_comm;
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
	{	"des-ecb", 		cli_des_ecb		},
	{	"des-cbc",		cli_des_cbc		},
	{	"genrsa",		cli_rsa_gen		},
	{	"rsa",			cli_rsa			},
	{	"rsautl",		cli_rsa_utl		},
	{	"test",			cli_test		},
	{	NULL,			NULL			}
};

static void __get_command(FUNC_COMM *func_comm, char **name_comm, const char *sarg);
static void __print_usage(void);

int	main(int ac, const char **av)
{
	FUNC_COMM	func_comm;
	char		*name_comm;

	if (ac < 2) {
		__print_usage();
		exit(1);
	}
	__get_command(&func_comm, &name_comm, av[1]);

	if (NULL == name_comm) {
		__print_usage();
		exit(1);
	}
	func_comm(av+2, name_comm);

	return (0);
}

static void	__get_command(FUNC_COMM *func_comm, char **name_comm, const char *sarg)
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
