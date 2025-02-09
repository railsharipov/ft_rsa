#include <json.h>

# define __JSON_TYPE_NAME_OBJECT	"object"
# define __JSON_TYPE_NAME_ARRAY		"array"
# define __JSON_TYPE_NAME_KV		"key-value"
# define __JSON_TYPE_NAME_CSTR		"string"
# define __JSON_TYPE_NAME_NUMBER	"number"
# define __JSON_TYPE_NAME_BOOLEAN	"boolean"
# define __JSON_TYPE_NAME_NULL		"null"
# define __JSON_TYPE_NAME_UNDEFINED	"undefined type"
# define __JSON_TYPE_NAME_UNKNOWN	"???"

static const char	*__JSON_TYPE_NAME_MAP[] = {
	[JSON_TYPE_UNDEFINED]	= __JSON_TYPE_NAME_UNDEFINED,
	[JSON_TYPE_OBJECT]		= __JSON_TYPE_NAME_OBJECT,
	[JSON_TYPE_KV]			= __JSON_TYPE_NAME_KV,
	[JSON_TYPE_ARRAY]		= __JSON_TYPE_NAME_ARRAY,
	[JSON_TYPE_STRING]		= __JSON_TYPE_NAME_CSTR,
	[JSON_TYPE_NUMBER]		= __JSON_TYPE_NAME_NUMBER,
	[JSON_TYPE_BOOLEAN]		= __JSON_TYPE_NAME_BOOLEAN,
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
