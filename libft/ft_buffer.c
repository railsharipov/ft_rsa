#include <common.h>
#include <alloc.h>
#include <libft/string.h>
#include <libft/std.h>
#include <libft/buffer.h>

t_buffer *ft_buffer_new(size_t capacity)
{
	t_buffer *buffer;

	if (capacity == 0) {
		return (NULL);
	}
	LIBFT_ALLOC(buffer, sizeof(t_buffer));
	LIBFT_ALLOC(buffer->arr, capacity);
	buffer->capacity = capacity;
	buffer->write_pos = 0;
	buffer->read_pos = 0;

	return (buffer);
}

void ft_buffer_del(t_buffer *buffer)
{
	LIBFT_FREE(buffer->arr);
	LIBFT_FREE(buffer);
}

size_t ft_buffer_used(t_buffer *buffer)
{
	return (buffer->write_pos - buffer->read_pos);
}

size_t ft_buffer_available(t_buffer *buffer)
{
	return (buffer->capacity - ft_buffer_used(buffer));
}

size_t ft_buffer_left_pad_size(t_buffer *buffer)
{
	return (buffer->read_pos);
}

size_t ft_buffer_right_pad_size(t_buffer *buffer)
{
	return (buffer->capacity - buffer->write_pos);
}

int ft_buffer_is_empty(t_buffer *buffer)
{
	return (ft_buffer_used(buffer) == 0);
}

int ft_buffer_is_full(t_buffer *buffer)
{
	return (ft_buffer_available(buffer) == 0);
}

void ft_buffer_reset(t_buffer *buffer)
{
	buffer->read_pos = 0;
	buffer->write_pos = 0;
}

void ft_buffer_left_align(t_buffer *buffer)
{
	size_t used;

	used = ft_buffer_used(buffer);

	if (buffer->read_pos > 0) {
		ft_memmove(buffer->arr, buffer->arr + buffer->read_pos, used);
		buffer->write_pos -= buffer->read_pos;
		buffer->read_pos = 0;
	}
	buffer->read_pos = 0;
}

void ft_buffer_right_align(t_buffer *buffer)
{
	size_t available;
	size_t used;

	available = ft_buffer_available(buffer);
	used = ft_buffer_used(buffer);

	if (available > 0 && used > 0) {
		ft_memmove(buffer->arr + available, buffer->arr + buffer->read_pos, used);
		buffer->read_pos += available;
		buffer->write_pos = buffer->capacity;
	}
}

ssize_t ft_buffer_read(t_buffer *buffer, char *buf, size_t size)
{
	size_t	used;
	ssize_t rbytes;

	if (NULL == buffer) {
		return (-1);
	}
	if (NULL == buf) {
		return (-1);
	}
	if (size == 0) {
		return (0);
	}
	used = ft_buffer_used(buffer);
	rbytes = MIN(size, used);
	ft_memcpy(buf, buffer->arr + buffer->read_pos, rbytes);
	buffer->read_pos += rbytes;

	if (buffer->read_pos >= buffer->write_pos) {
		ft_buffer_reset(buffer);
	}
	return (rbytes);
}

ssize_t ft_buffer_write(t_buffer *buffer, char *buf, size_t size)
{
	ssize_t wbytes;
	size_t	available;

	if (NULL == buffer) {
		return (-1);
	}
	if (NULL == buf) {
		return (-1);
	}
	if (size == 0) {
		return (0);
	}
	available = ft_buffer_available(buffer);
	wbytes = (size > available) ? available : size;

	if (wbytes > ft_buffer_right_pad_size(buffer)) {
		ft_buffer_left_align(buffer);
	}
	ft_memcpy(buffer->arr + buffer->write_pos, buf, wbytes);
	buffer->write_pos += wbytes;

	return (wbytes);
}

ssize_t ft_buffer_read_with_func(t_buffer *buffer, ssize_t (*func)(void *ctx, char *buf, size_t nbytes), void *ctx, size_t nbytes)
{
	ssize_t rbytes;

	if (NULL == buffer) {
		return (-1);
	}
	if (NULL == func) {
		return (-1);
	}
	if (nbytes == 0) {
		return (0);
	}
	rbytes = func(ctx, buffer->arr + buffer->read_pos, MIN(nbytes, ft_buffer_used(buffer)));
	if (rbytes < 0) {
		return (-1);
	}
	buffer->read_pos += rbytes;

	if (buffer->read_pos >= buffer->write_pos) {
		ft_buffer_reset(buffer);
	}
	return (rbytes);
}

ssize_t ft_buffer_write_with_func(t_buffer *buffer, ssize_t (*func)(void *ctx, char *buf, size_t nbytes), void *ctx, size_t nbytes)
{
	ssize_t wbytes;
	size_t available;

	if (NULL == buffer) {
		return (-1);
	}
	if (NULL == func) {
		return (-1);
	}
	if (nbytes == 0) {
		return (0);
	}
	available = ft_buffer_available(buffer);
	wbytes = (nbytes > available) ? available : nbytes;

	if (wbytes > ft_buffer_right_pad_size(buffer)) {
		ft_buffer_left_align(buffer);
	}
	wbytes = func(ctx, buffer->arr + buffer->write_pos, wbytes);
	if (wbytes < 0) {
		return (-1);
	}
	buffer->write_pos += wbytes;

	return (wbytes);
}