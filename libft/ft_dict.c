#include <libft.h>

typedef struct	s_dict
{
	t_htbl		*htbl;
	t_node		*tree;
}				t_dict;

static t_htbl	*__get_htbl(t_dict *dict, char **keys, size_t nkeys);
static t_htbl	*__add_item(t_dict *dict, char **keys, size_t nkeys);

t_dict *ft_dict_new(void)
{
	t_dict	*dict;

	LIBFT_ALLOC(dict, sizeof(t_dict));

	dict->htbl = NULL;

	return (dict);
}

int	ft_dict_add(t_dict *dict, const char *path, void *content, size_t size)
{
	t_htbl	*htbl;
	t_node	*node;
	char	**keys;
	size_t	nkeys;

	if (NULL == dict || NULL == path)
		return (LIBFT_ERR);

	htbl = NULL;

	// path shall contain keys delimited by DICT_DELIM character
	// each key, from left to right, corresponds to a value, 
	// pointing to a nested dictionary item.

	keys = ft_strsplit(path, '.');
	nkeys = ft_2darray_len_null_terminated(keys);

	// get target htable
	if (nkeys > 0)
		htbl = __get_htbl(dict, keys, nkeys-1);
	
	// add dict item to a target htable
	// if (NULL != htbl)
	// 	__add_item(htbl, content, keys[nkeys-1]);

	ft_2darray_del_null_terminated(keys);

	if (NULL == htbl)
		return (LIBFT_ERR);

	return (LIBFT_OK);
}

static t_htbl *__get_htbl(t_dict *dict, char **keys, size_t nkeys)
{
	t_dict	*nested_dict;
	t_htbl	*htbl;
	void	*content;
	size_t	idx;

	while (idx < nkeys)
	{
		htbl = dict->htbl;
		content = ft_htbl_get(htbl, keys[idx]);

		if (NULL == content)
			return (NULL);
		
		nested_dict = (t_dict *)content;
	}
}