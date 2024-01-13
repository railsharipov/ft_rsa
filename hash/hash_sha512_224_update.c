/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sha512.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/09 10:59:42 by rsharipo          #+#    #+#             */
/*   Updated: 2018/10/01 10:51:29 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ssl/ssl.h>
#include <ssl/hash.h>

void	hash_sha512_224_update(
	t_hash *sha512_224, const char *buf, size_t bufsize)
{
	hash_sha512_update(sha512_224, buf, bufsize);
}
