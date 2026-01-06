#include <common.h>
#include <alloc.h>
#include <libft/string.h>
#include <libft/std.h>
#include <libft/buffer.h>

t_buffer *ft_buffer_new(size_t capacity)
{
	t_buffer *buffer;

	LIBFT_ALLOC(buffer, sizeof(t_buffer));
	LIBFT_ALLOC(buffer->arr, capacity);
	buffer->capacity = capacity;
	buffer->read_pos = 0;
	buffer->write_pos = 0;
	return (buffer);
}

void ft_buffer_reset(t_buffer *buffer)
{
	if (NULL == buffer) {
		return ;
	}
	buffer->read_pos = 0;
	buffer->write_pos = 0;
}

void ft_buffer_del(t_buffer *buffer)
{
	if (NULL == buffer) {
		return ;
	}
	LIBFT_FREE(buffer->arr);
	LIBFT_FREE(buffer);
}

size_t ft_buffer_used(t_buffer *buffer)
{
	if (NULL == buffer) {
		return (0);
	}
	return (buffer->write_pos - buffer->read_pos);
}

size_t ft_buffer_avaialble(t_buffer *buffer)
{
	if (NULL == buffer) {
		return (0);
	}
	return (buffer->capacity - ft_buffer_used(buffer));
}

int ft_buffer_is_empty(t_buffer *buffer)
{
	if (NULL == buffer) {
		return (1);
	}
	return (ft_buffer_used(buffer) == 0);
}

int ft_buffer_is_full(t_buffer *buffer)
{
	if (NULL == buffer) {
		return (1);
	}
	return (ft_buffer_used(buffer) == buffer->capacity);
}

ssize_t ft_buffer_read(t_buffer *buffer, char *buf, size_t nbytes)
{
	// stub
	return (0);
}

ssize_t ft_buffer_write(t_buffer *buffer, char *buf, size_t nbytes)
{
	// stub
	return (0);
}

ssize_t ft_buffer_read_with_func(t_buffer *buffer, ssize_t (*func)(void *ctx, const char *buf, size_t nbytes), void *ctx, size_t nbytes)
{
	// stub
	return (0);
}

ssize_t ft_buffer_write_with_func(t_buffer *buffer, ssize_t (*func)(void *ctx, const char *buf, size_t nbytes), void *ctx, size_t nbytes)
{
	// stub
	return (0);
}