#include <ft_ssl.h>
#include <libft.h>
#include <string.h>
#include <ssl_test.h>

static char 	*__s1;
static char 	*__s2;
static char 	*__s3;
static size_t	__slen1;
static size_t	__slen2;
static size_t	__slen3;

static int	__test_libft_setup(void);
static int	__test_ft_strlen(void);
static int	__test_ft_strcpy(void);
static int	__test_ft_strncpy(void);
static int	__test_ft_strcat(void);
static int __test_ft_strncat(void);
static int __test_ft_strdup(void);
static int __test_ft_strchr(void);
static int __test_ft_strrchr(void);
static int __test_ft_strjoin(void);
static int __test_ft_strrev(void);
static int __test_ft_2darray_strjoin(void);
static int __test_ft_strsplit(void);

	int test_libft(void)
{
	int	res;

	if (SSL_OK != __test_libft_setup())
		return (SSL_ERROR(UNSPECIFIED_ERROR));

	res = __test_ft_strlen();
	res |= __test_ft_strcpy();
	res |= __test_ft_strncpy();
	res |= __test_ft_strcat();
	res |= __test_ft_strncat();
	res |= __test_ft_strdup();
	res |= __test_ft_strchr();
	res |= __test_ft_strrchr();
	res |= __test_ft_strjoin();
	res |= __test_ft_strrev();
	res |= __test_ft_2darray_strjoin();
	res |= __test_ft_strsplit();

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

static int __test_ft_2darray_strjoin(void)
{
	char *test_str_arr[] = {
		"apple",
		"banana",
		"orange",
		"vodka"
	};
	size_t test_str_len = sizeof(test_str_arr) / sizeof(char *);
	char *ref_str = "apple banana orange vodka";
	char *ref_str2 = "applebananaorangevodka";
	char *res = NULL;
	int pass;

	res = ft_2darray_strjoin(NULL, test_str_len, " ");
	pass = TEST_ASSERT(res == NULL);

	res = ft_2darray_strjoin(test_str_arr, 0, " ");
	pass |= TEST_ASSERT(res == NULL);

	res = ft_2darray_strjoin(test_str_arr, test_str_len, " ");
	pass |= TEST_ASSERT(res != NULL);
	pass |= TEST_ASSERT(!strcmp(res, ref_str));
	free(res);

	res = ft_2darray_strjoin(test_str_arr, test_str_len, "");
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