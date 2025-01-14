#include <unistd.h>
#include <io.h>

void	io_close(t_iodes * const iodes)
{
	if (iodes->fd > 2) {
		close(iodes->fd);
	}
}
