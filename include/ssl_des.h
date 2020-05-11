#ifndef SSL_DES_H
# define SSL_DES_H

# include <ssl_io.h>

# define DES_MES_BLOCK_SIZE	8

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

typedef struct  s_des
{
	uint8_t		*key;
	uint8_t		*salt;
	uint8_t		*vect;
	uint32_t	vflag;
}               t_des;

typedef void    (*FUNC_DES)(t_des *const, char *const, ssize_t);

t_des	*des_init(const uint8_t *key, const uint8_t *salt, const uint8_t *vect);
t_des	*des_hexinit(const char *key, const char *salt, const char *vect);
int		des_ecb_encrypt(t_des *des, t_ostring *mes, t_ostring *ciph);
int		des_ecb_decrypt(t_des *des, t_ostring *ciph, t_ostring *mes);
int		des_cbc_encrypt(t_des *des, t_ostring *mes, t_ostring *ciph);
int		des_cbc_decrypt(t_des *des, t_ostring *ciph, t_ostring *mes);

/* Low level functions */
void	des_encrypt_schedule(uint64_t *, uint64_t *);
void	des_decrypt_schedule(uint64_t *, uint64_t *);
void	des_permute_key(uint64_t *, uint8_t *);
void	des_permute_block_init(uint64_t *);
void	des_permute_block(uint64_t *, uint64_t *);
void	des_permute_block_final(uint64_t *);

#endif
