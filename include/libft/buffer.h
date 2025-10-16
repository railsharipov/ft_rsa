#ifndef LIBFT_BUFFER_H
# define LIBFT_BUFFER_H

# include <libft/common.h>

# define BUFFER_INIT_CAPACITY 1024

typedef struct s_buffer {
	void *data;
	size_t size;
	size_t capacity;
} t_buffer;

t_buffer *ft_buffer_create(void);
void ft_buffer_del(t_buffer *buffer);
void ft_buffer_clear(t_buffer *buffer);
void ft_buffer_append(t_buffer *buffer, void *buf, size_t bufsize);
size_t ft_buffer_lshift(t_buffer *buffer, size_t bufsize);
size_t ft_buffer_lshift_copy(t_buffer *buffer, void *buf, size_t bufsize);

#endif