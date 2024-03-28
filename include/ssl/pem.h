#ifndef SSL_PEM_H
# define SSL_PEM_H

#include <ssl/ssl.h>
#include <libft/string.h>

# define PEM_ERROR(MES, ...)	ssl_error_log(__func__, __FILE__, __LINE__, "pem error: ", MES __VA_OPT__(,) __VA_ARGS__)

typedef struct	s_pem
{
	char		*content;
	size_t		size;
}				t_pem;

t_pem	*pem_init();
void	pem_del(t_pem *);
void	pem_clear(t_pem *);
int		pem_encode(t_ostring *, t_pem **, const char *type, t_bool encrypt);
int		pem_decode(t_pem *, const char *type, t_ostring **);
void	pem_encap(t_pem **, const char *type, char *, size_t);
int		pem_decap(t_pem *pem, const char *type, char **con, size_t *);

#endif
