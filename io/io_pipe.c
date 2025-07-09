#include <stdint.h>
#include <io.h>
#include <libft/string.h>
#include <libft/std.h>

int io_pipe(t_iodes *iodes, uint32_t flags, t_iodes *iodes_1, t_iodes *iodes_2)
{
	IO_LOG(TRACE, "io_pipe: entering function with flags=0x%x", flags);
	
	if (NULL == iodes || NULL == iodes_1 || NULL == iodes_2) {
		IO_LOG(TRACE, "io_pipe: NULL parameter detected - iodes=%p, iodes_1=%p, iodes_2=%p", iodes, iodes_1, iodes_2);
		return (-1);
	}
	if (!io_is_init(iodes_1) || !io_is_init(iodes_2)) {
		IO_LOG(TRACE, "io_pipe: uninitialized iodes detected - iodes_1=%p, iodes_2=%p", iodes_1, iodes_2);
		return (-1);
	}

	IO_LOG(TRACE, "io_pipe: setting up pipe with iodes_1=%p, iodes_2=%p", iodes_1, iodes_2);
	iodes->pipe.iodes_1 = iodes_1;
	iodes->pipe.iodes_2 = iodes_2;
	iodes->mode = IO_MODE_PIPE;

	IO_LOG(TRACE, "io_pipe: pipe setup successful, returning 0");
	return (0);
}
