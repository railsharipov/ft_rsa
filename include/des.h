#ifndef SSL_DES_H
# define SSL_DES_H

# include <common.h>
# include <io.h>
# include <libft/string.h>
# include <libft/logger.h>

# define DES_MES_BLOCK_SIZE	8

# define DES_LOG(LEVEL, MES, ...)	des_logger_log(__func__, __FILE__, __LINE__, LIBFT_LOG_LEVEL_##LEVEL, MES __VA_OPT__(,) __VA_ARGS__)

enum	e_des
{
	DES_K = 1UL << 1,
	DES_P = 1UL << 2,
	DES_S = 1UL << 3,
	DES_V = 1UL << 4,
	DES_N = 1UL << 5,
	DES_A = 1UL << 6,
	DES_E = 1UL << 7,
	DES_D = 1UL << 8
};

int		des_logger_log(const char *func_name, const char *file_name, int line_number, uint8_t level, const char *fmt, ...);

int		des_ecb_encrypt(const uint8_t key[8], t_ostring *mes, t_ostring *ciph);
int		des_ecb_decrypt(const uint8_t key[8], t_ostring *ciph, t_ostring *mes);
int		des_cbc_encrypt(const uint8_t key[8], const uint8_t iv[8], t_ostring *mes, t_ostring *ciph);
int		des_cbc_decrypt(const uint8_t key[8], const uint8_t iv[8], t_ostring *ciph, t_ostring *mes);

/* Low level functions */
void	des_permute_key(uint64_t *pkey, const uint8_t key[8]);
void	des_encrypt_schedule(uint64_t *ksched, uint64_t *pkey);
void	des_decrypt_schedule(uint64_t *ksched, uint64_t *pkey);
void	des_permute_block_init(uint64_t *block);
void	des_permute_block(uint64_t *block, uint64_t *ksched);
void	des_permute_block_final(uint64_t *block);

#endif
