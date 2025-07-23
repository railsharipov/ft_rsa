#include <string.h>
#include <io.h>

int io_is_init(t_iodes *iodes)
{
	IO_LOG(TRACE, "checking if iodes=%p is initialized", iodes);
	
	if (NULL == iodes) {
		IO_LOG(TRACE, "NULL iodes, returning 0");
		return (0);
	}
	
	int result = (iodes->mode != 0);
	IO_LOG(TRACE, "iodes mode=%d, returning %d", iodes->mode, result);
	return (result);
}
