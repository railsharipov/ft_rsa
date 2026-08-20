#include <libft.h>
#include <common.h>
#include <logger.h>
#include "test.h"
#include <json.h>
#include <io.h>
#include <file.h>
#include <bnum.h>

static int	__test_json_setup(void);
static void	__test_json_cleanup(void);

static int	__test_json_parse_simple_string(void);
static int	__test_json_parse_simple_number(void);
static int	__test_json_parse_simple_boolean(void);
static int	__test_json_parse_simple_null(void);
static int	__test_json_parse_complex_object(void);
static int	__test_json_query_complex_object(void);
static int	__test_json_clone_complex_object(void);
static int	__test_json_map(void);

static const char	*__simple_null_json_file_path = "tests/files/json/simple-null.json";
static const char	*__simple_false_json_file_path = "tests/files/json/simple-false.json";
static const char	*__simple_true_json_file_path = "tests/files/json/simple-true.json";
static const char	*__simple_number_json_file_path = "tests/files/json/simple-number.json";
static const char	*__simple_string_json_file_path = "tests/files/json/simple-string.json";
static const char	*__complex_object_json_file_path = "tests/files/json/complex-object.json";
static const char	*__complex_object_no_ws_json_file_path = "tests/files/json/complex-object-no-ws.json";
static const char	*__complex_array_json_file_path = "tests/files/json/complex-array.json";

static t_ostring	__simple_null_json;
static t_ostring	__simple_false_json;
static t_ostring	__simple_true_json;
static t_ostring	__simple_number_json;
static t_ostring	__simple_string_json;
static t_ostring	__complex_object_json;
static t_ostring	__complex_array_json;
static t_ostring	__complex_object_no_ws_json;

int	test_json(void)
{
	int		ret;

	if (SSL_OK != __test_json_setup()) {
		TEST_LOG(ERROR, TEST_SETUP_ERROR);
		TEST_FAIL();
	}

	ret = __test_json_parse_simple_string()
		| __test_json_parse_simple_number()
		| __test_json_parse_simple_boolean()
		| __test_json_parse_simple_null()
		| __test_json_parse_complex_object()
		| __test_json_query_complex_object()
		| __test_json_clone_complex_object()
		| __test_json_map();

	__test_json_cleanup();

	return (ret);
}

static int	__test_json_setup(void)
{
	if (SSL_OK != file_read_all(__simple_null_json_file_path, &__simple_null_json)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != file_read_all(__simple_false_json_file_path, &__simple_false_json)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != file_read_all(__simple_true_json_file_path, &__simple_true_json)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != file_read_all(__simple_number_json_file_path, &__simple_number_json)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != file_read_all(__simple_string_json_file_path, &__simple_string_json)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != file_read_all(__complex_object_json_file_path, &__complex_object_json)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != file_read_all(__complex_array_json_file_path, &__complex_array_json)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != file_read_all(__complex_object_no_ws_json_file_path, &__complex_object_no_ws_json)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}

	return (SSL_OK);
}

static void	__test_json_cleanup(void)
{
	return ;
}

static int	__test_json_parse_simple_string(void)
{
	t_node	*node;
	char	*json_s, *ref_s;
	size_t	json_slen, ref_slen;
	int		ret;

	node = NULL;

	json_s = ft_ostr_to_cstr(&__simple_string_json, 0, __simple_string_json.size);
	json_slen = ft_strlen(json_s);
	assert(json_slen >= 2);
	assert(json_s[0] == '"');
	assert(json_s[json_slen-1] == '"');

	ret = json_parse(json_s, &node);
	TEST_ASSERT(SSL_OK == ret);

	ref_s = ft_strndup(json_s+1, json_slen-2);
	ref_slen = ft_strlen(ref_s);

	TEST_ASSERT(node->type == JSON_TYPE_STRING);
	TEST_ASSERT(node->size == ref_slen);
	TEST_ASSERT(node->content != NULL);
	TEST_ASSERT(ft_strcmp(node->content, ref_s) == 0);
	TEST_ASSERT(node->f_del_content == json_get_f_del(JSON_TYPE_STRING));

	SSL_FREE(json_s);
	json_del(node);

	TEST_PASS();
}

static int	__test_json_parse_simple_number(void)
{
	t_node	*node;
	char	*json_s;
	t_num	*ref_num;
	int		ret;

	node = NULL;

	json_s = ft_ostr_to_cstr(&__simple_number_json, 0, __simple_number_json.size);

	ret = json_parse(json_s, &node);
	TEST_ASSERT(SSL_OK == ret);
	TEST_ASSERT(node != NULL);

	ref_num = bnum_from_dec(json_s);

	TEST_ASSERT(node->type == JSON_TYPE_NUMBER);
	TEST_ASSERT(node->size == 0);
	TEST_ASSERT(node->content != NULL);
	TEST_ASSERT(bnum_cmp((t_num *)node->content, ref_num) == 0);
	TEST_ASSERT(node->f_del_content == json_get_f_del(JSON_TYPE_NUMBER));

	bnum_del(ref_num);
	SSL_FREE(json_s);
	json_del(node);

	TEST_PASS();
}

static int	__test_json_parse_simple_boolean(void)
{
	t_node	*node;
	char	*json_s;
	int		ret;

	// Test false boolean
	node = NULL;

	json_s = ft_ostr_to_cstr(&__simple_false_json, 0, __simple_false_json.size);

	ret = json_parse(json_s, &node);
	TEST_ASSERT(SSL_OK == ret);

	TEST_ASSERT(node->type == JSON_TYPE_BOOL_FALSE);
	TEST_ASSERT(node->f_del_content == json_get_f_del(JSON_TYPE_BOOL_FALSE));

	SSL_FREE(json_s);
	json_del(node);

	// Test true boolean
	node = NULL;

	json_s = ft_ostr_to_cstr(&__simple_true_json, 0, __simple_true_json.size);

	ret = json_parse(json_s, &node);
	TEST_ASSERT(SSL_OK == ret);

	TEST_ASSERT(node->type == JSON_TYPE_BOOL_TRUE);
	TEST_ASSERT(node->f_del_content == json_get_f_del(JSON_TYPE_BOOL_TRUE));

	SSL_FREE(json_s);
	json_del(node);

	TEST_PASS();
}

static int	__test_json_parse_simple_null(void)
{
	t_node	*node;
	char	*json_s;
	int		ret;

	json_s = ft_ostr_to_cstr(&__simple_null_json, 0, __simple_null_json.size);

	ret = json_parse(json_s, &node);
	TEST_ASSERT(SSL_OK == ret);

	TEST_ASSERT(node->type == JSON_TYPE_NULL);
	TEST_ASSERT(node->size == 0);
	TEST_ASSERT(node->content == NULL);
	TEST_ASSERT(node->f_del_content == json_get_f_del(JSON_TYPE_NULL));

	SSL_FREE(json_s);
	json_del(node);

	TEST_PASS();
}

static int	__test_json_parse_complex_object(void)
{
	t_node	*root_node, *node;
	char	*json_s;
	int		ret;

	root_node = NULL;

	json_s = ft_ostr_to_cstr(&__complex_object_json, 0, __complex_object_json.size);

	ret = json_parse(json_s, &root_node);
	TEST_ASSERT(SSL_OK == ret);
	TEST_ASSERT(root_node != NULL);
	TEST_ASSERT(root_node->type == JSON_TYPE_OBJECT);
	TEST_ASSERT(root_node->f_del_content == json_get_f_del(JSON_TYPE_OBJECT));

	root_node = ft_htbl_get(root_node->content, "/test/apiKey");
	TEST_ASSERT(root_node != NULL);
	TEST_ASSERT(root_node->type == JSON_TYPE_OBJECT);
	TEST_ASSERT(root_node->f_del_content == json_get_f_del(JSON_TYPE_OBJECT));

	root_node = ft_htbl_get(root_node->content, "post");
	TEST_ASSERT(root_node != NULL);
	TEST_ASSERT(root_node->type == JSON_TYPE_OBJECT);
	TEST_ASSERT(root_node->f_del_content == json_get_f_del(JSON_TYPE_OBJECT));

	node = ft_htbl_get(root_node->content, "tags");
	TEST_ASSERT(node != NULL);
	TEST_ASSERT(node->type == JSON_TYPE_ARRAY);
	TEST_ASSERT(ft_lst_size(node->content) == 1);
	TEST_ASSERT(node->f_del_content == json_get_f_del(JSON_TYPE_ARRAY));

	node = node->content;
	TEST_ASSERT(node->type == JSON_TYPE_STRING);
	TEST_ASSERT(node->f_del_content == json_get_f_del(JSON_TYPE_STRING));
	TEST_ASSERT(ft_strcmp(node->content, "test") == 0);

	node = ft_htbl_get(root_node->content, "summary");
	TEST_ASSERT(node->type == JSON_TYPE_STRING);
	TEST_ASSERT(node->f_del_content == json_get_f_del(JSON_TYPE_STRING));
	TEST_ASSERT(ft_strcmp(node->content, "Test JSON") == 0);

	node = ft_htbl_get(root_node->content, "description");
	TEST_ASSERT(node->type == JSON_TYPE_STRING);
	TEST_ASSERT(node->f_del_content == json_get_f_del(JSON_TYPE_STRING));
	TEST_ASSERT(ft_strcmp(node->content, "") == 0);

	node = ft_htbl_get(root_node->content, "operationId");
	TEST_ASSERT(node->type == JSON_TYPE_STRING);
	TEST_ASSERT(node->f_del_content == json_get_f_del(JSON_TYPE_STRING));
	TEST_ASSERT(ft_strcmp(node->content, "testJson") == 0);

	node = ft_htbl_get(root_node->content, "consumes");
	TEST_ASSERT(node->type == JSON_TYPE_ARRAY);
	TEST_ASSERT(ft_lst_size(node->content) == 1);
	TEST_ASSERT(node->f_del_content == json_get_f_del(JSON_TYPE_ARRAY));

	node = node->content;
	TEST_ASSERT(node->type == JSON_TYPE_STRING);
	TEST_ASSERT(node->f_del_content == json_get_f_del(JSON_TYPE_STRING));
	TEST_ASSERT(ft_strcmp(node->content, "application/json") == 0);

	node = ft_htbl_get(root_node->content, "produces");
	TEST_ASSERT(node->type == JSON_TYPE_ARRAY);
	TEST_ASSERT(ft_lst_size(node->content) == 1);
	TEST_ASSERT(node->f_del_content == json_get_f_del(JSON_TYPE_ARRAY));

	node = node->content;
	TEST_ASSERT(node->type == JSON_TYPE_STRING);
	TEST_ASSERT(node->f_del_content == json_get_f_del(JSON_TYPE_STRING));
	TEST_ASSERT(ft_strcmp(node->content, "application/json") == 0);

	root_node = ft_htbl_get(root_node->content, "parameters");
	TEST_ASSERT(root_node->type == JSON_TYPE_ARRAY);
	TEST_ASSERT(ft_lst_size(root_node->content) == 2);
	TEST_ASSERT(root_node->f_del_content == json_get_f_del(JSON_TYPE_ARRAY));

	root_node = root_node->content;
	TEST_ASSERT(root_node->type == JSON_TYPE_OBJECT);
	TEST_ASSERT(root_node->f_del_content == json_get_f_del(JSON_TYPE_OBJECT));

	node = ft_htbl_get(root_node->content, "name");
	TEST_ASSERT(node->type == JSON_TYPE_STRING);
	TEST_ASSERT(node->f_del_content == json_get_f_del(JSON_TYPE_STRING));
	TEST_ASSERT(ft_strcmp(node->content, "testId") == 0);

	node = ft_htbl_get(root_node->content, "in");
	TEST_ASSERT(node->type == JSON_TYPE_STRING);
	TEST_ASSERT(node->f_del_content == json_get_f_del(JSON_TYPE_STRING));
	TEST_ASSERT(ft_strcmp(node->content, "path") == 0);

	node = ft_htbl_get(root_node->content, "version");
	TEST_ASSERT(node->type == JSON_TYPE_NUMBER);
	TEST_ASSERT(node->f_del_content == json_get_f_del(JSON_TYPE_NUMBER));
	TEST_ASSERT(bnum_cmp((t_num *)node->content, bnum_from_dec("3")) == 0);

	node = ft_htbl_get(root_node->content, "size");
	TEST_ASSERT(node->type == JSON_TYPE_NUMBER);
	TEST_ASSERT(node->f_del_content == json_get_f_del(JSON_TYPE_NUMBER));
	TEST_ASSERT(bnum_cmp((t_num *)node->content, bnum_from_dec("36127812312")) == 0);

	node = ft_htbl_get(root_node->content, "value");
	TEST_ASSERT(node->type == JSON_TYPE_NUMBER);
	TEST_ASSERT(node->f_del_content == json_get_f_del(JSON_TYPE_NUMBER));
	TEST_ASSERT(bnum_cmp((t_num *)node->content, bnum_from_dec("-123123123123")) == 0);

	node = ft_htbl_get(root_node->content, "required");
	TEST_ASSERT(node->type == JSON_TYPE_BOOL_TRUE);
	TEST_ASSERT(node->f_del_content == json_get_f_del(JSON_TYPE_BOOL_TRUE));

	node = ft_htbl_get(root_node->content, "type");
	TEST_ASSERT(node->type == JSON_TYPE_STRING);
	TEST_ASSERT(node->f_del_content == json_get_f_del(JSON_TYPE_STRING));
	TEST_ASSERT(ft_strcmp(node->content, "integer") == 0);

	node = ft_htbl_get(root_node->content, "format");
	TEST_ASSERT(node->type == JSON_TYPE_STRING);
	TEST_ASSERT(node->f_del_content == json_get_f_del(JSON_TYPE_STRING));
	TEST_ASSERT(ft_strcmp(node->content, "int64") == 0);

	SSL_FREE(json_s);
	json_del(root_node);

	TEST_PASS();
}

static int	__test_json_query_complex_object(void)
{
	t_node	*json, *result;
	char	*json_s;

	json = NULL;
	result = NULL;

	json_s = ft_ostr_to_cstr(&__complex_object_json, 0, __complex_object_json.size);

	TEST_ASSERT(SSL_OK == json_parse(json_s, &json));
	TEST_ASSERT(SSL_OK == json_query("./test/apiKey.post.parameters[0].name", json, &result));
	TEST_ASSERT(result != NULL);
	TEST_ASSERT(result->type == JSON_TYPE_STRING);
	TEST_ASSERT(ft_strcmp(result->content, "testId") == 0);

	TEST_ASSERT(SSL_OK == json_query("['/test/apiKey'].post.parameters[0]", json, &result));
	TEST_ASSERT(result != NULL);
	TEST_ASSERT(result->type == JSON_TYPE_OBJECT);

	TEST_ASSERT(SSL_OK == json_query("['/test/apiKey'][\"post\"].parameters[0]['name']", json, &result));
	TEST_ASSERT(result != NULL);
	TEST_ASSERT(result->type == JSON_TYPE_STRING);
	TEST_ASSERT(ft_strcmp(result->content, "testId") == 0);

	TEST_ASSERT(SSL_OK == json_query(".", json, &result));
	TEST_ASSERT(result != NULL);
	TEST_ASSERT(result->type == JSON_TYPE_OBJECT);
	TEST_ASSERT(result == json);

	TEST_ASSERT(SSL_OK != json_query("['/test/apiKey'].post.parameters[0", json, &result));
	TEST_ASSERT(result == NULL);

	SSL_FREE(json_s);
	json_del(json);
	json_del(result);

	TEST_PASS();
}

static int	__test_json_clone_complex_object(void)
{
	t_node	*json, *cloned_json;
	t_node	*node, *cloned_node;
	char	*json_s;

	json = NULL;
	cloned_json = NULL;

	json_s = ft_ostr_to_cstr(&__complex_object_json, 0, __complex_object_json.size);

	TEST_ASSERT(SSL_OK == json_parse(json_s, &json));
	TEST_ASSERT(SSL_OK == json_clone(json, &cloned_json));

	TEST_ASSERT(SSL_OK == json_validate(json));
	TEST_ASSERT(SSL_OK == json_validate(cloned_json));

	TEST_ASSERT(SSL_OK == json_query("./test/apiKey", json, &node));
	TEST_ASSERT(SSL_OK == json_query("./test/apiKey", cloned_json, &cloned_node));
	TEST_ASSERT(node != NULL);
	TEST_ASSERT(cloned_node != NULL);
	TEST_ASSERT(node->type == cloned_node->type);

	TEST_ASSERT(SSL_OK == json_query("./test/apiKey.post", json, &node));
	TEST_ASSERT(SSL_OK == json_query("./test/apiKey.post", cloned_json, &cloned_node));
	TEST_ASSERT(node != NULL);
	TEST_ASSERT(cloned_node != NULL);
	TEST_ASSERT(node->type == cloned_node->type);

	TEST_ASSERT(SSL_OK == json_query("./test/apiKey.post.tags", json, &node));
	TEST_ASSERT(SSL_OK == json_query("./test/apiKey.post.tags", cloned_json, &cloned_node));
	TEST_ASSERT(node != NULL);
	TEST_ASSERT(cloned_node != NULL);
	TEST_ASSERT(node->type == cloned_node->type);
	TEST_ASSERT(ft_lst_size(node->content) == ft_lst_size(cloned_node->content));

	for (; node != NULL; node = node->next, cloned_node = cloned_node->next) {
		TEST_ASSERT(node->type == cloned_node->type);
		TEST_ASSERT(node->size == cloned_node->size);
	}

	TEST_ASSERT(SSL_OK == json_query("['/test/apiKey'].post.parameters", json, &node));
	TEST_ASSERT(SSL_OK == json_query("['/test/apiKey'].post.parameters", cloned_json, &cloned_node));
	TEST_ASSERT(node != NULL);
	TEST_ASSERT(cloned_node != NULL);
	TEST_ASSERT(node->type == cloned_node->type);
	TEST_ASSERT(node->size == cloned_node->size);
	TEST_ASSERT(cloned_node->type == JSON_TYPE_ARRAY);
	TEST_ASSERT(ft_lst_size(node->content) == ft_lst_size(cloned_node->content));

	TEST_ASSERT(SSL_OK == json_query("['/test/apiKey'].post.parameters[0]", json, &node));
	TEST_ASSERT(SSL_OK == json_query("['/test/apiKey'].post.parameters[0]", cloned_json, &cloned_node));
	TEST_ASSERT(node != NULL);
	TEST_ASSERT(cloned_node != NULL);
	TEST_ASSERT(node->type == cloned_node->type);
	TEST_ASSERT(node->size == cloned_node->size);
	TEST_ASSERT(cloned_node->type == JSON_TYPE_OBJECT);

	TEST_ASSERT(SSL_OK == json_query("['/test/apiKey'].post.parameters[0].size", json, &node));
	TEST_ASSERT(SSL_OK == json_query("['/test/apiKey'].post.parameters[0].size", cloned_json, &cloned_node));
	TEST_ASSERT(node != NULL);
	TEST_ASSERT(cloned_node != NULL);
	TEST_ASSERT(node->type == cloned_node->type);
	TEST_ASSERT(node->size == cloned_node->size);
	TEST_ASSERT(cloned_node->type == JSON_TYPE_NUMBER);
	TEST_ASSERT(bnum_cmp((t_num *)node->content, (t_num *)cloned_node->content) == 0);

	TEST_ASSERT(SSL_OK == json_query("['/test/apiKey'].post.parameters[0].name", json, &node));
	TEST_ASSERT(SSL_OK == json_query("['/test/apiKey'].post.parameters[0].name", cloned_json, &cloned_node));
	TEST_ASSERT(node != NULL);
	TEST_ASSERT(cloned_node != NULL);
	TEST_ASSERT(node->type == cloned_node->type);
	TEST_ASSERT(node->size == cloned_node->size);
	TEST_ASSERT(cloned_node->type == JSON_TYPE_STRING);
	TEST_ASSERT(ft_strcmp(node->content, cloned_node->content) == 0);

	TEST_ASSERT(SSL_OK == json_query("['/test/apiKey'].post.parameters[1].schema", json, &node));
	TEST_ASSERT(SSL_OK == json_query("['/test/apiKey'].post.parameters[1].schema", cloned_json, &cloned_node));
	TEST_ASSERT(node != NULL);
	TEST_ASSERT(cloned_node != NULL);
	TEST_ASSERT(node->type == cloned_node->type);
	TEST_ASSERT(node->size == cloned_node->size);
	TEST_ASSERT(cloned_node->type == JSON_TYPE_OBJECT);

	TEST_ASSERT(SSL_OK == json_query("['/test/apiKey'].post.parameters[1].required", json, &node));
	TEST_ASSERT(SSL_OK == json_query("['/test/apiKey'].post.parameters[1].required", cloned_json, &cloned_node));
	TEST_ASSERT(node != NULL);
	TEST_ASSERT(cloned_node != NULL);
	TEST_ASSERT(node->type == cloned_node->type);
	TEST_ASSERT(node->size == cloned_node->size);
	TEST_ASSERT(cloned_node->type == JSON_TYPE_BOOL_FALSE);
	TEST_ASSERT(node->content == cloned_node->content);

	TEST_ASSERT(SSL_OK == json_query("['/test/apiKey'].post.parameters[1].schema['$ref']", json, &node));
	TEST_ASSERT(SSL_OK == json_query("['/test/apiKey'].post.parameters[1].schema['$ref']", cloned_json, &cloned_node));
	TEST_ASSERT(node != NULL);
	TEST_ASSERT(cloned_node != NULL);
	TEST_ASSERT(node->type == cloned_node->type);
	TEST_ASSERT(node->size == cloned_node->size);
	TEST_ASSERT(cloned_node->type == JSON_TYPE_STRING);
	TEST_ASSERT(ft_strcmp(node->content, cloned_node->content) == 0);

	TEST_ASSERT(SSL_OK == json_query("['/test/apiKey'].post.responses['200'].description", json, &node));
	TEST_ASSERT(SSL_OK == json_query("['/test/apiKey'].post.responses['200'].description", cloned_json, &cloned_node));
	TEST_ASSERT(node != NULL);
	TEST_ASSERT(cloned_node != NULL);
	TEST_ASSERT(node->type == cloned_node->type);
	TEST_ASSERT(node->size == cloned_node->size);
	TEST_ASSERT(cloned_node->type == JSON_TYPE_STRING);
	TEST_ASSERT(ft_strcmp(node->content, cloned_node->content) == 0);

	TEST_ASSERT(SSL_OK == json_query("['/test/apiKey'].post.responses['200'].schema.type", json, &node));
	TEST_ASSERT(SSL_OK == json_query("['/test/apiKey'].post.responses['200'].schema.type", cloned_json, &cloned_node));
	TEST_ASSERT(node != NULL);
	TEST_ASSERT(cloned_node != NULL);
	TEST_ASSERT(node->type == cloned_node->type);
	TEST_ASSERT(node->size == cloned_node->size);
	TEST_ASSERT(cloned_node->type == JSON_TYPE_STRING);
	TEST_ASSERT(ft_strcmp(node->content, cloned_node->content) == 0);

	json_del(json);
	json_del(cloned_json);
	SSL_FREE(json_s);

	TEST_PASS();
}

static int	__test_json_map_f_map(t_node *node)
{
	void *content;

	switch (node->type) {
		case JSON_TYPE_STRING:
			content = ft_strup(ft_strdup(node->content));
			break;
		case JSON_TYPE_NUMBER:
			content = bnum_to_dec((t_num *)node->content);
			break;
		case JSON_TYPE_BOOL_TRUE:
			content = ft_strdup("true");
			break;
		case JSON_TYPE_BOOL_FALSE:
			content = ft_strdup("false");
			break;
		case JSON_TYPE_NULL:
			content = ft_strdup("null");
			break;
		default:
			return (SSL_OK);
	}

	node->type = JSON_TYPE_STRING;
	node->content = content;
	node->size = ft_strlen(content);
	node->f_del_content = json_get_f_del(JSON_TYPE_STRING);

	return (SSL_OK);
}

static int	__test_json_check_mapping_result(t_node *src, t_node *res)
{
	switch (src->type) {
		case JSON_TYPE_STRING:
			return (res->type == JSON_TYPE_STRING && ft_strcmp(res->content, ft_strup(src->content)) == 0);
		case JSON_TYPE_NUMBER:
			return (res->type == JSON_TYPE_STRING && ft_strcmp(res->content, bnum_to_dec((t_num *)src->content)) == 0);
		case JSON_TYPE_BOOL_TRUE:
			return (res->type == JSON_TYPE_STRING && ft_strcmp(res->content, "true") == 0);
		case JSON_TYPE_BOOL_FALSE:
			return (res->type == JSON_TYPE_STRING && ft_strcmp(res->content, "false") == 0);
		case JSON_TYPE_NULL:
			return (res->type == JSON_TYPE_STRING && ft_strcmp(res->content, "null") == 0);
		case JSON_TYPE_OBJECT:
		case JSON_TYPE_ARRAY:
			return (res->type == src->type);
		default:
			return (0);
	}
}

static int	__test_json_map(void)
{
	t_node	*json, *mapped_json;
	t_node	*node, *mapped_node;
	char	*json_s;

	mapped_json = NULL;

	json_s = ft_ostr_to_cstr(&__complex_object_json, 0, __complex_object_json.size);

	TEST_ASSERT(SSL_OK == json_parse(json_s, &json));
	TEST_ASSERT(SSL_OK == json_map(json, __test_json_map_f_map, &mapped_json));

	TEST_ASSERT(SSL_OK == json_query("./test/apiKey.post.tags[0]", json, &node));
	TEST_ASSERT(SSL_OK == json_query("./test/apiKey.post.tags[0]", mapped_json, &mapped_node));
	TEST_ASSERT(node != NULL);
	TEST_ASSERT(mapped_node != NULL);
	TEST_ASSERT(__test_json_check_mapping_result(node, mapped_node));

	TEST_ASSERT(SSL_OK == json_query("['/test/apiKey'].post.parameters", json, &node));
	TEST_ASSERT(SSL_OK == json_query("['/test/apiKey'].post.parameters", mapped_json, &mapped_node));
	TEST_ASSERT(node != NULL);
	TEST_ASSERT(mapped_node != NULL);
	TEST_ASSERT(__test_json_check_mapping_result(node, mapped_node));

	TEST_ASSERT(SSL_OK == json_query("['/test/apiKey'].post.parameters[0].size", json, &node));
	TEST_ASSERT(SSL_OK == json_query("['/test/apiKey'].post.parameters[0].size", mapped_json, &mapped_node));
	TEST_ASSERT(node != NULL);
	TEST_ASSERT(mapped_node != NULL);
	TEST_ASSERT(__test_json_check_mapping_result(node, mapped_node));

	TEST_ASSERT(SSL_OK == json_query("['/test/apiKey'].post.parameters[0].name", json, &node));
	TEST_ASSERT(SSL_OK == json_query("['/test/apiKey'].post.parameters[0].name", mapped_json, &mapped_node));
	TEST_ASSERT(node != NULL);
	TEST_ASSERT(mapped_node != NULL);
	TEST_ASSERT(__test_json_check_mapping_result(node, mapped_node));

	TEST_ASSERT(SSL_OK == json_query("['/test/apiKey'].post.parameters[1].schema", json, &node));
	TEST_ASSERT(SSL_OK == json_query("['/test/apiKey'].post.parameters[1].schema", mapped_json, &mapped_node));
	TEST_ASSERT(node != NULL);
	TEST_ASSERT(mapped_node != NULL);
	TEST_ASSERT(__test_json_check_mapping_result(node, mapped_node));

	TEST_ASSERT(SSL_OK == json_query("['/test/apiKey'].post.parameters[1].required", json, &node));
	TEST_ASSERT(SSL_OK == json_query("['/test/apiKey'].post.parameters[1].required", mapped_json, &mapped_node));
	TEST_ASSERT(node != NULL);
	TEST_ASSERT(mapped_node != NULL);
	TEST_ASSERT(__test_json_check_mapping_result(node, mapped_node));

	TEST_ASSERT(SSL_OK == json_query("['/test/apiKey'].post.parameters[1].schema['$ref']", json, &node));
	TEST_ASSERT(SSL_OK == json_query("['/test/apiKey'].post.parameters[1].schema['$ref']", mapped_json, &mapped_node));
	TEST_ASSERT(node != NULL);
	TEST_ASSERT(mapped_node != NULL);
	TEST_ASSERT(__test_json_check_mapping_result(node, mapped_node));

	TEST_ASSERT(SSL_OK == json_query("['/test/apiKey'].post.responses['200'].description", json, &node));
	TEST_ASSERT(SSL_OK == json_query("['/test/apiKey'].post.responses['200'].description", mapped_json, &mapped_node));
	TEST_ASSERT(node != NULL);
	TEST_ASSERT(mapped_node != NULL);
	TEST_ASSERT(__test_json_check_mapping_result(node, mapped_node));

	TEST_ASSERT(SSL_OK == json_query("['/test/apiKey'].post.responses['200'].schema.type", json, &node));
	TEST_ASSERT(SSL_OK == json_query("['/test/apiKey'].post.responses['200'].schema.type", mapped_json, &mapped_node));
	TEST_ASSERT(node != NULL);
	TEST_ASSERT(mapped_node != NULL);
	TEST_ASSERT(__test_json_check_mapping_result(node, mapped_node));

	json_del(json);
	json_del(mapped_json);
	SSL_FREE(json_s);

	TEST_PASS();
}
