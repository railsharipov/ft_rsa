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

#ifndef SSL_COMM_H
# define SSL_COMM_H

int     cli_base64(const char **, const char *);
int     cli_hash(const char **, const char *);
int     cli_des_ecb(const char **, const char *);
int     cli_des_cbc(const char **, const char *);
int     cli_rsa_gen(const char **, const char *);
int     cli_rsa(const char **, const char *);
int     cli_rsa_utl(const char **, const char *);
int     cli_test(const char **, const char *);

#endif
