#include <json.h>
#include <bnum.h>
#include <libft/string.h>
#include <libft/htable.h>

static int	__f_default_dumper(t_node *node, t_ostring *ostring);
static int	__dumps_object(t_node *node, t_ostring *ostring);
static int	__dumps_array(t_node *node, t_ostring *ostring);
static int	__dumps_string(t_node *node, t_ostring *ostring);
static int	__dumps_number(t_node *node, t_ostring *ostring);
static int	__dumps_bytes(t_node *node, t_ostring *ostring);

char	*json_dumps(t_node *node)
{
	return (json_dumps_with_f_dumper(node, __f_default_dumper));
}

size_t	json_dumpb(t_node *node, char *buf, size_t size)
{
	return (json_dumpb_with_f_dumper(node, buf, size, __f_default_dumper));
}

char	*json_dumps_with_f_dumper(t_node *node, t_func_json_dump f_dumper)
{
	t_ostring	ostring;
	char		*s;

	ft_ostr_init(&ostring);

	if (SSL_OK != f_dumper(node, &ostring)) {
		ft_ostr_clear(&ostring);
		return (NULL);
	}
	s = ft_ostr_to_cstr(&ostring, 0, ostring.size);
	ft_ostr_clear(&ostring);

	return (s);
}

size_t	json_dumpb_with_f_dumper(t_node *node, char *buf, size_t size, t_func_json_dump f_dumper)
{
	char	*s;
	size_t	len;

	s = json_dumps_with_f_dumper(node, f_dumper);

	if (NULL == s) {
		ft_bzero(buf, size);
		return (0);
	}

	len = ft_strlen(s);
	if (len > size) {
		ft_strncpy(buf, s, size - 1);
		buf[size - 1] = '\0';
	} else {
		ft_strncpy(buf, s, len);
		buf[len] = '\0';
		ft_bzero(buf + len, size - len);
	}
	SSL_FREE(s);

	return (len);
}

static int	__f_default_dumper(t_node *node, t_ostring *ostring)
{
	if (NULL == node) {
		JSON_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	switch (node->type)
	{
		case JSON_TYPE_OBJECT:
			return __dumps_object(node, ostring);
		case JSON_TYPE_ARRAY:
			return __dumps_array(node, ostring);
		case JSON_TYPE_STRING:
			return __dumps_string(node, ostring);
		case JSON_TYPE_NUMBER:
			return __dumps_number(node, ostring);
		case JSON_TYPE_BOOL_TRUE:
			ft_ostr_append(ostring, "true", 4);
			return (SSL_OK);
		case JSON_TYPE_BOOL_FALSE:
			ft_ostr_append(ostring, "false", 5);
			return (SSL_OK);
		case JSON_TYPE_NULL:
			ft_ostr_append(ostring, "null", 4);
			return (SSL_OK);
		default:
			return __dumps_bytes(node, ostring);
	}
}

static int __dumps_object(t_node *node, t_ostring *ostring)
{
	t_htbl	*htbl;
	t_node	*cur_node;
	int		is_first;

	htbl = (t_htbl *)node->content;
	ft_ostr_append(ostring, "{", 1);

	is_first = 1;
	cur_node = NULL;
	while ((cur_node = ft_htbl_node_next(htbl, cur_node)) != NULL) {
		if (!is_first) {
			ft_ostr_append(ostring, ",", 1);
		}
		is_first = 0;

		if (cur_node->key == NULL) {
			JSON_LOG(ERROR, "unexpected missing key");
			return (SSL_ERR);
		}
		if (cur_node->content == NULL) {
			JSON_LOG(ERROR, "unexpected missing content");
			return (SSL_ERR);
		}

		ft_ostr_append(ostring, "\"", 1);
		ft_ostr_append(ostring, cur_node->key, ft_strlen(cur_node->key));
		ft_ostr_append(ostring, "\":", 2);

		if (SSL_OK != __f_default_dumper((t_node *)cur_node->content, ostring)) {
			return (SSL_ERR);
		}
	}
	ft_ostr_append(ostring, "}", 1);

	return (SSL_OK);
}

static int __dumps_array(t_node *node, t_ostring *ostring)
{
	t_node	*cur_node;
	int		is_first;

	cur_node = (t_node *)node->content;
	ft_ostr_append(ostring, "[", 1);

	is_first = 1;
	while (cur_node) {
		if (!is_first) {
			ft_ostr_append(ostring, ",", 1);
		}
		is_first = 0;

		if (SSL_OK != __f_default_dumper(cur_node, ostring)) {
			return (SSL_ERR);
		}
		cur_node = cur_node->next;
	}
	ft_ostr_append(ostring, "]", 1);

	return (SSL_OK);
}

static int __dumps_string(t_node *node, t_ostring *ostring)
{
	ft_ostr_append(ostring, "\"", 1);
	ft_ostr_append(ostring, (char *)node->content, node->size);
	ft_ostr_append(ostring, "\"", 1);

	return (SSL_OK);
}

static int __dumps_bytes(t_node *node, t_ostring *ostring)
{
	char	*s;

	ft_sprintf(&s, "type:<%s>,content:<ptr=%p,size=%zu>", json_get_type_name(node->type), (char *)node->content, node->size);
	ft_ostr_append(ostring, s, ft_strlen(s));
	SSL_FREE(s);

	return (SSL_OK);
}

static int __dumps_number(t_node *node, t_ostring *ostring)
{
	char *s;

	s = bnum_to_dec((t_num *)node->content);
	ft_ostr_append(ostring, s, ft_strlen(s));
	SSL_FREE(s);

	return (SSL_OK);
}