#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <io.h>

void	io_fclose_multi(t_iodes *iodes, ...)
{
	va_list	ap;
	
	IO_LOG(TRACE, "closing multiple file descriptors");
	io_fclose(iodes);

	va_start(ap, iodes);

	while (NULL != (iodes = va_arg(ap, t_iodes *))) {
		io_fclose(iodes);
	}
	va_end(ap);
}
