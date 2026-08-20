#ifndef LIBFT_H
#define LIBFT_H

#include <sys/types.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <stdio.h>

#define COLOR_RED			"\x1b[31m"
#define COLOR_GREEN			"\x1b[32m"
#define COLOR_GREEN_BOLD	"\033[1m\033[32m"
#define COLOR_YELLOW		"\x1b[33m"
#define COLOR_BLUE			"\x1b[34m"
#define COLOR_MAGENTA		"\x1b[35m"
#define COLOR_CYAN			"\x1b[36m"
#define COLOR_RED_BOLD		"\033[1m\033[31m"
#define COLOR_RESET			"\x1b[0m"
#define COLOR_WHITE			"\x1b[37m"
#define COLOR_WHITE_BOLD	"\033[1m\033[37m"
#define COLOR_GRAY			"\x1b[90m"
#define COLOR_GRAY_BOLD		"\033[1m\033[90m"

#define TXT_RED(STR_LIT)		COLOR_RED STR_LIT COLOR_RESET
#define TXT_B_RED(STR_LIT)		COLOR_RED_BOLD STR_LIT COLOR_RESET
#define TXT_GREEN(STR_LIT)		COLOR_GREEN STR_LIT COLOR_RESET
#define TXT_B_GREEN(STR_LIT)	COLOR_GREEN_BOLD STR_LIT COLOR_RESET
#define TXT_YELL(STR_LIT)		COLOR_YELLOW STR_LIT COLOR_RESET
#define TXT_BLUE(STR_LIT)		COLOR_BLUE STR_LIT COLOR_RESET
#define TXT_MAGEN(STR_LIT)		COLOR_MAGENTA STR_LIT COLOR_RESET
#define TXT_CYAN(STR_LIT)		COLOR_CYAN STR_LIT COLOR_RESET
#define TXT_WHITE(STR_LIT)		COLOR_WHITE STR_LIT COLOR_RESET
#define TXT_B_WHITE(STR_LIT)	COLOR_WHITE_BOLD STR_LIT COLOR_RESET
#define TXT_GRAY(STR_LIT)		COLOR_GRAY STR_LIT COLOR_RESET
#define TXT_B_GRAY(STR_LIT)		COLOR_GRAY_BOLD STR_LIT COLOR_RESET

/* Round X down to a multiple of Y */
#define FLOOR(X,Y)	((X)-((X)%(Y)))

/* Round X up to a multiple of Y */
#define CEIL_TO_MULTIPLE(X,Y)	(((X)%(Y)==0)?(X):((X)+((Y)-((X)%(Y)))))

#define MAX(A,B)	((A)>(B)?(A):(B))
#define MIN(A,B)	((A)<(B)?(A):(B))
#define ASC(X)		(X-48)
#define ABS(X)		((X)>=(0)?(X):(-X))

#define NBITS_TO_NWORDS(X, WORD_BIT)	((((ssize_t)X)-1)/WORD_BIT+1)
#define NBITS_TO_NBYTES(X)				((((ssize_t)X)-1)/CHAR_BIT+1)
#define NWORDS_TO_NBITS(X, WORD_BIT)	((X)*WORD_BIT)
#define NBYTES_TO_NBITS(X)				((X)*CHAR_BIT)

#define IS_NONZERO_INT32(X)	((((X)|(0-(X)))>>31)&1)
#define IS_NONZERO_INT64(X)	((((X)|(0-(X)))>>63)&1)

#define IS_OF_TYPE(X, T)	_Generic((X), T:1, default:0)
#define DO_NOTHING			(void)0;

typedef enum e_libft_status {
	LIBFT_OK = 0,
	LIBFT_ERR,
} t_libft_status;

typedef __uint128_t	uint128_t;

/****************************************************************************/

void		ft_2darray_del(void **arr, int arr_len);
void 		ft_2darray_del_null_terminated(void **arr);
int			ft_2darray_len_null_terminated(void **arr);
char		*ft_2darray_strjoin(char **str_arr, size_t str_arr_len, const char *delim);

/****************************************************************************/

typedef enum e_transform_status {
	TRANSFORM_ERROR = -1,
	TRANSFORM_OK = 0,
    TRANSFORM_NEED_INPUT = 1,
    TRANSFORM_NEED_OUTPUT = 2,
    TRANSFORM_DONE = 3
} t_transform_status;

typedef struct s_transform_result {
    size_t consumed;
    size_t produced;
    t_transform_status status;
} t_transform_result;

typedef t_transform_result (*t_func_transform)(void *vctx, const void *in, size_t insize, void *out, size_t outsize);

/****************************************************************************/

typedef struct s_buffer {
	void	*arr;
	size_t	capacity;
	size_t	read_pos;
	size_t	write_pos;
} t_buffer;

typedef ssize_t (*t_func_buffer_read)(void *vctx, const void *buf, size_t nbytes);
typedef ssize_t (*t_func_buffer_write)(void *vctx, void *buf, size_t nbytes);

t_buffer	*ft_buffer_new(size_t capacity);
void		ft_buffer_del(t_buffer *buffer);
const char	*ft_buffer_view(t_buffer *buffer);
size_t		ft_buffer_used(t_buffer *buffer);
size_t		ft_buffer_available(t_buffer *buffer);
void		ft_buffer_reset(t_buffer *buffer);
int			ft_buffer_is_empty(t_buffer *buffer);
int			ft_buffer_is_full(t_buffer *buffer);
int			ft_buffer_is_valid(t_buffer *buffer);
ssize_t 	ft_buffer_read(t_buffer *buffer, void *buf, size_t nbytes);
ssize_t 	ft_buffer_write(t_buffer *buffer, const void *buf, size_t nbytes);
size_t 		ft_buffer_left_align(t_buffer *buffer);
ssize_t 	ft_buffer_read_with_func(t_buffer *buffer, t_func_buffer_read read, void *vctx, size_t nbytes);
ssize_t 	ft_buffer_write_with_func(t_buffer *buffer, t_func_buffer_write write, void *vctx, size_t nbytes);
t_transform_result ft_buffer_transform(t_buffer *src, t_buffer *dst, t_func_transform transform, void *transform_ctx);

/****************************************************************************/

char		*ft_bytes_to_hex(const void *bytes, size_t size);
char		*ft_bytes_to_hex_upper(const void *bytes, size_t size);
void		ft_bytes_reverse_bits(void *bytes, size_t size);
uint64_t	ft_bytes_to_uint(void *bytes, size_t size);
void		ft_bytes_lshift(void *bytes, size_t size, int shift);
void		ft_bytes_rshift(void *bytes, size_t size, int shift);
char		*ft_bytes_dumps_hex(void *bytes, size_t size, int cols, int del);
void		ft_bytes_dumpb_hex(void *bytes, size_t size, int cols, int del, char *buf, size_t bufsize);
void		ft_bytes_write_hex(int fd, void *bytes, size_t size);
void		ft_bytes_print_bits(void *, size_t);
void		ft_bytes_xor(void *res, void *bytes1, void *bytes2, size_t size);
void		ft_hex_to_bytes(void *bytes, const char *hex, size_t hexsize);

/****************************************************************************/

uint32_t	ft_uint_bswap32(uint32_t);
uint64_t	ft_uint_bswap64(uint64_t);
uint128_t	ft_uint_bswap128(uint128_t);
int 		ft_uint_lmbit(uint64_t uint, int uint_nbits);
int 		ft_uint_rmbit(uint64_t uint, int uint_nbits);
void		ft_uint_print_bits(uint64_t uint, int uint_nbits, int col_nbits);
void		ft_uint_to_bytes(void *bytes, uint64_t uint, int uint_size);

/****************************************************************************/

typedef struct s_chunk {
	uint8_t		*data;
	size_t		size;
} t_chunk;

t_chunk	*ft_chunk_create(size_t size);
void		ft_chunk_del(t_chunk *chunk);
void		ft_chunk_clear(t_chunk *chunk);
void		ft_chunk_append(t_chunk *chunk, void *data, size_t size);
void		ft_chunk_append_str(t_chunk *chunk, char *str);
void		ft_chunk_append_bytes(t_chunk *chunk, void *bytes, size_t size);
void		ft_chunk_append_chunk(t_chunk *chunk, t_chunk *other);
char		*ft_chunk_to_cstr(t_chunk *chunk);

/****************************************************************************/

typedef	void	(*t_func_content_del)(void *content);
typedef	void	*(*t_func_content_copy)(void *content);

typedef struct s_node {
	uint64_t		hash;
	uint8_t			type;
	char			*key;
	void			*content;
	size_t			size;
	struct s_node	*next;
	struct s_node	*nodes;
	t_func_content_del	f_del_content;
	t_func_content_copy	f_copy_content;
} t_node;

int 	ft_node_is_parent(t_node *);
void 	ft_node_init(t_node *);
void 	ft_node_init_with_f_del(t_node *, t_func_content_del);
t_node	*ft_node_new(const char *, void *, size_t);
t_node	*ft_node_new_with_f_del(const char *, void *, size_t, t_func_content_del);
void 	ft_node_del(t_node *);
void 	ft_node_del_with_f_del(t_node *, t_func_content_del);
t_node	*ft_node_create(void);
t_node	*ft_node_create_with_f_del(t_func_content_del);

/****************************************************************************/

#define LIBFT_HT_SIZE	512

typedef struct s_htbl {
	void 	**arr;
	int		size;
} t_htbl;

uint32_t	ft_hash(const unsigned char *key, size_t keysize);
void		*ft_htbl_create(int size);
t_node		*ft_htbl_node_next(t_htbl *htbl, t_node *node);
int			ft_htbl_has(t_htbl *htbl, const char *key);
void		*ft_htbl_get(t_htbl *htbl, const char *key);
void		*ft_htbl_get_rawkey(t_htbl *htbl, const void *rawkey, size_t rksize);
void		ft_htbl_dump(t_htbl *htbl);
void 		ft_htbl_resize(t_htbl *htbl, int size);
void		ft_htbl_add(t_htbl *htbl, void *content, const char *key);
void		ft_htbl_add_with_f_del(t_htbl *htbl, void *content, const char *key, t_func_content_del f_del);
t_htbl		*ft_htbl_copy(t_htbl *htbl);
t_htbl		*ft_htbl_copy_with_f_copy(t_htbl *htbl, t_func_content_copy f_copy);
void		ft_htbl_merge(t_htbl *htbl_to, t_htbl *htbl_from);
void		ft_htbl_merge_with_f_copy(t_htbl *htbl_to, t_htbl *htbl_from, t_func_content_copy f_copy);
void		ft_htbl_add_rawkey(t_htbl *htbl, void *content, const void *rawkey, size_t rksize);
void		ft_htbl_add_rawkey_with_f_del(t_htbl *htbl, void *content, const void *rawkey, size_t rksize, t_func_content_del f_del);
void		ft_htbl_assign(t_htbl *htbl, void *content, const char *key);
void		ft_htbl_assign_with_f_del(t_htbl *htbl, void *content, const char *key, t_func_content_del f_del);
void		ft_htbl_assign_rawkey(t_htbl *htbl, void *content, const void *rawkey, size_t rksize);
void		ft_htbl_assign_rawkey_with_f_del(t_htbl *htbl, void *content, const void *rawkey, size_t rksize, t_func_content_del f_del);
void		ft_htbl_del(t_htbl *htbl);
void		ft_htbl_del_with_f_del(t_htbl *htbl, t_func_content_del f_del);
void		ft_htbl_del_key(t_htbl *htbl, const char *key);
void		ft_htbl_del_key_with_f_del(t_htbl *htbl, const char *key, t_func_content_del f_del);
void		ft_htbl_del_rawkey(t_htbl *htbl, const char *key, const void *rawkey, size_t rksize);
void		ft_htbl_del_rawkey_with_f_del(t_htbl *htbl, const char *key, const void *rawkey, size_t rksize, t_func_content_del f_del);

/****************************************************************************/

t_node		*ft_lst_append(t_node **lst, t_node *node);
t_node		*ft_lst_prepend(t_node **lst, t_node *node);
t_node		*ft_lst_concat(t_node *lst_a, t_node *lst_b);
void		ft_lst_del(t_node *lst);
void		ft_lst_del_with_f_del(t_node *lst, t_func_content_del f_del);
void		ft_lst_del_first(t_node **lst);
void		ft_lst_del_first_with_f_del(t_node **lst, t_func_content_del f_del);
void		ft_lst_del_last(t_node **lst);
void		ft_lst_del_last_with_f_del(t_node **lst, t_func_content_del f_del);
void		ft_lst_del_one(t_node **lst, t_node *node);
void		ft_lst_del_one_with_f_del(t_node **lst, t_node *node, t_func_content_del f_del);
size_t		ft_lst_size(t_node *lst);
void		ft_lst_rev(t_node **lst);
int			ft_lst_map(t_node *lst, void *farg, int (*f)(t_node *, void *));
t_htbl		*ft_lst_to_htable(t_node *lst);

/****************************************************************************/

typedef int	(*t_func_log)(const char *mes);

enum	e_logger_level
{
	LIBFT_LOG_LEVEL_ALWAYS = 0,
	LIBFT_LOG_LEVEL_CRIT,
	LIBFT_LOG_LEVEL_ERROR,
	LIBFT_LOG_LEVEL_WARN,
	LIBFT_LOG_LEVEL_INFO,
	LIBFT_LOG_LEVEL_DEBUG,
	LIBFT_LOG_LEVEL_TRACE,
	LIBFT_NUM_LOG_LEVELS,
};

typedef struct	s_logger {
	t_func_log	log_writer;
	uint8_t 	log_level_thres;
	uint8_t 	debug_info_thres;
	uint8_t 	is_ansi_colored;
} t_logger;

int	ft_log_log(const char *func_name, const char *file_name, int line_number, t_logger *logger, uint8_t level, const char *fmt, ...);
int	ft_log_va_log(const char *func_name, const char *file_name, int line_number, t_logger *logger, uint8_t level, const char *fmt, va_list va_arg);

/****************************************************************************/

#define LIBFT_LOG(LEVEL, FMT, ...)	ft_log_log(__func__, __FILE__, __LINE__, libft_get_logger(), LIBFT_LOG_LEVEL_##LEVEL, FMT __VA_OPT__(,) __VA_ARGS__)

t_logger	*libft_get_logger(void);

/****************************************************************************/

typedef int 	(*t_func_ntree_map)(t_node *node, const void *farg);
typedef void	(*t_func_ntree_print)(t_node *node, int space);

t_node	*ft_ntree_construct(const char *map);
int 	ft_ntree_dfs(t_node **res, t_node *ntree, const void *farg, t_func_ntree_map f);
int		ft_ntree_dfs_cur_depth(void);
int		ft_ntree_bfs(t_node **res, t_node *node, const void *farg, t_func_ntree_map f);
void	ft_ntree_del(t_node *ntree, t_func_content_del f_del);
void	ft_ntree_print(t_node *ntree, t_func_ntree_print f_print);
int		ft_ntree_size(t_node *node);
t_node	*ft_ntree_iter(t_node *ntree);
t_htbl	*ft_ntree_to_htable(t_node *node);

/****************************************************************************/

typedef struct s_queue
{
	t_node	*first;
	t_node	*last;
} t_queue;

t_queue		*ft_queue_create(void);
void		ft_queue_enqueue(t_queue *queue, const char *key, void *content, size_t size);
void		ft_queue_enqueue_node(t_queue *queue, t_node *node);
void		*ft_queue_dequeue(t_queue *queue);
t_node		*ft_queue_dequeue_node(t_queue *queue);
t_node		*ft_queue_first(t_queue *queue);
t_node		*ft_queue_last(t_queue *queue);
int			ft_queue_is_empty(t_queue *queue);
int			ft_queue_size(t_queue *queue);
void		ft_queue_del(t_queue *queue);
void		ft_queue_del_with_f_del(t_queue *queue, t_func_content_del f_del);
void		ft_queue_del_node(t_queue *queue, t_node *node);
void		ft_queue_del_node_with_f_del(t_queue *queue, t_node *node, t_func_content_del f_del);
t_htbl		*ft_queue_to_htable(t_queue *queue);
t_node		*ft_queue_pop_list(t_queue *queue);

/****************************************************************************/

typedef struct	s_stack {
	t_node	*top;
} t_stack;

t_stack		*ft_stack_create(void);
void		*ft_stack_pop(t_stack *stack);
t_node		*ft_stack_pop_node(t_stack *stack);
void		ft_stack_push(t_stack *stack, const char *key, void *content, size_t size);
void		ft_stack_push_node(t_stack *stack, t_node *node);
t_node		*ft_stack_peek(t_stack *stack);
int			ft_stack_is_empty(t_stack *stack);
int			ft_stack_size(t_stack *stack);
void		ft_stack_del(t_stack *stack);
void		ft_stack_del_with_f_del(t_stack *stack, t_func_content_del f_del);
t_htbl		*ft_stack_to_htable(t_stack *stack);
t_node		*ft_stack_pop_list(t_stack *stack);

/****************************************************************************/

#define LIBFT_BUFF     256

typedef struct	s_pf
{
	char		flag;
	char		len;
	int			fwid;
	int			prec;
	char		type;
	int			num;
	size_t		size;
	size_t		str_size;
	int			space;
	int			base;
	int			fd;
	char		**buf;
}				t_pf;

/****************************************************************************/

typedef enum e_pf_flag {
	PF_PREC = (1 << 6),
	PF_SHARP = (1 << 5),
	PF_ZERO = (1 << 4),
	PF_MINUS = (1 << 3),
	PF_PLUS = (1 << 2),
	PF_SPACE = (1 << 1),
	PF_APOS = (1 << 0)
} t_flag;

typedef enum e_pf_len
{
	PF_Z = (1 << 5),
	PF_J = (1 << 4),
	PF_LL = (1 << 3),
	PF_L = (1 << 2),
	PF_H = (1 << 1),
	PF_HH = (1 << 0)
} t_len;

int		ft_printf(const char *format, ...);
int		ft_fprintf(int fd, const char *format, ...);
int		ft_sprintf(char **buf, const char *format, ...);
int		ft_snprintf(char *buf, size_t size, const char *format, ...);
int		ft_vprintf(const char *format, va_list arg);
int		ft_vfprintf(int fd, const char *format, va_list arg);
int		ft_vsprintf(char **buf, const char *format, va_list arg);

/****************************************************************************/

void 	*ft_memset(void *b, int c, size_t len);
void	ft_bzero(void *s, size_t n);
void	*ft_memcpy(void *dst, const void *src, size_t n);
void	*ft_memzcpy(void *dst, const void *src, size_t dstsize, size_t srcsize);
void	*ft_memmove(void *dst, const void *src, size_t len);
int		ft_memcmp(const void *s1, const void *s2, size_t n);
void	*ft_memdup(void *src, size_t srcsize);

/****************************************************************************/

ssize_t	    ft_atoi(const char *str);
size_t		ft_atoi_u(const char *str);
char		*ft_itoa(intmax_t n);
char		*ft_itoa_base(intmax_t value, int base);

/****************************************************************************/

int	get_next_line(int fd, char **line);

/****************************************************************************/

#define OSTRING_INIT_CAPACITY 16

typedef	struct s_ostring {
	unsigned char	*content;
	size_t	size;
	size_t	capacity;
} t_ostring;

t_ostring	*ft_ostr_new(void);
t_ostring	*ft_ostr_create_from_cstr(const char *init_str);
void 		ft_ostr_init(t_ostring *ostring);
void		ft_ostr_clear(t_ostring *ostring);
void		ft_ostr_del(t_ostring *ostring);
t_ostring	*ft_ostr_dup(const t_ostring *ostring);
t_ostring	*ft_ostr_append_cstr(t_ostring *ostring, const char *cstr);
t_ostring	*ft_ostr_append_ostr(t_ostring *ostring, const t_ostring *ostr);
t_ostring	*ft_ostr_append(t_ostring *ostring, void *content, size_t size);
t_ostring	*ft_ostr_appendf(t_ostring *ostring, const char *fmt, ...);
t_ostring	*ft_ostr_prepend(t_ostring *ostring, void *content, size_t size);
char		*ft_ostr_to_cstr(const t_ostring *ostr, size_t offset, size_t len);

/****************************************************************************/

void	ft_strdel(char *s);
size_t	ft_strlen(const char *s);
char	*ft_strdup(const char *s);
char	*ft_strndup(const char *s, size_t n);
char	*ft_strrev(const char *s);
char	*ft_strcpy(char *dst, const char *src);
char	*ft_strncpy(char *dst, const char *src, size_t len);
char	*ft_strcat(char *s1, const char *s2);
char	*ft_strncat(char *s1, const char *s2, size_t n);
char	*ft_strchr(const char *s, int c);
char	*ft_strrchr(const char *s, int c);
int		ft_strcmp(const char *s1, const char *s2);
int		ft_strncmp(const char *s1, const char *s2, size_t n);
int		ft_streq(const char *a, const char *b);
int		ft_strneq(const char *a, const char *b, size_t n);
char	*ft_strsub(char const *s, unsigned int start, size_t len);
char	*ft_strjoin(char const *s1, char const *s2);
char	*ft_strjoin_multi(int n, ...);
char	**ft_strsplit(char const *s, char c);
char	*ft_str_strip(const char *s);
char	*ft_strup(const char *s);
char	*ft_strnup(const char *s, size_t n);
char	*ft_strlow(const char *s);
int		ft_str_isalpha(const char *s);
int		ft_str_isalnum(const char *s);
int		ft_str_isnum(const char *s);
int		ft_str_isascii(const char *s);
int		ft_str_isprint(const char *s);
int		ft_str_ishex(const char *s);

/****************************************************************************/

int		ft_isalpha(int c);
int		ft_isdigit(int c);
int		ft_isalnum(int c);
int		ft_isascii(int c);
int		ft_isprint(int c);
int		ft_iseol(int c);
int		ft_iseolws(int c);
int		ft_iswspace(int c);
int		ft_isblank(int c);
int		ft_ishex(int c);
int		ft_toupper(int c);
int		ft_tolower(int c);

/****************************************************************************/

// Get rid of these functions
void		ft_putstr(char const *s);
void		ft_putstr_fd(int fd, char const *s);

/****************************************************************************/

typedef struct s_tuple {
	t_node	*__head;
	t_node	*__tail;
} t_tuple;

t_tuple		*ft_tuple_new(void *head, size_t head_size, void *tail, size_t tail_size);
t_tuple		*ft_tuple_new_with_f_del(void *head, size_t head_size, void *tail, size_t tail_size, t_func_content_del f_del);
void		ft_tuple_del(t_tuple *tuple);
void		ft_tuple_del_with_f_del(t_tuple *tuple, t_func_content_del f_del);
void		*ft_tuple_get(t_tuple *tuple, int index);

/****************************************************************************/

/* Overload macros on number of args (up to 10 args) */
#define __NARG__(...)	__NARG_I_(__VA_ARGS__ __VA_OPT__(,) 10,9,8,7,6,5,4,3,2,1,0)
#define __NARG_I_(...)	__ARG_N(__VA_ARGS__)
#define __ARG_N(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,N,...) N

#define _VFUNC_(name, n)	name##n
#define _VFUNC(name, n)	_VFUNC_(name, n)
#define VFUNC(func, ...)	_VFUNC(func, __NARG__(__VA_ARGS__)) (__VA_ARGS__)

/****************************************************************************/

enum e_libft_alloc_flag {
	LIBFT_ALLOC_NONE = 0,
	LIBFT_ALLOC_DEBUG,
};

enum e_libft_alloc_err {
	LIBFT_MEM_OK = 0,
	LIBFT_MEM_ERR,
	LIBFT_MEM_LEAK,
	LIBFT_MEM_DOUBLE_FREE,
	LIBFT_MEM_FATAL,
};

extern int	libft_alloc_err_extern;

void    *ft_malloc(const char *memkey, size_t memsize);
void    ft_free(const char *memkey, void *memptr);
void    ft_free_all(void);

#define LIBFT_MALLOC_HT_SIZE	512

// #define LIBFT_MEM_ALLOC
#ifdef LIBFT_MEM_ALLOC

#define LIBFT_FREE(PTR) \
	do { \
		if (NULL != PTR) { \
			ft_free(#PTR, PTR); \
			if (LIBFT_OK == libft_alloc_err_extern) { PTR = NULL; } \
		} \
	} while (0)

#define LIBFT_ALLOC(PTR, SZ) \
	do { \
		PTR = ft_malloc(#PTR, SZ); \
		if (LIBFT_OK != libft_alloc_err_extern) { \
			PTR = NULL; \
		} \
	} while (0)

#define LIBFT_REALLOC(PTR, SZ, NSZ) \
	do { \
		void	*NEWPTR; \
		NEWPTR = ft_malloc(#PTR "_realloc_", NSZ); \
		if (LIBFT_OK != libft_alloc_err_extern) { \
			NEWPTR = NULL; \
		} \
		if (NULL != PTR && NULL != NEWPTR) { \
			ft_memcpy(NEWPTR, PTR, SZ); \
			ft_free(#PTR, PTR); \
		} \
		PTR = NEWPTR; \
	} while (0)

#define LIBFT_MUST_ALLOC(PTR, SZ) \
	do { \
		PTR = ft_malloc(#PTR, SZ); \
		if (LIBFT_OK != libft_alloc_err_extern) { \
			exit(1); \
		} \
	} while (0)

#define LIBFT_MUST_REALLOC(PTR, SZ, NSZ) \
	do { \
		void	*NEWPTR; \
		NEWPTR = ft_malloc(#PTR "_realloc_", NSZ); \
		if (LIBFT_OK != libft_alloc_err_extern) { \
			exit(1); \
		} \
		if (NULL != PTR && NULL != NEWPTR) { \
			ft_memcpy(NEWPTR, PTR, SZ); \
			ft_free(#PTR, PTR); \
		} \
		PTR = NEWPTR; \
	} while (0)

#else //LIBFT_MEM_ALLOC

#define LIBFT_FREE(PTR) \
	do { \
		if (NULL != PTR) { \
			free(PTR); \
			PTR = NULL; \
		} \
	} while (0)

#define LIBFT_ALLOC(PTR, SZ) \
	do { \
		PTR = malloc(SZ); \
		if (ENOMEM == errno) { \
			PTR = NULL; \
		} else { \
			ft_bzero(PTR, SZ); \
		} \
	} while (0)

#define LIBFT_REALLOC(PTR, SZ, NSZ) \
	do { \
		void *NEWPTR; \
		LIBFT_ALLOC(NEWPTR, NSZ); \
		if (NULL != PTR && NULL != NEWPTR) { \
			ft_memcpy(NEWPTR, PTR, SZ); \
			free(PTR); \
		} \
		PTR = NEWPTR; \
	} while (0)

#define LIBFT_MUST_ALLOC(PTR, SZ) \
	do { \
		PTR = malloc(SZ); \
		if (ENOMEM == errno) { \
			exit(ENOMEM); \
		} else { \
			ft_bzero(PTR, SZ); \
		} \
	} while (0)

#define LIBFT_MUST_REALLOC(PTR, SZ, NSZ) \
	do { \
		void *NEWPTR; \
		LIBFT_MUST_ALLOC(NEWPTR, NSZ); \
		if (NULL != PTR && NULL != NEWPTR) { \
			ft_memcpy(NEWPTR, PTR, SZ); \
			free(PTR); \
		} \
		PTR = NEWPTR; \
	} while (0)

#endif //LIBFT_MEM_ALLOC

#endif //LIBFT_H
