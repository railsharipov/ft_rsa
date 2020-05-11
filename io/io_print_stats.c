#include <ft_ssl.h>
#include <ssl_io.h>

static const struct {
	FUNC_IO		f;
	char		*f_name;
} F[] = {
	{ &io_fread, "io_fread" },
	{ &io_sread, "io_sread" },
	{ &io_fwrite, "io_fwrite" },
	{ NULL, "no function" }
};

void 	io_print_stats(t_io *const io, const char *name)
{
	char	*f_name;
	int		ix;

	SSL_CHECK(NULL != io);

	for (ix = 0, f_name = NULL; ix < sizeof(F)/(sizeof(F[0])); ix++)
	{
		if (io->func == F[ix].f)
			f_name = F[ix].f_name;
	}

	SSL_CHECK(NULL != f_name);

	ft_printf("---------------------------------------------\n");

	if (name != NULL)
		ft_printf("IO STATS (%s)\n", name);
	else
		ft_printf("IO STATS\n");

	ft_printf("---------------------------------------------\n");
	ft_printf("stream fd = %d\n", io->fd);
	ft_printf("stream seek = %d\n", io->seek);

	if ('\n' == io->delim)
		ft_printf("stream delim = newline\n");
	else if (!ft_isprint(io->delim))
		ft_printf("stream delim = white space (ascii=%d)\n", io->delim);
	else
		ft_printf("stream delim = %c\n", io->delim);

	ft_printf("stream line width = %d\n", io->lwidth);
	ft_printf("func = %s\n", f_name);
	ft_printf("input = ");
	write(1, io->input, MIN(20, io->insize));
	ft_printf("\ninsize = %d\n", io->insize);
	ft_printf("---------------------------------------------\n");
}
