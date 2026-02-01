#include <common.h>
#include <alloc.h>
#include <libft/string.h>
#include <libft/std.h>
#include <libft/buffer.h>

static int	__is_valid_buffer(t_buffer *buffer);
static size_t __left_pad_size(t_buffer *buffer);
static size_t __right_pad_size(t_buffer *buffer);
static size_t __left_align(t_buffer *buffer);
// static size_t __right_align(t_buffer *buffer);

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

void ft_buffer_reset(t_buffer *buffer)
{
	if (buffer == NULL) {
		return ;
	}
	buffer->read_pos = 0;
	buffer->write_pos = 0;
}

void ft_buffer_del(t_buffer *buffer)
{
	if (buffer == NULL) {
		return ;
	}
	LIBFT_FREE(buffer->arr);
	LIBFT_FREE(buffer);
}

const char *ft_buffer_view(t_buffer *buffer)
{
	if (NULL == buffer) {
		return (NULL);
	}
	if (!__is_valid_buffer(buffer)) {
		return (NULL);
	}
	return ((const char *)buffer->arr + buffer->read_pos);
}

size_t ft_buffer_used(t_buffer *buffer)
{
	if (NULL == buffer) {
		return (0);
	}
	if (buffer->write_pos < buffer->read_pos) {
		return (0);
	}
	return (buffer->write_pos - buffer->read_pos);
}

size_t ft_buffer_available(t_buffer *buffer)
{
	if (NULL == buffer) {
		return (0);
	}
	if (!__is_valid_buffer(buffer)) {
		return (0);
	}
	return (buffer->capacity - ft_buffer_used(buffer));
}

int ft_buffer_is_empty(t_buffer *buffer)
{
	return (ft_buffer_used(buffer) == 0);
}

int ft_buffer_is_full(t_buffer *buffer)
{
	return (ft_buffer_available(buffer) == 0);
}

ssize_t ft_buffer_read(t_buffer *buffer, void *buf, size_t size)
{
	size_t	used;
	ssize_t rbytes;

	if (NULL == buffer) {
		return (-1);
	}
	if (!__is_valid_buffer(buffer)) {
		return (-1);
	}
	if (NULL == buf) {
		return (-1);
	}
	if (size == 0) {
		return (0);
	}
	used = ft_buffer_used(buffer);
	if (used == 0) {
		return (0);
	}
	rbytes = MIN(size, used);
	ft_memcpy(buf, (char *)buffer->arr + buffer->read_pos, rbytes);
	buffer->read_pos += rbytes;

	if (buffer->read_pos >= buffer->write_pos) {
		ft_buffer_reset(buffer);
	}
	return (rbytes);
}

ssize_t ft_buffer_write(t_buffer *buffer, const void *buf, size_t size)
{
	ssize_t wbytes;
	size_t	available;

	if (NULL == buffer) {
		return (-1);
	}
	if (!__is_valid_buffer(buffer)) {
		return (-1);
	}
	if (NULL == buf) {
		return (-1);
	}
	if (size == 0) {
		return (0);
	}
	available = ft_buffer_available(buffer);
	if (available == 0) {
		return (0);
	}
	wbytes = (size > available) ? available : size;

	if (wbytes > __right_pad_size(buffer)) {
		__left_align(buffer);
	}
	ft_memcpy((char *)buffer->arr + buffer->write_pos, buf, wbytes);
	buffer->write_pos += wbytes;

	return (wbytes);
}

ssize_t ft_buffer_read_with_func(t_buffer *buffer, ssize_t (*func)(void *ctx, const void *buf, size_t nbytes), void *ctx, size_t nbytes)
{
	ssize_t rbytes;
	size_t	used;

	if (NULL == buffer) {
		return (-1);
	}
	if (!__is_valid_buffer(buffer)) {
		return (-1);
	}
	if (NULL == func) {
		return (-1);
	}
	if (nbytes == 0) {
		return (0);
	}
	used = ft_buffer_used(buffer);
	if (used == 0) {
		return (0);
	}
	rbytes = func(ctx, (char *)buffer->arr + buffer->read_pos, MIN(nbytes, used));
	if (rbytes < 0) {
		return (-1);
	}
	buffer->read_pos += rbytes;

	if (buffer->read_pos >= buffer->write_pos) {
		ft_buffer_reset(buffer);
	}
	return (rbytes);
}

ssize_t ft_buffer_write_with_func(t_buffer *buffer, ssize_t (*func)(void *ctx, void *buf, size_t nbytes), void *ctx, size_t nbytes)
{
	ssize_t wbytes;
	size_t available;

	if (NULL == buffer) {
		return (-1);
	}
	if (!__is_valid_buffer(buffer)) {
		return (-1);
	}
	if (NULL == func) {
		return (-1);
	}
	if (nbytes == 0) {
		return (0);
	}
	available = ft_buffer_available(buffer);
	if (available == 0) {
		return (0);
	}
	wbytes = MIN(nbytes, available);

	if (wbytes > __right_pad_size(buffer)) {
		__left_align(buffer);
	}
	wbytes = func(ctx, (char *)buffer->arr + buffer->write_pos, wbytes);
	if (wbytes < 0) {
		return (-1);
	}
	buffer->write_pos += wbytes;

	return (wbytes);
}

static int	__is_valid_buffer(t_buffer *buffer)
{
	if (NULL == buffer->arr) {
		return (0);
	}
	if (buffer->capacity == 0) {
		return (0);
	}
	if (buffer->read_pos > buffer->capacity) {
		return (0);
	}
	if (buffer->write_pos > buffer->capacity) {
		return (0);
	}
	if (buffer->read_pos > buffer->write_pos) {
		return (0);
	}
	return (1);
}

static size_t __left_pad_size(t_buffer *buffer)
{
	return (buffer->read_pos);
}

static size_t __right_pad_size(t_buffer *buffer)
{
	return (buffer->capacity - buffer->write_pos);
}

static size_t __left_align(t_buffer *buffer)
{
	size_t shifted;

	shifted = __left_pad_size(buffer);
	if (buffer->read_pos > 0) {
		ft_memmove(buffer->arr, buffer->arr + shifted, ft_buffer_used(buffer));
		buffer->write_pos -= shifted;
		buffer->read_pos = 0;
	}
	return (shifted);
}

// static size_t __right_align(t_buffer *buffer)
// {
//  	size_t shifted;

// 	shifted = __right_pad_size(buffer);
// 	if (shifted > 0) {
// 		ft_memmove(buffer->arr + buffer->read_pos + shifted, buffer->arr + buffer->read_pos, ft_buffer_used(buffer));
// 		buffer->read_pos += shifted;
// 		buffer->write_pos = buffer->capacity;
// 	}
// 	return (shifted);
// }