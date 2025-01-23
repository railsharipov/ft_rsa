#include <libft/list.h>
#include <libft/tuple.h>
#include <common.h>
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
static int	__test_json_parse_complex_object(void);

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
	int		ret;

	if (SSL_OK != __test_json_setup()) {
		TEST_LOG(ERROR, UNSPECIFIED_ERROR);
		TEST_FAIL();
	}

	ret = __test_json_query()
		| __test_json_parse_simple_string()
		| __test_json_parse_simple_number()
		| __test_json_parse_simple_boolean()
		| __test_json_parse_simple_null()
		| __test_json_parse_complex_object();

	__test_json_cleanup();

	return (ret);
}

static int	__test_json_setup(void)
{
	if (SSL_OK != test_get_file_content(__simple_null_json_file_path, &__simple_null_json)) {
		TEST_LOG(ERROR, UNSPECIFIED_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != test_get_file_content(__simple_false_json_file_path, &__simple_false_json)) {
		TEST_LOG(ERROR, UNSPECIFIED_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != test_get_file_content(__simple_true_json_file_path, &__simple_true_json)) {
		TEST_LOG(ERROR, UNSPECIFIED_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != test_get_file_content(__simple_number_json_file_path, &__simple_number_json)) {
		TEST_LOG(ERROR, UNSPECIFIED_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != test_get_file_content(__simple_string_json_file_path, &__simple_string_json)) {
		TEST_LOG(ERROR, UNSPECIFIED_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != test_get_file_content(__complex_object_json_file_path, &__complex_object_json)) {
		TEST_LOG(ERROR, UNSPECIFIED_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != test_get_file_content(__complex_array_json_file_path, &__complex_array_json)) {
		TEST_LOG(ERROR, UNSPECIFIED_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != test_get_file_content(__complex_object_no_ws_json_file_path, &__complex_object_no_ws_json)) {
		TEST_LOG(ERROR, UNSPECIFIED_ERROR);
		return (SSL_ERR);
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

	TEST_PASS();
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

	TEST_ASSERT(node->type == JSON_CSTR);
	TEST_ASSERT(node->size == ref_slen);
	TEST_ASSERT(node->content != NULL);
	TEST_ASSERT(ft_strcmp(node->content, ref_s) == 0);
	TEST_ASSERT(node->f_del_content != NULL);

	SSL_FREE(json_s);
	json_del(node);

	TEST_PASS();
}

static int	__test_json_parse_simple_number(void)
{
	t_node	*node;
	char	*json_s;
	size_t	json_slen;
	t_num	*ref_num;
	int		ret;

	node = NULL;

	json_s = ft_ostr_to_cstr(&__simple_number_json, 0, __simple_number_json.size);
	json_slen = ft_strlen(json_s);

	ret = json_parse(json_s, &node);
	TEST_ASSERT(SSL_OK == ret);
	TEST_ASSERT(node != NULL);

	ref_num = bnum_from_dec(json_s);

	TEST_ASSERT(node->type == JSON_NUMBER);
	TEST_ASSERT(node->size == sizeof(t_num));
	TEST_ASSERT(node->content != NULL);
	TEST_ASSERT(bnum_cmp((t_num *)node->content, ref_num) == 0);
	TEST_ASSERT(node->f_del_content != NULL);

	bnum_del(ref_num);
	SSL_FREE(json_s);
	json_del(node);

	TEST_PASS();
}

static int	__test_json_parse_simple_boolean(void)
{
	t_node	*node;
	char	*json_s;
	size_t	json_slen;
	int		ret;

	// Test false boolean
	node = NULL;

	json_s = ft_ostr_to_cstr(&__simple_false_json, 0, __simple_false_json.size);
	json_slen = ft_strlen(json_s);

	ret = json_parse(json_s, &node);
	TEST_ASSERT(SSL_OK == ret);



	TEST_ASSERT(node->type == JSON_BOOLEAN);
	TEST_ASSERT(node->size == 5);
	TEST_ASSERT(node->content != NULL);
	TEST_ASSERT(ft_strcmp(node->content, "false") == 0);
	TEST_ASSERT(node->f_del_content != NULL);

	SSL_FREE(json_s);
	json_del(node);

	// Test true boolean
	node = NULL;

	json_s = ft_ostr_to_cstr(&__simple_true_json, 0, __simple_true_json.size);
	json_slen = ft_strlen(json_s);

	ret = json_parse(json_s, &node);
	TEST_ASSERT(SSL_OK == ret);



	TEST_ASSERT(node->type == JSON_BOOLEAN);
	TEST_ASSERT(node->size == 4);
	TEST_ASSERT(node->content != NULL);
	TEST_ASSERT(ft_strcmp(node->content, "true") == 0);
	TEST_ASSERT(node->f_del_content != NULL);

	SSL_FREE(json_s);
	json_del(node);

	TEST_PASS();
}

static int	__test_json_parse_simple_null(void)
{
	t_node	*node;
	char	*json_s;
	size_t	json_slen;
	int		ret;

	json_s = ft_ostr_to_cstr(&__simple_null_json, 0, __simple_null_json.size);
	json_slen = ft_strlen(json_s);

	ret = json_parse(json_s, &node);
	TEST_ASSERT(SSL_OK == ret);



	TEST_ASSERT(node->type == JSON_NULL);
	TEST_ASSERT(node->size == 0);
	TEST_ASSERT(node->content == NULL);
	TEST_ASSERT(node->f_del_content != NULL);

	SSL_FREE(json_s);
	json_del(node);

	TEST_PASS();
}

static int	__test_json_parse_complex_object(void)
{
	t_node	*root_node, *obj_node;
	t_node	*kv_node, *k, *v;
	t_node	*array_node, *array_item;
	t_tuple	*tuple;
	char	*json_s;
	size_t	json_slen;
	int		ret;

	root_node = NULL;

	json_s = ft_ostr_to_cstr(&__complex_object_json, 0, __complex_object_json.size);
	json_slen = ft_strlen(json_s);

    /*
	{
      "/test/apiKey" : {
        "post" : {
          "tags" : [ "test" ],
          "summary" : "Test JSON",
          "description" : "",
          "operationId" : "testJson",
          "consumes" : [ "application/json" ],
          "produces" : [ "application/json" ],
          "parameters" : [
            {
              "name" : "testId",
              "in" : "path",
              "version" : 3,
              "size" : 36127812312,
              "value" : -123123123123,
              "required" : true,
              "type" : "integer",
              "format" : "int64"
            }, {
              "in" : "body",
              "name" : "body",
              "required" : false,
              "schema" : {
                "$ref" : "#/test/json"
              }
            }
          ],
          "responses" : {
            "200" : {
              "description" : "Success",
              "schema" : {
                "type" : "string"
              }
            },
            "400" : {
              "description" : "Missing data in request"
            },
            "500" : {
              "description" : "Something went wrong"
            }
          }
        }
      }
    }
	*/

	ret = json_parse(json_s, &root_node);
	TEST_ASSERT(SSL_OK == ret);
	TEST_ASSERT(root_node != NULL);

	obj_node = root_node;
	TEST_ASSERT(obj_node->type == JSON_OBJECT);

	kv_node = obj_node->content;
	TEST_ASSERT(kv_node->type == JSON_KV);

	tuple = kv_node->content;
	k = ft_tuple_get(tuple, 0);
	v = ft_tuple_get(tuple, 1);

	TEST_ASSERT(k->type == JSON_CSTR);
	TEST_ASSERT(ft_strcmp(k->content, "/test/apiKey") == 0);
	TEST_ASSERT(v->type == JSON_OBJECT);

	kv_node = v->content;
	TEST_ASSERT(kv_node->type == JSON_KV);

	tuple = kv_node->content;
	k = ft_tuple_get(tuple, 0);
	v = ft_tuple_get(tuple, 1);

	TEST_ASSERT(k->type == JSON_CSTR);
	TEST_ASSERT(ft_strcmp(k->content, "post") == 0);
	TEST_ASSERT(v->type == JSON_OBJECT);

	kv_node = v->content;
	TEST_ASSERT(kv_node->type == JSON_KV);

	TEST_ASSERT(ft_lst_size(kv_node) == 8);

	tuple = kv_node->content;
	k = ft_tuple_get(tuple, 0);
	v = ft_tuple_get(tuple, 1);

	TEST_ASSERT(k->type == JSON_CSTR);

	TEST_LOG(INFO, "k->content: %s", k->content);

	TEST_ASSERT(ft_strcmp(k->content, "tags") == 0);
	TEST_ASSERT(v->type == JSON_ARRAY);

	TEST_ASSERT(ft_lst_size(v->content) == 1);

	array_item = v->content;
	TEST_ASSERT(array_item->type == JSON_CSTR);
	TEST_ASSERT(ft_strcmp(array_item->content, "test") == 0);

	kv_node = kv_node->next;
	TEST_ASSERT(kv_node->type == JSON_KV);

	tuple = kv_node->content;
	k = ft_tuple_get(tuple, 0);
	v = ft_tuple_get(tuple, 1);

	TEST_ASSERT(k->type == JSON_CSTR);
	TEST_ASSERT(ft_strcmp(k->content, "summary") == 0);
	TEST_ASSERT(v->type == JSON_CSTR);
	TEST_ASSERT(ft_strcmp(v->content, "Test JSON") == 0);

	kv_node = kv_node->next;
	TEST_ASSERT(kv_node->type == JSON_KV);

	TEST_ASSERT(k->type == JSON_CSTR);
	TEST_ASSERT(ft_strcmp(k->content, "description") == 0);
	TEST_ASSERT(v->type == JSON_CSTR);
	TEST_ASSERT(ft_strcmp(v->content, "") == 0);

	kv_node = kv_node->next;
	TEST_ASSERT(kv_node->type == JSON_KV);

	tuple = kv_node->content;
	k = ft_tuple_get(tuple, 0);
	v = ft_tuple_get(tuple, 1);

	TEST_ASSERT(k->type == JSON_CSTR);
	TEST_ASSERT(ft_strcmp(k->content, "operationId") == 0);
	TEST_ASSERT(v->type == JSON_CSTR);
	TEST_ASSERT(ft_strcmp(v->content, "testJson") == 0);

	kv_node = kv_node->next;
	TEST_ASSERT(kv_node->type == JSON_KV);

	tuple = kv_node->content;
	k = ft_tuple_get(tuple, 0);
	v = ft_tuple_get(tuple, 1);

	TEST_ASSERT(k->type == JSON_CSTR);
	TEST_ASSERT(ft_strcmp(k->content, "consumes") == 0);
	TEST_ASSERT(v->type == JSON_ARRAY);

	TEST_ASSERT(ft_lst_size(v->content) == 1);

	array_item = v->content;
	TEST_ASSERT(array_item->type == JSON_CSTR);
	TEST_ASSERT(ft_strcmp(array_item->content, "application/json") == 0);

	kv_node = kv_node->next;
	TEST_ASSERT(kv_node->type == JSON_KV);

	tuple = kv_node->content;
	k = ft_tuple_get(tuple, 0);
	v = ft_tuple_get(tuple, 1);

	TEST_ASSERT(k->type == JSON_CSTR);
	TEST_ASSERT(ft_strcmp(k->content, "produces") == 0);
	TEST_ASSERT(v->type == JSON_ARRAY);

	TEST_ASSERT(ft_lst_size(v->content) == 1);

	array_item = v->content;
	TEST_ASSERT(array_item->type == JSON_CSTR);
	TEST_ASSERT(ft_strcmp(array_item->content, "application/json") == 0);

	kv_node = kv_node->next;
	TEST_ASSERT(kv_node->type == JSON_KV);

	tuple = kv_node->content;
	k = ft_tuple_get(tuple, 0);
	v = ft_tuple_get(tuple, 1);

	TEST_ASSERT(k->type == JSON_CSTR);
	TEST_ASSERT(ft_strcmp(k->content, "parameters") == 0);
	TEST_ASSERT(v->type == JSON_ARRAY);
	TEST_ASSERT(ft_lst_size(v->content) == 2);

	array_item = v->content;
	obj_node = array_item;

	TEST_ASSERT(obj_node->type == JSON_OBJECT);

	kv_node = obj_node->content;
	TEST_ASSERT(kv_node->type == JSON_KV);

	tuple = kv_node->content;
	k = ft_tuple_get(tuple, 0);
	v = ft_tuple_get(tuple, 1);

	TEST_ASSERT(k->type == JSON_CSTR);
	TEST_ASSERT(ft_strcmp(k->content, "name") == 0);
	TEST_ASSERT(v->type == JSON_CSTR);
	TEST_ASSERT(ft_strcmp(v->content, "testId") == 0);

	kv_node = kv_node->next;
	TEST_ASSERT(kv_node->type == JSON_KV);

	tuple = kv_node->content;
	k = ft_tuple_get(tuple, 0);
	v = ft_tuple_get(tuple, 1);

	TEST_ASSERT(k->type == JSON_CSTR);
	TEST_ASSERT(ft_strcmp(k->content, "in") == 0);
	TEST_ASSERT(v->type == JSON_CSTR);
	TEST_ASSERT(ft_strcmp(v->content, "path") == 0);

	kv_node = kv_node->next;
	TEST_ASSERT(kv_node->type == JSON_KV);

	tuple = kv_node->content;
	k = ft_tuple_get(tuple, 0);
	v = ft_tuple_get(tuple, 1);

	TEST_ASSERT(k->type == JSON_CSTR);
	TEST_ASSERT(ft_strcmp(k->content, "version") == 0);
	TEST_ASSERT(v->type == JSON_NUMBER);
	TEST_ASSERT(bnum_cmp((t_num *)v->content, bnum_from_dec("3")) == 0);

	kv_node = kv_node->next;
	TEST_ASSERT(kv_node->type == JSON_KV);

	tuple = kv_node->content;
	k = ft_tuple_get(tuple, 0);
	v = ft_tuple_get(tuple, 1);

	TEST_ASSERT(k->type == JSON_CSTR);
	TEST_ASSERT(ft_strcmp(k->content, "size") == 0);
	TEST_ASSERT(v->type == JSON_NUMBER);
	TEST_ASSERT(bnum_cmp((t_num *)v->content, bnum_from_dec("36127812312")) == 0);

	kv_node = kv_node->next;
	TEST_ASSERT(kv_node->type == JSON_KV);

	tuple = kv_node->content;
	k = ft_tuple_get(tuple, 0);
	v = ft_tuple_get(tuple, 1);

	TEST_ASSERT(k->type == JSON_CSTR);
	TEST_ASSERT(ft_strcmp(k->content, "value") == 0);
	TEST_ASSERT(v->type == JSON_NUMBER);
	TEST_ASSERT(bnum_cmp((t_num *)v->content, bnum_from_dec("-123123123123")) == 0);

	kv_node = kv_node->next;
	TEST_ASSERT(kv_node->type == JSON_KV);

	tuple = kv_node->content;
	k = ft_tuple_get(tuple, 0);
	v = ft_tuple_get(tuple, 1);

	TEST_ASSERT(k->type == JSON_CSTR);
	TEST_ASSERT(ft_strcmp(k->content, "required") == 0);
	TEST_ASSERT(v->type == JSON_BOOLEAN);
	TEST_ASSERT(ft_strcmp(v->content, "true") == 0);

	kv_node = kv_node->next;
	TEST_ASSERT(kv_node->type == JSON_KV);

	tuple = kv_node->content;
	k = ft_tuple_get(tuple, 0);
	v = ft_tuple_get(tuple, 1);

	TEST_ASSERT(k->type == JSON_CSTR);
	TEST_ASSERT(ft_strcmp(k->content, "type") == 0);
	TEST_ASSERT(v->type == JSON_CSTR);
	TEST_ASSERT(ft_strcmp(v->content, "integer") == 0);

	kv_node = kv_node->next;
	TEST_ASSERT(kv_node->type == JSON_KV);

	tuple = kv_node->content;
	k = ft_tuple_get(tuple, 0);
	v = ft_tuple_get(tuple, 1);

	TEST_ASSERT(k->type == JSON_CSTR);
	TEST_ASSERT(ft_strcmp(k->content, "format") == 0);
	TEST_ASSERT(v->type == JSON_CSTR);
	TEST_ASSERT(ft_strcmp(v->content, "int64") == 0);

	SSL_FREE(json_s);
	json_del(root_node);

	TEST_PASS();
}
