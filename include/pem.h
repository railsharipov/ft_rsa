#ifndef SSL_PEM_H
# define SSL_PEM_H

#include <common.h>
#include <libft/string.h>

# define PEM_LOG(LEVEL, MES, ...)	pem_logger_log(__func__, __FILE__, __LINE__, LIBFT_LOG_LEVEL_##LEVEL, MES __VA_OPT__(,) __VA_ARGS__)

typedef enum e_pem_proc {
    PEM_PROC_TYPE_NONE = 0,        // No Proc-Type header (unencrypted)
    PEM_PROC_TYPE_ENCRYPTED = 4    // Proc-Type: 4,ENCRYPTED
} t_pem_proc;

typedef enum e_pem_cipher {
    PEM_CIPHER_NONE = 0,
    PEM_CIPHER_DES_CBC = 1,
} t_pem_cipher;

typedef struct s_pem {
    t_pem_proc proc;
    t_pem_cipher cipher;
    char salt[8];
    char iv[8];
    char key[8];
} t_pem;

int		pem_logger_log(const char *func_name, const char *file_name, int line_number, uint8_t level, const char *fmt, ...);

void    pem_init(t_pem *pem);
int		pem_encode(t_pem *pem, t_ostring *data, t_ostring *enc, const char *label, const char *pass);
int		pem_decode(t_pem *pem, t_ostring *enc, t_ostring *data, const char *label, const char *pass);

#endif
