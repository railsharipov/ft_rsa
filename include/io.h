#ifndef SSL_IO_H
# define SSL_IO_H

# include <stdint.h>
# include <common.h>
# include <libft/string.h>
# include <libft/logger.h>
# include <libft/buffer.h>

# define IO_BUFSIZE 512

# define IO_INIT_ERROR		"i/o init error"
# define IO_READ_ERROR		"i/o read error"
# define IO_WRITE_ERROR		"i/o write error"

# define IO_CREATE_STREAM_ERROR	"i/o stream create error"

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

// IO V1 API

// Initialization and configuration
int		io_fopen(t_iodes *iodes, uint32_t flags, const char *filename);
int		io_osbuf(t_iodes *iodes, uint32_t flags, t_ostring *osbuf);
int		io_pipe(t_iodes *iodes, uint32_t flags, t_iodes *iodes_in, t_iodes *iodes_out);
int		io_is_init(t_iodes *iodes);

// Read operations
ssize_t	io_read(t_iodes *iodes, char *buf, size_t nbytes);
ssize_t	io_fread(t_iodes *iodes, char *buf, size_t nbytes);
ssize_t	io_sread(t_iodes *iodes, char *buf, size_t nbytes);

// Write operations
ssize_t	io_write(t_iodes *iodes, const char *buf, size_t nbytes);
ssize_t	io_fwrite(t_iodes *iodes, const char *buf, size_t nbytes);
ssize_t	io_swrite(t_iodes *iodes, const char *buf, size_t nbytes);

// Pipe operations
ssize_t	io_pump(t_iodes *iodes, size_t nbytes);

// Cleanup operations
void	io_close(t_iodes * const iodes);
void	io_fclose(t_iodes * const iodes);
void	io_close_multi(t_iodes *iodes, ...);
void	io_fclose_multi(t_iodes *iodes, ...);

// Utility operations
void	io_copy(t_iodes * const dest, t_iodes * const src);
void	io_print_stats(const t_iodes *iodes, const char *name);

// IO V2 API

typedef ssize_t	(*t_func_io_v2_read)(void *ctx, void *buf, size_t nbytes);
typedef ssize_t	(*t_func_io_v2_write)(void *ctx, const void *buf, size_t nbytes);
typedef ssize_t	(*t_func_io_v2_flush)(void *ctx);
typedef ssize_t	(*t_func_io_v2_close)(void *ctx);

typedef int	(*t_func_io_v2_filter)(const void *input, size_t insize, void *output, size_t outsize, size_t *consumed, size_t *produced);

typedef struct s_io_v2_interface {
	t_func_io_v2_read	read;
	t_func_io_v2_write	write;
	t_func_io_v2_flush	flush;
	t_func_io_v2_close	close;
} t_io_v2_interface;

typedef enum e_io_v2_flag {
	IO_V2_FLAG_READ		= 1UL << 1,
	IO_V2_FLAG_WRITE	= 1UL << 2,
	IO_V2_FLAG_FLUSH 	= 1UL << 3,
	IO_V2_FLAG_CLOSE 	= 1UL << 4,
} t_io_v2_flag;

typedef enum e_io_v2_status {
	IO_V2_STATUS_OK		= 0,
	IO_V2_STATUS_ERROR	= -1,
	IO_V2_STATUS_EOF	= -2,
	IO_V2_STATUS_CLOSED	= -3,
} t_io_v2_status;

struct s_io_v2_pipe;

typedef struct s_io_v2_stream {
	t_io_v2_interface	interface;
	t_io_v2_flag		flags;
	t_io_v2_status		status;
	void				*ctx;
} t_io_v2_stream;

typedef enum e_io_v2_pipe_type {
	IO_V2_PIPE_TYPE_UNIDIR,
	IO_V2_PIPE_TYPE_BIDIR,
} t_io_v2_pipe_type;

typedef struct s_io_v2_pipe {
	t_io_v2_pipe_type	type;
	t_io_v2_status		status;
	void				*ctx;
} t_io_v2_pipe;

ssize_t	io_v2_read(t_io_v2_stream *stream, void *buf, size_t nbytes);
ssize_t	io_v2_write(t_io_v2_stream *stream, const void *buf, size_t nbytes);
ssize_t	io_v2_flush(t_io_v2_stream *stream);
ssize_t	io_v2_close(t_io_v2_stream *stream);

int		io_v2_stream(t_io_v2_stream **stream, t_io_v2_interface interface, void *ctx);

int		io_v2_buffered_reader(t_io_v2_stream **stream, t_io_v2_stream *upstream, size_t capacity);
int		io_v2_buffered_writer(t_io_v2_stream **stream, t_io_v2_stream *downstream, size_t capacity);

int		io_v2_file_reader(t_io_v2_stream **stream, const char *file_path);
int		io_v2_file_writer(t_io_v2_stream **stream, const char *file_path);

int		io_v2_bytes_reader(t_io_v2_stream **stream, t_ostring *ostring);
int		io_v2_bytes_writer(t_io_v2_stream **stream, t_ostring *ostring);

int		io_v2_pipe_unidir(t_io_v2_pipe **pipe, t_io_v2_stream *source, t_io_v2_stream *destination, size_t capacity);
int		io_v2_pipe_bidir(t_io_v2_pipe **pipe, t_io_v2_stream *source, t_io_v2_stream *destination, size_t capacity);
ssize_t	io_v2_pipe_pump(t_io_v2_pipe *pipe, size_t nbytes);
ssize_t	io_v2_pipe_flush(t_io_v2_pipe *pipe);
ssize_t	io_v2_pipe_close(t_io_v2_pipe *pipe);

#endif
