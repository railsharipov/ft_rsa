#include <json.h>

# define __JSON_Q_TYPE_NAME_OBJECT_KEY		"object-key"
# define __JSON_Q_TYPE_NAME_ARRAY_INDEX		"array-index"
# define __JSON_Q_TYPE_NAME_SELF			"self"
# define __JSON_Q_TYPE_NAME_UNKNOWN			"???"

const char	*json_get_query_type_name(int type)
{
	switch (type) {
		case JSON_Q_OBJECT_KEY:
			return __JSON_Q_TYPE_NAME_OBJECT_KEY;
		case JSON_Q_ARRAY_INDEX:
			return __JSON_Q_TYPE_NAME_ARRAY_INDEX;
		case JSON_Q_SELF:
			return __JSON_Q_TYPE_NAME_SELF;
		default:
			return __JSON_Q_TYPE_NAME_UNKNOWN;
	}
}
