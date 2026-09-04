#ifndef LIBFT_V2_H
#define LIBFT_V2_H

#include <assert.h>
#include <stddef.h>

typedef _Bool bool;

#define true  1
#define false 0

/****************************************************************************/

#define CONTAINER_OF(ptr, type, member)	((type *)((char *)(ptr) - offsetof(type, member)))

typedef	void (*t_func_content_del)(void *content);
typedef	void *(*t_func_content_copy)(void *content);
typedef	void *(*t_func_content_map)(void **content, const void *farg);

typedef struct s_node_v2 {
	struct s_node_v2 *next;
	struct s_node_v2 *nodes;
} t_node_v2;

typedef void (*t_func_del_node)(t_node_v2 *node, const void *farg);
typedef int (*t_func_find_node)(t_node_v2 *node, const void *farg);

/****************************************************************************/

typedef struct s_list {
	t_node_v2	*first;
	t_node_v2	*last;
	size_t		size;
} t_list;

void		ft_list_append(t_list *list, t_node_v2 *node);
void		ft_list_prepend(t_list *list, t_node_v2 *node);
t_node_v2	*ft_list_pop(t_list *list);
t_node_v2	*ft_list_remove(t_list *list, t_func_find_node f_find_node, const void *farg);
void		ft_list_reverse(t_list *list);
char 		*ft_list_dumps(t_list *list);

void ft_list_append(t_list *list, t_node_v2 *node)
{
	if (NULL == list) {
		return ;
	}
	if (NULL == list->last) {
		list->first = node;
		list->last = node;
	} else {
		list->last->next = node;
		list->last = node;
	}
	if (NULL == list->first) {
		list->first = node;
	}
	list->size += 1;
}

void ft_list_prepend(t_list *list, t_node_v2 *node)
{
	if (NULL == list) {
		return ;
	}
	node->next = list->first;
	list->first = node;

	if (NULL == list->last) {
		list->last = list->first;
	}
	list->size += 1;
}

t_node_v2 *ft_list_pop(t_list *list)
{
	if (NULL == list || NULL == list->first) {
		return (NULL);
	}
	t_node_v2 *node = list->first;
	list->first = node->next;
	node->next = NULL;
	if (list->first == NULL) {
		list->last = NULL;
	}
	list->size -= 1;
	return (node);
}

t_node_v2 *ft_list_remove(t_list *list, t_func_find_node f_find_node, const void *farg)
{
	if (NULL == list || list->size == 0 || NULL == f_find_node) {
		return (NULL);
	}
	t_node_v2 *cur = list->first;
	if (NULL != cur && f_find_node(cur, farg)) {
		// Short path: remove first node from the list.
		list->first = cur->next;
		list->size -= 1;
	}
	else {
		// Iterate through a list to find a node to remove.
		t_node_v2 *prev = NULL;
		while (NULL != cur && !f_find_node(cur, farg)) {
			prev = cur;
			cur = cur->next;
		}
		if (NULL != cur) {
			// Remove the node from a list.
			if (cur == list->last) {
				list->last = prev;
			}
			prev->next = cur->next;
			list->size -= 1;
		}
	}
	if (list->first == NULL) {
		list->last = NULL;
	}
	return (cur);
}

t_node_v2 *ft_list_find(t_list *list, t_func_find_node f_find_node, void *farg)
{
	if (NULL == list || NULL == f_find_node) {
		return (NULL);
	}
	t_node_v2 *node = list->first;
	while (NULL != node) {
		if (f_find_node(node, farg)) {
			return (node);
		}
		node = node->next;
	}
	return (NULL);
}

void	ft_list_reverse(t_list *list)
{
	if (NULL == list || NULL == list->first) {
		return ;
	}
	t_node_v2 *prev = NULL;
	t_node_v2 *cur = list->first;

	while (cur) {
		t_node_v2 *next = cur->next;
		cur->next = prev;
		prev = cur;
		cur = next;
	}
	list->last = list->first;
	list->first = prev;
}

char *ft_list_dumps(t_list *list)
{
	if (NULL == list) {
		return (ft_strdup("null"));
	}
	t_ostring ostring = {0};
	ft_ostr_init_with_capacity(&ostring, 256);
	ft_ostr_appendf(&ostring, "{\"first\":\"%p\",\"last\":\"%p\",\"items\":[", list->first, list->last);
	t_node_v2 *node = list->first;
	size_t commas = 0;
	while (NULL != node) {
		if (commas++) ft_ostr_append_cstr(&ostring, ",");
		ft_ostr_appendf(&ostring, "{\"ptr\":\"%p\"}", node);
		node = node->next;
	}
	ft_ostr_appendf(&ostring, "],\"size\":%zu}", list->size);
	char *dumps = ft_ostr_to_cstr(&ostring, 0, ostring.size);
	ft_ostr_clear(&ostring);
	return (dumps);
}

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

typedef struct s_htbl_v2_entry {
	t_node_v2	base;
	uint32_t	hash;
	char		*key;
	void		*content;
} t_htbl_v2_entry;

static inline uint32_t	__ft_htbl_v2_calc_hash(const char *key)
{
	return (ft_hash((uint8_t *)key, ft_strlen(key)));
}

static inline uint32_t	__ft_htbl_v2_calc_array_idx(const t_htbl_v2 *htbl, uint32_t hash)
{
	return (hash % htbl->size);
}

static void __ft_htbl_v2_delete_node(t_node_v2 *node, t_func_content_del f_del_content)
{
	t_htbl_v2_entry *entry = CONTAINER_OF(node, t_htbl_v2_entry, base);
	if (NULL != entry->key) {
		LIBFT_FREE(entry->key);
	}
	if (NULL != entry->content && NULL != f_del_content) {
		f_del_content(entry->content);
	}
	LIBFT_FREE(entry);
}

static t_node_v2 *__ft_htbl_v2_create_node(const char *key, void *content)
{
	t_htbl_v2_entry *entry = NULL;
	LIBFT_ALLOC(entry, sizeof(t_htbl_v2_entry));
	*entry = (t_htbl_v2_entry){0};
	entry->hash = __ft_htbl_v2_calc_hash(key);
	entry->key = ft_strdup(key);
	entry->content = content;
	return (&entry->base);
}

static int __ft_htbl_v2_find_node(t_node_v2 *node, const char *key)
{
	t_htbl_v2_entry *entry = CONTAINER_OF(node, t_htbl_v2_entry, base);
	return (ft_streq(entry->key, key));
}

static int __ft_htbl_v2_next_entry(t_htbl_v2 *htbl, t_htbl_v2_next *next, t_htbl_v2_entry **ret_entry)
{
	assert(NULL != htbl && NULL != htbl->arr);
	assert(NULL != next);

	if (next->idx >= htbl->size) {
		return (0);
	}
	for (uint32_t idx = next->idx; idx < htbl->size; idx++) {
		t_list *list = htbl->arr + idx;
		if (list->size == 1) {
			t_node_v2 *node = list->first;
			if (NULL != ret_entry) {
				*ret_entry = CONTAINER_OF(node, t_htbl_v2_entry, base);
			}
			next->idx = idx + 1;
			next->nidx = 0;
			return (1);
		}
		else if (list->size > 1) {
			size_t nidx = 0;
			t_node_v2 *node = list->first;
			for (; NULL != node; node = node->next) {
				if (nidx == next->nidx) break;
				nidx++;
			}
			if (NULL != node) {
				if (NULL != ret_entry) {
					*ret_entry = CONTAINER_OF(node, t_htbl_v2_entry, base);
				}
				next->idx = idx;
				next->nidx += 1;
				return (1);
			}
			else {
				next->idx = idx + 1;
				next->nidx = 0;
			}
		}
	}
	return (0);
}

t_htbl_v2 *ft_htbl_v2_create(uint32_t size)
{
	t_htbl_v2 *htbl = NULL;
	LIBFT_ALLOC(htbl, sizeof(t_htbl_v2));
	*htbl = (t_htbl_v2){0};
	htbl->size = MAX(LIBFT_HT_SIZE, CEIL_TO_MULTIPLE(size, LIBFT_HT_SIZE));
	LIBFT_ALLOC(htbl->arr, htbl->size * sizeof(t_list));
	return (htbl);
}

void ft_htbl_v2_init(t_htbl_v2 *htbl, uint32_t size)
{
	assert(NULL != htbl);

	if (NULL != htbl) {
		*htbl = (t_htbl_v2){0};
		htbl->size = MAX(LIBFT_HT_SIZE, CEIL_TO_MULTIPLE(size, LIBFT_HT_SIZE));
		LIBFT_ALLOC(htbl->arr, htbl->size * sizeof(t_list));
	}
}

bool ft_htbl_v2_set(t_htbl_v2 *htbl, const char *key, void *content)
{
	assert(NULL != htbl && NULL != htbl->arr);
	if (NULL == key) return (false);

	uint32_t hash = __ft_htbl_v2_calc_hash(key);
	uint32_t idx = __ft_htbl_v2_calc_array_idx(htbl, hash);

	// We use the list because different keys may resolve to the same array index.
	t_list *list = htbl->arr + idx;
	t_node_v2 *node = list->first;
	for (; NULL != node; node = node->next) {
		if (__ft_htbl_v2_find_node(node, key)) break;
	}
	if (NULL != node) {
		// The key has already been set.
		return (false);
	}
	if (list->size > 0) htbl->colls += 1;
	ft_list_append(list, __ft_htbl_v2_create_node(key, content));
	return (true);
}

bool ft_htbl_v2_reset(t_htbl_v2 *htbl, const char *key, void *content, t_func_content_del f_del_content)
{
	assert(NULL != htbl && NULL != htbl->arr);
	if (NULL == key) return (false);

	uint32_t hash = __ft_htbl_v2_calc_hash(key);
	uint32_t idx = __ft_htbl_v2_calc_array_idx(htbl, hash);

	// We use the list because different keys may resolve to the same array index.
	t_list *list = htbl->arr + idx;
	t_node_v2 *node = list->first;
	for (; NULL != node; node = node->next) {
		if (__ft_htbl_v2_find_node(node, key)) break;
	}
	if (NULL != node) {
		// The key has already been set: replace the value for the key.
		t_htbl_v2_entry *entry = CONTAINER_OF(node, t_htbl_v2_entry, base);
		if (NULL != entry->content) {
			f_del_content(entry->content);
		}
		entry->content = content;
	} else {
		// The key has not been set yet: set new value.
		if (list->size > 0) htbl->colls += 1;
		ft_list_append(list, __ft_htbl_v2_create_node(key, content));
	}
	return (true);
}

bool ft_htbl_v2_unset(t_htbl_v2 *htbl, const char *key, t_func_content_del f_del_content)
{
	assert(NULL != htbl && NULL != htbl->arr);
	if (NULL == key) return (false);

	uint32_t hash = __ft_htbl_v2_calc_hash(key);
	uint32_t idx = __ft_htbl_v2_calc_array_idx(htbl, hash);
	t_list *list = htbl->arr + idx;
	t_node_v2 *node = ft_list_remove(list, (t_func_find_node)__ft_htbl_v2_find_node, key);
	if (NULL != node) {
		__ft_htbl_v2_delete_node(node, f_del_content);
	}
	return (true);
}

bool ft_htbl_v2_has(const t_htbl_v2 *htbl, const char *key)
{
	assert(NULL != htbl && NULL != htbl->arr);
	if (NULL == key) return (false);

	uint32_t hash = __ft_htbl_v2_calc_hash(key);
	uint32_t idx = __ft_htbl_v2_calc_array_idx(htbl, hash);
	t_list *list = htbl->arr + idx;
	for (t_node_v2 *node = list->first; NULL != node; node = node->next) {
		if (__ft_htbl_v2_find_node(node, key)) return (true);
	}
	return (false);
}

void *ft_htbl_v2_get(const t_htbl_v2 *htbl, const char *key)
{
	assert(NULL != htbl && NULL != htbl->arr);
	if (NULL == key) return (NULL);

	uint32_t hash = __ft_htbl_v2_calc_hash(key);
	uint32_t idx = __ft_htbl_v2_calc_array_idx(htbl, hash);
	t_list *list = htbl->arr + idx;
	for (t_node_v2 *node = list->first; NULL != node; node = node->next) {
		if (__ft_htbl_v2_find_node(node, key)) {
			t_htbl_v2_entry *entry = CONTAINER_OF(node, t_htbl_v2_entry, base);
			return (entry->content);
		}
	}
	return (NULL);
}

void ft_htbl_v2_clear(t_htbl_v2 *htbl, t_func_content_del f_del_content)
{
	assert(NULL != htbl);
	if (NULL == htbl->arr) return;

	for (uint32_t idx = 0; idx < htbl->size; idx++) {
		t_list *list = htbl->arr + idx;
		t_node_v2 *node = ft_list_pop(list);
		while (NULL != node) {
			__ft_htbl_v2_delete_node(node, f_del_content);
			node = ft_list_pop(list);
		}
	}
	LIBFT_FREE(htbl->arr);
	htbl->size = 0;
}

void ft_htbl_v2_del(t_htbl_v2 *htbl, t_func_content_del f_del_content)
{
	if (NULL == htbl) return;
	ft_htbl_v2_clear(htbl, f_del_content);
	LIBFT_FREE(htbl);
}

bool ft_htbl_v2_next(t_htbl_v2 *htbl, t_htbl_v2_next *next, const char **key, void **content)
{
	t_htbl_v2_entry *entry = NULL;
	if (__ft_htbl_v2_next_entry(htbl, next, &entry)) {
		if (NULL != key) *key = entry->key;
		if (NULL != content) *content = entry->content;
		return (true);
	}
	return (false);
}

void ft_htbl_v2_resize(t_htbl_v2 *htbl, uint32_t size)
{
	assert(NULL != htbl && NULL != htbl->arr);
	if (size <= htbl->size) return;

	// Temporary new htable.
	t_htbl_v2 *tmp_htbl = ft_htbl_v2_create(size);
	t_htbl_v2_next next = {0};
	t_htbl_v2_entry *entry = NULL;
	while (__ft_htbl_v2_next_entry(htbl, &next, &entry)) {
		// Shallow copy the content.
		ft_htbl_v2_set(tmp_htbl, entry->key, entry->content);
	}
	// Cleanup current htable and then shallow copy the temporary htable to the current htable.
	ft_htbl_v2_clear(htbl, NULL);
	htbl->arr = tmp_htbl->arr;
	htbl->size = tmp_htbl->size;
	LIBFT_FREE(tmp_htbl);
}

char *ft_htbl_v2_dumps(t_htbl_v2 *htbl)
{
	if (NULL == htbl) {
		return (ft_strdup("null"));
	}
	t_ostring ostring;
	ft_ostr_init_with_capacity(&ostring, 1024*1024);

	if (htbl->arr == NULL) {
		ft_ostr_append_cstr(&ostring, "null");
	}
	else {
		ft_ostr_append_cstr(&ostring, "[");
		uint32_t commas = 0;
		for (uint32_t idx = 0; idx < htbl->size; idx++) {
			t_list *list = htbl->arr + idx;
			if (list->size > 0) {
				t_node_v2 *node = list->first;
				for (; node != NULL; node = node->next) {
					if (commas++) {
						ft_ostr_append_cstr(&ostring, ",");
					}
					t_htbl_v2_entry *entry = CONTAINER_OF(node, t_htbl_v2_entry, base);
					ft_ostr_appendf(&ostring, "{\"idx\":%lu,\"key\":\"%s\",\"hash\":\"%#lx\",\"content\":\"%p\"}",
						idx, entry->key, entry->hash, entry->content);
				}
			}
		}
		ft_ostr_append_cstr(&ostring, "]");
	}

	char *arr_dumps = ft_ostr_to_cstr(&ostring, 0, ostring.size);
	ft_ostr_clear(&ostring);
	char *dumps = NULL;
	ft_sprintf(&dumps, "{\"size\":%d,\"arr\":%s}", htbl->size, arr_dumps);
	LIBFT_FREE(arr_dumps);

	return (dumps);
}

#endif //LIBFT_V2_H
