/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_hash.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/13 09:35:41 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/27 10:18:24 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <libft.h>

void *ft_htbl_init(int size)
{
	t_htbl	*htbl;

	LIBFT_ALLOC(htbl, sizeof(t_htbl));

	htbl->size = MAX(LIBFT_HT_SIZE, size);
	htbl->size = CEIL(htbl->size, LIBFT_HT_SIZE);
	
	LIBFT_ALLOC(htbl->arr, htbl->size * sizeof(void *));

	return (htbl);
}