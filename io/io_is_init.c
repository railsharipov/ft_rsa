#include <common.h>
#include <string.h>
#include <io.h>

int io_is_init(t_iodes *iodes)
{
	IO_LOG(TRACE, "checking if iodes=%p is initialized", iodes);
	
	if (NULL == iodes) {
		IO_LOG(ERROR, INVALID_INPUT_ERROR);
		return (0);
	}
	
	IO_LOG(TRACE, "iodes mode: %#x, is initialized: %d", iodes->mode, (iodes->mode != 0));
	return ((iodes->mode != 0));
}
