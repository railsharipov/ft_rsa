#include <util/json.h>
#include <util/bnum.h>
#include <libft/htable.h>
#include <libft/alloc.h>
#include <libft/error.h>
#include <libft/string.h>
#include <libft/list.h>

typedef ssize_t (*FUNC_JSON_PARSE)(const char *, t_node *);

static t_htbl	*__htable;

void	__init_htable(void);
int		__is_ws_only(const char *);

ssize_t __parse(const char *, t_node *);
ssize_t __parse_array(const char *, t_node *);
ssize_t __parse_object(const char *, t_node *);
ssize_t __parse_string(const char *, t_node *);
ssize_t __parse_number(const char *, t_node *);
ssize_t __parse_boolean(const char *, t_node *);
ssize_t __parse_null(const char *, t_node *);

void	__delete_array(void *content);
void	__delete_object(void *content);
void	__delete_string(void *content);
void	__delete_number(void *content);
void	__delete_boolean(void *content);
void	__delete_null(void *content);

typedef struct		s_json_ctx
{
	FUNC_JSON_PARSE f_parse;
	char 			open;
	char 			close;
}					t_json_ctx;

static const t_json_ctx T[] = {
    {	__parse_object,		'{', 	'}',   	},
    {	__parse_array,		'[', 	']',   	},
    {	__parse_string,		'"', 	'"',   	},
    {	__parse_boolean,	't', 	0,   	},
    {	__parse_boolean,	'f', 	0,   	},
    {	__parse_null,		'n', 	0,		},
    {	__parse_number,		'-', 	0,   	},
    {	__parse_number,		'0', 	0,   	},
    {	__parse_number,		'1', 	0,   	},
    {	__parse_number,		'2', 	0,   	},
    {	__parse_number,		'3', 	0,   	},
    {	__parse_number,		'4', 	0,   	},
    {	__parse_number,		'5', 	0,   	},
    {	__parse_number,		'6', 	0,   	},
    {	__parse_number,		'7', 	0,   	},
    {	__parse_number,		'8', 	0,   	},
    {	__parse_number,		'9', 	0,   	},
};

int json_parse(const char *s, t_node **node)
{
	t_node 	*json_node;
	t_node 	end_node;
	ssize_t	rbytes;

	if (NULL == s || NULL == node) {
        return JSON_ERROR(INVALID_INPUT_ERROR);
	}

    *node = NULL;
	__init_htable();

	if (NULL == __htable) {
		return JSON_ERROR("unspecified error");
	}
	json_node = ft_node_create();
	rbytes = __parse(s, json_node);

	if (rbytes < 0) {
		json_del(json_node);
		return JSON_ERROR("json parse failed");
	}
	if (!__is_ws_only(s + rbytes)) {
		json_del(json_node);
		return JSON_ERROR("unexpected characters at the end");
	}
	*node = json_node;
    ft_htbl_del(__htable);

    return (0);
}

void	__init_htable(void)
{
    int idx;
    size_t ht_size;

    ht_size = sizeof(T)/sizeof(T[0]);

	if (NULL == (__htable = ft_htbl_init(ht_size))) {
		return ;
	}

	idx = 0;
    while (idx < ht_size) {
		ft_htbl_add_rawkey(__htable, (void *)&(T[idx]), &(T[idx].open), sizeof(T[idx].open));
		idx++;
    }
}

int	__is_ws_only(const char *s)
{
	while (ft_iseolws(*s)) {
		s++;
	}
	if (*s == '\0') {
		return (1);
	} else {
		return (0);
	}
}

ssize_t __parse(const char *s, t_node *node)
{
	FUNC_JSON_PARSE	f_parse;
	t_json_ctx 		*ctx;
    ssize_t 		rbytes;
    ssize_t 		idx;
	char			open, close;

    idx = 0;

	while (ft_iseolws(s[idx])) {
		idx++;
	}
	open = s[idx];
	ctx = ft_htbl_get_rawkey(__htable, &open, sizeof(open));

	if (NULL == ctx) {
		JSON_ERROR("invalid format");
		return (-1);
	} else {
		f_parse = ctx->f_parse;
		close = ctx->close;
	}

	if (close != 0) {
		idx++;
	}
	while (ft_iseolws(s[idx])) {
		idx++;
	}

	rbytes = f_parse(s + idx, node);

	if (rbytes <= 0) {
		JSON_ERROR("invalid format");
		return (-1);
	} else {
		idx += rbytes;
	}

	if (close) {
		while (ft_iseolws(s[idx])) {
			idx++;
		}
		if (s[idx] != close) {
			JSON_ERROR("unexpected end of content");
			return (-1);
		} else {
			idx++;
		}
	}

	return (idx);
}

ssize_t __parse_number(const char *s, t_node *node)
{
    t_num   *num;
    ssize_t	idx;
    char    *sub_s;

    idx = 0;
    if (s[idx] == '-') {
        idx++;
    }
	if (!ft_isdigit(s[idx])) {
		JSON_ERROR("invalid format");
		return (-1);
	}
    while (ft_isdigit(s[idx])) {
		idx++;
    }
    sub_s = ft_strsub(s, 0, idx);
    num = bnum_create();
	bnum_from_dec(num, sub_s);

	node->content = num;
	node->size = sizeof(t_num);
	node->type = JSON_NUMBER;
	node->f_del = __delete_number;

	LIBFT_FREE(sub_s);

    return (idx);
}

ssize_t __parse_string(const char *s, t_node *node)
{
	ssize_t idx;

	idx = 0;
	while (s[idx] != '\0' && s[idx] != '"') {
		idx++;
	}
	node->content = ft_strsub(s, 0, idx);
	node->size = (size_t)idx;
	node->type = JSON_CSTR;
	node->f_del = __delete_string;

	return (idx);
}

ssize_t __parse_null(const char *s, t_node *node)
{
	const char	*s_null = "null";
	size_t	slen_null;
	size_t	idx;

	slen_null = ft_strlen(s_null);
	idx = 0;

	if (!ft_strncmp(s, s_null, slen_null)) {
		idx += slen_null;
	} else {
		JSON_ERROR("invalid format");
		return (-1);
	}
	node->content = NULL;
	node->size = 0;
	node->type = JSON_NULL;
	node->f_del = __delete_null;

	return (idx);
}

ssize_t __parse_boolean(const char *s, t_node *node)
{
	const char	*s_false = "false";
	const char	*s_true = "true";
	uint8_t	boolean;
	size_t	slen_false;
	size_t	slen_true;
	size_t	idx;

	slen_false = ft_strlen(s_false);
	slen_true = ft_strlen(s_true);
	idx = 0;

	if (!ft_strncmp(s, s_false, slen_false)) {
		boolean = 0u;
		idx += slen_false;
	} else if (!ft_strncmp(s, s_true, slen_true)) {
		boolean = 1u;
		idx += slen_true;
	} else {
		JSON_ERROR("invalid format");
		return (-1);
	}
	node->content = ft_memdup(&boolean, sizeof(boolean));
	node->size = sizeof(boolean);
	node->type = JSON_BOOLEAN;
	node->f_del = __delete_boolean;

	return (idx);
}

ssize_t __parse_object(const char *s, t_node *node)
{
	t_node	*key_node;
	t_node	*content_node;
	t_htbl	*htbl;
	ssize_t	rbytes;
	ssize_t idx;
	int		next;

	htbl = ft_htbl_init(0);
	idx = 0;

	do {
		key_node = ft_node_create();
		rbytes = __parse(s + idx, key_node);

		if (rbytes < 0) {
			JSON_ERROR("invalid format");
			goto err;
		}
		idx += rbytes;

		if (key_node->type != JSON_CSTR) {
			JSON_ERROR("invalid format");
			goto err;
		}

		while (ft_iseolws(s[idx])) {
			idx++;
		}
		if (s[idx] != ':') {
			JSON_ERROR("invalid format");
			goto err;
		} else {
			idx++;
		}

		content_node = ft_node_create();
		rbytes = __parse(s + idx, content_node);

		if (rbytes < 0) {
			JSON_ERROR("invalid format");
			goto err;
		}
		idx += rbytes;

		ft_htbl_add(htbl, content_node, (const char *)key_node->content);
		ft_node_del(key_node);

		while (ft_iseolws(s[idx])) {
			idx++;
		}
		if (s[idx] == ',') {
			next = 1;
			idx++;
		} else {
			next = 0;
		}
	} while (next);

	node->content = htbl;
	node->size = sizeof(t_htbl);
	node->type = JSON_OBJECT;
	node->f_del = __delete_object;

	return (idx);
err:
	ft_node_del(key_node);
	ft_node_del(content_node);
	ft_htbl_del(htbl);
	return (-1);
}

ssize_t __parse_array(const char *s, t_node *node)
{
	t_node	*list;
	t_node	*content_node;
	ssize_t	rbytes;
	ssize_t idx;
	int		next;

	list = NULL;
	idx = 0;

	do {
		while (ft_iseolws(s[idx])) {
			idx++;
		}

		content_node = ft_node_create();
		rbytes = __parse(s + idx, content_node);

		if (rbytes < 0) {
			JSON_ERROR("invalid format");
			goto err;
		}
		idx += rbytes;

		ft_lst_prepend(&list, content_node);

		while (ft_iseolws(s[idx])) {
			idx++;
		}
		if (s[idx] == ',') {
			next = 1;
			idx++;
		} else {
			next = 0;
		}
	} while (next);

	node->content = list;
	node->size = sizeof(t_node);
	node->type = JSON_ARRAY;
	node->f_del = __delete_array;

	return (idx);
err:
	ft_node_del(content_node);
	ft_lst_del(list, NULL);
	return (-1);
}

void	__delete_array(void *content)
{
	ft_lst_del((t_node *)(content), NULL);
}

void	__delete_object(void *content)
{
	ft_htbl_del((t_htbl *)(content));
}

void	__delete_string(void *content)
{
	LIBFT_FREE(content);
}

void	__delete_null(void *content)
{
	return ;
}

void	__delete_boolean(void *content)
{
	LIBFT_FREE(content);
}

void	__delete_number(void *content)
{
	bnum_del((t_num *)(content));
}