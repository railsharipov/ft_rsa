#include <json_v2.h>
#include <io.h>
#include <bnum.h>
#include <logger.h>
#include <libft.h>
#include <libft_v2.h>
#include <file.h>

enum __e_json_parse_status {
	__JSON_V2_MATCH,
	__JSON_V2_NO_MATCH,
	__JSON_V2_BAD_FORMAT,
};

typedef int (*t_func_json_parse)(const char *s, t_json_v2_value *value, size_t *pos);

static t_json_v2 *__json_create(void);
static void __json_delete_value(t_json_v2 *json);
static void __json_delete(t_json_v2 *json);

static int	__json_parse_value(const char *s, t_json_v2 *json, size_t *pos);

static int	__json_parse_null(const char *s, t_json_v2_value *value, size_t *pos);
static int	__json_parse_boolean(const char *s, t_json_v2_value *value, size_t *pos);
static int	__json_parse_number(const char *s, t_json_v2_value *value, size_t *pos);
static int	__json_parse_string(const char *s, t_json_v2_value *value, size_t *pos);
static int	__json_parse_object(const char *s, t_json_v2_value *value, size_t *pos);
static int	__json_parse_array(const char *s, t_json_v2_value *value, size_t *pos);
static void	__json_parse_ws(const char *s, size_t *pos);

int json_v2_parse(const char *s, t_json_v2 **ret_json)
{
	if (s == NULL) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (JSON_V2_ERR);
	}
	*ret_json = NULL;
	size_t pos = 0;

	t_json_v2 *json = __json_create();
	int status = __json_parse_value(s, json, &pos);
	__json_parse_ws(s, &pos);

	if (status != __JSON_V2_MATCH || s[pos] != '\0') {
		__json_delete(json);
		SSL_LOG(ERROR, "bad format");
		return (JSON_V2_ERR);
	}
	*ret_json = json;

	return (JSON_V2_OK);
}

int	json_v2_parse_file(const char *filename, t_json_v2 **json)
{
	t_ostring file_content;
	if (JSON_V2_OK != file_read_all(filename, &file_content)) {
		SSL_LOG(ERROR, "file read error");
		return (JSON_V2_ERR);
	}
	char *json_s = ft_ostr_to_cstr(&file_content, 0, file_content.size);
	ft_ostr_clear(&file_content);
	if (NULL == json_s) {
		SSL_LOG(ERROR, "memory error");
		return (JSON_V2_ERR);
	}
	int ret = json_v2_parse(json_s, json);
	LIBFT_FREE(json_s);

	if (JSON_V2_OK != ret) {
		SSL_LOG(ERROR, "json file parse error");
		return (JSON_V2_ERR);
	}
	return (JSON_V2_OK);
}

int	json_v2_parse_stream(t_io_v2_stream *stream, t_json_v2 **json)
{
	// TODO: Refactor JSON parsing to support stream inputs.
	// Read all data from stream into the memory before parsing JSON.
	t_ostring file_content;
	ft_ostr_init_with_capacity(&file_content, IO_BUFSIZE);
	char buf[IO_BUFSIZE] = {0};

	ssize_t rbytes = 0;
	while (1) {
		rbytes = io_v2_read(stream, buf, sizeof(buf));
		if (rbytes < 0) {
			break;
		}
		ft_ostr_append(&file_content, buf, rbytes);
	}
	if (stream->status != IO_V2_STATUS_EOF) {
		SSL_LOG(ERROR, IO_READ_ERROR);
		return (JSON_V2_ERR);
	}
	char *json_s = ft_ostr_to_cstr(&file_content, 0, file_content.size);
	ft_ostr_clear(&file_content);
	if (NULL == json_s) {
		SSL_LOG(ERROR, "memory error");
		return (JSON_V2_ERR);
	}
	int ret = json_v2_parse(json_s, json);
	LIBFT_FREE(json_s);

	if (JSON_V2_OK != ret) {
		SSL_LOG(ERROR, "json stream parse error");
		return (JSON_V2_ERR);
	}
	return (JSON_V2_OK);
}

static void __json_delete_value(t_json_v2 *json)
{
	assert(NULL != json);

	switch (json->type) {
	case JSON_V2_TYPE_NULL:
		break;
	case JSON_V2_TYPE_BOOL:
		break;
	case JSON_V2_TYPE_ARRAY:
		ft_list_clear_all_content(&json->value.as.list, (t_func_content_del)__json_delete);
		break;
	case JSON_V2_TYPE_OBJECT:
		ft_htbl_v2_clear(&json->value.as.htable, (t_func_content_del)__json_delete);
		break;
	case JSON_V2_TYPE_STRING:
		LIBFT_FREE(json->value.as.cstr);
		break;
	case JSON_V2_TYPE_NUMBER:
		bnum_clear(&json->value.as.number);
		break;
	default:
		SSL_UNREACHABLE("__json_delete_value");
	}
	json->type = JSON_V2_TYPE_NULL;
}

// static void __json_set_value(t_json_v2 *json, t_json_v2_type type, t_json_v2_value value)
// {
// 	assert(NULL != json);

// 	__json_delete_value(json);
// 	json->type = type;
// 	json->value = value;
// }

static t_json_v2 *__json_create(void)
{
	t_json_v2 *json = NULL;
	LIBFT_ALLOC(json, sizeof(t_json_v2));
	*json = (t_json_v2){0};
	json->type = JSON_V2_TYPE_NULL;
	return (json);
}

static void __json_delete(t_json_v2 *json)
{
	__json_delete_value(json);
	LIBFT_FREE(json);
}

typedef struct __s_json_parse_ctx {
	t_json_v2_type type;
	t_func_json_parse f;
} __t_json_parse_ctx;

static int	__json_parse_value(const char *s, t_json_v2 *json, size_t *pos)
{
	const __t_json_parse_ctx arr[] = {
		{ JSON_V2_TYPE_NULL, __json_parse_null },
		{ JSON_V2_TYPE_BOOL, __json_parse_boolean },
		{ JSON_V2_TYPE_NUMBER, __json_parse_number },
		{ JSON_V2_TYPE_STRING, __json_parse_string },
		{ JSON_V2_TYPE_OBJECT, __json_parse_object },
		{ JSON_V2_TYPE_ARRAY, __json_parse_array },
	};
	const int arr_size = sizeof(arr) / sizeof(arr[0]);

	__json_parse_ws(s, pos);

	for (int i = 0; i < arr_size; i++) {
		__t_json_parse_ctx ctx = arr[i];
		t_json_v2_value value = {0};
		int status = ctx.f(s, &value, pos);

		if (__JSON_V2_MATCH == status) {
			json->type = ctx.type;
			json->value = value;
			return (__JSON_V2_MATCH);
		}
		else if (__JSON_V2_NO_MATCH == status) {
			continue ;
		}
		else {
			return (status);
		}
	}
	return (__JSON_V2_NO_MATCH);
}

static void	__json_parse_ws(const char *s, size_t *pos)
{
	while (ft_iseolws(s[*pos]) && s[*pos] != '\0') {
		(*pos)++;
	}
}

static int	__json_parse_null(const char *s, t_json_v2_value *value, size_t *pos)
{
	size_t old_pos = *pos;
	__json_parse_ws(s, pos);

	SSL_LOG(TRACE, "parsing null at index %zu: %.20s...", *pos, s + *pos);

	if (ft_strncmp(s + *pos, "null", 4) == 0) {
		*pos += 4;
		*value = (t_json_v2_value){0};
		return (__JSON_V2_MATCH);
	} else {
		SSL_LOG(TRACE, "no match at index %zu: %c", *pos, s[*pos]);
		*pos = old_pos;
		return (__JSON_V2_NO_MATCH);
	}
}

static int	__json_parse_boolean(const char *s, t_json_v2_value *value, size_t *pos)
{
	size_t	old_pos = *pos;
	__json_parse_ws(s, pos);

	SSL_LOG(TRACE, "parsing boolean at index %zu: %.20s...", *pos, s + *pos);

	if (ft_strncmp(s + *pos, "true", 4) == 0) {
		*pos += 4;
		value->as.boolean = 1;
		return (__JSON_V2_MATCH);
	} else if (ft_strncmp(s + *pos, "false", 5) == 0) {
		*pos += 5;
		value->as.boolean = 0;
		return (__JSON_V2_MATCH);
	} else {
		SSL_LOG(TRACE, "no match at index %zu: %c", *pos, s[*pos]);
		*pos = old_pos;
		return (__JSON_V2_NO_MATCH);
	}
}

static int	__json_parse_number(const char *s, t_json_v2_value *value, size_t *pos)
{
	size_t old_pos = *pos;
	__json_parse_ws(s, pos);

	SSL_LOG(TRACE, "parsing number at index %zu: %.20s...", *pos, s + *pos);

	int is_neg_mantissa = 0;
	int is_neg_exponent = 0;
	int is_float = 0;
	int is_exponent = 0;

	if (!ft_isdigit(s[*pos]) && s[*pos] != '-') {
		SSL_LOG(TRACE, "no match at index %zu: %c", *pos, s[*pos]);
		return (__JSON_V2_NO_MATCH);
	}

	if (s[*pos] == '-') {
		is_neg_mantissa = 1;
		(*pos)++;
		if (!ft_isdigit(s[*pos])) {
			SSL_LOG(ERROR, "bad negative number format at index %d, %.20s...: expected digit, got '%c'", *pos, s + *pos, s[*pos]);
			*pos = old_pos;
			return (__JSON_V2_BAD_FORMAT);
		}
	}
	size_t mantissa_start = 0, mantissa_end = 0;
	size_t fraction_start = 0, fraction_end = 0;
	size_t exponent_start = 0, exponent_end = 0;

	mantissa_start = *pos;
	while (ft_isdigit(s[*pos])) {
		(*pos)++;
	}
	mantissa_end = *pos;

	if (s[*pos] == '.') {
		is_float = 1;
		(*pos)++;
		fraction_start = *pos;

		if (!ft_isdigit(s[*pos])) {
			SSL_LOG(ERROR, "bad float format at index %d, %.20s...: expected digit, got '%c'", *pos, s + *pos, s[*pos]);
			*pos = old_pos;
			return (__JSON_V2_BAD_FORMAT);
		}
		while (ft_isdigit(s[*pos])) {
			(*pos)++;
		}
		fraction_end = *pos;
	}

	if (s[*pos] == 'e' || s[*pos] == 'E') {
		is_exponent = 1;
		(*pos)++;
		exponent_start = *pos;

		if (s[*pos] == '-') {
			is_neg_exponent = 1;
			(*pos)++;
		}
		if (!ft_isdigit(s[*pos])) {
			SSL_LOG(ERROR, "bad float format at index %d, %.20s...: expected digit, got '%c'", *pos, s + *pos, s[*pos]);
			*pos = old_pos;
			return (__JSON_V2_BAD_FORMAT);
		}
		while (ft_isdigit(s[*pos])) {
			(*pos)++;
		}
		exponent_end = *pos;
	}
	SSL_LOG(TRACE, "float: start=%zu, end=%zu, fraction_start=%zu, fraction_end=%zu, exponent_start=%zu, exponent_end=%zu", mantissa_start, mantissa_end, fraction_start, fraction_end, exponent_start, exponent_end);

	if (is_float || is_neg_exponent) {
		SSL_LOG(ERROR, NOT_IMPLEMENTED_ERROR);
		return (__JSON_V2_BAD_FORMAT);
	}

	uint64_t exponent = 1;
	if (is_exponent) {
		char *exponent_str = ft_strsub(s, exponent_start, exponent_end - exponent_start);
		exponent = (uint64_t)ft_atoi(exponent_str);
		LIBFT_FREE(exponent_str);
	}

	char *mantissa_str = ft_strsub(s, mantissa_start, mantissa_end - mantissa_start);
	t_num *mantissa = bnum_from_dec(mantissa_str);
	LIBFT_FREE(mantissa_str);

	t_num *number = &value->as.number;
	bnum_init(number);

	if (is_exponent) {
		bnum_exp(mantissa, exponent, number);
	} else {
		bnum_copy(mantissa, number);
	}
	if (is_neg_mantissa) {
		number->sign = BNUM_NEG;
	}
	bnum_del(mantissa);

	return (__JSON_V2_MATCH);
}

static int	__json_parse_string(const char *s, t_json_v2_value *value, size_t *pos)
{
	size_t old_pos = *pos;
	size_t str_start = 0, str_end = 0;

	__json_parse_ws(s, pos);
	SSL_LOG(TRACE, "parsing string at index %zu: %.20s...", *pos, s + *pos);

	if (s[*pos] != '"') {
		SSL_LOG(TRACE, "no match at index %zu: %c", *pos, s[*pos]);
		return (__JSON_V2_NO_MATCH);
	}
	(*pos)++;
	str_start = *pos;

	while (s[*pos] != '"' && s[*pos] != '\0') {
		(*pos)++;
	}
	str_end = *pos;

	if (s[*pos] == '\0') {
		SSL_LOG(ERROR, "bad string format at index %d, %.20s...: expected '\"', got '%c'", *pos, s + *pos, s[*pos]);
		*pos = old_pos;
		return (__JSON_V2_BAD_FORMAT);
	}
	(*pos)++;

	value->as.cstr = ft_strsub(s, str_start, str_end - str_start);

	return (__JSON_V2_MATCH);
}

// static int	__json_parse_kv(const char *s, t_htbl_v2 *htbl, size_t *pos)
// {
// 	size_t old_pos = *pos;
// 	int status = __JSON_V2_MATCH;

// 	SSL_LOG(TRACE, "parsing key-value at index %zu: %.20s...", *pos, s + *pos);

// 	t_json_v2_value *key = __json_create();
// 	t_json_v2_value *value = __json_create();

// 	if (__JSON_V2_MATCH != (status = __json_parse_string(s, key, pos))) {
// 		SSL_LOG(TRACE, "no match at index %zu: %c", *pos, s[*pos]);
// 		goto label_exit;
// 	}
// 	__json_parse_ws(s, pos);

// 	if (s[*pos] != ':') {
// 		SSL_LOG(ERROR, "bad key-value format at index %d, %.20s...: expected ':', got '%c'", *pos, s + *pos, s[*pos]);
// 		status = __JSON_V2_BAD_FORMAT;
// 		goto label_exit;
// 	}
// 	(*pos)++;

// 	if (__JSON_V2_MATCH != (status = __json_parse_value(s, value, pos))) {
// 		SSL_LOG(ERROR, "bad key-value format at index %d, %.20s...: expected value", *pos, s + *pos);
// 		goto label_exit;
// 	}
// 	t_string *key_string = __json_get_value_ptr(key);
// 	void *any = __json_get_value_ptr(value);

// 	char *key_cstr = ft_ostr_to_cstr(key_string, 0, key_string.size);
// 	bool ok = ft_htbl_v2_set(htable, key_cstr, any);
// 	LIBFT_FREE(key_cstr);
// 	if (!ok) {
// 		SSL_LOG(ERROR, "bad key-value: duplicate key?");
// 		status = __JSON_V2_BAD_FORMAT;
// 		goto label_exit;
// 	}

// label_exit:
// 	__json_delete(&key);

// 	if (status != __JSON_V2_MATCH) {
// 		*pos = old_pos;
// 		ft_node_del(value_node);
// 	}

// 	return (status);
// }

static int	__json_parse_object(const char *s, t_json_v2_value *value, size_t *pos)
{
	size_t old_pos = *pos;
	__json_parse_ws(s, pos);

	SSL_LOG(TRACE, "parsing object at index %zu: %.20s...", *pos, s + *pos);

	int status = __JSON_V2_MATCH;
	t_htbl_v2 htable = {0};
	ft_htbl_v2_init(&htable, 1024);

	if (s[*pos] != '{') {
		SSL_LOG(TRACE, "no match at index %zu: %c", *pos, s[*pos]);
		status = __JSON_V2_NO_MATCH;
		goto label_exit;
	}
	(*pos)++;
	__json_parse_ws(s, pos);

	while (s[*pos] != '}' && s[*pos]) {
		SSL_LOG(TRACE, "parsing key at index %zu: %.20s...", *pos, s + *pos);

		t_json_v2 *key = __json_create();
		if (__JSON_V2_MATCH != (status = __json_parse_value(s, key, pos))) {
			SSL_LOG(TRACE, "no match at index %zu: %c", *pos, s[*pos]);
			goto label_exit;
		}
		if (key->type != JSON_V2_TYPE_STRING) {
			SSL_LOG(TRACE, "bad format: expected string at index %zu: %c", *pos, s[*pos]);
			status = __JSON_V2_BAD_FORMAT;
			__json_delete(key);
			goto label_exit;
		}
		__json_parse_ws(s, pos);

		if (s[*pos] != ':') {
			SSL_LOG(ERROR, "bad key-value format at index %d, %.20s...: expected ':', got '%c'", *pos, s + *pos, s[*pos]);
			status = __JSON_V2_BAD_FORMAT;
			__json_delete(key);
			goto label_exit;
		}
		(*pos)++;

		t_json_v2 *value = __json_create();
		if (__JSON_V2_MATCH != (status = __json_parse_value(s, value, pos))) {
			SSL_LOG(ERROR, "bad key-value format at index %d, %.20s...: expected value", *pos, s + *pos);
			__json_delete(key);
			goto label_exit;
		}

		bool ok = ft_htbl_v2_set(&htable, key->value.as.cstr, value);
		__json_delete(key);

		if (!ok) {
			SSL_LOG(ERROR, "bad key-value: duplicate key?");
			status = __JSON_V2_BAD_FORMAT;
			__json_delete(value);
			goto label_exit;
		}
		__json_parse_ws(s, pos);

		if (s[*pos] != ',') break;
		(*pos)++;
	};
	if (s[*pos] != '}') {
		SSL_LOG(ERROR, "bad object format at index %d, %.20s...: expected '}', got '%c'", *pos, s + *pos, s[*pos]);
		status = __JSON_V2_BAD_FORMAT;
		goto label_exit;
	}
	(*pos)++;

	value->as.htable = htable;

label_exit:
	if (status != __JSON_V2_MATCH) {
		*pos = old_pos;
		ft_htbl_v2_clear(&htable, (t_func_content_del)__json_delete_value);
	}
	return (status);
}

static int	__json_parse_array(const char *s, t_json_v2_value *value, size_t *pos)
{
	size_t old_pos = *pos;
	int status = __JSON_V2_NO_MATCH;

	__json_parse_ws(s, pos);

	SSL_LOG(TRACE, "parsing array at index %zu: %.20s...", *pos, s + *pos);

	if (s[*pos] != '[') {
		SSL_LOG(TRACE, "no match at index %zu: %c", *pos, s[*pos]);
		status = __JSON_V2_NO_MATCH;
		goto label_exit;
	}
	(*pos)++;

	t_list list = {0};
	while (s[*pos] != ']' && s[*pos]) {
		t_json_v2 *item = __json_create();
		ft_list_append_content(&list, item);

		if (__JSON_V2_MATCH != (status = __json_parse_value(s, item, pos))) {
			SSL_LOG(ERROR, "bad array format at index %d, %.20s...: expected value", *pos, s + *pos);
			goto label_exit;
		}
		__json_parse_ws(s, pos);

		if (s[*pos] != ',') break;
		(*pos)++;
	}
	if (s[*pos] != ']') {
		SSL_LOG(ERROR, "bad array format at index %d, %.20s...: expected ']', got '%c'", *pos, s + *pos, s[*pos]);
		status = __JSON_V2_BAD_FORMAT;
		goto label_exit;
	}
	(*pos)++;

	value->as.list = list;

label_exit:
	if (status != __JSON_V2_MATCH) {
		*pos = old_pos;
		ft_list_clear_all_content(&list, (t_func_content_del)__json_delete);
	}

	return (status);
}

/****************************************************************************/

# define __JSON_V2_TYPE_NAME_OBJECT		"object"
# define __JSON_V2_TYPE_NAME_ARRAY		"array"
# define __JSON_V2_TYPE_NAME_STRING		"string"
# define __JSON_V2_TYPE_NAME_NUMBER		"number"
# define __JSON_V2_TYPE_NAME_BOOLEAN	"boolean"
# define __JSON_V2_TYPE_NAME_NULL		"null"
# define __JSON_V2_TYPE_NAME_UNKNOWN	"unknown"

const char	*json_v2_get_type_name(t_json_v2_type type)
{
	switch (type) {
	case JSON_V2_TYPE_OBJECT:	return __JSON_V2_TYPE_NAME_OBJECT;
	case JSON_V2_TYPE_ARRAY:	return __JSON_V2_TYPE_NAME_ARRAY;
	case JSON_V2_TYPE_STRING:	return __JSON_V2_TYPE_NAME_STRING;
	case JSON_V2_TYPE_NUMBER:	return __JSON_V2_TYPE_NAME_NUMBER;
	case JSON_V2_TYPE_BOOL:		return __JSON_V2_TYPE_NAME_BOOLEAN;
	case JSON_V2_TYPE_NULL:		return __JSON_V2_TYPE_NAME_NULL;
	default:
		return __JSON_V2_TYPE_NAME_UNKNOWN;
	}
}

/****************************************************************************/

static void	__json_v2_f_default_dumper(t_json_v2 *json, t_ostring *ostring);

char	*json_v2_dumps(t_json_v2 *json)
{
	return (json_v2_dumps_with_f_dumper(json, __json_v2_f_default_dumper));
}

char	*json_v2_dumps_with_f_dumper(t_json_v2 *json, t_func_json_v2_dump f_dumper)
{
	assert(NULL != json);
	assert(NULL != f_dumper);

	t_ostring ostring = {0};
	ft_ostr_init(&ostring);

	f_dumper(json, &ostring);
	char *dumps = ft_ostr_to_cstr(&ostring, 0, ostring.size);
	ft_ostr_clear(&ostring);
	return (dumps);
}

size_t	json_v2_dumpb(t_json_v2 *json, char *buf, size_t size)
{
	return (json_v2_dumpb_with_f_dumper(json, buf, size, __json_v2_f_default_dumper));
}

size_t	json_v2_dumpb_with_f_dumper(t_json_v2 *json, char *buf, size_t size, t_func_json_v2_dump f_dumper)
{
	if (size == 0) return (0);

	char *dumps = json_v2_dumps_with_f_dumper(json, f_dumper);
	size_t len = ft_strlen(dumps);
	if (len >= size) {
		len = size-1;
	}
	ft_strncpy(buf, dumps, len);
	buf[len] = '\0';
	SSL_FREE(dumps);
	return (len);
}

static void	__json_v2_f_default_dumper(t_json_v2 *json, t_ostring *ostring)
{
	switch (json->type) {
	case JSON_V2_TYPE_OBJECT: {
		ft_ostr_append_cstr(ostring, "{");
		const char *key = NULL;
		void *value = NULL;
		t_htbl_v2_next next = {0};
		size_t commas = 0;
		while (ft_htbl_v2_next(&json->value.as.htable, &next, &key, &value)) {
			if (commas++) ft_ostr_append_cstr(ostring, ",");
			ft_ostr_appendf(ostring, "\"%s\":", key);
			__json_v2_f_default_dumper(value, ostring);
		}
		ft_ostr_append_cstr(ostring, "}");
	}
	break;
	case JSON_V2_TYPE_ARRAY: {
		ft_ostr_append(ostring, "[", 1);
		void *content = NULL;
		t_list_next next = {0};
		size_t commas = 0;
		while (ft_list_next_content(&json->value.as.list, &next, &content)) {
			if (commas++) ft_ostr_append_cstr(ostring, ",");
			__json_v2_f_default_dumper(content, ostring);
		}
		ft_ostr_append(ostring, "]", 1);
	}
	break;
	case JSON_V2_TYPE_STRING: {
		ft_ostr_appendf(ostring, "\"%s\"", json->value.as.cstr);
	}
	break;
	case JSON_V2_TYPE_NUMBER: {
		char *s = bnum_to_dec(&json->value.as.number);
		ft_ostr_append_cstr(ostring, s);
		LIBFT_FREE(s);
	}
	break;
	case JSON_V2_TYPE_BOOL: {
		ft_ostr_append_cstr(ostring, (json->value.as.boolean) ? "true" : "false");
	}
	break;
	case JSON_V2_TYPE_NULL: {
		ft_ostr_append_cstr(ostring, "null");
	}
	break;
	default: {
		ft_ostr_append_cstr(ostring, "\"<_unknown_type_>\"");
	}
	}
}
