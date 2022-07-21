#include <ft_ssl.h>
#include <ssl_io.h>

static const struct {
	int		mode;
	char	*mode_desc;
} F[] = {
	{ IO_MODE_FILDES, "fildes mode" },
	{ IO_MODE_OSBUF, "osbuf mode" },
	{ 0, "no mode" }
};

void 	io_print_stats(const t_iodes *iodes, const char *name)
{
	char	*mode_desc;
	int		ix;

	SSL_CHECK(NULL != iodes);

	for (ix = 0, mode_desc = NULL; ix < sizeof(F)/(sizeof(F[0])); ix++)
	{
		if (iodes->mode == F[ix].mode)
			mode_desc = F[ix].mode_desc;
	}

	SSL_CHECK(NULL != mode_desc);

	ft_printf("---------------------------------------------\n");

	if (name != NULL)
		ft_printf("IO STATS (%s)\n", name);
	else
		ft_printf("IO STATS\n");

	ft_printf("---------------------------------------------\n");
	ft_printf("stream fd = %d\n", iodes->fd);
	ft_printf("stream seek = %d\n", iodes->seek);

	if ('\n' == iodes->delim)
		ft_printf("stream delim = newline\n");
	else if (!ft_isprint(iodes->delim))
		ft_printf("stream delim = white space (ascii=%d)\n", iodes->delim);
	else
		ft_printf("stream delim = %c\n", iodes->delim);

	ft_printf("stream line width = %d\n", iodes->lwidth);
	ft_printf("io mode = %s\n", mode_desc);
	ft_printf("---------------------------------------------\n");
}
