#ifndef LIBFT_V2_H
#define LIBFT_V2_H

#include <assert.h>
#include <stddef.h>

typedef _Bool bool;

#define true  1
#define false 0

/****************************************************************************/

#define container_of(ptr, type, member)	((type *)((char *)(ptr) - offsetof(type, member)))
#define node_of(ptr, type) (&((type *)(ptr))->base)

typedef struct s_node_v2 {
	struct s_node_v2 *next;
	struct s_node_v2 *nodes;
} t_node_v2;

typedef void (*t_func_node_delete)(t_node_v2 *node, const void *vctx);
typedef void (*t_func_node_map)(t_node_v2 *node, const void *vctx);
typedef bool (*t_func_node_op)(t_node_v2 *node, const void *vctx);

/****************************************************************************/

typedef struct s_ref {
	t_node_v2  base;
	void       *ptr;
} t_ref;

/****************************************************************************/

typedef struct s_list {
	t_node_v2	*first;
	t_node_v2	*last;
	size_t		size;
} t_list;

void		ft_list_append(t_list *list, t_node_v2 *node);
void		ft_list_prepend(t_list *list, t_node_v2 *node);
t_node_v2	*ft_list_pop(t_list *list);
t_node_v2	*ft_list_find(t_list *list, t_func_node_op f_find, const void *vctx);
t_node_v2	*ft_list_remove(t_list *list, t_func_node_op f_find, const void *vctx);
void		ft_list_clear(t_list *list, t_func_node_delete f_del, const void *vctx);
void		ft_list_del(t_list *list, t_func_node_delete f_del, const void *vctx);
void		ft_list_reverse(t_list *list);
char 		*ft_list_dumps(t_list *list);

/****************************************************************************/

typedef struct s_htbl_v2 {
	t_list 		*arr;
	uint32_t	size;
	size_t		colls;
} t_htbl_v2;

typedef struct s_htbl_v2_next {
	uint32_t	idx;
	size_t		nidx;
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
bool		ft_htbl_v2_next(t_htbl_v2 *htbl, t_htbl_v2_next *next, const char **key, void **content);
void 		ft_htbl_v2_resize(t_htbl_v2 *htbl, uint32_t size);
char		*ft_htbl_v2_dumps(t_htbl_v2 *htbl);

/****************************************************************************/

typedef struct s_ntree {
	t_node_v2 *root;
} t_ntree;

t_node_v2	*ft_ntree_v2_dfs(t_ntree *ntree, t_func_node_op f_find, const void *vctx);
t_node_v2	*ft_ntree_v2_bfs(t_ntree *ntree, t_func_node_op f_find, const void *vctx);
void		ft_ntree_v2_del(t_ntree *ntree, t_func_node_delete f_del, const void *vctx);

#endif //LIBFT_V2_H
