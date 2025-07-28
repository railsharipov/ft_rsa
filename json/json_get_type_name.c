#include <json.h>

# define __JSON_TYPE_NAME_OBJECT		"object"
# define __JSON_TYPE_NAME_ARRAY			"array"
# define __JSON_TYPE_NAME_CSTR			"string"
# define __JSON_TYPE_NAME_NUMBER		"number"
# define __JSON_TYPE_NAME_BOOLEAN		"boolean"
# define __JSON_TYPE_NAME_BOOL_TRUE		"bool-true"
# define __JSON_TYPE_NAME_BOOL_FALSE	"bool-false"
# define __JSON_TYPE_NAME_NULL			"null"
# define __JSON_TYPE_NAME_BYTES			"bytes"
# define __JSON_TYPE_NAME_UNKNOWN		"???"

const char	*json_get_type_name(int type)
{
	switch (type) {
		case JSON_TYPE_BYTES:
			return __JSON_TYPE_NAME_BYTES;
		case JSON_TYPE_OBJECT:
			return __JSON_TYPE_NAME_OBJECT;
		case JSON_TYPE_ARRAY:
			return __JSON_TYPE_NAME_ARRAY;
		case JSON_TYPE_STRING:
			return __JSON_TYPE_NAME_CSTR;
		case JSON_TYPE_NUMBER:
			return __JSON_TYPE_NAME_NUMBER;
		case JSON_TYPE_BOOL_TRUE:
			return __JSON_TYPE_NAME_BOOL_TRUE;
		case JSON_TYPE_BOOL_FALSE:
			return __JSON_TYPE_NAME_BOOL_FALSE;
		case JSON_TYPE_NULL:
			return __JSON_TYPE_NAME_NULL;
		default:
			return __JSON_TYPE_NAME_UNKNOWN;
	}
}
