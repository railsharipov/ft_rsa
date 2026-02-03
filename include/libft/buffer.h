#ifndef LIBFT_BUFFER_H
# define LIBFT_BUFFER_H

# include <libft/common.h>

typedef struct s_buffer {
	void		*arr;
	size_t		capacity;
	size_t		read_pos;
	size_t		write_pos;
}				t_buffer;

typedef ssize_t (*t_func_buffer_read)(void *vctx, const void *buf, size_t nbytes);
typedef ssize_t (*t_func_buffer_write)(void *vctx, void *buf, size_t nbytes);
typedef int 	(*t_func_buffer_transform)(const void *src, size_t srcsize, void *dst, size_t dstsize, size_t *consumed, size_t *produced);

t_buffer	*ft_buffer_new(size_t capacity);
void		ft_buffer_del(t_buffer *buffer);
const char	*ft_buffer_view(t_buffer *buffer);
size_t		ft_buffer_used(t_buffer *buffer);
size_t		ft_buffer_available(t_buffer *buffer);
void		ft_buffer_reset(t_buffer *buffer);
int			ft_buffer_is_empty(t_buffer *buffer);
int			ft_buffer_is_full(t_buffer *buffer);
ssize_t 	ft_buffer_read(t_buffer *buffer, void *buf, size_t nbytes);
ssize_t 	ft_buffer_write(t_buffer *buffer, const void *buf, size_t nbytes);

ssize_t 	ft_buffer_read_with_func(t_buffer *buffer, t_func_buffer_read read, void *vctx, size_t nbytes);
ssize_t 	ft_buffer_write_with_func(t_buffer *buffer, t_func_buffer_write write, void *vctx, size_t nbytes);

int 		ft_buffer_transform_read(t_buffer *buffer, t_func_buffer_transform transform, void *buf, size_t nbytes, size_t *consumed, size_t *produced);
int 		ft_buffer_transform_write(t_buffer *buffer, t_func_buffer_transform transform, const void *buf, size_t nbytes, size_t *consumed, size_t *produced);

#endif