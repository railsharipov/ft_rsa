#ifndef SSL_PEM_H
# define SSL_PEM_H

#include <common.h>
#include <libft/string.h>

# define PEM_LOG(LEVEL, MES, ...)	pem_logger_log(__func__, __FILE__, __LINE__, LIBFT_LOG_LEVEL_##LEVEL, MES __VA_OPT__(,) __VA_ARGS__)

typedef struct	s_pem
{
	char		*content;
	size_t		size;
}				t_pem;

int		pem_logger_log(const char *func_name, const char *file_name, int line_number, uint8_t level, const char *fmt, ...);

t_pem	*pem_init(void);
void	pem_del(t_pem *pem);
void	pem_clear(t_pem *pem);
int		pem_encode(t_ostring *content, t_pem **pem, const char *type, const char *pass);
int		pem_decode(t_pem *pem, const char *type, t_ostring **content, const char *pass);
void	pem_encap(t_pem **pem, const char *type, char *content, size_t consize);
int		pem_decap(t_pem *pem, const char *type, char **content, size_t *consize);

#endif
