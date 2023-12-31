#include <ft_ssl.h>
#include <ssl_json.h>
#include <ssl_map.h>
#include <bnum.h>

typedef ssize_t (*FUNC_JSON_PARSE)(const char *, t_node **);

t_htbl  *__init_htable(void);

ssize_t __parse(const char *, t_node **, t_htbl *);
ssize_t __parse_array(const char *, t_node **);
ssize_t __parse_object(const char *, t_node **);
ssize_t __parse_string(const char *, t_node **);
ssize_t __parse_integer(const char *, t_node **);

static const struct
{
    char    symbol;
    FUNC_JSON_PARSE f;
} T[] = {
    {'{',   __parse_object},
    {'[',   __parse_array},
    {'"',   __parse_string},
    {'\'',   __parse_string}
};

int json_parse(const char *s, t_node **node)
{
    t_htbl  *ht;

    if (NULL == s || NULL == node)
        return SSL_ERROR(UNSPECIFIED_ERROR);

    *node = NULL;

    ht = __init_htable();

    if (NULL == ht)
        return SSL_ERROR(UNSPECIFIED_ERROR);

    if (SSL_OK != __parse(s, node, ht))
        return SSL_ERROR(UNSPECIFIED_ERROR);

    return SSL_OK;

err:
    ft_htbl_del(ht);
    return SSL_ERROR(UNSPECIFIED_ERROR);
}

t_htbl  *__init_htable(void)
{
    t_htbl  *htbl;
    int idx;
    size_t ht_size;

    ht_size = sizeof(T)/sizeof(T[0]);

    if (NULL == (htbl = ft_htbl_init(ht_size)))
        return NULL;
    
    idx = 0;
    while (idx < ht_size)
    {
        ft_htbl_bin_add(htbl, (void *)&(T[idx]), &(T[idx].symbol), sizeof(T[idx].symbol));
        idx++;
    }

    return htbl;
}

ssize_t __parse(const char *s, t_node **node, t_htbl *ht)
{
    FUNC_JSON_PARSE f_parse;
    t_node  *cur_node;
    ssize_t rbytes;
    ssize_t idx;

    idx = 0;
    cur_node = NULL;

    while (s[idx] != '\0') {
        while (ft_iseolws(s[idx]))
            idx++;
        
        f_parse = ft_htbl_bin_get(ht, s, sizeof(char));

        if (NULL == f_parse)
            f_parse = __parse_integer;

        rbytes = f_parse(s + idx, &cur_node);

        if (rbytes <= 0)
            return SSL_ERROR(UNSPECIFIED_ERROR);

        idx += rbytes;

        ft_lst_append(node, cur_node);
    }

    return SSL_OK;
}

ssize_t __parse_integer(const char *s, t_node **node)
{
    t_node      *node;
    t_num       *num;
    ssize_t     idx;
    ssize_t     integer;
    char        *sub_s;

    idx = 0;
    while (s[idx] == '-') {
        idx++;
    }
    while (s[idx] != '\0' && ft_isdigit(s[idx])) {
        idx++;
    }
    sub_s = ft_strsub(s, 0, idx);
    integer = ft_atoi(sub_s);
    SSL_FREE(sub_s);
    value = ft_memdup(integer, sizeof(integer));

    node = ft_node_new(NULL, &value, sizeof(value));

    return ();
}
