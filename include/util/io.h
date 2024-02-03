#ifndef SSL_IO_H
# define SSL_IO_H

# include <stdint.h>
# include <libft/string.h>

# define IO_BUFSIZE (256 * 1024)

# if (IO_BUFSIZE > (256 * 1024))
#   undef IO_BUFSIZE
#   define IO_BUFSIZE (256 * 1024)
# endif

# if (IO_BUFSIZE < 256)
#   undef IO_BUFSIZE
#   define IO_BUFSIZE 256
# endif

# define IO_ERROR(MES, ...)	ssl_error_log(__func__, __FILE__, __LINE__, "io error: ", MES __VA_OPT__(,) __VA_ARGS__)

enum	e_io
{
	IO_INPUT		= 1UL << 1,
	IO_OUTPUT		= 1UL << 2,
	IO_READ			= 1UL << 3,
	IO_WRITE		= 1UL << 4,
	IO_FILE			= 1UL << 5,
	IO_OSBUF		= 1UL << 6,
	IO_STDIN		= 1UL << 7,
	IO_STDOUT		= 1UL << 8,
	IO_MODE_FILDES	= 1UL << 9,
	IO_MODE_OSBUF	= 1UL << 10,
	IO_READ_STDIN	= IO_READ | IO_STDIN,
	IO_READ_FILE	= IO_READ | IO_FILE,
	IO_WRITE_STDOUT	= IO_WRITE | IO_STDOUT,
	IO_WRITE_FILE	= IO_WRITE | IO_FILE,
};

typedef struct	s_iodes
{
	int			fd;
	t_ostring	*osbuf;
	ssize_t		seek;
	int			delim;
	int			lwidth;
	int			mode;
}				t_iodes;

struct s_io;
typedef ssize_t	(*FUNC_IO)(struct s_io *, char *const, size_t);
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

// Overload macro on number of args
# define SEL23(_1,_2,_3,MACRO_NAME,...)	MACRO_NAME

# define _IOINIT3(iodes, flags, x)	_Generic((x), \
	const char *: io_open, \
	char *: io_open, \
	const struct s_ostring *: io_set_osbuf, \
	struct s_ostring *: io_set_osbuf) \
	(iodes, flags, x)

# define _IOINIT2(iodes, flags)		io_open(iodes, flags, NULL)

# define io_init(...)	SEL23(__VA_ARGS__,_IOINIT3,_IOINIT2)(__VA_ARGS__)

int				io_set_osbuf(t_iodes *, uint32_t flags, t_ostring *osbuf);
int				io_open(t_iodes *, uint32_t flags, const char *);
int				io_is_initialized(t_iodes *iodes);

ssize_t			io_read(t_iodes *iodes, char *buf, size_t nbytes);
ssize_t			io_sread(t_iodes *iodes, char *buf, size_t nbytes);
ssize_t			io_fread(t_iodes *iodes, char *buf, size_t nbytes);
ssize_t			io_write(t_iodes *iodes, const char *buf, size_t nbytes);
ssize_t			io_fwrite(t_iodes *iodes, const char *buf, size_t nbytes);
ssize_t			io_swrite(t_iodes *iodes, const char *buf, size_t nbytes);
void 			io_print_stats(const t_iodes *, const char *);
void 			io_copy(t_iodes * const, t_iodes * const);
void			io_close(t_iodes * const);
void			io_close_multi(t_iodes *, ...);

#endif
