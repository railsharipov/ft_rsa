/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/09 13:36:51 by rsharipo          #+#    #+#             */
/*   Updated: 2018/10/13 09:00:28 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_BYTES_H
# define LIBFT_BYTES_H

# include <string.h>
# include <stdint.h>
# include <libft/common.h>

typedef __uint128_t	uint128_t;

char		*ft_bytes_to_hex(const void *bytes, size_t size);
void		ft_bytes_reverse_bits(void *bytes, size_t size);
uint64_t	ft_bytes_to_uint(void *bytes, size_t size);
void		ft_bytes_lshift(void *bytes, size_t size, int shift);
void		ft_bytes_rshift(void *bytes, size_t size, int shift);
void		ft_bytes_dump_hex(void *bytes, size_t size, int cols, int del);
void		ft_bytes_write_hex(int fd, void *bytes, size_t size);
void		ft_bytes_print_bits(void *, size_t);
void		ft_bytes_xor(void *res, void *bytes1, void *bytes2, size_t size);

void		ft_hex_to_bytes(void *bytes, const char *hex, size_t hexsize);

uint32_t	ft_uint_bswap32(uint32_t);
uint64_t	ft_uint_bswap64(uint64_t);
uint128_t	ft_uint_bswap128(uint128_t);

int 		ft_uint_lmbit(uint64_t uint, int uint_nbits);
int 		ft_uint_rmbit(uint64_t uint, int uint_nbits);
void		ft_uint_print_bits(uint64_t uint, int uint_nbits, int col_nbits);
void		ft_uint_to_bytes(void *bytes, uint64_t uint, int uint_size);

#endif
