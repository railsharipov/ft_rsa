#include <common.h>
#include <io.h>
#include <libft/htable.h>

typedef struct	s_io_processor_ctx
{
	t_iodes_v2	io_pipe;
    
}				t_io_processor_ctx;

static ssize_t	__io_processor_read(void *vctx, char *buf, size_t nbytes);
static ssize_t	__io_processor_write(void *vctx, const char *buf, size_t nbytes);

int	io_v2_processor(t_iodes_v2 *iodes, uint32_t flags)
{
    t_io_processor_ctx	*ctx;
}