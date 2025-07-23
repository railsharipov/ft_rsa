#include <libft/std.h>
#include <io.h>

void 	io_copy(t_iodes * const dest, t_iodes * const src)
{
	IO_LOG(TRACE, "entering function with dest=%p, src=%p", dest, src);
	
	if (dest == NULL || src == NULL) {
		IO_LOG(TRACE, "NULL parameter detected - dest=%p, src=%p", dest, src);
		return;
	}
	
	IO_LOG(TRACE, "copying %zu bytes from src to dest", sizeof(t_iodes));
	ft_memcpy(dest, src, sizeof(t_iodes));
	
	IO_LOG(TRACE, "copy completed successfully");
}
