#ifndef SSL_PEM_H
# define SSL_PEM_H

#include <ssl.h>
#include <libft/string.h>

# define PEM_LOG(LEVEL, MES, ...)	util_logger_log(__func__, __FILE__, __LINE__, LIBFT_LOG_LEVEL_##LEVEL, "pem: ", MES __VA_OPT__(,) __VA_ARGS__)

typedef struct	s_pem
{
	char		*content;
	size_t		size;
}				t_pem;

t_pem	*pem_init();
void	pem_del(t_pem *);
void	pem_clear(t_pem *);
int		pem_encode(t_ostring *, t_pem **, const char *type, const char *pass);
int		pem_decode(t_pem *, const char *type, t_ostring **, const char *pass);
void	pem_encap(t_pem **, const char *type, char *, size_t);
int		pem_decap(t_pem *pem, const char *type, char **con, size_t *);

#endif
