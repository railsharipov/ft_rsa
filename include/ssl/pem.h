#ifndef SSL_PEM_H
# define SSL_PEM_H

#include <libft/string.h>

# define PEM_ERROR(ERROR)	SSL_ERROR(ADD_ERROR_CTX(ERROR, PEM_ERROR_CTX))

enum	e_pem_error
{
	INVALID_PEM_ENCODING = 1,
	INVALID_PEM_ENCAPSULATION,
};

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
int		pem_remove_encap(t_pem *pem, const char *type, char **con, size_t *);

#endif
