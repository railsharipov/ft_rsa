#include <ft_ssl.h>
#include <ssl_json.h>
#include <ssl_test.h>
#include <ssl_io.h>

static int	__test_json_setup(void);
static void	__test_json_cleanup(void);

static int	__test_json_parse_simple_string(void);

static const char	*__simple_null_json_file_path = "test/testfiles/json/simple-null.json";
static const char	*__simple_false_json_file_path = "test/testfiles/json/simple-boolean-false.json";
static const char	*__simple_true_json_file_path = "test/testfiles/json/simple-boolean-true.json";
static const char	*__simple_number_json_file_path = "test/testfiles/json/simple-number.json";
static const char	*__simple_string_json_file_path = "test/testfiles/json/simple-string.json";
static const char	*__complex_object_json_file_path = "test/testfiles/json/complex-object.json";
static const char	*__complex_array_json_file_path = "test/testfiles/json/complex-array.json";

static t_ostring	__simple_null_json;
static t_ostring	__simple_false_json;
static t_ostring	__simple_true_json;
static t_ostring	__simple_number_json;
static t_ostring	__simple_string_json;
static t_ostring	__complex_object_json;
static t_ostring	__complex_array_json;

int	test_json(void)
{
	int	res;

	if (SSL_OK != __test_json_setup())
		return (SSL_ERROR(UNSPECIFIED_ERROR));

	res = __test_json_parse_simple_string();

	__test_json_cleanup();

	return (res);
}

static int	__test_json_setup(void)
{
	if (SSL_OK != test_get_file_content(__simple_null_json_file_path, &__simple_null_json)) {
		return (SSL_ERROR(UNSPECIFIED_ERROR));
	}
	if (SSL_OK != test_get_file_content(__simple_false_json_file_path, &__simple_false_json)) {
		return (SSL_ERROR(UNSPECIFIED_ERROR));
	}
	if (SSL_OK != test_get_file_content(__simple_true_json_file_path, &__simple_true_json)) {
		return (SSL_ERROR(UNSPECIFIED_ERROR));
	}
	if (SSL_OK != test_get_file_content(__simple_number_json_file_path, &__simple_number_json)) {
		return (SSL_ERROR(UNSPECIFIED_ERROR));
	}
	if (SSL_OK != test_get_file_content(__simple_string_json_file_path, &__simple_string_json)) {
		return (SSL_ERROR(UNSPECIFIED_ERROR));
	}
	if (SSL_OK != test_get_file_content(__complex_object_json_file_path, &__complex_object_json)) {
		return (SSL_ERROR(UNSPECIFIED_ERROR));
	}
	if (SSL_OK != test_get_file_content(__complex_array_json_file_path, &__complex_array_json)) {
		return (SSL_ERROR(UNSPECIFIED_ERROR));
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
	char	*json_str;
	int		ret;
	int		pass;

	json_str = util_ostr_to_cstr(&__simple_string_json, 0, __simple_string_json.size);
	ret = json_parse(json_str, &node);

	pass = TEST_ASSERT(SSL_OK == ret);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}