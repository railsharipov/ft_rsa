#include <common.h>
#include <asn.h>
#include <der.h>
#include <bnum.h>
#include <rand.h>
#include <test.h>
#include <io.h>

static int	__test_der_setup(void);
static void	__test_der_cleanup(void);

static int	__test_der_decode(void);

static const char	*__private_key_der_file_path = "test/testfiles/der/test-private-key.der";

static const char	*modulus = "0090c698de843004925c02940780296d103900047942d8c784309b6f37935b9327de6d45c06fb6257daa65b9fbbb2895afa54677b750bba37ad56ea5fd78a5439b5a7452e139cdedfc65ba52bbd9dbe8d9cd0a0c27e2a6bed45e81b3efb25512fb85dec6e7c0c789c58536aa747b006bfb3b5ab24dd237b689dc91786de2d0fd97f798abb51a433989c24414722aef236835beb0db8de65c2d6665e8fb3024f35a95cbc5490d56183af8c0ad16ffbd4212f55eb730ea3fbffcdad593c7acfe4430d825747ca1c860d248495c1d8a675bb1bd3f4089f125730ba2a41026e92c8b7da92a1f8026f46a688019c98d7ba64aa91aaa84e20317064f9f07e1cc5e5fb993";
static const char	*publicExponent = "10001";
static const char	*privateExponent = "244504b636529b4a996f71eb6121bfb781382e66bca6f770f80670b9f11a4f86fcf3ce363426634514c3d070aec62f9de5df7e0d7855d1d26b970151bb8057ac1b60aa04cfc8f5f6b5fb944f88784de106bb9e28fde0bd4d11f2178494de3857b805843a3a3323224f49cf314841664bf5e6366645a47c801438837db005a08a29b22033b96b4f8365fb8c77947cd4460ad33c03e579b412c3f59036e25264ab89f15c19d2ef3c58fb6a39bc0e90ecad7f5c5abdf66a1b0cdee1d1c7a79ed192926bda5a23b0fc650d57a5863dd9088e4e0e93c6c0df59c6f216e734d4e1bbe5b4b1d00f5d7aa5c01ef69ae5a9a338980ee0432baea73896837574edf7335c4d";
static const char	*prime1 = "00ccbb7b13c5296cc194bd640303c2ba5e2145dd1310fdb2033f98d22fd875aef862d3b59f7bdd9d4db21bf5d3f8c8c5f136170258bfcb7499ec6ea566fab922481e484888bc2c2f88e1455d02960593097f4bef4aace55eb06bd07bc0604d823867fcca173e59f121c87e6068f2d5796c29e50314bcfdbe548bf171f4f9e70335";
static const char	*prime2 = "00b5078e337dd3b4d63ff4f7307f215bf3a62b2e472ea673a9a7f02931a265e1f984f2469b3710f9981f7efd004db8698a14e2272d43f44689a3ce61f67deb757a1d83c10f8c87b5fbb7f193915f0f2d927d39adfbf3f0f4693fcb59a743a01a146f207d50af8133faff91b6ed88538bd73bb7d2fdbaaed26784717339e0dc5aa7";
static const char	*exponent1 = "0d4939627e100323c8e04586408f3982f44bd15df46352530da4803764a7b6741e0db922a16a2f542473d60f2c7cf303854c16f5e574719baf913aa0a532b07bd7a33f39c86461ad53592a0109f550a674215c650f354f60c98e5d098df659b609618947dadf015bfce3dea3d0e6739bc6583b4bf43bcf473351b6e07bfb36e5";
static const char	*exponent2 = "009402746a773333c4849035d6a9eacd727d670ab45a58ef724403a0ba9b2bafc1c1c810a0713a68c7cee095dcbcaecd637f45948076f1ef06342ed67ba162752f7d982eae16d9129a973f65c6d2b0e732c13dfa99684e9696c2f8f0f947da4bd86f2f622629087b4679d45a7b051e41d49d57e3a35e11f7a7796b04a9cac67f3b";
static const char	*coefficient = "120d5b172b432fce78b1e7a7775cdfbdeecd33c925326a21517669a26fa54613e53497a3a5ba92fb0f54b4585885b95b41c91eb88393d69d1bad4f6853744094715c9e45bd88788b29fc33484b06cae8517766e0ff74dde4aceb79c371532a6a22743aaef84a42c3943e13f924839a99aac3aa664d751f9e0ebc3cd21227ee96";

static t_num	*private_key_num[9];

static t_num	*version_num;
static t_num	*modulus_num;
static t_num	*publicExponent_num;
static t_num	*privateExponent_num;
static t_num	*prime1_num;
static t_num	*prime2_num;
static t_num	*exponent1_num;
static t_num	*exponent2_num;
static t_num	*coefficient_num;


static t_ostring	__private_key_der;

int	test_der(void)
{
	if (SSL_OK != __test_der_setup()) {
		TEST_LOG(ERROR, "failed to setup test");
		TEST_FAIL();
	}

	__test_der_cleanup();

	return (__test_der_decode());
}

static int	__test_der_setup(void)
{
	if (SSL_OK != test_get_file_content(__private_key_der_file_path, &__private_key_der)) {
		TEST_LOG(ERROR, "failed to get file content");
		return (SSL_ERR);
	}

	version_num = bnum_from_hex_u("0");
	modulus_num = bnum_from_hex_u(modulus);
	publicExponent_num = bnum_from_hex_u(publicExponent);
	privateExponent_num = bnum_from_hex_u(privateExponent);
	prime1_num = bnum_from_hex_u(prime1);
	prime2_num = bnum_from_hex_u(prime2);
	exponent1_num = bnum_from_hex_u(exponent1);
	exponent2_num = bnum_from_hex_u(exponent2);
	coefficient_num = bnum_from_hex_u(coefficient);

	private_key_num[0] = version_num;
	private_key_num[1] = modulus_num;
	private_key_num[2] = publicExponent_num;
	private_key_num[3] = privateExponent_num;
	private_key_num[4] = prime1_num;
	private_key_num[5] = prime2_num;
	private_key_num[6] = exponent1_num;
	private_key_num[7] = exponent2_num;
	private_key_num[8] = coefficient_num;

	return (SSL_OK);
}

static void	__test_der_cleanup(void)
{
	return ;
}

static int	__test_der_decode(void)
{
	t_node	*tree, *asn_node;
	t_iasn	*asn_item;
	char 	*num_str;
	t_num	num;
	t_iodes	iodes;
	int		ret;

	tree = NULL;

	if (SSL_OK != io_osbuf(&iodes, IO_READ, &__private_key_der)) {
		TEST_LOG(ERROR, "failed to set io buffer");
		return (SSL_ERR);
	}

	ret = der_decode(&tree, &iodes);

	TEST_ASSERT(SSL_OK == ret);
	TEST_ASSERT(tree != NULL);

	asn_item = tree->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(asn_item->tagnum == ASN_TAGNUM_SEQUENCE);
	TEST_ASSERT(asn_item->content != NULL);
	TEST_ASSERT(asn_item->size == 3);

	// version integer
	ret = asn_tree_query("[0]", tree, &asn_node);
	TEST_ASSERT(SSL_OK == ret);
	TEST_ASSERT(asn_node != NULL);
	asn_item = asn_node->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(asn_item->tagnum == ASN_TAGNUM_INT);
	TEST_ASSERT(asn_item->content != NULL);

	// privateKeyAlgorithm sequence
	ret = asn_tree_query("[1]", tree, &asn_node);
	TEST_ASSERT(SSL_OK == ret);
	TEST_ASSERT(asn_node != NULL);
	asn_item = asn_node->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(asn_item->tagnum == ASN_TAGNUM_SEQUENCE);
	TEST_ASSERT(asn_item->content != NULL);

	// privateKey octet string which encapsulates sequence
	ret = asn_tree_query("[2]", tree, &asn_node);
	TEST_ASSERT(SSL_OK == ret);
	TEST_ASSERT(asn_node != NULL);
	asn_item = asn_node->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(asn_item->tagnum == ASN_TAGNUM_OCTET_STRING);
	TEST_ASSERT(asn_item->content != NULL);

	t_ostring	temp_ostring;

	temp_ostring.content = asn_item->content;
	temp_ostring.size = asn_item->size;

	if (SSL_OK != io_osbuf(&iodes, IO_READ, &temp_ostring)) {
		TEST_LOG(ERROR, "failed to set io buffer");
		return (SSL_ERR);
	}

	// privateKey sequence
	ret = der_decode(&tree, &iodes);

	TEST_ASSERT(SSL_OK == ret);
	TEST_ASSERT(tree != NULL);

	asn_item = tree->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(asn_item->tagnum == ASN_TAGNUM_SEQUENCE);
	TEST_ASSERT(asn_item->content != NULL);
	TEST_ASSERT(asn_item->size == 9);

	size_t size = asn_item->size;

	for (size_t i = 0; i < size; i++) {
		char *query;

		ft_sprintf(&query, "[%zu]", i);

		ret = asn_tree_query(query, tree, &asn_node);
		TEST_ASSERT(SSL_OK == ret);

		asn_item = asn_node->content;
		TEST_ASSERT(asn_item != NULL);
		TEST_ASSERT(SSL_FLAG(ASN_ENCODE_PRIMITIVE, asn_item->tag));
		TEST_ASSERT(asn_item->tagnum == ASN_TAGNUM_INT);
		TEST_ASSERT(asn_item->content != NULL);

		bnum_init(&num);
		bnum_from_bytes_u(&num, asn_item->content, asn_item->size);

		TEST_ASSERT(bnum_cmp(&num, private_key_num[i]) == 0);
	}

	TEST_PASS();
}
