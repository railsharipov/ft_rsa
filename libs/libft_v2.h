#ifndef LIBFT_V2_H
#define LIBFT_V2_H

#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <libft.h>

/****************************************************************************/

#define container_of(ptr, type, member)	((type *)((char *)(ptr) - offsetof(type, member)))
#define container_next(ptr, type, member) (container_of((ptr)->base.next, type, member))
#define container_nodes(ptr, type, member) (container_of((ptr)->base.nodes, type, member))

typedef struct s_node_v2 {
	struct s_node_v2 *next;
	struct s_node_v2 *nodes;
} t_node_v2;

typedef void (*t_func_node_del)(t_node_v2 *node);
typedef void (*t_func_node_map)(t_node_v2 *node, void *vctx);
typedef bool (*t_func_node_op)(t_node_v2 *node, void *vctx);
typedef bool (*t_func_node_find)(t_node_v2 *node, const void *vctx);
typedef char *(*t_func_node_dumps)(const t_node_v2 *node, int depth);
typedef void (*t_func_node_dump)(const t_node_v2 *node, int depth);
typedef t_node_v2 *(*t_func_node_copy)(const t_node_v2 *node);

/****************************************************************************/

typedef struct s_list {
	t_node_v2	*first;
	t_node_v2	*last;
	size_t		size;
} t_list;

typedef struct s_list_next {
	t_node_v2 *node;
	bool init;
} t_list_next;

t_list		*ft_list_create(void);
void		ft_list_init(t_list *list);
void		ft_list_append(t_list *list, t_node_v2 *node);
void		ft_list_prepend(t_list *list, t_node_v2 *node);
t_node_v2	*ft_list_pop(t_list *list);
t_node_v2	*ft_list_remove(t_list *list, t_node_v2 *node);
t_node_v2 	*ft_list_at(t_list *list, size_t idx);
void		ft_list_reverse(t_list *list);
char 		*ft_list_dumps(t_list *list);
void		ft_list_copy(const t_list *src, t_list *dst, t_func_node_copy f_copy);
void		ft_list_move(t_list *src, t_list *dst);
void		ft_list_clear(t_list *list, t_func_node_del f_del);
void		ft_list_del(t_list *list, t_func_node_del f_del);

/****************************************************************************/

void	ft_list_append_ref(t_list *list, void *ptr);
void	ft_list_prepend_ref(t_list *list, void *ptr);
void	*ft_list_pop_ref(t_list *list);
bool	ft_list_next_ref(const t_list *list, t_list_next *next, void **ref);
void	ft_list_copy_all_ref(const t_list *src, t_list *dst);
void	ft_list_clear_all_ref(t_list *list);
void	ft_list_del_all_ref(t_list *list);

/****************************************************************************/

typedef	void (*t_func_content_del)(void *content);
typedef	void *(*t_func_content_copy)(const void *content);
typedef	bool (*t_func_content_eq)(const void *content, void *vctx);

void	ft_list_append_content(t_list *list, void *content);
void	ft_list_prepend_content(t_list *list, void *content);
void	*ft_list_pop_content(t_list *list);
void	*ft_list_first_content(t_list *list);
void	*ft_list_last_content(t_list *list);
void	*ft_list_content_at(t_list *list, size_t idx);
bool	ft_list_has_content(const t_list *list, t_func_content_eq f_eq, void *vctx);
bool	ft_list_next_content(const t_list *list, t_list_next *next, void **content);
void	ft_list_copy_all_content(const t_list *src, t_list *dst, t_func_content_copy f_copy);
void	ft_list_move_all_content(t_list *src, t_list *dst);
void	ft_list_clear_all_content(t_list *list, t_func_content_del f_del);
void	ft_list_del_all_content(t_list *list, t_func_content_del f_del);

/****************************************************************************/

typedef uint32_t (*t_func_hash)(uint8_t *buf, size_t size);

typedef struct s_htbl_v2 {
	t_list 		*arr;
	uint32_t	size;
	t_func_hash f_hash;
	size_t		colls;
	size_t		active_count;
	t_list		values;
} t_htbl_v2;

typedef struct s_htbl_v2_next {
	t_node_v2	*node;
	bool		init;
} t_htbl_v2_next;

t_htbl_v2	*ft_htbl_v2_create(uint32_t size);
void		ft_htbl_v2_init(t_htbl_v2 *htbl, uint32_t size);
bool		ft_htbl_v2_has(const t_htbl_v2 *htbl, const char *key);
void		*ft_htbl_v2_get(const t_htbl_v2 *htbl, const char *key);
bool		ft_htbl_v2_set(t_htbl_v2 *htbl, const char *key, void *content);
bool		ft_htbl_v2_reset(t_htbl_v2 *htbl, const char *key, void *content, t_func_content_del f_del_content);
bool		ft_htbl_v2_unset(t_htbl_v2 *htbl, const char *key, t_func_content_del f_del_content);
void		ft_htbl_v2_clear(t_htbl_v2 *htbl, t_func_content_del f_del_content);
void		ft_htbl_v2_del(t_htbl_v2 *htbl, t_func_content_del f_del_content);
t_htbl_v2	*ft_htbl_v2_clone(t_htbl_v2 *htbl, t_func_content_copy f_copy);
bool		ft_htbl_v2_next(const t_htbl_v2 *htbl, t_htbl_v2_next *next, const char **key, void **content);
void 		ft_htbl_v2_resize(t_htbl_v2 *htbl, uint32_t size);
char		*ft_htbl_v2_dumps(t_htbl_v2 *htbl);

/****************************************************************************/

t_node_v2	*ft_ntree_v2_dfs(t_node_v2 *node, t_func_node_find f_find, const void *vctx);
t_node_v2	*ft_ntree_v2_dfs_with_unwind(t_node_v2 *node, t_func_node_find f_find, const void *find_vctx, t_func_node_map f_on_unwind, void *unwind_vctx);
void		ft_ntree_v2_dfs_map(t_node_v2 *node, t_func_node_map f_map, void *vctx);
t_node_v2	*ft_ntree_v2_bfs(t_node_v2 *node, t_func_node_find f_find, const void *vctx);
void		ft_ntree_v2_bfs_map(t_node_v2 *node, t_func_node_map f_map, void *vctx);
t_node_v2	*ft_ntree_v2_clone(const t_node_v2 *node, t_func_node_copy f_copy);
void		ft_ntree_v2_del(t_node_v2 *node, t_func_node_del f_del);
char		*ft_ntree_v2_dumps(const t_node_v2 *node, t_func_node_dumps f_dumps);
void		ft_ntree_v2_dump(const t_node_v2 *node, t_func_node_dump f_dump);

/****************************************************************************/

bool	utf8_next(const uint8_t *bytes, size_t size, size_t *next);
bool	utf8_is_valid(const uint8_t *bytes, size_t size);
bool	utf8_escape(const uint8_t *bytes, size_t size, uint8_t **escaped, size_t *escaped_size,
				const uint8_t *escape_prefix, size_t prefix_size, const uint8_t *escape_charset, size_t charset_size);
bool	utf8_escape_nt(const uint8_t *bytes, size_t size, char **escaped, const char *escape_prefix,
				const uint8_t *escape_charset, size_t charset_size);

/****************************************************************************/

#define IO_BUFSIZE 16*1024

#define IO_INPUT_ERROR				"i/o input error"
#define IO_INIT_ERROR				"i/o init error"
#define IO_READ_ERROR				"i/o read error"
#define IO_WRITE_ERROR				"i/o write error"
#define IO_CLOSE_ERROR				"i/o close error"
#define IO_CREATE_STREAM_ERROR		"i/o stream create error"

typedef ssize_t	(*t_func_io_v2_read)(void *ctx, void *buf, size_t nbytes);
typedef ssize_t	(*t_func_io_v2_write)(void *ctx, const void *buf, size_t nbytes);
typedef ssize_t	(*t_func_io_v2_flush)(void *ctx);
typedef ssize_t	(*t_func_io_v2_finish)(void *ctx);
typedef ssize_t	(*t_func_io_v2_close)(void *ctx);

typedef struct s_io_v2_interface {
	t_func_io_v2_read	read;
	t_func_io_v2_write	write;
	t_func_io_v2_flush	flush;
	t_func_io_v2_finish	finish;
	t_func_io_v2_close	close;
} t_io_v2_interface;

typedef enum e_io_v2_flag {
	IO_V2_FLAG_READ		= 1UL << 1,
	IO_V2_FLAG_WRITE	= 1UL << 2,
	IO_V2_FLAG_FLUSH 	= 1UL << 3,
	IO_V2_FLAG_FINISH 	= 1UL << 4,
	IO_V2_FLAG_CLOSE 	= 1UL << 5,
} t_io_v2_flag;

typedef enum e_io_v2_status {
	IO_V2_STATUS_OK		  = 0,
	IO_V2_STATUS_ERROR	  = -1,
	IO_V2_STATUS_EOF	  = -2,
	IO_V2_STATUS_FINISHED = -3,
	IO_V2_STATUS_CLOSED	  = -4,
} t_io_v2_status;

typedef struct s_io_v2_stream {
	t_io_v2_interface	interface;
	t_io_v2_flag		flags;
	t_io_v2_status		status;
	void				*ctx;
} t_io_v2_stream;

extern t_io_v2_stream *io_v2_stdin_reader;
extern t_io_v2_stream *io_v2_stdin_writer;
extern t_io_v2_stream *io_v2_stdout_reader;
extern t_io_v2_stream *io_v2_stdout_writer;
extern t_io_v2_stream *io_v2_stderr_reader;
extern t_io_v2_stream *io_v2_stderr_writer;

// NON-BLOCKING
ssize_t	io_v2_read(t_io_v2_stream *stream, void *buf, size_t nbytes);
ssize_t	io_v2_write(t_io_v2_stream *stream, const void *buf, size_t nbytes);

// BLOCKING
ssize_t	io_v2_read_all(t_io_v2_stream *stream, void *buf, size_t nbytes);
ssize_t io_v2_write_all(t_io_v2_stream *stream, const void *buf, size_t nbytes);
ssize_t	io_v2_finish(t_io_v2_stream *stream);
ssize_t	io_v2_flush(t_io_v2_stream *stream);
ssize_t	io_v2_close(t_io_v2_stream *stream);

bool	io_v2_stream(t_io_v2_stream **stream, t_io_v2_interface interface, void *ctx);

bool	io_v2_buffered_reader(t_io_v2_stream **stream, t_io_v2_stream *upstream, size_t capacity);
bool	io_v2_buffered_writer(t_io_v2_stream **stream, t_io_v2_stream *downstream, size_t capacity);

bool	io_v2_file_reader(t_io_v2_stream **stream, const char *file_path);
bool	io_v2_fd_reader(t_io_v2_stream **stream, int fd);
bool	io_v2_file_writer(t_io_v2_stream **stream, const char *file_path);
bool	io_v2_fd_writer(t_io_v2_stream **stream, int fd);

bool	io_v2_bytes_reader(t_io_v2_stream **stream, t_ostring *ostring);
bool	io_v2_bytes_writer(t_io_v2_stream **stream, t_ostring *ostring);

bool	io_v2_filter_reader(t_io_v2_stream **stream, t_io_v2_stream *upstream,
							t_func_transform update, t_func_transform final, void *filter_ctx);

bool	io_v2_filter_writer(t_io_v2_stream **stream, t_io_v2_stream *downstream,
							t_func_transform update, t_func_transform final, void *filter_ctx);

typedef enum e_io_v2_pipe_status {
    IO_V2_STATUS_PIPE_OK,
    IO_V2_STATUS_PIPE_END,
    IO_V2_STATUS_PIPE_ERROR
} t_io_v2_pipe_status;

typedef enum e_io_v2_pipe_type {
	IO_V2_PIPE_TYPE_UNIDIR,
	IO_V2_PIPE_TYPE_BIDIR,
} t_io_v2_pipe_type;

typedef struct s_io_v2_pipe {
	t_io_v2_pipe_type	type;
	t_io_v2_pipe_status	status;
	void				*ctx;
} t_io_v2_pipe;

bool	io_v2_pipe_unidir(t_io_v2_pipe **pipe, t_io_v2_stream *upstream, t_io_v2_stream *downstream);
bool	io_v2_pipe_bidir(t_io_v2_pipe **pipe, t_io_v2_stream *upstream, t_io_v2_stream *downstream);
bool	io_v2_pipe_del(t_io_v2_pipe *pipe);
bool	io_v2_pipe_pump(t_io_v2_pipe *pipe);

/****************************************************************************/

#endif //LIBFT_V2_H
