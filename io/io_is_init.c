#include <common.h>
#include <string.h>
#include <io.h>

int io_is_init(t_iodes *iodes)
{
	SSL_LOG(TRACE, "checking if iodes=%p is initialized", iodes);
	
	if (NULL == iodes) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (0);
	}
	
	SSL_LOG(TRACE, "iodes mode: %#x, is initialized: %d", iodes->mode, (iodes->mode != 0));
	return ((iodes->mode != 0));
}
