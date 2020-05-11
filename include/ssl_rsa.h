#ifndef SSL_RSA_H
# define SSL_RSA_H

# include <ssl_io.h>
# include <bnum.h>

# define RSA_EXPPUB 65537ULL
# define RM_TRIALS  20

enum  e_rsa
{
	RSA_PUBKEY		= 1UL << 1,
	RSA_PRIVKEY		= 1UL << 2,
	RSA_ENCRYPT		= 1UL << 3,
	RSA_DECRYPT		= 1UL << 4,
	RSA_MODULUS		= 1UL << 5,
	RSA_INFORM		= 1UL << 6,
	RSA_OUTFORM		= 1UL << 7,
	RSA_PASSIN		= 1UL << 8,
	RSA_PASSOUT		= 1UL << 9,
	RSA_PUBIN		= 1UL << 10,
	RSA_PUBOUT		= 1UL << 11,
	RSA_PEM			= 1UL << 12,
	RSA_DER			= 1UL << 13,
	RSA_CHECK		= 1UL << 14,
	RSA_TEXT		= 1UL << 15,
	RSA_NOOUT		= 1UL << 16,
	RSA_HEXDUMP		= 1UL << 17
};

typedef struct	s_rsa
{
	uint32_t	tflag;
	int			keysize;
	t_num		*version;
	t_num		*modulus;
	t_num		*prime1;
	t_num		*prime2;
	t_num		*pubexp;
	t_num		*privexp;
	t_num		*exponent1;
	t_num		*exponent2;
	t_num		*coeff;
}				t_rsa;

int			rsa_key_items(t_node *asn_key, t_rsa **rsa_key);
int			rsa_gen_key(t_node **asn_pkey, int modsize, const char *frand);
int			rsa_check(t_node *asn_key);
int 		rsa_encrypt(t_ostring *mes, t_ostring *ciph, t_node *asn_key);
int 		rsa_decrypt(t_ostring *ciph, t_ostring *mes, t_node *asn_key);
int			rsa_os2i(t_num *, unsigned char *, size_t);
int			rsa_i2os(t_num *, unsigned char **, size_t);

#endif
