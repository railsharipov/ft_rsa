#include <libft/list.h>
#include <ssl.h>
#include <test.h>
#include <json.h>
#include <io.h>
#include <bnum.h>

static int	__test_json_setup(void);
static void	__test_json_cleanup(void);

static int	__test_json_query(void);
static int	__test_json_parse_simple_string(void);
static int	__test_json_parse_simple_number(void);
static int	__test_json_parse_simple_boolean(void);
static int	__test_json_parse_simple_null(void);
static int	__test_json_parse_complex_object_no_ws(void);
static const char	*__simple_null_json_file_path = "test/testfiles/json/simple-null.json";
static const char	*__simple_false_json_file_path = "test/testfiles/json/simple-false.json";
static const char	*__simple_true_json_file_path = "test/testfiles/json/simple-true.json";
static const char	*__simple_number_json_file_path = "test/testfiles/json/simple-number.json";
static const char	*__simple_string_json_file_path = "test/testfiles/json/simple-string.json";
static const char	*__complex_object_json_file_path = "test/testfiles/json/complex-object.json";
static const char	*__complex_object_no_ws_json_file_path = "test/testfiles/json/complex-object-no-ws.json";
static const char	*__complex_array_json_file_path = "test/testfiles/json/complex-array.json";

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
	int	res;

	if (SSL_OK != __test_json_setup()) {
		return (TEST_LOG(ERROR, UNSPECIFIED_ERROR));
	}
	res = SSL_OK;

	res |= __test_json_query();
	res |= __test_json_parse_simple_string();
	res |= __test_json_parse_simple_number();
	res |= __test_json_parse_simple_boolean();
	res |= __test_json_parse_simple_null();
	res |= __test_json_parse_complex_object_no_ws();

	__test_json_cleanup();

	return (res);
}

static int	__test_json_setup(void)
{
	if (SSL_OK != test_get_file_content(__simple_null_json_file_path, &__simple_null_json)) {
		return (TEST_LOG(ERROR, UNSPECIFIED_ERROR));
	}
	if (SSL_OK != test_get_file_content(__simple_false_json_file_path, &__simple_false_json)) {
		return (TEST_LOG(ERROR, UNSPECIFIED_ERROR));
	}
	if (SSL_OK != test_get_file_content(__simple_true_json_file_path, &__simple_true_json)) {
		return (TEST_LOG(ERROR, UNSPECIFIED_ERROR));
	}
	if (SSL_OK != test_get_file_content(__simple_number_json_file_path, &__simple_number_json)) {
		return (TEST_LOG(ERROR, UNSPECIFIED_ERROR));
	}
	if (SSL_OK != test_get_file_content(__simple_string_json_file_path, &__simple_string_json)) {
		return (TEST_LOG(ERROR, UNSPECIFIED_ERROR));
	}
	if (SSL_OK != test_get_file_content(__complex_object_json_file_path, &__complex_object_json)) {
		return (TEST_LOG(ERROR, UNSPECIFIED_ERROR));
	}
	if (SSL_OK != test_get_file_content(__complex_array_json_file_path, &__complex_array_json)) {
		return (TEST_LOG(ERROR, UNSPECIFIED_ERROR));
	}
	if (SSL_OK != test_get_file_content(__complex_object_no_ws_json_file_path, &__complex_object_no_ws_json)) {
		return (TEST_LOG(ERROR, UNSPECIFIED_ERROR));
	}

	return (SSL_OK);
}

static void	__test_json_cleanup(void)
{
	return ;
}

static int __test_json_query(void)
{
	t_node *json_lst;
	t_node *json_node;
	t_num *num;
	int ret;
	int pass;

	pass = SSL_OK;
	json_lst = NULL;

	num = bnum_from_dec("0");

	json_node = ft_node_new_with_f_del(NULL, num, sizeof(*num), json_get_f_del(JSON_NUMBER));
	ft_lst_append(&json_lst, json_node);

	json_node = ft_node_new_with_f_del(NULL, "\"apple\"", 5, json_get_f_del(JSON_CSTR));
	ft_lst_append(&json_lst, json_node);

	json_node = ft_node_new_with_f_del(NULL, "null", 4, json_get_f_del(JSON_NULL));
	ft_lst_append(&json_lst, json_node);

	json_node = ft_node_new_with_f_del(NULL, "true", 4, json_get_f_del(JSON_BOOLEAN));
	ft_lst_append(&json_lst, json_node);

	char *json_s = "[ 0, 'apple', null, true ]";
	size_t json_slen;

	// json_s = ft_ostr_to_cstr(&__complex_array_json, 0, __complex_array_json.size);
	// json_slen = ft_strlen(json_s);

	// assert(json_slen >= 2);
	// assert(json_s[0] == '[');
	// assert(json_s[json_slen - 1] == ']');

	

	return (TEST_PASS());
}

static int	__test_json_parse_simple_string(void)
{
	t_node	*node;
	char	*json_s, *ref_s;
	size_t	json_slen, ref_slen;
	int		ret;
	int		pass;

	pass = SSL_OK;
	node = NULL;

	json_s = ft_ostr_to_cstr(&__simple_string_json, 0, __simple_string_json.size);
	json_slen = ft_strlen(json_s);
	assert(json_slen >= 2);
	assert(json_s[0] == '"');
	assert(json_s[json_slen-1] == '"');

	ret = json_parse(json_s, &node);
	pass |= TEST_ASSERT(SSL_OK == ret);

	ref_s = ft_strndup(json_s+1, json_slen-2);
	ref_slen = ft_strlen(ref_s);

	pass |= TEST_ASSERT(node->type == JSON_CSTR);
	pass |= TEST_ASSERT(node->size == ref_slen);
	pass |= TEST_ASSERT(node->content != NULL);
	pass |= TEST_ASSERT(ft_strcmp(node->content, ref_s) == 0);
	pass |= TEST_ASSERT(node->f_del_content != NULL);

	SSL_FREE(json_s);
	json_del(node);

	if (SSL_OK == pass) {
		return (TEST_PASS());
	}

	return (TEST_FAIL());
}

static int	__test_json_parse_simple_number(void)
{
	t_node	*node;
	char	*json_s;
	size_t	json_slen;
	t_num	*ref_num;
	int		ret;
	int		pass;

	pass = SSL_OK;
	node = NULL;

	json_s = ft_ostr_to_cstr(&__simple_number_json, 0, __simple_number_json.size);
	json_slen = ft_strlen(json_s);

	ret = json_parse(json_s, &node);
	pass |= TEST_ASSERT(SSL_OK == ret);

	ref_num = bnum_from_dec(json_s);

	pass |= TEST_ASSERT(node->type == JSON_NUMBER);
	pass |= TEST_ASSERT(node->size == sizeof(t_num));
	pass |= TEST_ASSERT(node->content != NULL);
	pass |= TEST_ASSERT(bnum_cmp((t_num *)node->content, ref_num) == 0);
	pass |= TEST_ASSERT(node->f_del_content != NULL);

	bnum_del(ref_num);
	SSL_FREE(json_s);
	json_del(node);

	if (SSL_OK == pass) {
		return (TEST_PASS());
	}

	return (TEST_FAIL());
}

static int	__test_json_parse_simple_boolean(void)
{
	t_node	*node;
	char	*json_s;
	size_t	json_slen;
	int		ret;
	int		pass;

	pass = SSL_OK;

	// Test false boolean
	node = NULL;

	json_s = ft_ostr_to_cstr(&__simple_false_json, 0, __simple_false_json.size);
	json_slen = ft_strlen(json_s);

	ret = json_parse(json_s, &node);
	pass |= TEST_ASSERT(SSL_OK == ret);

	if (SSL_OK != ret) {
		return (TEST_FAIL());
	}

	pass |= TEST_ASSERT(node->type == JSON_BOOLEAN);
	pass |= TEST_ASSERT(node->size == sizeof(uint8_t));
	pass |= TEST_ASSERT(node->content != NULL);
	pass |= TEST_ASSERT(*(uint8_t *)node->content == 0u);
	pass |= TEST_ASSERT(node->f_del_content != NULL);

	SSL_FREE(json_s);
	json_del(node);

	// Test true boolean
	node = NULL;

	json_s = ft_ostr_to_cstr(&__simple_true_json, 0, __simple_true_json.size);
	json_slen = ft_strlen(json_s);

	ret = json_parse(json_s, &node);
	pass |= TEST_ASSERT(SSL_OK == ret);

	if (SSL_OK != ret) {
		return (TEST_FAIL());
	}

	pass |= TEST_ASSERT(node->type == JSON_BOOLEAN);
	pass |= TEST_ASSERT(node->size == sizeof(uint8_t));
	pass |= TEST_ASSERT(node->content != NULL);
	pass |= TEST_ASSERT(*(uint8_t *)node->content == 1u);
	pass |= TEST_ASSERT(node->f_del_content != NULL);

	SSL_FREE(json_s);
	json_del(node);

	if (SSL_OK == pass) {
		return (TEST_PASS());
	}

	return (TEST_FAIL());
}

static int	__test_json_parse_simple_null(void)
{
	t_node	*node;
	char	*json_s;
	size_t	json_slen;
	int		ret;
	int		pass;

	pass = SSL_OK;

	json_s = ft_ostr_to_cstr(&__simple_null_json, 0, __simple_null_json.size);
	json_slen = ft_strlen(json_s);

	ret = json_parse(json_s, &node);
	pass |= TEST_ASSERT(SSL_OK == ret);

	if (SSL_OK != ret) {
		return (TEST_FAIL());
	}

	pass |= TEST_ASSERT(node->type == JSON_NULL);
	pass |= TEST_ASSERT(node->size == 0);
	pass |= TEST_ASSERT(node->content == NULL);
	pass |= TEST_ASSERT(node->f_del_content != NULL);

	SSL_FREE(json_s);
	json_del(node);

	if (SSL_OK == pass) {
		return (TEST_PASS());
	}

	return (TEST_FAIL());
}

static int	__test_json_parse_complex_object_no_ws(void)
{
	t_node	*node;
	char	*json_s;
	size_t	json_slen;
	int		ret;
	int		pass;

	pass = SSL_OK;
	node = NULL;

	json_s = ft_ostr_to_cstr(&__complex_object_no_ws_json, 0, __complex_object_no_ws_json.size);
	json_slen = ft_strlen(json_s);

	ret = json_parse_new(json_s, &node);
	pass |= TEST_ASSERT(SSL_OK == ret);

	SSL_FREE(json_s);
	json_del(node);

	if (SSL_OK == pass) {
		return (TEST_PASS());
	}

	return (TEST_FAIL());
}
