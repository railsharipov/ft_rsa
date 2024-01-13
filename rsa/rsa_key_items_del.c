#include <ssl/ssl.h>
#include <ssl/rsa.h>

void	rsa_key_items_del(t_rsa *key)
{
	key->tflag = 0;
	bnum_del(key->version);
	bnum_del(key->modulus);
	bnum_del(key->pubexp);
	bnum_del(key->privexp);
	bnum_del(key->prime1);
	bnum_del(key->prime2);
	bnum_del(key->exponent1);
	bnum_del(key->exponent2);
	bnum_del(key->coeff);
}
