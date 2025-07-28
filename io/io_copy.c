#include <libft/std.h>
#include <io.h>

void 	io_copy(t_iodes * const dest, t_iodes * const src)
{
	IO_LOG(TRACE, "io copy with dest=%p, src=%p", dest, src);
	
	if (dest == NULL || src == NULL) {
		IO_LOG(ERROR, INVALID_INPUT_ERROR);
		return;
	}
	
	IO_LOG(TRACE, "copying %zu bytes from src to dest", sizeof(t_iodes));
	ft_memcpy(dest, src, sizeof(t_iodes));
}
