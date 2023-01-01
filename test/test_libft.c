#include <ft_ssl.h>
#include <libft.h>
#include <string.h>
#include <ssl_test.h>
#include <inttypes.h>

static char 	*__s1;
static char 	*__s2;
static char 	*__s3;
static size_t	__slen1;
static size_t	__slen2;
static size_t	__slen3;

static int __test_libft_setup(void);
static int __test_ft_strlen(void);
static int __test_ft_strcmp(void);
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
static int __test_ft_2darray_len_null_terminated(void);
static int __test_ft_2darray_strjoin(void);
static int __test_ft_strsplit(void);
static int __test_ft_atoi(void);
static int __test_ft_memcmp(void);
static int __test_ft_memcpy(void);
static int __test_ft_memdup(void);
static int __test_ft_memmove(void);
static int __test_ft_memset(void);
static int __test_ft_memzcpy(void);
static int __test_ft_itoa_base(void);
static int __test_ft_itoa(void);
static int __test_ft_printf(void);

int test_libft(void)
{
	int	res;

	if (SSL_OK != __test_libft_setup())
		return (SSL_ERROR(UNSPECIFIED_ERROR));

	res = __test_ft_strlen();
	res |= __test_ft_strcmp();
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
	res |= __test_ft_2darray_len_null_terminated();
	res |= __test_ft_2darray_strjoin();
	res |= __test_ft_strsplit();
	res |= __test_ft_atoi();
	res |= __test_ft_memcmp();
	res |= __test_ft_memcpy();
	res |= __test_ft_memdup();
	res |= __test_ft_memmove();
	res |= __test_ft_memset();
	res |= __test_ft_memzcpy();
	res |= __test_ft_itoa_base();
	res |= __test_ft_itoa();
	res |= __test_ft_printf();

	return (res);
}

static int	__test_libft_setup(void)
{
	__s1 = "Cernantur iis sunt, voluptate export nulla arbitror noster.";
	__s2 = "Se nescius exercitation. Malis nescius o consectetur \
		est excepteur multos dolor excepteur quae id ut aute incididunt \
		domesticarum eu malis a voluptate ubi ita esse pariatur offendit de \
		quamquam quid qui commodo cohaerescant si do elit incurreret singulis, \
		est ita consectetur.";
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
	int		pass;

	ref_value = strlen(__s1);
	test_value = ft_strlen(__s1);

	pass = TEST_ASSERT(test_value == ref_value);
	pass |= TEST_ASSERT(ft_strlen("") == 0);
	pass |= TEST_ASSERT(ft_strlen(NULL) == 0);

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int	__test_ft_strcmp(void)
{
	char	*ref_str = "elevator";
	int		pass;

	pass = TEST_ASSERT(ft_strcmp(ref_str, ref_str) == 0);
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

static int	__test_ft_strcpy(void)
{
	size_t	total_len = __slen1 + __slen2;
	char	test_str[total_len+1];
	char	ref_str[total_len+1];
	char	*temp_str;
	int		pass;

	strcpy(ref_str, __s1);
	strcpy(ref_str + __slen1, __s2);

	ft_strcpy(test_str, __s1);
	ft_strcpy(test_str + __slen1, __s2);

	pass = TEST_ASSERT(!strcmp(test_str, ref_str));

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
	int		pass;

	bzero(ref_str, total_len+1);

	strcpy(ref_str, __s1);
	strncpy(ref_str + __slen1, __s2, len);

	strcpy(test_str, __s1);
	ft_strncpy(test_str + __slen1, __s2, len);

	pass = TEST_ASSERT(!strcmp(test_str, ref_str));

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
	int		pass;


	strcpy(ref_str, __s1);
	strcat(ref_str, __s2);

	strcpy(test_str, __s1);
	ft_strcat(test_str, __s2);

	pass = TEST_ASSERT(!strcmp(test_str, ref_str));

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
	int		pass;


	strcpy(ref_str, __s1);
	strncpy(ref_str + __slen1, __s2, len);
	ref_str[__slen1 + len] = 0;

	strcpy(test_str, __s1);
	ft_strncat(test_str, __s2, len);

	pass = TEST_ASSERT(!strcmp(test_str, ref_str));

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
	int pass;

	ref_str = strdup(__s1);
	test_str = ft_strdup(__s1);
	pass = TEST_ASSERT(!strcmp(test_str, ref_str));
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
	int pass;

	bzero(ref_str, sizeof(ref_str));
	strncpy(ref_str, __s2 + offset, len);
	test_str = ft_strsub(__s2, offset, len);
	pass = TEST_ASSERT(!strcmp(test_str, ref_str));
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
	int pass;

	res = ft_strchr(NULL, 'e');
	pass = TEST_ASSERT(res == NULL);

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
	int pass;

	res = ft_strrchr(NULL, 'e');
	pass = TEST_ASSERT(res == NULL);

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
	int pass;

	strcpy(ref_str, __s1);
	strcat(ref_str, __s2);

	test_str = ft_strjoin(__s1, __s2);

	pass = TEST_ASSERT(test_str != NULL);
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
	int pass;

	res = ft_strrev(NULL);
	pass = TEST_ASSERT(res == NULL);

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

static int __test_ft_2darray_len_null_terminated(void)
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
	int pass;

	res = ft_2darray_len_null_terminated(NULL);
	pass = TEST_ASSERT(res == 0);

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
	int pass;

	res = ft_2darray_strjoin(NULL, test_str_arr_len, " ");
	pass = TEST_ASSERT(res == NULL);

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
	int pass;

	res = ft_strsplit(NULL, ' ');
	pass = TEST_ASSERT(res == NULL);

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
	int pass;

	res = ft_atoi("0");
	pass = TEST_ASSERT(res == 0);

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
	int pass;

	pass = TEST_ASSERT(ft_memcmp(ref_str, ref_str, ref_str_len) == 0);
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
	int pass;

	memcpy(ref_str, __s1, __slen1);
	memcpy(ref_str + __slen1, __s2, __slen2);

	ft_memcpy(test_str, __s1, __slen1);
	ft_memcpy(test_str + __slen1, __s2, __slen2);
	pass = TEST_ASSERT(!memcmp(test_str, ref_str, total_len));

	ft_memcpy(test_str, "", 0);
	pass |= TEST_ASSERT(!memcmp(test_str, ref_str, total_len));

	if (SSL_OK == pass)
		return (TEST_PASS());

	return (TEST_FAIL());
}

static int __test_ft_memdup(void)
{
	char *test_str;
	int pass;

	test_str = ft_memdup(__s1, __slen1);
	pass = TEST_ASSERT(!memcmp(test_str, __s1, __slen1));
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
	int pass;

	memcpy(ref_str, __s2, __slen2);
	memcpy(test_str, __s2, __slen2);

	memmove(ref_str, ref_str + offset, len);
	ft_memmove(test_str, test_str + offset, len);
	pass = TEST_ASSERT(!memcmp(test_str, ref_str, __slen2));

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
	int pass;

	memcpy(ref_str, __s2, __slen2);
	memcpy(test_str, __s2, __slen2);

	memset(ref_str, 190, len);
	ft_memset(test_str, 190, len);
	pass = TEST_ASSERT(!memcmp(test_str, ref_str, __slen2));

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

static int __test_ft_memzcpy(void)
{
	char test_str[__slen2];
	char zero_str[__slen2];
	size_t len = __slen2 / 2;
	int pass;

	memset(zero_str, 0, sizeof(zero_str));

	memcpy(test_str, __s2, __slen2);
	ft_memzcpy(test_str, __s2, len, len / 2);
	pass = TEST_ASSERT(!memcmp(test_str, __s2, len / 2));
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
	int pass;

	res = ft_itoa_base(0, 10);
	pass = TEST_ASSERT(strcmp(res, "0") == 0);
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
	int pass;

	res = ft_itoa(0);
	pass = TEST_ASSERT(strcmp(res, "0") == 0);
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
	int pass;

	test_num = ft_sprintf(&test_str, "%s", "apple\n");
	ref_num = asprintf(&ref_str, "%s", "apple\n");
	pass = TEST_ASSERT(strcmp(test_str, ref_str) == 0);
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