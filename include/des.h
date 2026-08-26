#ifndef SSL_DES_H
# define SSL_DES_H

# include <common.h>
# include <io.h>
#include <libft.h>

# define DES_BLOCK_SIZE	8
# define DES_KSCHED_LEN	16

typedef struct	s_des_ctx
{
	uint64_t	ksched[DES_KSCHED_LEN];
	uint8_t		vect[DES_BLOCK_SIZE];
	uint8_t		buf[DES_BLOCK_SIZE];
	size_t		bufsize;
	size_t		messize;
	int			done;
}				t_des_ctx;

void	des_ecb_encrypt_permute_block(t_des_ctx *des, uint64_t *block);
void	des_ecb_decrypt_permute_block(t_des_ctx *des, uint64_t *block);

int		des_ecb_encrypt_init(t_des_ctx *des, const uint8_t key[8]);
ssize_t	des_ecb_encrypt_update(t_des_ctx *des, const char *in, char *out, size_t size);
ssize_t	des_ecb_encrypt_final(t_des_ctx *des, char *out, size_t size);

int		des_ecb_decrypt_init(t_des_ctx *des, const uint8_t key[8]);
ssize_t	des_ecb_decrypt_update(t_des_ctx *des, const char *in, char *out, size_t size);
ssize_t	des_ecb_decrypt_final(t_des_ctx *des, char *out, size_t size);

t_transform_result des_ecb_encrypt_transform_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize);
t_transform_result des_ecb_encrypt_transform_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize);

void	des_cbc_encrypt_permute_block(t_des_ctx *des, uint64_t *block);
void	des_cbc_decrypt_permute_block(t_des_ctx *des, uint64_t *block);

int		des_cbc_encrypt_init(t_des_ctx *des, const uint8_t key[8], const uint8_t iv[8]);
ssize_t	des_cbc_encrypt_update(t_des_ctx *des, const char *in, char *out, size_t size);
ssize_t	des_cbc_encrypt_final(t_des_ctx *des, char *out, size_t size);

int		des_cbc_decrypt_init(t_des_ctx *des, const uint8_t key[8], const uint8_t iv[8]);
ssize_t	des_cbc_decrypt_update(t_des_ctx *des, const char *in, char *out, size_t size);
ssize_t	des_cbc_decrypt_final(t_des_ctx *des, char *out, size_t size);

#endif
