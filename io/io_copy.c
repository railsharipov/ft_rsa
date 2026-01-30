#include <libft/std.h>
#include <io.h>

void 	io_copy(t_iodes * const dest, t_iodes * const src)
{
	SSL_LOG(TRACE, "io copy with dest=%p, src=%p", dest, src);
	
	if (dest == NULL || src == NULL) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return;
	}
	
	SSL_LOG(TRACE, "copying %zu bytes from src to dest", sizeof(t_iodes));
	ft_memcpy(dest, src, sizeof(t_iodes));
}
