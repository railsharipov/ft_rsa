/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/09 13:36:51 by rsharipo          #+#    #+#             */
/*   Updated: 2018/10/13 09:00:28 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_H
# define LIBFT_H

# include <string.h>
# include <limits.h>
# include <stdlib.h>
# include <stdint.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/types.h>
# include <sys/uio.h>
# include <sys/errno.h>
# include <stdio.h>

/* enable ft_malloc wrapper */
// # define LIBFT_MEM_ALLOC

# define LIBFT_BUFF				256
# define LIBFT_ERRBUF			1024
# define LIBFT_HT_SIZE			512

# define COLOR_RED				"\x1b[31m"
# define COLOR_GREEN			"\x1b[32m"
# define COLOR_YELLOW			"\x1b[33m"
# define COLOR_BLUE				"\x1b[34m"
# define COLOR_MAGENTA			"\x1b[35m"
# define COLOR_CYAN				"\x1b[36m"
# define COLOR_RED_BOLD			"\033[1m\033[31m"
# define COLOR_RESET			"\x1b[0m"

# define TXT_RED(STR_LITERAL)	COLOR_RED STR_LITERAL COLOR_RESET
# define TXT_GREEN(STR_LITERAL)	COLOR_GREEN STR_LITERAL COLOR_RESET
# define TXT_YELL(STR_LITERAL)	COLOR_YELLOW STR_LITERAL COLOR_RESET
# define TXT_CYAN(STR_LITERAL)	COLOR_CYAN STR_LITERAL COLOR_RESET
# define TXT_MAG(STR_LITERAL)	COLOR_MAGENTA STR_LITERAL COLOR_RESET

/* Round X down to a multiple of Y */
# define FLOOR(X,Y)				((X)-((X)%(Y)))

/* Round X up to a multiple of Y */
# define CEIL(X,Y)				(((X)%(Y)==0)?(X):((X)+((Y)-((X)%(Y)))))

# define MAX(A,B)				((A)>(B)?(A):(B))
# define MIN(A,B)				((A)<(B)?(A):(B))
# define ASC(X)					(X-48)
# define ABS(X)					((X)>=(0)?(X):(-X))

# define NBITS_TO_NWORDS(X, WORD_BIT)	((((ssize_t)X)-1)/WORD_BIT+1)
# define NBITS_TO_NBYTES(X)				((((ssize_t)X)-1)/CHAR_BIT+1)
# define NWORDS_TO_NBITS(X, WORD_BIT)	((X)*WORD_BIT)
# define NBYTES_TO_NBITS(X)				((X)*CHAR_BIT)

# define IS_OF_TYPE(X, T)		_Generic((X), T:1, default:0)

# define DO_NOTHING				(void)0;

/* Overload macros on number of args (up to 10 args) */
# define __NARG__(...)	__NARG_I_(__VA_ARGS__ __VA_OPT__(,) 10,9,8,7,6,5,4,3,2,1,0)
# define __NARG_I_(...)	__ARG_N(__VA_ARGS__)
# define __ARG_N(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,N,...) N

# define _VFUNC_(name, n)	name##n
# define _VFUNC(name, n)	_VFUNC_(name, n)
# define VFUNC(func, ...)	_VFUNC(func, __NARG__(__VA_ARGS__)) (__VA_ARGS__)

enum	e_libft_err
{
	LIBFT_OK				= 0,
	LIBFT_ERR				= -1,
	LIBFT_MEM_ERR			= -2,
	LIBFT_MEM_LEAK			= -3,
	LIBFT_MEM_DOUBLE_FREE	= -4,
	LIBFT_MEM_FATAL			= -5,
};

enum	e_libft_type
{
	LIBFT_INT,
	LIBFT_UINT,
	LIBFT_STRING,
	LIBFT_OSTRING,
	LIBFT_HTABLE,
	LIBFT_ARRAY,
};

enum	e_libft_alloc_flag
{
	LIBFT_ALLOC_NONE = 0,
	LIBFT_ALLOC_DEBUG,
};

typedef uint32_t	t_bool;

typedef struct		s_node
{
	uint64_t		hash;
	uint8_t			type;
	char			*key;
	void			*content;
	size_t			size;
	struct s_node	*next;
	struct s_node	*nodes;
	void			(*f_del)(struct s_node *); /* FUNC_NODE_DEL */
}					t_node;

typedef struct		s_queue
{
	t_node			*first;
	t_node			*last;
}					t_queue;

typedef struct		s_stack
{
	t_node			*top;
}					t_stack;

typedef struct		s_htbl
{
	void 			**arr;
	int				size;
}					t_htbl;

typedef	void (*FUNC_NODE_DEL)(t_node *);

void		*ft_malloc(const char *memkey, size_t memsize);
void		ft_free(const char *memkey, void *memptr);
void		ft_free_all(void);

uint32_t	ft_hash(const unsigned char *, size_t);
void		*ft_htbl_init(int);
t_node		*ft_htbl_iter(t_htbl *);
void		ft_htbl_del(t_htbl *);

void		(ft_htbl_add)(t_htbl *, void *, const char *);
void		*(ft_htbl_get)(t_htbl *, const char *);
void		(ft_htbl_assign)(t_htbl *, void *, const char *);
void		(ft_htbl_erase)(t_htbl *htbl, const char *key);
void		ft_htbl_bin_add(t_htbl *, void *, const void *, size_t);
void		*ft_htbl_bin_get(t_htbl *, const void *, size_t);
void		ft_htbl_bin_assign(t_htbl *, void *, const void *, size_t);
void 		ft_htbl_resize(t_htbl *htbl, int size);

# define 	ft_htbl_add4(...) ft_htbl_bin_add(__VA_ARGS__)
# define 	ft_htbl_add3(...) ft_htbl_add(__VA_ARGS__)
# define 	ft_htbl_get3(...) ft_htbl_bin_get(__VA_ARGS__)
# define 	ft_htbl_get2(...) ft_htbl_get(__VA_ARGS__)
# define 	ft_htbl_assign4(...) ft_htbl_bin_assign(__VA_ARGS__)
# define 	ft_htbl_assign3(...) ft_htbl_assign(__VA_ARGS__)

# define	ft_htbl_add(...) VFUNC(ft_htbl_add, __VA_ARGS__)
# define	ft_htbl_get(...) VFUNC(ft_htbl_get, __VA_ARGS__)
# define	ft_htbl_assign(...) VFUNC(ft_htbl_assign, __VA_ARGS__)

void 		(ft_node_init)(t_node *);
t_node		*(ft_node_create)(void);
t_node		*(ft_node_new)(const char *, void *, size_t);
void 		(ft_node_del)(t_node *);
void 		ft_node_init_with_f_del(t_node *, FUNC_NODE_DEL);
t_node		*ft_node_create_with_f_del(FUNC_NODE_DEL);
t_node		*ft_node_new_with_f_del(const char *, void *, size_t, FUNC_NODE_DEL);
void 		ft_node_del_with_f_del(t_node *, FUNC_NODE_DEL);
int			ft_node_is_parent(t_node *);

# define 	ft_node_init2(...) ft_node_init_with_f_del(__VA_ARGS__)
# define 	ft_node_init1(...) ft_node_init(__VA_ARGS__)
# define 	ft_node_create1(...) ft_node_create_with_f_del(__VA_ARGS__)
# define 	ft_node_create0(...) ft_node_create(__VA_ARGS__)
# define 	ft_node_new4(...) ft_node_new_with_f_del(__VA_ARGS__)
# define 	ft_node_new3(...) ft_node_new(__VA_ARGS__)
# define 	ft_node_del2(...) ft_node_del_with_f_del(__VA_ARGS__)
# define 	ft_node_del1(...) ft_node_del(__VA_ARGS__)

# define	ft_node_init(...) VFUNC(ft_node_init, __VA_ARGS__)
# define	ft_node_create(...) VFUNC(ft_node_create, __VA_ARGS__)
# define 	ft_node_new(...) VFUNC(ft_node_new, __VA_ARGS__)
# define 	ft_node_del(...) VFUNC(ft_node_del, __VA_ARGS__)

void		ft_lst_append(t_node **, t_node *);
void		ft_lst_prepend(t_node **, t_node *);
void		ft_lst_del(t_node *, FUNC_NODE_DEL);
void		ft_lst_del_first(t_node **, FUNC_NODE_DEL);
void		ft_lst_del_last(t_node **, FUNC_NODE_DEL);
void		ft_lst_del_one(t_node **, t_node *, FUNC_NODE_DEL);
size_t		ft_lst_size(t_node *);
int			ft_lst_map(t_node *, void *farg, int (*f)(t_node *, void *));
t_htbl		*ft_lst_htable(t_node *);

t_stack		*ft_stack_init(void);
void		*ft_stack_pop(t_stack *);
void		ft_stack_push(t_stack *, const char *, void *, size_t);
t_node		*ft_stack_peek(t_stack *);
int 		ft_stack_is_empty(t_stack *);
int 		ft_stack_size(t_stack *);
void		ft_stack_clear(t_stack *);
void		ft_stack_del(t_stack *, FUNC_NODE_DEL);
t_htbl		*ft_stack_htable(t_stack *);

t_queue		*ft_queue_init(void);
void		ft_queue_enqueue(t_queue *, const char *, void *, size_t);
void		*ft_queue_dequeue(t_queue *);
t_node		*ft_queue_peek(t_queue *);
int			ft_queue_is_empty(t_queue *);
int			ft_queue_size(t_queue *);
void		ft_queue_del(t_queue *, FUNC_NODE_DEL);
void		ft_queue_del_node(t_queue *, t_node *, FUNC_NODE_DEL);
t_htbl		*ft_queue_htable(t_queue *);

t_node		*ft_ntree_construct(const char *);
int 		ft_ntree_dfs(t_node **res, t_node *, const void *, int (*f)(t_node *, const void *));
int			ft_ntree_dfs_cur_depth(void);
int			ft_ntree_bfs(t_node **res, t_node *, const void *, int (*f)(t_node *, const void *));
void		ft_ntree_del(t_node *, FUNC_NODE_DEL);
void		ft_ntree_print(t_node *, void (*f_print)(t_node *, int));
int			ft_ntree_size(t_node *);
t_node		*ft_ntree_iter(t_node *);
t_htbl		*ft_ntree_to_htable(t_node *);

void 		*ft_memset(void *b, int c, size_t len);
void		ft_bzero(void *s, size_t n);
void		*ft_memcpy(void *dst, const void *src, size_t n);
void		*ft_memzcpy(void *dst, const void *src, size_t, size_t);
void		*ft_memmove(void *dst, const void *src, size_t len);
int			ft_memcmp(const void *s1, const void *s2, size_t n);
void		*ft_memdup(void *, size_t);
void		ft_strdel(char *s);
size_t		ft_strlen(const char *s);
char		*ft_strdup(const char *s);
char		*ft_strndup(const char *s, size_t n);
char		*ft_strrev(const char *s);
char		*ft_strcpy(char *dst, const char *src);
char		*ft_strncpy(char *dst, const char *src, size_t len);
char		*ft_strcat(char *s1, const char *s2);
char		*ft_strncat(char *s1, const char *s2, size_t n);
char		*ft_strchr(const char *s, int c);
char		*ft_strrchr(const char *s, int c);
int			ft_strcmp(const char *s1, const char *s2);
int			ft_strncmp(const char *s1, const char *s2, size_t n);
long long	ft_atoi(const char *str);
int			ft_isalpha(int c);
int			ft_isdigit(int c);
int			ft_isalnum(int c);
int			ft_isascii(int c);
int			ft_isprint(int c);
int			ft_iseol(int c);
int			ft_iseolws(int c);
int			ft_iswspace(int c);
int			ft_isblank(int c);
int			ft_ishex(int c);
int			ft_str_isalpha(const char *s);
int			ft_str_isalnum(const char *s);
int			ft_str_isdigit(const char *s);
int			ft_str_isascii(const char *s);
int			ft_str_isprint(const char *s);
int			ft_str_ishex(const char *s);
char		*ft_strup(char *s);
char		*ft_strlow(char *s);
int			ft_toupper(int c);
int			ft_tolower(int c);
char		*ft_strsub(char const *s, unsigned int start, size_t len);
char		*ft_strjoin(char const *s1, char const *s2);
char		**ft_strsplit(char const *s, char c);
char		*ft_itoa(intmax_t n);
char		*ft_itoa_base(intmax_t value, int base);
void		ft_putstr(char const *s);
void		ft_putstr_fd(int fd, char const *s);
int			get_next_line(int fd, char **line);
int			ft_printf(const char *format, ...);
char		*ft_binhex(const void *bin, size_t binsize);
void		ft_hexbin(void *bin, const char *hex, size_t hexsize);
void		ft_revbits(void *src, size_t size);
void		ft_intbytes(char *buf, intmax_t integer, int intsize);
void		ft_2darray_del(void **, int);
void 		ft_2darray_del_null_terminated(void **);
int			ft_2darray_len_null_terminated(void **);
char		*ft_2darray_strjoin(char **, size_t, const char *);

extern int	global_ft_malloc_error;

# if defined (LIBFT_MEM_ALLOC)

#  define MEM_ERROR() \
	do \
	{ \
		ft_printf("%@critical memory error %d\n", global_ft_malloc_error); \
		ft_printf("%@\t%s in %s:%d\n", __func__, __FILE__, __LINE__); \
	} \
	while (0)

#  define LIBFT_FREE(PTR) \
	do \
	{ \
		if (NULL != PTR) \
		{ \
			ft_free(#PTR, PTR); \
			\
			if (LIBFT_OK != global_ft_malloc_error) \
			{ \
				MEM_ERROR() \
			} \
			PTR = NULL; \
		} \
	} \
	while (0)

#  define LIBFT_ALLOC(PTR, SZ) \
	do \
	{ \
		PTR = ft_malloc(#PTR, SZ); \
		if (LIBFT_OK != global_ft_malloc_error) \
		{ \
			MEM_ERROR() \
			PTR = NULL; \
		} \
	} \
	while (0)

#  define LIBFT_REALLOC(PTR, SZ, NSZ) \
	do \
	{ \
		void	*NEWPTR; \
		NEWPTR = ft_malloc(#PTR "_realloc_", NSZ); \
		if (LIBFT_OK != global_ft_malloc_error) \
		{ \
			MEM_ERROR() \
			NEWPTR = NULL; \
		} \
		if (NULL != PTR && NULL != NEWPTR) \
		{ \
			ft_memcpy(NEWPTR, PTR, SZ); \
			ft_free(#PTR, PTR); \
		} \
		PTR = NEWPTR; \
	} \
	while (0)

# else

#  define LIBFT_FREE(PTR) \
	do \
	{ \
		if (NULL != PTR) \
		{ \
			free(PTR); \
			PTR = NULL; \
		} \
	} \
	while (0)

#  define LIBFT_ALLOC(PTR, SZ) \
	do \
	{ \
		PTR = malloc(SZ); \
		if (ENOMEM == errno) \
		{ \
			perror(TXT_RED("critical memory error")); \
			PTR = NULL; \
		} \
		else \
		{ \
			ft_bzero(PTR, SZ); \
		} \
	} \
	while (0)

#  define LIBFT_REALLOC(PTR, SZ, NSZ) \
	do \
	{ \
		void	*NEWPTR; \
		LIBFT_ALLOC(NEWPTR, NSZ); \
		if (NULL != PTR && NULL != NEWPTR) \
		{ \
			ft_memcpy(NEWPTR, PTR, SZ); \
			free(PTR); \
		} \
		PTR = NEWPTR; \
	} \
	while (0)

# endif

#endif
