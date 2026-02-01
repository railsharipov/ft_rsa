#ifndef LIBFT_BUFFER_H
# define LIBFT_BUFFER_H

# include <libft/common.h>

typedef struct s_buffer {
	void		*arr;
	size_t		capacity;
	size_t		read_pos;
	size_t		write_pos;
}				t_buffer;

t_buffer	*ft_buffer_new(size_t capacity);
void		ft_buffer_del(t_buffer *buffer);
const char	*ft_buffer_view(t_buffer *buffer);
size_t		ft_buffer_used(t_buffer *buffer);
size_t		ft_buffer_available(t_buffer *buffer);
void		ft_buffer_reset(t_buffer *buffer);
int			ft_buffer_is_empty(t_buffer *buffer);
int			ft_buffer_is_full(t_buffer *buffer);
ssize_t 	ft_buffer_write(t_buffer *buffer, const void *buf, size_t size);
ssize_t 	ft_buffer_read(t_buffer *buffer, void *buf, size_t size);
ssize_t 	ft_buffer_read_with_func(t_buffer *buffer, ssize_t (*func)(void *ctx, const void *buf, size_t nbytes), void *ctx, size_t nbytes);
ssize_t 	ft_buffer_write_with_func(t_buffer *buffer, ssize_t (*func)(void *ctx, void *buf, size_t nbytes), void *ctx, size_t nbytes);

#endif