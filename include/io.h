#ifndef SSL_IO_H
# define SSL_IO_H

# include <stdint.h>
# include <common.h>
# include <libft/string.h>
# include <libft/logger.h>

# define IO_BUFSIZE 512

# define IO_LOG(LEVEL, MES, ...)	io_logger_log(__func__, __FILE__, __LINE__, LIBFT_LOG_LEVEL_##LEVEL, MES __VA_OPT__(,) __VA_ARGS__)

# define IO_INIT_ERROR		"i/o init error"
# define IO_READ_ERROR		"i/o read error"
# define IO_WRITE_ERROR		"i/o write error"

enum	e_io
{
	IO_INPUT		= 1UL << 1,
	IO_OUTPUT		= 1UL << 2,
	IO_READ			= 1UL << 3,
	IO_WRITE		= 1UL << 4,
	IO_FILE			= 1UL << 5,
	IO_OSBUF		= 1UL << 6,
	IO_IODES		= 1UL << 7,
	IO_STDIN		= 1UL << 8,
	IO_STDOUT		= 1UL << 9,
	IO_MODE_FILDES	= 1UL << 10,
	IO_MODE_OSBUF	= 1UL << 11,
	IO_MODE_PIPE	= 1UL << 12,
	IO_READ_STDIN	= IO_READ | IO_STDIN,
	IO_READ_FILE	= IO_READ | IO_FILE,
	IO_WRITE_STDOUT	= IO_WRITE | IO_STDOUT,
	IO_WRITE_FILE	= IO_WRITE | IO_FILE,
};

typedef struct s_iopipe {
	struct s_iodes *iodes_in;
	struct s_iodes *iodes_out;
} t_iopipe;

typedef struct s_iodes
{
	t_iopipe		pipe;
	t_ostring		*osbuf;
	int				fd;
	ssize_t			seek;
	int				delim;
	int				lwidth;
	int				mode;
} t_iodes;

int				io_logger_log(const char *func_name, const char *file_name, int line_number, uint8_t level, const char *fmt, ...);

int				io_fopen(t_iodes *, uint32_t flags, const char *filename);
int				io_osbuf(t_iodes *, uint32_t flags, t_ostring *osbuf);
int				io_pipe(t_iodes *, uint32_t flags, t_iodes *iodes_1, t_iodes *iodes_2);
int				io_is_init(t_iodes *iodes);

ssize_t			io_read(t_iodes *iodes, char *buf, size_t nbytes);
ssize_t			io_sread(t_iodes *iodes, char *buf, size_t nbytes);
ssize_t			io_fread(t_iodes *iodes, char *buf, size_t nbytes);
ssize_t			io_write(t_iodes *iodes, const char *buf, size_t nbytes);
ssize_t			io_fwrite(t_iodes *iodes, const char *buf, size_t nbytes);
ssize_t			io_swrite(t_iodes *iodes, const char *buf, size_t nbytes);
ssize_t			io_pump(t_iodes *iodes, size_t nbytes);
void 			io_print_stats(const t_iodes *iodes, const char *name);
void 			io_copy(t_iodes * const dst, t_iodes * const src);
void			io_fclose(t_iodes * const iodes);
void			io_fclose_multi(t_iodes *iodes, ...);



typedef ssize_t	(*t_func_io_read)(void *ctx, char *buf, size_t nbytes);
typedef ssize_t	(*t_func_io_write)(void *ctx, const char *buf, size_t nbytes);
typedef ssize_t	(*t_func_io_flush)(void *ctx, const char *buf, size_t nbytes);
typedef void	(*t_func_io_close)(void *ctx);

typedef struct	s_io_v2_interface
{
	t_func_io_read	read;
	t_func_io_write	write;
	t_func_io_close	close;
	t_func_io_flush	flush;
}				t_io_v2_interface;

typedef enum	e_io_v2_type
{
	IO_V2_TYPE_BASIC = 0,
	IO_V2_TYPE_BUFFERED,
	IO_V2_TYPE_PIPE,
}				t_io_v2_type;

typedef enum	e_io_v2_state
{
	IO_V2_STATUS_OK = 0,
	IO_V2_STATUS_EOF,
	IO_V2_STATUS_CLOSED,
}				t_io_v2_status;

typedef enum	e_io_v2_flag
{
	IO_V2_FLAG_NONE = 0UL,
	IO_V2_FLAG_READ = 1UL << 1,
	IO_V2_FLAG_WRITE = 1UL << 2,
	IO_V2_FLAG_FLUSH = 1UL << 3,
	IO_V2_FLAG_CLOSE = 1UL << 4,
}				t_io_v2_flag;

typedef struct		s_io_v2_stream
{
	void				*ctx;
	t_io_v2_interface	interface;
	t_io_v2_type		type;
	t_io_v2_flag		flags;
	t_io_v2_status		status;
}					t_io_v2_stream;

typedef struct		s_io_v2_pipe {
	t_io_v2_stream	*in;
	t_io_v2_stream	*out;
	uint8_t			*buf;
	size_t			bufsize;
} 					t_io_v2_pipe;

int	    io_v2_stream(t_io_v2_stream **stream, void *ctx, const t_io_v2_interface interface, t_io_v2_flag flags);

ssize_t	io_v2_read(t_io_v2_stream *stream, char *buf, size_t nbytes);
ssize_t	io_v2_write(t_io_v2_stream *stream, const char *buf, size_t nbytes);
void	io_v2_close(t_io_v2_stream *stream);


#endif
