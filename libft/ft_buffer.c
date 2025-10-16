#include <common.h>
#include <alloc.h>
#include <libft/string.h>
#include <libft/std.h>
#include <libft/buffer.h>

t_buffer *ft_buffer_create(void)
{
	t_buffer *buffer;

	SSL_ALLOC(buffer, sizeof(t_buffer));
	SSL_ALLOC(buffer->data, BUFFER_INIT_CAPACITY);
	buffer->capacity = BUFFER_INIT_CAPACITY;
	buffer->size = 0;
	return (buffer);
}

void ft_buffer_del(t_buffer *buffer)
{
	SSL_FREE(buffer->data);
	SSL_FREE(buffer);
}

void ft_buffer_clear(t_buffer *buffer)
{
	buffer->size = 0;
}

void ft_buffer_append(t_buffer *buffer, void *buf, size_t bufsize)
{
	if (buffer->size + bufsize > buffer->capacity) {
		if (bufsize < BUFFER_INIT_CAPACITY) {
			buffer->capacity += BUFFER_INIT_CAPACITY;
		} else {
			buffer->capacity += bufsize;
		}
		SSL_REALLOC(buffer->data, buffer->size, buffer->capacity);
	}
	ft_memcpy(buffer->data + buffer->size, buf, bufsize);
	buffer->size += bufsize;
}

size_t ft_buffer_lshift(t_buffer *buffer, size_t bufsize)
{
	if (buffer->size < bufsize) {
		bufsize = buffer->size;
	}
	ft_memmove(buffer->data, buffer->data + bufsize, buffer->size - bufsize);
	buffer->size -= bufsize;
	return (bufsize);
}

size_t ft_buffer_lshift_copy(t_buffer *buffer, void *buf, size_t bufsize) {
	if (buffer->size < bufsize) {
		bufsize = buffer->size;
	}
	ft_memcpy(buf, buffer->data, bufsize);
	ft_buffer_lshift(buffer, bufsize);
	return (bufsize);
}