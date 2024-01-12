#include <ft_ssl.h>
#include <libft.h>
#include <string.h>
#include <ssl_test.h>
#include <inttypes.h>
#include <ctype.h>

static char 	*__s1;
static char 	*__s2;
static char 	*__s3;
static size_t	__slen1;
static size_t	__slen2;
static size_t	__slen3;

static int __test_libft_setup(void);
static int __test_ft_strlen(void);
static int __test_ft_strcmp(void);
static int __test_ft_strncmp(void);
static int __test_ft_strcpy(void);
static int __test_ft_strncpy(void);
static int __test_ft_strcat(void);
static int __test_ft_strncat(void);
static int __test_ft_strdup(void);
static int __test_ft_strsub(void);
static int __test_ft_strchr(void);
static int __test_ft_strrchr(void);
static int __test_ft_strjoin(void);
static int __test_ft_strrev(void);
static int __test_ft_strlow(void);
static int __test_ft_strup(void);
static int __test_ft_strsplit(void);
static int __test_ft_2darray_len(void);
static int __test_ft_2darray_strjoin(void);
static int __test_ft_atoi(void);
static int __test_ft_memcmp(void);
static int __test_ft_memcpy(void);
static int __test_ft_memdup(void);
static int __test_ft_memmove(void);
static int __test_ft_memset(void);
static int __test_ft_bzero(void);
static int __test_ft_memzcpy(void);
static int __test_ft_itoa_base(void);
static int __test_ft_itoa(void);
static int __test_ft_printf(void);
static int __test_ft_str_isalnum(void);
static int __test_ft_str_isalpha(void);
static int __test_ft_str_isascii(void);
static int __test_ft_str_isdigit(void);
static int __test_ft_str_ishex(void);
static int __test_ft_str_isprint(void);
static int __test_ft_binhex(void);
static int __test_ft_hexbin(void);
static int __test_ft_intbytes(void);
static int __test_get_next_line(void);
static int __test_ft_node(void);
static int __test_ft_list(void);
static int __test_ft_stack(void);
static int __test_ft_queue(void);
static int __test_ft_htbl(void);
static int __test_ft_htbl_rawkey(void);
static int __test_ft_htbl_conversion(void);
static int __test_ft_ntree(void);

int test_libft(void)
{
	int	res = SSL_OK;

	if (SSL_OK != __test_libft_setup())
		return (SSL_ERROR(UNSPECIFIED_ERROR));

	res |= __test_ft_strlen();
	res |= __test_ft_strcmp();
	res |= __test_ft_strncmp();
	res |= __test_ft_strcpy();
	res |= __test_ft_strncpy();
	res |= __test_ft_strcat();
	res |= __test_ft_strncat();
	res |= __test_ft_strdup();
	res |= __test_ft_strsub();
	res |= __test_ft_strchr();
	res |= __test_ft_strrchr();
	res |= __test_ft_strjoin();
	res |= __test_ft_strrev();
	res |= __test_ft_strlow();
	res |= __test_ft_strup();
	res |= __test_ft_2darray_len();
	res |= __test_ft_2darray_strjoin();
	res |= __test_ft_strsplit();
	res |= __test_ft_atoi();
	res |= __test_ft_memcmp();
	res |= __test_ft_memcpy();
	res |= __test_ft_memdup();
	res |= __test_ft_memmove();
	res |= __test_ft_memset();
	res |= __test_ft_bzero();
	res |= __test_ft_memzcpy();
	res |= __test_ft_itoa_base();
	res |= __test_ft_itoa();
	res |= __test_ft_printf();
	res |= __test_ft_str_isalnum();
	res |= __test_ft_str_isalpha();
	res |= __test_ft_str_isascii();
	res |= __test_ft_str_isdigit();
	res |= __test_ft_str_ishex();
	res |= __test_ft_str_isprint();
	res |= __test_ft_binhex();
	res |= __test_ft_hexbin();
	res |= __test_ft_intbytes();
	res |= __test_get_next_line();
	res |= __test_ft_node();
	res |= __test_ft_list();
	res |= __test_ft_stack();
	res |= __test_ft_queue();
	res |= __test_ft_htbl();
	res |= __test_ft_htbl_rawkey();
	res |= __test_ft_htbl_conversion();
	res |= __test_ft_ntree();

	return (res);
}

static int	__test_libft_setup(void)
{
	// all strings contain unique words (case is important)

	__s1 = "Cernantur iis sunt, voluptate export nulla arbitror noster.";

	__s2 = "Se Nescius exercitation. Malis nescius o consectetur \
		Est Excepteur multos dolor excepteur quae id ut aute incididunt \
		domesticarum eu malis a voluptate ubi ita esse pariatur offendit de \
		quamquam quid qui commodo cohaerescant si do elit incurreret singulis, \
		est Ita Consectetur.";

	__s3 = "Ut veniam constias deserunt, nisi cupidatat est esse cillum.";

	__slen1 = strlen(__s1);
	__slen2 = strlen(__s2);
	__slen3 = strlen(__s3);

	return (SSL_OK);
}

static int	__test_ft_strlen(void)
{
	size_t	test_value;
	size_t	ref_value;
	int		pass = SSL_OK;

	ref_value = strlen(__s1);
	test_value = ft_strlen(__s1);

	pass |= TEST_ASSERT(test_value == ref_value);
	pass |= TEST_ASSERT(ft_strlen("") == 0);
	pass |= TEST_ASSERT(ft_strlen(NULL) == 0);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_ft_strcmp(void)
{
	char	*ref_str = "elevator";
	int		pass = SSL_OK;

	pass |= TEST_ASSERT(ft_strcmp(ref_str, ref_str) == 0);
	pass |= TEST_ASSERT(ft_strcmp(ref_str, "") > 0);
	pass |= TEST_ASSERT(ft_strcmp("", ref_str) < 0);
	pass |= TEST_ASSERT(ft_strcmp("", "") == 0);
	pass |= TEST_ASSERT(ft_strcmp(ref_str, "not elevator") < 0);
	pass |= TEST_ASSERT(ft_strcmp("not elevator", ref_str) > 0);
	pass |= TEST_ASSERT(ft_strcmp(ref_str, "elevators") < 0);
	pass |= TEST_ASSERT(ft_strcmp("elevators", ref_str) > 0);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_ft_strncmp(void)
{
	char	*ref_str = "elevator";
	size_t	ref_size = strlen(ref_str);
	size_t	test_size = ref_size / 2;
	int		pass = SSL_OK;

	pass |= TEST_ASSERT(ft_strncmp(ref_str, ref_str, ref_size) == 0);
	pass |= TEST_ASSERT(ft_strncmp(ref_str, ref_str, test_size) == 0);
	pass |= TEST_ASSERT(ft_strncmp(ref_str, ref_str, 2 * ref_size) == 0);
	pass |= TEST_ASSERT(ft_strncmp(ref_str, "", test_size) > 0);
	pass |= TEST_ASSERT(ft_strncmp("", ref_str, test_size) < 0);
	pass |= TEST_ASSERT(ft_strncmp("", "", test_size) == 0);
	pass |= TEST_ASSERT(ft_strncmp("", "", 0) == 0);
	pass |= TEST_ASSERT(ft_strncmp(ref_str, "not elevator", test_size) < 0);
	pass |= TEST_ASSERT(ft_strncmp("not elevator", ref_str, test_size) > 0);
	pass |= TEST_ASSERT(ft_strncmp(ref_str, "elevators", ref_size) == 0);
	pass |= TEST_ASSERT(ft_strncmp("elevators", ref_str, ref_size) == 0);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_ft_strcpy(void)
{
	size_t	total_len = __slen1 + __slen2;
	char	test_str[total_len+1];
	char	ref_str[total_len+1];
	char	*temp_str;
	int		pass = SSL_OK;

	strcpy(ref_str, __s1);
	strcpy(ref_str + __slen1, __s2);

	ft_strcpy(test_str, __s1);
	ft_strcpy(test_str + __slen1, __s2);

	pass |= TEST_ASSERT(!strcmp(test_str, ref_str));

	ft_strcpy(test_str, "");

	pass |= TEST_ASSERT(strlen(test_str) == 0);

	bzero(test_str, total_len+1);
	ft_strcpy(test_str, __s1);
	ft_strcpy(test_str + __slen1, NULL);

	pass |= TEST_ASSERT(strlen(test_str) == __slen1);

	temp_str = NULL;
	temp_str = ft_strcpy(temp_str, __s1);

	pass |= TEST_ASSERT(NULL == temp_str);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_ft_strncpy(void)
{
	size_t	total_len = __slen1 + __slen2;
	size_t	len = __slen2 /2;
	char	test_str[total_len+1];
	char	ref_str[total_len+1];
	char	*temp_str;
	int		pass = SSL_OK;

	bzero(ref_str, total_len+1);

	strcpy(ref_str, __s1);
	strncpy(ref_str + __slen1, __s2, len);

	strcpy(test_str, __s1);
	ft_strncpy(test_str + __slen1, __s2, len);

	pass |= TEST_ASSERT(!strcmp(test_str, ref_str));

	ft_strncpy(test_str, __s2, 0);

	pass |= TEST_ASSERT(strlen(test_str) == 0);

	strcpy(test_str, __s1);
	ft_strncpy(test_str + __slen1, __s2, 2 * __slen2);

	pass |= TEST_ASSERT(strlen(test_str) == total_len);

	strcpy(test_str, __s1);
	ft_strncpy(test_str + __slen1, NULL, len);

	pass |= TEST_ASSERT(strlen(test_str) == __slen1);

	temp_str = NULL;
	temp_str = ft_strncpy(temp_str, __s1, len);

	pass |= TEST_ASSERT(NULL == temp_str);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_ft_strcat(void)
{
	size_t	total_len = __slen1 + __slen2;
	char	test_str[total_len+1];
	char	ref_str[total_len+1];
	char	*temp_str;
	int		pass = SSL_OK;


	strcpy(ref_str, __s1);
	strcat(ref_str, __s2);

	strcpy(test_str, __s1);
	ft_strcat(test_str, __s2);

	pass |= TEST_ASSERT(!strcmp(test_str, ref_str));

	ft_strcat(test_str, "");

	pass |= TEST_ASSERT(strlen(test_str) == total_len);

	strcpy(test_str, __s1);
	ft_strcat(test_str, NULL);

	pass |= TEST_ASSERT(strlen(test_str) == __slen1);

	temp_str = NULL;
	temp_str = ft_strcat(temp_str, __s1);

	pass |= TEST_ASSERT(NULL == temp_str);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_ft_strncat(void)
{
	size_t	total_len = __slen1 + __slen2;
	size_t 	len = __slen2 / 2;
	char 	test_str[total_len + 1];
	char	ref_str[total_len+1];
	char	*temp_str;
	int		pass = SSL_OK;


	strcpy(ref_str, __s1);
	strncpy(ref_str + __slen1, __s2, len);
	ref_str[__slen1 + len] = 0;

	strcpy(test_str, __s1);
	ft_strncat(test_str, __s2, len);

	pass |= TEST_ASSERT(!strcmp(test_str, ref_str));

	ft_strncat(test_str, "", len);

	pass |= TEST_ASSERT(strlen(test_str) == __slen1 + len);

	strcpy(test_str, __s1);
	ft_strncat(test_str, NULL, len);

	pass |= TEST_ASSERT(strlen(test_str) == __slen1);

	temp_str = NULL;
	temp_str = ft_strncat(temp_str, __s2, len);

	pass |= TEST_ASSERT(NULL == temp_str);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_strdup(void)
{
	char *test_str;
	char *ref_str;
	int pass = SSL_OK;

	ref_str = strdup(__s1);
	test_str = ft_strdup(__s1);
	pass |= TEST_ASSERT(!strcmp(test_str, ref_str));
	free(test_str);

	test_str = ft_strdup("");
	pass |= TEST_ASSERT(strlen(test_str) == 0);
	free(test_str);

	test_str = ft_strdup(NULL);
	pass |= TEST_ASSERT(NULL == test_str);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_strsub(void)
{
	char *test_str;
	size_t offset = __slen2 / 5;
	size_t len = MIN(10, __slen2 - offset);
	char ref_str[len + 1];
	int pass = SSL_OK;

	bzero(ref_str, sizeof(ref_str));
	strncpy(ref_str, __s2 + offset, len);
	test_str = ft_strsub(__s2, offset, len);
	pass |= TEST_ASSERT(!strcmp(test_str, ref_str));
	free(test_str);

	test_str = ft_strsub(__s2, offset, 0);
	pass |= TEST_ASSERT(strlen(test_str) == 0);
	free(test_str);

	test_str = ft_strsub(NULL, offset, len);
	pass |= TEST_ASSERT(NULL == test_str);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_strchr(void)
{
	char *test_str = "apple";
	char *res = NULL;
	int pass = SSL_OK;

	res = ft_strchr(NULL, 'e');
	pass |= TEST_ASSERT(res == NULL);

	res = ft_strchr(test_str, 'z');
	pass |= TEST_ASSERT(res == NULL);

	res = ft_strchr("", 'e');
	pass |= TEST_ASSERT(res == NULL);

	res = ft_strchr(test_str, 'e');
	pass |= TEST_ASSERT(res != NULL);
	pass |= TEST_ASSERT(!strcmp(res, "e"));

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_strrchr(void)
{
	char *test_str = "apple";
	char *res = NULL;
	int pass = SSL_OK;

	res = ft_strrchr(NULL, 'e');
	pass |= TEST_ASSERT(res == NULL);

	res = ft_strrchr(test_str, 'z');
	pass |= TEST_ASSERT(res == NULL);

	res = ft_strrchr("", 'e');
	pass |= TEST_ASSERT(res == NULL);

	res = ft_strrchr(test_str, 'e');
	pass |= TEST_ASSERT(res != NULL);
	pass |= TEST_ASSERT(!strcmp(res, "e"));

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_strjoin(void)
{
	size_t total_len = __slen1 + __slen2;
	char *test_str = NULL;
	char ref_str[total_len + 1];
	char *temp_str;
	int pass = SSL_OK;

	strcpy(ref_str, __s1);
	strcat(ref_str, __s2);

	test_str = ft_strjoin(__s1, __s2);

	pass |= TEST_ASSERT(test_str != NULL);
	pass |= TEST_ASSERT(!strcmp(test_str, ref_str));
	free(test_str);

	test_str = ft_strjoin(NULL, NULL);
	pass |= TEST_ASSERT(test_str == NULL);

	test_str = ft_strjoin(__s1, "");
	pass |= TEST_ASSERT(strlen(test_str) == __slen1);
	free(test_str);

	test_str = ft_strjoin("", __s1);
	pass |= TEST_ASSERT(strlen(test_str) == __slen1);
	free(test_str);

	test_str = ft_strjoin(__s1, NULL);
	pass |= TEST_ASSERT(strlen(test_str) == __slen1);
	free(test_str);

	test_str = ft_strjoin(NULL, __s1);
	pass |= TEST_ASSERT(strlen(test_str) == __slen1);
	free(test_str);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_strrev(void)
{
	char *test_str = "apple";
	char *ref_str = "elppa";
	char *res = NULL;
	int pass = SSL_OK;

	res = ft_strrev(NULL);
	pass |= TEST_ASSERT(res == NULL);

	res = ft_strrev(test_str);
	pass |= TEST_ASSERT(!strcmp(res, ref_str));
	free(res);

	res = ft_strrev("");
	pass |= TEST_ASSERT(strlen(res) == 0);
	free(res);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_strlow(void)
{
	char *str = "ApPl!e# !@#$@FSDdf234523#$%^$%&\n";
	char *test_str;
	char *ref_str;
	int pass = SSL_OK;

	ref_str = strdup(str);

	for (int i = 0; i < strlen(ref_str); i++)
		ref_str[i] = tolower(ref_str[i]);

	test_str = ft_strlow(NULL);
	pass |= TEST_ASSERT(test_str == NULL);

	test_str = ft_strlow(str);
	pass |= TEST_ASSERT(strcmp(test_str, ref_str) == 0);

	free(test_str);
	free(ref_str);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_strup(void)
{
	char *str = "ApPl!e# !@#$@FsDdf234523#$%^$%&\n";
	char *test_str;
	char *ref_str;
	int pass = SSL_OK;

	ref_str = strdup(str);

	for (int i = 0; i < strlen(ref_str); i++)
		ref_str[i] = toupper(ref_str[i]);

	test_str = ft_strup(NULL);
	pass |= TEST_ASSERT(test_str == NULL);

	test_str = ft_strup(str);
	pass |= TEST_ASSERT(strcmp(test_str, ref_str) == 0);

	free(test_str);
	free(ref_str);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_2darray_len(void)
{
	char *test_str_arr[] = {
		"apple",
		"banana",
		"orange",
		NULL
	};
	size_t test_str_arr_len = sizeof(test_str_arr) / sizeof(char *) - 1;
	void *null_ptr = NULL;
	int res = 0;
	int pass = SSL_OK;

	res = ft_2darray_len_null_terminated(NULL);
	pass |= TEST_ASSERT(res == 0);

	res = ft_2darray_len_null_terminated((void **)test_str_arr);
	pass |= TEST_ASSERT(res == test_str_arr_len);

	res = ft_2darray_len_null_terminated(&null_ptr);
	pass |= TEST_ASSERT(res == 0);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_2darray_strjoin(void)
{
	char *test_str_arr[] = {
		"apple",
		"banana",
		"orange",
		"vodka"
	};
	size_t test_str_arr_len = sizeof(test_str_arr) / sizeof(char *);
	char *ref_str = "apple banana orange vodka";
	char *ref_str2 = "applebananaorangevodka";
	char *res = NULL;
	int pass = SSL_OK;

	res = ft_2darray_strjoin(NULL, test_str_arr_len, " ");
	pass |= TEST_ASSERT(res == NULL);

	res = ft_2darray_strjoin(test_str_arr, 0, " ");
	pass |= TEST_ASSERT(res == NULL);

	res = ft_2darray_strjoin(test_str_arr, test_str_arr_len, " ");
	pass |= TEST_ASSERT(res != NULL);
	pass |= TEST_ASSERT(!strcmp(res, ref_str));
	free(res);

	res = ft_2darray_strjoin(test_str_arr, test_str_arr_len, "");
	pass |= TEST_ASSERT(res != NULL);
	pass |= TEST_ASSERT(!strcmp(res, ref_str2));
	free(res);

	res = ft_2darray_strjoin(test_str_arr, 1, " ");
	pass |= TEST_ASSERT(!strcmp(res, "apple"));
	free(res);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_strsplit(void)
{
	char *test_str = "apple banana orange vodka";
	char *ref_str_arr[] = {
		"apple",
		"banana",
		"orange",
		"vodka"
	};
	size_t ref_str_len = sizeof(ref_str_arr) / sizeof(char *);
	char **res = NULL;
	int pass = SSL_OK;

	res = ft_strsplit(NULL, ' ');
	pass |= TEST_ASSERT(res == NULL);

	res = ft_strsplit(test_str, ' ');
	pass |= TEST_ASSERT(res != NULL);
	pass |= TEST_ASSERT(ref_str_len == ft_2darray_len_null_terminated((void **)res));

	for (int i = 0; i < ref_str_len; i++)
		pass |= TEST_ASSERT(!strcmp(res[i], ref_str_arr[i]));

	ft_2darray_del_null_terminated((void **)res);

	res = ft_strsplit("apple", ' ');
	pass |= TEST_ASSERT(res != NULL);
	pass |= TEST_ASSERT(ft_2darray_len_null_terminated((void **)res) == 1);

	pass |= TEST_ASSERT(!strcmp(res[0], "apple"));

	ft_2darray_del_null_terminated((void **)res);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_atoi(void)
{
	long long res = 0;
	int pass = SSL_OK;

	res = ft_atoi("0");
	pass |= TEST_ASSERT(res == 0);

	res = ft_atoi("+0");
	pass |= TEST_ASSERT(res == 0);

	res = ft_atoi("-0");
	pass |= TEST_ASSERT(res == 0);

	res = ft_atoi("");
	pass |= TEST_ASSERT(res == 0);

	res = ft_atoi("12316235");
	pass |= TEST_ASSERT(res == 12316235);

	res = ft_atoi("12316aa235");
	pass |= TEST_ASSERT(res == 12316);

	res = ft_atoi("aa235");
	pass |= TEST_ASSERT(res == 0);

	res = ft_atoi("123-235");
	pass |= TEST_ASSERT(res == 123);

	res = ft_atoi("123+235");
	pass |= TEST_ASSERT(res == 123);

	res = ft_atoi("+123235");
	pass |= TEST_ASSERT(res == 123235);

	res = ft_atoi("-123235");
	pass |= TEST_ASSERT(res == -123235);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_memcmp(void)
{
	char *ref_str = "elevator";
	size_t ref_str_len = strlen(ref_str);
	int pass = SSL_OK;

	pass |= TEST_ASSERT(ft_memcmp(ref_str, ref_str, ref_str_len) == 0);
	pass |= TEST_ASSERT(ft_memcmp(ref_str, "", 1) > 0);
	pass |= TEST_ASSERT(ft_memcmp("", ref_str, 1) < 0);
	pass |= TEST_ASSERT(ft_memcmp("", "", 1) == 0);
	pass |= TEST_ASSERT(ft_memcmp(ref_str, "not elevator", ref_str_len) < 0);
	pass |= TEST_ASSERT(ft_memcmp("not elevator", ref_str, ref_str_len) > 0);
	pass |= TEST_ASSERT(ft_memcmp(ref_str, "elevators", ref_str_len) == 0);
	pass |= TEST_ASSERT(ft_memcmp("elevators", ref_str, ref_str_len) == 0);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_memcpy(void)
{
	size_t total_len = __slen1 + __slen2;
	char test_str[total_len];
	char ref_str[total_len];
	char *temp_str;
	int pass = SSL_OK;

	memcpy(ref_str, __s1, __slen1);
	memcpy(ref_str + __slen1, __s2, __slen2);

	ft_memcpy(test_str, __s1, __slen1);
	ft_memcpy(test_str + __slen1, __s2, __slen2);
	pass |= TEST_ASSERT(!memcmp(test_str, ref_str, total_len));

	ft_memcpy(test_str, "", 0);
	pass |= TEST_ASSERT(!memcmp(test_str, ref_str, total_len));

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_memdup(void)
{
	char *test_str;
	int pass = SSL_OK;

	test_str = ft_memdup(__s1, __slen1);
	pass |= TEST_ASSERT(!memcmp(test_str, __s1, __slen1));
	free(test_str);

	test_str = ft_memdup(__s1, 0);
	pass |= TEST_ASSERT(NULL != test_str);
	free(test_str);

	test_str = ft_memdup(NULL, __slen1);
	pass |= TEST_ASSERT(NULL == test_str);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_memmove(void)
{
	char test_str[__slen2];
	char ref_str[__slen2];
	size_t offset = __slen2 / 4;
	size_t len = __slen2 / 2;
	int pass = SSL_OK;

	memcpy(ref_str, __s2, __slen2);
	memcpy(test_str, __s2, __slen2);

	memmove(ref_str, ref_str + offset, len);
	ft_memmove(test_str, test_str + offset, len);
	pass |= TEST_ASSERT(!memcmp(test_str, ref_str, __slen2));

	memmove(ref_str + offset, ref_str, len);
	ft_memmove(test_str + offset, test_str, len);
	pass |= TEST_ASSERT(!memcmp(test_str, ref_str, __slen2));

	memmove(ref_str + offset, ref_str, 0);
	ft_memmove(test_str + offset, test_str, 0);
	pass |= TEST_ASSERT(!memcmp(test_str, ref_str, __slen2));

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_memset(void)
{
	char test_str[__slen2];
	char ref_str[__slen2];
	size_t len = __slen2 / 2;
	int pass = SSL_OK;

	memcpy(ref_str, __s2, __slen2);
	memcpy(test_str, __s2, __slen2);

	memset(ref_str, 190, len);
	ft_memset(test_str, 190, len);
	pass |= TEST_ASSERT(!memcmp(test_str, ref_str, __slen2));

	memset(ref_str, -94, len);
	ft_memset(test_str, -94, len);
	pass |= TEST_ASSERT(!memcmp(test_str, ref_str, __slen2));

	memset(ref_str, 123456, len);
	ft_memset(test_str, 123456, len);
	pass |= TEST_ASSERT(!memcmp(test_str, ref_str, __slen2));

	memset(ref_str, -123456, len);
	ft_memset(test_str, -123456, len);
	pass |= TEST_ASSERT(!memcmp(test_str, ref_str, __slen2));

	memset(ref_str, 190, (0));
	ft_memset(test_str, 190, 0);
	pass |= TEST_ASSERT(!memcmp(test_str, ref_str, __slen2));

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_bzero(void)
{
	char test_str[32];
	char ref_str[32] = {0};
	int pass = SSL_OK;

	memset(test_str, 127, sizeof(test_str));
	ft_bzero(test_str, sizeof(test_str));
	pass |= TEST_ASSERT(memcmp(test_str, ref_str, sizeof(test_str)) == 0);

	memset(test_str, 127, sizeof(test_str));
	memset(ref_str, 127, sizeof(ref_str));
	ft_bzero(test_str, 0);
	pass |= TEST_ASSERT(memcmp(test_str, ref_str, sizeof(test_str)) == 0);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_memzcpy(void)
{
	char test_str[__slen2];
	char zero_str[__slen2];
	size_t len = __slen2 / 2;
	int pass = SSL_OK;

	memset(zero_str, 0, sizeof(zero_str));

	memcpy(test_str, __s2, __slen2);
	ft_memzcpy(test_str, __s2, len, len / 2);
	pass |= TEST_ASSERT(!memcmp(test_str, __s2, len / 2));
	pass |= TEST_ASSERT(!memcmp(test_str + len / 2, zero_str, len - len / 2));

	memcpy(test_str, __s2, __slen2);
	ft_memzcpy(test_str, zero_str, len / 2, len);
	pass |= TEST_ASSERT(!memcmp(test_str, zero_str, len / 2));
	pass |= TEST_ASSERT(!memcmp(test_str + len / 2, __s2 + len / 2, len - len / 2));

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_itoa_base(void)
{
	char *res = NULL;
	intmax_t	ref_num;
	int pass = SSL_OK;

	res = ft_itoa_base(0, 10);
	pass |= TEST_ASSERT(strcmp(res, "0") == 0);
	free(res);

	res = ft_itoa_base(0, 16);
	pass |= TEST_ASSERT(strcmp(res, "0") == 0);
	free(res);

	res = ft_itoa_base(1234, 10);
	pass |= TEST_ASSERT(strcmp(res, "1234") == 0);
	free(res);

	res = ft_itoa_base(-1234, 10);
	pass |= TEST_ASSERT(strcmp(res, "-1234") == 0);
	free(res);

	res = ft_itoa_base(-255, 16);
	pass |= TEST_ASSERT(strcmp(res, "ff") == 0);
	free(res);

	res = ft_itoa_base(255, 16);
	pass |= TEST_ASSERT(strcmp(res, "ff") == 0);
	free(res);

	res = ft_itoa_base(255, 2);
	pass |= TEST_ASSERT(strcmp(res, "11111111") == 0);
	free(res);

	res = ft_itoa_base(255, 8);
	pass |= TEST_ASSERT(strcmp(res, "377") == 0);
	free(res);

	res = ft_itoa_base(255, 12);
	pass |= TEST_ASSERT(strcmp(res, "193") == 0);
	free(res);

	res = ft_itoa_base(INTMAX_MAX, 10);
	ref_num = strtoimax(res, NULL, 10);
	pass |= TEST_ASSERT(ref_num == INTMAX_MAX);
	free(res);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_itoa(void)
{
	char *res = NULL;
	intmax_t	ref_num;
	int pass = SSL_OK;

	res = ft_itoa(0);
	pass |= TEST_ASSERT(strcmp(res, "0") == 0);
	free(res);

	res = ft_itoa(1234);
	pass |= TEST_ASSERT(strcmp(res, "1234") == 0);
	free(res);

	res = ft_itoa(-1234);
	pass |= TEST_ASSERT(strcmp(res, "-1234") == 0);
	free(res);

	res = ft_itoa(INTMAX_MAX);
	ref_num = strtoimax(res, NULL, 10);
	pass |= TEST_ASSERT(ref_num == INTMAX_MAX);
	free(res);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_printf(void)
{
	char *test_str = NULL;
	char *ref_str = NULL;
	void *ptr;
	int	test_num;
	int	ref_num;
	int pass = SSL_OK;

	test_num = ft_sprintf(&test_str, "%s", "apple\n");
	ref_num = asprintf(&ref_str, "%s", "apple\n");
	pass |= TEST_ASSERT(strcmp(test_str, ref_str) == 0);
	pass |= TEST_ASSERT(test_num == ref_num);
	free(test_str);
	free(ref_str);

	test_num = ft_sprintf(&test_str, "%.2s", "apple\n");
	ref_num = asprintf(&ref_str, "%.2s", "apple\n");
	pass |= TEST_ASSERT(strcmp(test_str, ref_str) == 0);
	pass |= TEST_ASSERT(test_num == ref_num);
	free(test_str);
	free(ref_str);

	test_num = ft_sprintf(&test_str, "%9s", "apple\n");
	ref_num = asprintf(&ref_str, "%9s", "apple\n");
	pass |= TEST_ASSERT(strcmp(test_str, ref_str) == 0);
	pass |= TEST_ASSERT(test_num == ref_num);
	free(test_str);
	free(ref_str);

	test_num = ft_sprintf(&test_str, "%d", 1244);
	ref_num = asprintf(&ref_str, "%d", 1244);
	pass |= TEST_ASSERT(strcmp(test_str, ref_str) == 0);
	pass |= TEST_ASSERT(test_num == ref_num);
	free(test_str);
	free(ref_str);

	test_num = ft_sprintf(&test_str, "%ld", LONG_MAX);
	ref_num = asprintf(&ref_str, "%ld", LONG_MAX);
	pass |= TEST_ASSERT(strcmp(test_str, ref_str) == 0);
	pass |= TEST_ASSERT(test_num == ref_num);
	free(test_str);
	free(ref_str);

	test_num = ft_sprintf(&test_str, "%lld", LLONG_MAX);
	ref_num = asprintf(&ref_str, "%lld", LLONG_MAX);
	pass |= TEST_ASSERT(strcmp(test_str, ref_str) == 0);
	pass |= TEST_ASSERT(test_num == ref_num);
	free(test_str);
	free(ref_str);

	test_num = ft_sprintf(&test_str, "%u", 1244u);
	ref_num = asprintf(&ref_str, "%u", 1244u);
	pass |= TEST_ASSERT(strcmp(test_str, ref_str) == 0);
	pass |= TEST_ASSERT(test_num == ref_num);
	free(test_str);
	free(ref_str);

	test_num = ft_sprintf(&test_str, "%lu", ULONG_MAX);
	ref_num = asprintf(&ref_str, "%lu", ULONG_MAX);
	pass |= TEST_ASSERT(strcmp(test_str, ref_str) == 0);
	pass |= TEST_ASSERT(test_num == ref_num);
	free(test_str);
	free(ref_str);

	test_num = ft_sprintf(&test_str, "%llu", ULLONG_MAX);
	ref_num = asprintf(&ref_str, "%llu", ULLONG_MAX);
	pass |= TEST_ASSERT(strcmp(test_str, ref_str) == 0);
	pass |= TEST_ASSERT(test_num == ref_num);
	free(test_str);
	free(ref_str);

	test_num = ft_sprintf(&test_str, "%zu", SIZE_T_MAX);
	ref_num = asprintf(&ref_str, "%zu", SIZE_T_MAX);
	pass |= TEST_ASSERT(strcmp(test_str, ref_str) == 0);
	pass |= TEST_ASSERT(test_num == ref_num);
	free(test_str);
	free(ref_str);

	test_num = ft_sprintf(&test_str, "%x", 0x1244);
	ref_num = asprintf(&ref_str, "%x", 0x1244);
	pass |= TEST_ASSERT(strcmp(test_str, ref_str) == 0);
	pass |= TEST_ASSERT(test_num == ref_num);
	free(test_str);
	free(ref_str);

	test_num = ft_sprintf(&test_str, "%p", ptr);
	ref_num = asprintf(&ref_str, "%p", ptr);
	pass |= TEST_ASSERT(strcmp(test_str, ref_str) == 0);
	pass |= TEST_ASSERT(test_num == ref_num);
	free(test_str);
	free(ref_str);

	test_num = ft_sprintf(&test_str, "%o", 01244);
	ref_num = asprintf(&ref_str, "%o", 01244);
	pass |= TEST_ASSERT(strcmp(test_str, ref_str) == 0);
	pass |= TEST_ASSERT(test_num == ref_num);
	free(test_str);
	free(ref_str);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_str_isalpha(void)
{
	int res;
	int pass = SSL_OK;

	res = ft_str_isalpha(NULL);
	pass |= TEST_ASSERT(res == 0);

	res = ft_str_isalpha("abcASDFacwqer31");
	pass |= TEST_ASSERT(res == 0);

	res = ft_str_isalpha("abASDcqwe");
	pass |= TEST_ASSERT(res == 1);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_str_isalnum(void)
{
	int res;
	int pass = SSL_OK;

	res = ft_str_isalnum(NULL);
	pass |= TEST_ASSERT(res == 0);

	res = ft_str_isalnum("abcAS2345123DFacwqer31");
	pass |= TEST_ASSERT(res == 1);

	res = ft_str_isalnum("abASDc#$%^qwe");
	pass |= TEST_ASSERT(res == 0);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_str_isascii(void)
{
	int res;
	int pass = SSL_OK;
	char test_str[] = {255, 127, 44, 45, 0};
	char test_str2[] = {2, 22, 55, 127, 44, 45, 0};

	res = ft_str_isascii(NULL);
	pass |= TEST_ASSERT(res == 0);

	res = ft_str_isascii(test_str);
	pass |= TEST_ASSERT(res == 0);

	res = ft_str_isascii(test_str2);
	pass |= TEST_ASSERT(res == 1);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_str_isdigit(void)
{
	int res;
	int pass = SSL_OK;

	res = ft_str_isdigit(NULL);
	pass |= TEST_ASSERT(res == 0);

	res = ft_str_isdigit("abcAS2345123DFacwqer31");
	pass |= TEST_ASSERT(res == 0);

	res = ft_str_isdigit("0123456789");
	pass |= TEST_ASSERT(res == 1);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_str_ishex(void)
{
	int res;
	int pass = SSL_OK;

	res = ft_str_ishex(NULL);
	pass |= TEST_ASSERT(res == 0);

	res = ft_str_ishex("1234123sczxvz");
	pass |= TEST_ASSERT(res == 0);

	res = ft_str_ishex("0123456789ABCDEFabcdef");
	pass |= TEST_ASSERT(res == 1);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_str_isprint(void)
{
	int res;
	int pass = SSL_OK;
	char test_str[] = {9, 1, 2, 3, 15, 127, 44, 45, 0};

	res = ft_str_isprint(NULL);
	pass |= TEST_ASSERT(res == 0);

	res = ft_str_isprint(test_str);
	pass |= TEST_ASSERT(res == 0);

	res = ft_str_isprint("0123456789ABCDEFabcdef");
	pass |= TEST_ASSERT(res == 1);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_binhex(void)
{
	int pass = SSL_OK;
	char *test_str;
	char octets[] = {9, 1, 127, 3, 15, 0};
	char *ref_str = "9017f030f00";

	test_str = ft_binhex(NULL, 20);
	pass |= TEST_ASSERT(test_str == NULL);

	test_str = ft_binhex(octets, 0);
	pass |= TEST_ASSERT(test_str == NULL);

	test_str = ft_binhex(octets, sizeof(octets));
	pass |= TEST_ASSERT(strcmp(test_str, ref_str) == 0);
	free(test_str);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_hexbin(void)
{
	int pass = SSL_OK;
	char ref_str[6] = {9, 1, 127, 3, 15, 0};
	char test_str[6] = {0};
	char *hex = "9017f030f00";

	ft_hexbin(test_str, hex, strlen(hex));
	pass |= TEST_ASSERT(memcmp(test_str, ref_str, sizeof(test_str)) == 0);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_intbytes(void)
{
	intmax_t integer = 3271454934;
	intmax_t zero = 0;
	char ref_str[6] = {194, 254, 112, 214};
	char buf[4] = {0};
	int pass = SSL_OK;

	ft_intbytes(buf, integer, 4);
	pass |= ft_strncmp(buf, ref_str, sizeof(buf));

	bzero(buf, sizeof(buf));
	bzero(ref_str, sizeof(ref_str));
	ft_intbytes(buf, zero, 4);
	pass |= ft_strncmp(buf, ref_str, sizeof(buf));

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_get_next_line(void)
{
	int fd;
	FILE *ref_stream;
	const char *file = "test/testfiles/lines.txt";
	char *line = NULL;
	char *ref_line = NULL;
	ssize_t rbytes;
	ssize_t ref_rbytes;
	size_t linecapp = 0;
	int pass = SSL_OK;

	ref_stream = fopen(file, "r");
	fd = open(file, O_RDONLY);
	pass |= TEST_ASSERT(fd >= 0);
	pass |= TEST_ASSERT(ref_stream != NULL);

	if (fd < 0 || ref_stream == NULL)
		return (TEST_FAIL());

	while ((rbytes = get_next_line(fd, &line)) > 0)
	{
		ref_rbytes = getline(&ref_line, &linecapp, ref_stream);

		pass |= TEST_ASSERT(ref_rbytes > 0);
		pass |= TEST_ASSERT(ref_rbytes == rbytes);

		if (ref_line && line)
		{
			pass |= TEST_ASSERT(strlen(ref_line) == strlen(line));
			pass |= TEST_ASSERT(strcmp(ref_line, line) == 0);
		}

		if (NULL != ref_line)
			free(ref_line);

		if (NULL != line)
			free(line);

		ref_line = NULL;
		linecapp = 0;
		line = NULL;
	}

	close(fd);
	fclose(ref_stream);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_node(void)
{
	t_node *node;
	char *key = "some-key";
	char *content = "some-content";
	int pass = SSL_OK;

	node = ft_node_create();
	pass |= TEST_ASSERT(node != NULL);

	ft_node_del(node, NULL);

	node = ft_node_new(key, content, strlen(content));
	pass |= TEST_ASSERT(node != NULL);
	pass |= TEST_ASSERT(node->key != NULL);
	pass |= TEST_ASSERT(strcmp(node->key, key) == 0);
	pass |= TEST_ASSERT(node->content != NULL);
	pass |= TEST_ASSERT(strcmp(node->content, content) == 0);

	ft_node_del(node, NULL);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

// helper for __test_ft_list()
static int __func_for_test_ft_list(t_node *node, void *farg)
{
	if (NULL == node)
		return (LIBFT_OK);

	node->content = NULL;
	node->size = 0;

	return (LIBFT_OK);
}

static int __test_ft_list(void)
{
	char *node_key;
	t_node *node;
	t_node *tmp_node;
	t_node *lst;
	int lst_size;
	char **words;
	int	nwords;
	int idx;
	int pass = SSL_OK;
	int ret;

	// get words, assume all words are unique (required for some tests)
	words = ft_strsplit(__s2, ' ');
	pass |= TEST_ASSERT(words != NULL);

	nwords = ft_2darray_len_null_terminated((void **)words);

	// expect to have at least 6 words
	pass |= TEST_ASSERT(nwords >= 6);

	if (NULL == words || nwords == 0)
		return (TEST_PASS());

	lst = NULL;

	lst_size = ft_lst_size(lst);
	pass |= TEST_ASSERT(lst_size == 0);

	// test list append
	idx = 0;
	while (idx < nwords)
	{
		node_key = ft_itoa(idx);
		node = ft_node_new(node_key, words[idx], strlen(words[idx]));
		pass |= TEST_ASSERT(node != NULL);

		ft_lst_append(&lst, node);
		pass |= TEST_ASSERT(lst != NULL);

		if (node_key != NULL)
			free(node_key);

		idx++;
	}

	// check list size
	lst_size = ft_lst_size(lst);
	pass |= TEST_ASSERT(lst_size == nwords);

	// inspect list node content and order
	idx = 0;
	tmp_node = lst;
	while (tmp_node != NULL)
	{
		node_key = ft_itoa(idx);

		pass |= TEST_ASSERT(idx < nwords);
		pass |= TEST_ASSERT(strcmp(tmp_node->key, node_key) == 0);
		pass |= TEST_ASSERT(strcmp((char *)tmp_node->content, words[idx]) == 0);

		if (node_key != NULL)
			free(node_key);

		tmp_node = tmp_node->next;
		idx++;
	}

	// check list delete first node
	ft_lst_del_first(&lst, NULL);
	lst_size = ft_lst_size(lst);
	pass |= TEST_ASSERT(lst_size == nwords-1);
	pass |= TEST_ASSERT(strcmp((char *)lst->content, words[1]) == 0);

	// check list delete last node
	ft_lst_del_last(&lst, NULL);
	lst_size = ft_lst_size(lst);
	pass |= TEST_ASSERT(lst_size == nwords - 2);

	// get last node pointer
	tmp_node = lst;
	while (tmp_node != NULL)
	{
		node = tmp_node;
		tmp_node = tmp_node->next;
	}

	// check if last node has been deleted
	pass |= TEST_ASSERT(strcmp((char *)node->content, words[nwords - 2]) == 0);

	// delete all list nodes
	ft_lst_del(lst, NULL);
	lst = NULL;

	// test list prepend
	idx = 0;
	while (idx < nwords)
	{
		node_key = ft_itoa(idx);
		node = ft_node_new(node_key, words[idx], strlen(words[idx]));
		pass |= TEST_ASSERT(node != NULL);

		ft_lst_prepend(&lst, node);

		// test first node content
		pass |= TEST_ASSERT(lst != NULL);
		pass |= TEST_ASSERT(strcmp(lst->key, node_key) == 0);
		pass |= TEST_ASSERT(strcmp((char *)lst->content, words[idx]) == 0);

		if (node_key != NULL)
			free(node_key);

		idx++;
	}

	// check list size
	lst_size = ft_lst_size(lst);
	pass |= TEST_ASSERT(lst_size == nwords);

	// select some node
	tmp_node = lst;
	while (tmp_node != NULL && strcmp(tmp_node->content, words[3]))
		tmp_node = tmp_node->next;

	// expect to find node
	pass |= TEST_ASSERT(tmp_node != NULL);

	// test list delete node
	ft_lst_del_one(&lst, tmp_node, NULL);

	// search for deleted node
	tmp_node = lst;
	while (tmp_node != NULL && strcmp(tmp_node->content, words[3]))
		tmp_node = tmp_node->next;

	// do not expect to find node
	pass |= TEST_ASSERT(tmp_node == NULL);

	// test list map (func sets content to null and size to 0)
	ft_lst_map(lst, NULL, __func_for_test_ft_list);

	// test if nodes have changed
	tmp_node = lst;
	while (tmp_node != NULL)
	{
		pass |= TEST_ASSERT(tmp_node->content == NULL);
		pass |= TEST_ASSERT(tmp_node->size == 0);
		tmp_node = tmp_node->next;
	}

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_stack(void)
{
	t_stack *stack;
	t_node *node;
	void *content;
	char *key;
	char **words;
	int nwords;
	int idx;
	int pass = SSL_OK;

	stack = ft_stack_init();
	pass |= TEST_ASSERT(stack != NULL);
	pass |= TEST_ASSERT(ft_stack_is_empty(stack));
	pass |= TEST_ASSERT(ft_stack_size(stack) == 0);

	if (SSL_OK != pass)
		return (TEST_FAIL());

	words = ft_strsplit(__s2, ' ');
	nwords = ft_2darray_len_null_terminated((void **)words);
	pass |= TEST_ASSERT(words != NULL);
	pass |= TEST_ASSERT(nwords > 6);

	idx = 0;
	while (idx < nwords)
	{
		key = ft_itoa(idx);
		ft_stack_push(stack, key, words[idx], strlen(words[idx]));

		node = stack->top;
		pass |= TEST_ASSERT(node != NULL);
		pass |= TEST_ASSERT(strcmp(node->content, words[idx]) == 0);

		if (key != NULL)
			free(key);

		idx++;
	}

	pass |= TEST_ASSERT(ft_stack_size(stack) == nwords);

	node = ft_stack_peek(stack);
	pass |= TEST_ASSERT(node != NULL);
	pass |= TEST_ASSERT(!ft_stack_is_empty(stack));
	pass |= TEST_ASSERT(strcmp(node->content, words[nwords-1]) == 0);

	if (SSL_OK != pass)
		return (TEST_FAIL());

	while (!ft_stack_is_empty(stack))
	{
		if (--idx < 0)
			break ;

		content = ft_stack_pop(stack);
		pass |= TEST_ASSERT(strcmp(content, words[idx]) == 0);
	}

	pass |= TEST_ASSERT(ft_stack_is_empty(stack));
	pass |= TEST_ASSERT(ft_stack_size(stack) == 0);
	ft_stack_del(stack, NULL);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_queue(void)
{
	t_queue *queue;
	t_node *node;
	void *content;
	char *key;
	char **words;
	int nwords;
	int idx;
	int pass = SSL_OK;

	queue = ft_queue_init();
	pass |= TEST_ASSERT(queue != NULL);
	pass |= TEST_ASSERT(ft_queue_is_empty(queue));
	pass |= TEST_ASSERT(ft_queue_size(queue) == 0);

	if (SSL_OK != pass)
		return (TEST_FAIL());

	words = ft_strsplit(__s2, ' ');
	nwords = ft_2darray_len_null_terminated((void **)words);
	pass |= TEST_ASSERT(words != NULL);
	pass |= TEST_ASSERT(nwords > 6);

	idx = 0;
	while (idx < nwords)
	{
		key = ft_itoa(idx);
		ft_queue_enqueue(queue, key, words[idx], strlen(words[idx]));

		node = queue->last;
		pass |= TEST_ASSERT(node != NULL);
		pass |= TEST_ASSERT(strcmp(node->content, words[idx]) == 0);

		if (key != NULL)
			free(key);

		idx++;
	}

	pass |= TEST_ASSERT(ft_queue_size(queue) == nwords);

	node = ft_queue_peek(queue);
	pass |= TEST_ASSERT(node != NULL);
	pass |= TEST_ASSERT(!ft_queue_is_empty(queue));
	pass |= TEST_ASSERT(strcmp(node->content, words[0]) == 0);

	if (SSL_OK != pass)
		return (TEST_FAIL());

	idx = 0;
	while (!ft_queue_is_empty(queue))
	{
		if (idx >= nwords)
			break ;

		content = ft_queue_dequeue(queue);
		pass |= TEST_ASSERT(strcmp(content, words[idx]) == 0);

		idx++;
	}

	pass |= TEST_ASSERT(ft_queue_is_empty(queue));
	pass |= TEST_ASSERT(ft_queue_size(queue) == 0);

	idx = 0;
	while (idx < nwords)
	{
		key = ft_itoa(idx);
		ft_queue_enqueue(queue, key, words[idx], strlen(words[idx]));

		if (key != NULL)
			free(key);

		idx++;
	}

	// select some node
	node = ft_queue_peek(queue);
	while (node != NULL && strcmp(node->content, words[3]))
		node = node->next;

	// expect to find node
	pass |= TEST_ASSERT(node != NULL);

	// test queue delete node
	ft_queue_del_node(queue, node, NULL);

	// search for deleted node
	node = ft_queue_peek(queue);
	while (node != NULL && strcmp(node->content, words[3]))
		node = node->next;

	// do not expect to find node
	pass |= TEST_ASSERT(node == NULL);

	ft_queue_del(queue, NULL);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_htbl(void)
{
	t_htbl *htbl;
	t_node *node;
	void *content;
	char *key;
	char **words;
	char **test_content;
	int test_num;
	int nwords;
	int idx;
	int pass = SSL_OK;

	words = ft_strsplit(__s2, ' ');
	nwords = ft_2darray_len_null_terminated((void **)words);
	pass |= TEST_ASSERT(words != NULL);
	pass |= TEST_ASSERT(nwords > 6);

	LIBFT_ALLOC(test_content, sizeof(char *) * (nwords + 1));

	idx = 0;
	while (idx < nwords)
	{
		asprintf(test_content + idx, "%s-%d", words[idx], idx);
		idx++;
	}

	htbl = ft_htbl_init(nwords);
	pass |= TEST_ASSERT(htbl != NULL);
	pass |= TEST_ASSERT(htbl->size > 0);
	pass |= TEST_ASSERT(htbl->arr != NULL);

	idx = 0;
	while (idx < nwords)
	{
		ft_htbl_add(htbl, words[idx], words[idx]);
		content = ft_htbl_get(htbl, words[idx]);
		pass |= TEST_ASSERT(content != NULL);
		pass |= TEST_ASSERT(strcmp(content, words[idx]) == 0);

		idx++;
	}

	idx = 0;
	while (idx < nwords)
	{
		content = test_content[idx];
		ft_htbl_assign(htbl, content, words[idx]);
		content = ft_htbl_get(htbl, words[idx]);
		pass |= TEST_ASSERT(content != NULL);

		if (content)
			pass |= TEST_ASSERT(strcmp(content, test_content[idx]) == 0);

		idx++;
	}

	test_num = htbl->size;
	ft_htbl_resize(htbl, 2 * LIBFT_HT_SIZE);
	pass |= TEST_ASSERT(htbl->size == 2 * LIBFT_HT_SIZE);

	idx = 0;
	while (idx < nwords)
	{
		content = ft_htbl_get(htbl, words[idx]);
		pass |= TEST_ASSERT(content != NULL);

		if (content)
			pass |= TEST_ASSERT(strcmp(content, test_content[idx]) == 0);

		idx++;
	}

	t_node *iter;
	t_node *item;
	char **content_parts;
	int	nparts;
	int word_ht[nwords];

	bzero(word_ht, sizeof(word_ht));
	iter = ft_htbl_iter(htbl);
	node = iter;

	pass |= TEST_ASSERT(node != NULL);

	idx = 0;
	while (idx < nwords)
	{
		pass |= TEST_ASSERT(node != NULL);

		if (node == NULL)
			break ;

		item = node->content;
		content = item->content;
		content_parts = ft_strsplit(content, '-');

		if (content_parts == NULL)
			break ;

		nparts = ft_2darray_len_null_terminated((void **)content_parts);

		if (nparts == 2)
			test_num = atoi(content_parts[1]);

		ft_2darray_del_null_terminated((void **)content_parts);

		if (nparts != 2 || test_num > nwords-1)
			break ;

		word_ht[test_num] += 1;

		if (strcmp(content, test_content[test_num]) != 0)
			break ;

		node = node->next;
		idx++;
	}

	ft_lst_del(iter, NULL);
	pass |= TEST_ASSERT(idx == nwords);
	pass |= TEST_ASSERT(node == NULL);

	idx = 0;
	while (idx < nwords)
	{
		if (word_ht[idx] != 1)
			break ;

		idx++;
	}

	pass |= TEST_ASSERT(idx == nwords);

	ft_htbl_del_key(htbl, words[3]);
	content = ft_htbl_get(htbl, words[3]);

	pass |= TEST_ASSERT(content == NULL);

	ft_htbl_del(htbl);
	ft_2darray_del_null_terminated((void **)words);
	ft_2darray_del_null_terminated((void **)test_content);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_htbl_rawkey(void)
{
	t_htbl *htbl;
	void *content;
	char **words;
	char *word;
	size_t len;
	size_t *len_arr;
	int nwords;
	int idx;
	int pass = SSL_OK;

	words = ft_strsplit(__s2, ' ');
	nwords = ft_2darray_len_null_terminated((void **)words);
	pass |= TEST_ASSERT(words != NULL);
	pass |= TEST_ASSERT(nwords > 6);

	LIBFT_ALLOC(len_arr, sizeof(size_t *) * nwords);

	idx = 0;
	while (idx < nwords)
	{
		word = words[idx];
		len = strlen(word);

		word[len / 2] = 0;
		len_arr[idx] = len;

		idx++;
	}

	htbl = ft_htbl_init(nwords);
	pass |= TEST_ASSERT(htbl != NULL);
	pass |= TEST_ASSERT(htbl->size > 0);
	pass |= TEST_ASSERT(htbl->arr != NULL);

	idx = 0;
	while (idx < nwords)
	{
		ft_htbl_add_rawkey(htbl, words[idx], words[idx], len_arr[idx]);
		content = ft_htbl_get_rawkey(htbl, words[idx], len_arr[idx]);
		pass |= TEST_ASSERT(content != NULL);
		pass |= TEST_ASSERT(memcmp(content, words[idx], len_arr[idx]) == 0);

		idx++;
	}

	word = "test";

	idx = 0;
	while (idx < nwords)
	{
		ft_htbl_assign_rawkey(htbl, word, words[idx], len_arr[idx]);
		content = ft_htbl_get_rawkey(htbl, words[idx], len_arr[idx]);
		pass |= TEST_ASSERT(content != NULL);

		if (content)
			pass |= TEST_ASSERT(memcmp(content, word, strlen(word)) == 0);

		idx++;
	}

	ft_htbl_del(htbl);
	ft_2darray_del_null_terminated((void **)words);
	LIBFT_FREE(len_arr);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_htbl_conversion(void)
{
	char *key;
	t_node *node;
	t_node *iter;
	t_node *lst;
	t_stack *stack;
	t_queue *queue;
	t_htbl *htbl;
	char *word;
	char **words;
	int nwords;
	int idx;
	int pass = SSL_OK;

	words = ft_strsplit(__s2, ' ');
	nwords = ft_2darray_len_null_terminated((void **)words);
	pass |= TEST_ASSERT(words != NULL);
	pass |= TEST_ASSERT(nwords > 6);

	lst = NULL;
	stack = ft_stack_init();
	queue = ft_queue_init();

	idx = 0;
	while (idx < nwords)
	{
		key = ft_itoa(idx);

		node = ft_node_new(key, words[idx], strlen(words[idx]));
		ft_lst_prepend(&lst, node);

		ft_queue_enqueue(queue, key, words[idx], strlen(words[idx]));

		ft_stack_push(stack, key, words[idx], strlen(words[idx]));

		LIBFT_FREE(key);
		idx++;
	}

	htbl = ft_lst_htable(lst);

	idx = 0;
	while (idx < nwords)
	{
		key = ft_itoa(idx);
		word = ft_htbl_get(htbl, key);

		pass |= TEST_ASSERT(word != NULL);

		if (word)
			pass |= TEST_ASSERT(strcmp(word, words[idx]) == 0);

		LIBFT_FREE(key);
		idx++;
	}

	ft_htbl_del(htbl);

	htbl = ft_queue_htable(queue);

	idx = 0;
	while (idx < nwords)
	{
		key = ft_itoa(idx);
		word = ft_htbl_get(htbl, key);

		pass |= TEST_ASSERT(word != NULL);

		if (word)
			pass |= TEST_ASSERT(strcmp(word, words[idx]) == 0);

		LIBFT_FREE(key);
		idx++;
	}

	ft_htbl_del(htbl);

	htbl = ft_stack_htable(stack);

	idx = 0;
	while (idx < nwords)
	{
		key = ft_itoa(idx);
		word = ft_htbl_get(htbl, key);

		pass |= TEST_ASSERT(word != NULL);

		if (word)
			pass |= TEST_ASSERT(strcmp(word, words[idx]) == 0);

		LIBFT_FREE(key);
		idx++;
	}

	ft_htbl_del(htbl);

	ft_lst_del(lst, NULL);
	ft_queue_del(queue, NULL);
	ft_stack_del(stack, NULL);

	ft_2darray_del_null_terminated((void **)words);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_ntree_dfs_depth_func(t_node *node, const void *farg)
{
	char **key_parts;
	int cur_depth;
	int	expected_depth;
	int	nparts;
	int	ret;

	if (NULL == node)
		return (-1);

	(void)farg;

	if (NULL != node->key)
	{
		key_parts = ft_strsplit(node->key, ':');

		if (NULL == key_parts)
			return (-1);

		nparts = ft_2darray_len_null_terminated((void **)key_parts);

		if (nparts == 2)
		{
			expected_depth = atoi(key_parts[1]);
			cur_depth = ft_ntree_dfs_cur_depth();
		}

		ft_2darray_del_null_terminated((void **)key_parts);

		if (nparts != 2 || expected_depth != cur_depth)
			return (-2);
	}

	return (0);
}

static int __test_ft_ntree_dfs_order_func(t_node *node, const void *farg)
{
	char ***words;
	int	ret;

	if (NULL == node)
		return (-1);

	words = (char ***)farg;

	if (NULL != node->key)
	{
		if (**words == NULL)
			return (-1);

		if (strcmp(**words, node->key) != 0)
			return (-1);

		*words = *words + 1;
	}

	return (0);
}

static int __test_ft_ntree_helper_func(t_node *node, const void *farg)
{
	t_htbl *word_ht;
	void *content;
	int	*counter_ptr;

	if (NULL == node)
		return (-1);

	if (NULL != node->key)
	{
		word_ht = (t_htbl *)farg;
		content = ft_htbl_get(word_ht, node->key);

		counter_ptr = (int *)content;

		if (NULL == counter_ptr)
			return (-3);

		*counter_ptr += 1;
	}

	return (0);
}

static int __test_ft_ntree(void)
{
	t_node *ntree;
	t_node *node;
	t_htbl *word_ht;
	char **words;
	int nwords;
	int size;
	int idx;
	int ret;
	int pass = SSL_OK;

	const char *map = "product:0{ "\
		"colors:1{ "\
			"RGB:2{ "\
				"25:3 "\
				"27:3 "\
				"30:3 "\
			"} "\
			"RGBA:2{ "\
				"32:3 "\
				"35:3 "\
				"40:3 "\
				"0.5:3 "\
			"} "\
		"} "\
		"sizes:1{ "\
			"S:2 "\
			"M:2 "\
			"L:2 "\
		"} "\
	"}";

	// get words array
	words = ft_strsplit(map, ' ');
	nwords = ft_2darray_len_null_terminated((void **)words);

	// construct tree from map
	ntree = ft_ntree_construct(map);
	pass |= TEST_ASSERT(ntree != NULL);

	// filter out words
	// make tmp words array
	char **tmp;
	LIBFT_ALLOC(tmp, (nwords + 1) * sizeof(char *));

	nwords = 0;
	idx = 0;
	while (words[idx] != NULL)
	{
		// if word contains '{' symbol, get part of the word before that sytmbol
		if (ft_strchr(words[idx], '{'))
		{
			char **word_parts;

			word_parts = ft_strsplit(words[idx], '{');
			tmp[nwords++] = ft_strdup(word_parts[0]);
			ft_2darray_del_null_terminated((void **)word_parts);
		}
		// if word is "}", drop it
		else if (strcmp(words[idx], "}") == 0)
		{
			DO_NOTHING;
		}
		// anything else leave unchanged
		else
		{
			tmp[nwords++] = ft_strdup(words[idx]);
		}

		idx++;
	}
	tmp[nwords] = NULL;

	ft_2darray_del_null_terminated((void **)words);
	words = tmp;

	//
	// test ntree BFS
	//

	// init words htable
	word_ht = ft_htbl_init(nwords);

	// create hash map for word counts
	int counter_arr[nwords];

	// populate words htable with word counts initialized to zero
	idx = 0;
	while (idx < nwords)
	{
		counter_arr[idx] = 0;
		ft_htbl_add(word_ht, counter_arr + idx, words[idx]);
		idx++;
	}

	// count words in ntree using words htable
	ret = ft_ntree_bfs(NULL, ntree, word_ht, __test_ft_ntree_helper_func);
	pass |= TEST_ASSERT(ret == 0);

	// check if all words are present in ntree exactly once
	idx = 0;
	while (idx < nwords)
		if (counter_arr[idx++] != 1)
			break;

	pass |= TEST_ASSERT(idx == nwords);

	//
	// test ntree DFS
	//

	// populate words htable with word counts initialized to zero
	idx = 0;
	while (idx < nwords)
	{
		counter_arr[idx] = 0;
		ft_htbl_add(word_ht, counter_arr + idx, words[idx]);
		idx++;
	}

	// count words in ntree using words htable
	ret = ft_ntree_dfs(NULL, ntree, word_ht, __test_ft_ntree_helper_func);
	pass |= TEST_ASSERT(ret == 0);

	// check if all words are present in ntree exactly once
	idx = 0;
	while (idx < nwords)
		if (counter_arr[idx++] != 1)
			break;

	pass |= TEST_ASSERT(idx == nwords);

	//
	// check word depth distribution within ntree using DFS
	//

	ret = ft_ntree_dfs(NULL, ntree, NULL, __test_ft_ntree_dfs_depth_func);
	pass |= TEST_ASSERT(ret == 0);

	//
	// check topology of ntree nodes by matching the order of words
	// in word array with order of words during DFS 
	//

	char **tmp_ptr = words;

	ret = ft_ntree_dfs(NULL, ntree, &tmp_ptr, __test_ft_ntree_dfs_order_func);
	pass |= TEST_ASSERT(ret == 0);

	//
	// test ntree "iterator"
	//

	t_node *iter;
	t_node *item;
	void *content;
	int *counter;

	// get iterator list
	iter = ft_ntree_iter(ntree);
	pass |= TEST_ASSERT(iter != NULL);

	// init counter array to zeros
	bzero(counter_arr, sizeof(counter_arr));

	// get counter pointer from word htable using node keys from iterator list
	// increment counter to count number of times each word is seen in iterator list
	node = iter;
	while (node != NULL)
	{
		item = node->content;

		if (item != NULL)
		{
			content = ft_htbl_get(word_ht, item->key);
			counter = (int *)content;

			if (counter != NULL)
				*counter = *counter + 1;
		}

		node = node->next;
	}

	ft_lst_del(iter, NULL);

	// check if all words are present in iterator list exactly once
	idx = 0;
	while (idx < nwords)
		if (counter_arr[idx++] != 1)
			break;

	// size of ntree
	size = ft_ntree_size(ntree);

	pass |= TEST_ASSERT(size == nwords);

	ft_2darray_del_null_terminated((void **)words);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

