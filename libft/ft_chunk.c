#include <libft/chunk.h>
#include <libft/std.h>
#include <libft/alloc.h>
#include <libft/string.h>

t_chunk    *ft_chunk_create(size_t size)
{
	t_chunk	*chunk;

	LIBFT_ALLOC(chunk, sizeof(t_chunk));
	LIBFT_ALLOC(chunk->data, size);
	chunk->size = size;
	return (chunk);
}

void	ft_chunk_del(t_chunk *chunk)
{
	LIBFT_FREE(chunk->data);
	LIBFT_FREE(chunk);
}

void	ft_chunk_clear(t_chunk *chunk)
{
	LIBFT_FREE(chunk->data);
	chunk->size = 0;
}

void	ft_chunk_append(t_chunk *chunk, void *data, size_t size)
{
	size_t	new_size;

	new_size = chunk->size + size;
    LIBFT_REALLOC(chunk->data, size, new_size);

	ft_memcpy(chunk->data + chunk->size, data, size);
	chunk->size = new_size;
}

void	ft_chunk_append_str(t_chunk *chunk, char *str)
{
	ft_chunk_append(chunk, str, ft_strlen(str));
}

void	ft_chunk_append_bytes(t_chunk *chunk, void *bytes, size_t size)
{
	ft_chunk_append(chunk, bytes, size);
}

void	ft_chunk_append_chunk(t_chunk *chunk, t_chunk *other)
{
	ft_chunk_append(chunk, other->data, other->size);
}

char	*ft_chunk_to_cstr(t_chunk *chunk)
{
	char	*cstr;

	LIBFT_ALLOC(cstr, chunk->size + 1);
	ft_memcpy(cstr, chunk->data, chunk->size);
	cstr[chunk->size] = '\0';

	return (cstr);
}
