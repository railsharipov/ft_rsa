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

static const char	*__JSON_TYPE_NAME_MAP[] = {
	[JSON_TYPE_BYTES]		= __JSON_TYPE_NAME_BYTES,
	[JSON_TYPE_OBJECT]		= __JSON_TYPE_NAME_OBJECT,
	[JSON_TYPE_ARRAY]		= __JSON_TYPE_NAME_ARRAY,
	[JSON_TYPE_STRING]		= __JSON_TYPE_NAME_CSTR,
	[JSON_TYPE_NUMBER]		= __JSON_TYPE_NAME_NUMBER,
	[JSON_TYPE_BOOL_TRUE]	= __JSON_TYPE_NAME_BOOL_TRUE,
	[JSON_TYPE_BOOL_FALSE]	= __JSON_TYPE_NAME_BOOL_FALSE,
	[JSON_TYPE_NULL]		= __JSON_TYPE_NAME_NULL,
};

static const int	__JSON_TYPE_NAME_COUNT = sizeof(__JSON_TYPE_NAME_MAP) / sizeof(__JSON_TYPE_NAME_MAP[0]);

const char	*json_get_type_name(int type)
{
	if (type < 0 || type >= JSON_TYPE_COUNT) {
		return (__JSON_TYPE_NAME_UNKNOWN);
	}
	if (type >= __JSON_TYPE_NAME_COUNT) {
		return (__JSON_TYPE_NAME_UNKNOWN);
	}
	return (__JSON_TYPE_NAME_MAP[type]);
}
