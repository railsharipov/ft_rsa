#include <bytes.h>
#include <libft/std.h>
#include <libft/alloc.h>

void	bytes_del(t_bytes *bytes)
{
	BYTES_LOG(TRACE, "bytes del with bytes=%p", bytes);

	if (NULL == bytes) {
		BYTES_LOG(ERROR, INVALID_INPUT_ERROR);
		return ;
	}
	ft_bzero(bytes, sizeof(t_bytes));
	SSL_FREE(bytes);

	return ;
}
