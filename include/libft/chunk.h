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

#ifndef LIBFT_CHUNK_H
# define LIBFT_CHUNK_H

# include <string.h>
# include <stdint.h>
# include <stddef.h>
# include <libft/common.h>

typedef struct  s_chunk {
	uint8_t		*data;
	size_t		size;
}			    t_chunk;

t_chunk	*ft_chunk_create(size_t size);
void		ft_chunk_del(t_chunk *chunk);
void		ft_chunk_clear(t_chunk *chunk);
void		ft_chunk_append(t_chunk *chunk, void *data, size_t size);
void		ft_chunk_append_str(t_chunk *chunk, char *str);
void		ft_chunk_append_bytes(t_chunk *chunk, void *bytes, size_t size);
void		ft_chunk_append_chunk(t_chunk *chunk, t_chunk *other);
char		*ft_chunk_to_cstr(t_chunk *chunk);

#endif
