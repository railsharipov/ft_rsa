#ifndef SSL_JSON_H
# define SSL_JSON_H

enum e_json_type{
    JSON_ARRAY,
    JSON_OBJECT,
    JSON_STRING,
    JSON_INTEGER
};

int     json_parse(const char *s, t_node **node);

#endif