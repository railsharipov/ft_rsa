#include <ft_ssl.h>
#include <ssl_rsa.h>

void	rsa_key_items_del(t_rsa *key)
{
	key->tflag = 0;
	del_num(key->version);
	del_num(key->modulus);
	del_num(key->pubexp);
	del_num(key->privexp);
	del_num(key->prime1);
	del_num(key->prime2);
	del_num(key->exponent1);
	del_num(key->exponent2);
	del_num(key->coeff);
}
