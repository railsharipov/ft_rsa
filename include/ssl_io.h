#ifndef SSL_IO_H
# define SSL_IO_H

# define IO_BUFSIZE (256 * 1024)

# if (IO_BUFSIZE > (256 * 1024))
#   undef IO_BUFSIZE
#   define IO_BUFSIZE (256 * 1024)
# endif

# if (IO_BUFSIZE < 256)
#   undef IO_BUFSIZE
#   define IO_BUFSIZE 256
# endif

# define IO_ERROR(ERROR)	SSL_ERROR(ADD_ERROR_CTX(ERROR, IO_ERROR_CTX))

enum	e_io_error
{
	INVALID_IO = 1,
	INVALID_IO_PARAMETERS,
};

enum	e_io
{
	IO_INPUT		= 1UL << 1,
	IO_OUTPUT		= 1UL << 2,
	IO_READ			= 1UL << 3,
	IO_WRITE		= 1UL << 4,
	IO_FILE			= 1UL << 5,
	IO_STRING		= 1UL << 6,
	IO_STDIN		= 1UL << 7,
	IO_STDOUT		= 1UL << 8,
	IO_READ_STDIN	= IO_READ | IO_STDIN,
	IO_READ_FILE	= IO_READ | IO_FILE,
	IO_WRITE_STDOUT	= IO_WRITE | IO_STDOUT,
	IO_WRITE_FILE	= IO_WRITE | IO_FILE,
};

struct s_io;
typedef ssize_t	(*FUNC_IO)(struct s_io *, char *const, size_t);
typedef int		(*FUNC_SYSTEM_IO)(const char *, int, ...);

typedef struct  s_io
{
	FUNC_IO		func;
	int			fd;
	ssize_t		seek;
	int			delim;
	int			lwidth;
	char		*input;
	ssize_t		insize;
}               t_io;

void 			io_print_stats(t_io *const, const char *);
void 			io_copy(t_io * const, t_io * const);
int				io_init(t_io * const, const char *, int, uint32_t);
int 			io_error(t_io * const, const char *);
void			io_close(t_io * const);
void			io_close_multi(t_io *, ...);
ssize_t			io_fread(t_io * const, char *const, size_t);
ssize_t			io_sread(t_io * const, char *const, size_t);
ssize_t			io_fwrite(t_io * const, char *const, size_t);
ssize_t			io_putchar(t_io * const, char);

#endif
