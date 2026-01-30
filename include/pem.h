#ifndef SSL_PEM_H
# define SSL_PEM_H

#include <common.h>
#include <libft/string.h>

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
    char *label;
    uint8_t salt[8];
    int has_salt;
} t_pem;

t_pem	*pem_create(const char *label, const uint8_t salt[8], t_pem_proc proc, t_pem_cipher cipher);
void    pem_init(t_pem *pem);
void    pem_del(t_pem *pem);
void    pem_clear(t_pem *pem);
int		pem_encode(t_pem *pem, t_ostring *data, t_ostring *enc, const char *pass);
int		pem_decode(t_pem *pem, t_ostring *enc, t_ostring *data, const char *pass);

#endif
