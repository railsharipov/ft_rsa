#include <string.h>
#include <inttypes.h>
#include <limits.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <common.h>
#include <file.h>
#include "test.h"
#include <libft/std.h>
#include <libft/node.h>
#include <libft/list.h>
#include <libft/stack.h>
#include <libft/queue.h>
#include <libft/tuple.h>
#include <libft/ntree.h>
#include <libft/2darray.h>
#include <libft/bytes.h>
#include <libft/buffer.h>

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
static int __test_ft_str_isnum(void);
static int __test_ft_str_ishex(void);
static int __test_ft_str_isprint(void);
static int __test_ft_bytes_to_hex(void);
static int __test_ft_hex_to_bytes(void);
static int __test_ft_uint_to_bytes(void);
static int __test_get_next_line(void);
static int __test_ft_node(void);
static int __test_ft_list(void);
static int __test_ft_stack(void);
static int __test_ft_queue(void);
static int __test_ft_tuple(void);
static int __test_ft_htbl(void);
static int __test_ft_htbl_rawkey(void);
static int __test_ft_htbl_conversion(void);
static int __test_ft_ntree(void);
static int __test_ft_buffer(void);

int test_libft(void)
{
	int		ret;

	if (SSL_OK != __test_libft_setup()) {
		TEST_LOG(ERROR, TEST_SETUP_ERROR);
		TEST_FAIL();
	}

	ret = __test_ft_strlen()
		| __test_ft_strcmp()
		| __test_ft_strncmp()
		| __test_ft_strcpy()
		| __test_ft_strncpy()
		| __test_ft_strcat()
		| __test_ft_strncat()
		| __test_ft_strdup()
		| __test_ft_strsub()
		| __test_ft_strchr()
		| __test_ft_strrchr()
		| __test_ft_strjoin()
		| __test_ft_strrev()
		| __test_ft_strlow()
		| __test_ft_strup()
		| __test_ft_2darray_len()
		| __test_ft_2darray_strjoin()
		| __test_ft_strsplit()
		| __test_ft_atoi()
		| __test_ft_memcmp()
		| __test_ft_memcpy()
		| __test_ft_memdup()
		| __test_ft_memmove()
		| __test_ft_memset()
		| __test_ft_bzero()
		| __test_ft_memzcpy()
		| __test_ft_itoa_base()
		| __test_ft_itoa()
		| __test_ft_printf()
		| __test_ft_str_isalnum()
		| __test_ft_str_isalpha()
		| __test_ft_str_isascii()
		| __test_ft_str_isnum()
		| __test_ft_str_ishex()
		| __test_ft_str_isprint()
		| __test_ft_bytes_to_hex()
		| __test_ft_hex_to_bytes()
		| __test_ft_uint_to_bytes()
		| __test_get_next_line()
		| __test_ft_node()
		| __test_ft_list()
		| __test_ft_stack()
		| __test_ft_queue()
		| __test_ft_tuple()
		| __test_ft_htbl()
		| __test_ft_htbl_rawkey()
		| __test_ft_htbl_conversion()
		| __test_ft_ntree()
		| __test_ft_buffer();

	return (ret);
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

	ref_value = strlen(__s1);
	test_value = ft_strlen(__s1);

	TEST_ASSERT(test_value == ref_value);
	TEST_ASSERT(ft_strlen("") == 0);
	TEST_ASSERT(ft_strlen(NULL) == 0);

	TEST_PASS();
}

static int	__test_ft_strcmp(void)
{
	char	*ref_str = "elevator";

	TEST_ASSERT(ft_strcmp(ref_str, ref_str) == 0);
	TEST_ASSERT(ft_strcmp(ref_str, "") > 0);
	TEST_ASSERT(ft_strcmp("", ref_str) < 0);
	TEST_ASSERT(ft_strcmp("", "") == 0);
	TEST_ASSERT(ft_strcmp(ref_str, "not elevator") < 0);
	TEST_ASSERT(ft_strcmp("not elevator", ref_str) > 0);
	TEST_ASSERT(ft_strcmp(ref_str, "elevators") < 0);
	TEST_ASSERT(ft_strcmp("elevators", ref_str) > 0);

	TEST_PASS();
}

static int	__test_ft_strncmp(void)
{
	char	*ref_str = "elevator";
	size_t	ref_size = strlen(ref_str);
	size_t	test_size = ref_size / 2;

	TEST_ASSERT(ft_strncmp(ref_str, ref_str, ref_size) == 0);
	TEST_ASSERT(ft_strncmp(ref_str, ref_str, test_size) == 0);
	TEST_ASSERT(ft_strncmp(ref_str, ref_str, 2 * ref_size) == 0);
	TEST_ASSERT(ft_strncmp(ref_str, "", test_size) > 0);
	TEST_ASSERT(ft_strncmp("", ref_str, test_size) < 0);
	TEST_ASSERT(ft_strncmp("", "", test_size) == 0);
	TEST_ASSERT(ft_strncmp("", "", 0) == 0);
	TEST_ASSERT(ft_strncmp(ref_str, "not elevator", test_size) < 0);
	TEST_ASSERT(ft_strncmp("not elevator", ref_str, test_size) > 0);
	TEST_ASSERT(ft_strncmp(ref_str, "elevators", ref_size) == 0);
	TEST_ASSERT(ft_strncmp("elevators", ref_str, ref_size) == 0);

	TEST_PASS();
}

static int	__test_ft_strcpy(void)
{
	size_t	total_len = __slen1 + __slen2;
	char	test_str[total_len+1];
	char	ref_str[total_len+1];
	char	*temp_str;

	strcpy(ref_str, __s1);
	strcpy(ref_str + __slen1, __s2);

	ft_strcpy(test_str, __s1);
	ft_strcpy(test_str + __slen1, __s2);

	TEST_ASSERT(!strcmp(test_str, ref_str));

	ft_strcpy(test_str, "");

	TEST_ASSERT(strlen(test_str) == 0);

	bzero(test_str, total_len+1);
	ft_strcpy(test_str, __s1);
	ft_strcpy(test_str + __slen1, NULL);

	TEST_ASSERT(strlen(test_str) == __slen1);

	temp_str = NULL;
	temp_str = ft_strcpy(temp_str, __s1);

	TEST_ASSERT(NULL == temp_str);

	TEST_PASS();
}

static int	__test_ft_strncpy(void)
{
	size_t	total_len = __slen1 + __slen2;
	size_t	len = __slen2 /2;
	char	test_str[total_len+1];
	char	ref_str[total_len+1];
	char	*temp_str;

	bzero(ref_str, total_len+1);

	strcpy(ref_str, __s1);
	strncpy(ref_str + __slen1, __s2, len);

	strcpy(test_str, __s1);
	ft_strncpy(test_str + __slen1, __s2, len);

	TEST_ASSERT(!strcmp(test_str, ref_str));

	test_str[0] = 0;
	ft_strncpy(test_str, __s2, 0);
	TEST_ASSERT(strlen(test_str) == 0);

	strcpy(test_str, __s1);
	ft_strncpy(test_str + __slen1, __s2, __slen2);

	TEST_ASSERT(strlen(test_str) == total_len);

	strcpy(test_str, __s1);
	ft_strncpy(test_str + __slen1, NULL, len);

	TEST_ASSERT(strlen(test_str) == __slen1);

	temp_str = NULL;
	temp_str = ft_strncpy(temp_str, __s1, len);

	TEST_ASSERT(NULL == temp_str);

	TEST_PASS();
}

static int	__test_ft_strcat(void)
{
	size_t	total_len = __slen1 + __slen2;
	char	test_str[total_len+1];
	char	ref_str[total_len+1];
	char	*temp_str;

	strcpy(ref_str, __s1);
	strcat(ref_str, __s2);

	strcpy(test_str, __s1);
	ft_strcat(test_str, __s2);

	TEST_ASSERT(!strcmp(test_str, ref_str));

	ft_strcat(test_str, "");

	TEST_ASSERT(strlen(test_str) == total_len);

	strcpy(test_str, __s1);
	ft_strcat(test_str, NULL);

	TEST_ASSERT(strlen(test_str) == __slen1);

	temp_str = NULL;
	temp_str = ft_strcat(temp_str, __s1);

	TEST_ASSERT(NULL == temp_str);

	TEST_PASS();
}

static int	__test_ft_strncat(void)
{
	size_t	total_len = __slen1 + __slen2;
	size_t 	len = __slen2 / 2;
	char 	test_str[total_len + 1];
	char	ref_str[total_len+1];
	char	*temp_str;

	strcpy(ref_str, __s1);
	strncpy(ref_str + __slen1, __s2, len);
	ref_str[__slen1 + len] = 0;

	strcpy(test_str, __s1);
	ft_strncat(test_str, __s2, len);

	TEST_ASSERT(!strcmp(test_str, ref_str));

	ft_strncat(test_str, "", len);

	TEST_ASSERT(strlen(test_str) == __slen1 + len);

	strcpy(test_str, __s1);
	ft_strncat(test_str, NULL, len);

	TEST_ASSERT(strlen(test_str) == __slen1);

	temp_str = NULL;
	temp_str = ft_strncat(temp_str, __s2, len);

	TEST_ASSERT(NULL == temp_str);

	TEST_PASS();
}

static int __test_ft_strdup(void)
{
	char *test_str;
	char *ref_str;

	ref_str = strdup(__s1);
	test_str = ft_strdup(__s1);
	TEST_ASSERT(!strcmp(test_str, ref_str));
	free(test_str);

	test_str = ft_strdup("");
	TEST_ASSERT(strlen(test_str) == 0);
	free(test_str);

	test_str = ft_strdup(NULL);
	TEST_ASSERT(NULL == test_str);

	TEST_PASS();
}

static int __test_ft_strsub(void)
{
	char *test_str;
	size_t offset = __slen2 / 5;
	size_t len = MIN(10, __slen2 - offset);
	char ref_str[len + 1];

	bzero(ref_str, sizeof(ref_str));
	strncpy(ref_str, __s2 + offset, len);
	test_str = ft_strsub(__s2, offset, len);
	TEST_ASSERT(!strcmp(test_str, ref_str));
	free(test_str);

	test_str = ft_strsub(__s2, offset, 0);
	TEST_ASSERT(strlen(test_str) == 0);
	free(test_str);

	test_str = ft_strsub(NULL, offset, len);
	TEST_ASSERT(NULL == test_str);

	TEST_PASS();
}

static int __test_ft_strchr(void)
{
	char *test_str = "apple";
	char *res = NULL;

	res = ft_strchr(NULL, 'e');
	TEST_ASSERT(res == NULL);

	res = ft_strchr(test_str, 'z');
	TEST_ASSERT(res == NULL);

	res = ft_strchr("", 'e');
	TEST_ASSERT(res == NULL);

	res = ft_strchr(test_str, 'e');
	TEST_ASSERT(res != NULL);
	TEST_ASSERT(!strcmp(res, "e"));

	TEST_PASS();
}

static int __test_ft_strrchr(void)
{
	char *test_str = "apple";
	char *res = NULL;

	res = ft_strrchr(NULL, 'e');
	TEST_ASSERT(res == NULL);

	res = ft_strrchr(test_str, 'z');
	TEST_ASSERT(res == NULL);

	res = ft_strrchr("", 'e');
	TEST_ASSERT(res == NULL);

	res = ft_strrchr(test_str, 'e');
	TEST_ASSERT(res != NULL);
	TEST_ASSERT(!strcmp(res, "e"));

	TEST_PASS();
}

static int __test_ft_strjoin(void)
{
	size_t total_len = __slen1 + __slen2;
	char *test_str = NULL;
	char ref_str[total_len + 1];

	strcpy(ref_str, __s1);
	strcat(ref_str, __s2);

	test_str = ft_strjoin(__s1, __s2);

	TEST_ASSERT(test_str != NULL);
	TEST_ASSERT(!strcmp(test_str, ref_str));
	free(test_str);

	test_str = ft_strjoin(NULL, NULL);
	TEST_ASSERT(test_str == NULL);

	test_str = ft_strjoin(__s1, "");
	TEST_ASSERT(strlen(test_str) == __slen1);
	free(test_str);

	test_str = ft_strjoin("", __s1);
	TEST_ASSERT(strlen(test_str) == __slen1);
	free(test_str);

	test_str = ft_strjoin(__s1, NULL);
	TEST_ASSERT(strlen(test_str) == __slen1);
	free(test_str);

	test_str = ft_strjoin(NULL, __s1);
	TEST_ASSERT(strlen(test_str) == __slen1);
	free(test_str);

	TEST_PASS();
}

static int __test_ft_strrev(void)
{
	char *test_str = "apple";
	char *ref_str = "elppa";
	char *res = NULL;

	res = ft_strrev(NULL);
	TEST_ASSERT(res == NULL);

	res = ft_strrev(test_str);
	TEST_ASSERT(!strcmp(res, ref_str));
	free(res);

	res = ft_strrev("");
	TEST_ASSERT(strlen(res) == 0);
	free(res);

	TEST_PASS();
}

static int __test_ft_strlow(void)
{
	char *str = "ApPl!e# !@#$@FSDdf234523#$%^$%&\n";
	char *test_str;
	char *ref_str;

	ref_str = strdup(str);

	for (int i = 0; i < strlen(ref_str); i++)
		ref_str[i] = tolower(ref_str[i]);

	test_str = ft_strlow(NULL);
	TEST_ASSERT(test_str == NULL);

	test_str = ft_strlow(str);
	TEST_ASSERT(strcmp(test_str, ref_str) == 0);

	free(test_str);
	free(ref_str);

	TEST_PASS();
}

static int __test_ft_strup(void)
{
	char *str = "ApPl!e# !@#$@FsDdf234523#$%^$%&\n";
	char *test_str;
	char *ref_str;

	ref_str = strdup(str);

	for (int i = 0; i < strlen(ref_str); i++)
		ref_str[i] = toupper(ref_str[i]);

	test_str = ft_strup(NULL);
	TEST_ASSERT(test_str == NULL);

	test_str = ft_strup(str);
	TEST_ASSERT(strcmp(test_str, ref_str) == 0);

	free(test_str);
	free(ref_str);

	TEST_PASS();
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

	res = ft_2darray_len_null_terminated(NULL);
	TEST_ASSERT(res == 0);

	res = ft_2darray_len_null_terminated((void **)test_str_arr);
	TEST_ASSERT(res == test_str_arr_len);

	res = ft_2darray_len_null_terminated(&null_ptr);
	TEST_ASSERT(res == 0);

	TEST_PASS();
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

	res = ft_2darray_strjoin(NULL, test_str_arr_len, " ");
	TEST_ASSERT(res == NULL);

	res = ft_2darray_strjoin(test_str_arr, 0, " ");
	TEST_ASSERT(res == NULL);

	res = ft_2darray_strjoin(test_str_arr, test_str_arr_len, " ");
	TEST_ASSERT(res != NULL);
	TEST_ASSERT(!strcmp(res, ref_str));
	free(res);

	res = ft_2darray_strjoin(test_str_arr, test_str_arr_len, "");
	TEST_ASSERT(res != NULL);
	TEST_ASSERT(!strcmp(res, ref_str2));
	free(res);

	res = ft_2darray_strjoin(test_str_arr, 1, " ");
	TEST_ASSERT(!strcmp(res, "apple"));
	free(res);

	TEST_PASS();
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

	res = ft_strsplit(NULL, ' ');
	TEST_ASSERT(res == NULL);

	res = ft_strsplit(test_str, ' ');
	TEST_ASSERT(res != NULL);
	TEST_ASSERT(ref_str_len == ft_2darray_len_null_terminated((void **)res));

	for (int i = 0; i < ref_str_len; i++)
		TEST_ASSERT(!strcmp(res[i], ref_str_arr[i]));

	ft_2darray_del_null_terminated((void **)res);

	res = ft_strsplit("apple", ' ');
	TEST_ASSERT(res != NULL);
	TEST_ASSERT(ft_2darray_len_null_terminated((void **)res) == 1);

	TEST_ASSERT(!strcmp(res[0], "apple"));

	ft_2darray_del_null_terminated((void **)res);

	TEST_PASS();
}

static int __test_ft_atoi(void)
{
	long long res = 0;

	res = ft_atoi("0");
	TEST_ASSERT(res == 0);

	res = ft_atoi("+0");
	TEST_ASSERT(res == 0);

	res = ft_atoi("-0");
	TEST_ASSERT(res == 0);

	res = ft_atoi("");
	TEST_ASSERT(res == 0);

	res = ft_atoi("12316235");
	TEST_ASSERT(res == 12316235);

	res = ft_atoi("12316aa235");
	TEST_ASSERT(res == 12316);

	res = ft_atoi("aa235");
	TEST_ASSERT(res == 0);

	res = ft_atoi("123-235");
	TEST_ASSERT(res == 123);

	res = ft_atoi("123+235");
	TEST_ASSERT(res == 123);

	res = ft_atoi("+123235");
	TEST_ASSERT(res == 123235);

	res = ft_atoi("-123235");
	TEST_ASSERT(res == -123235);

	TEST_PASS();
}

static int __test_ft_memcmp(void)
{
	char *ref_str = "elevator";
	size_t ref_str_len = strlen(ref_str);

	TEST_ASSERT(ft_memcmp(ref_str, ref_str, ref_str_len) == 0);
	TEST_ASSERT(ft_memcmp(ref_str, "", 1) > 0);
	TEST_ASSERT(ft_memcmp("", ref_str, 1) < 0);
	TEST_ASSERT(ft_memcmp("", "", 1) == 0);
	TEST_ASSERT(ft_memcmp(ref_str, "not elevator", ref_str_len) < 0);
	TEST_ASSERT(ft_memcmp("not elevator", ref_str, ref_str_len) > 0);
	TEST_ASSERT(ft_memcmp(ref_str, "elevators", ref_str_len) == 0);
	TEST_ASSERT(ft_memcmp("elevators", ref_str, ref_str_len) == 0);

	TEST_PASS();
}

static int __test_ft_memcpy(void)
{
	size_t total_len = __slen1 + __slen2;
	char test_str[total_len];
	char ref_str[total_len];

	memcpy(ref_str, __s1, __slen1);
	memcpy(ref_str + __slen1, __s2, __slen2);

	ft_memcpy(test_str, __s1, __slen1);
	ft_memcpy(test_str + __slen1, __s2, __slen2);
	TEST_ASSERT(!memcmp(test_str, ref_str, total_len));

	ft_memcpy(test_str, "", 0);
	TEST_ASSERT(!memcmp(test_str, ref_str, total_len));

	TEST_PASS();
}

static int __test_ft_memdup(void)
{
	char *test_str;

	test_str = ft_memdup(__s1, __slen1);
	TEST_ASSERT(!memcmp(test_str, __s1, __slen1));
	free(test_str);

	test_str = ft_memdup(__s1, 0);
	TEST_ASSERT(NULL != test_str);
	free(test_str);

	test_str = ft_memdup(NULL, __slen1);
	TEST_ASSERT(NULL == test_str);

	TEST_PASS();
}

static int __test_ft_memmove(void)
{
	char test_str[__slen2];
	char ref_str[__slen2];
	size_t offset = __slen2 / 4;
	size_t len = __slen2 / 2;

	memcpy(ref_str, __s2, __slen2);
	memcpy(test_str, __s2, __slen2);

	memmove(ref_str, ref_str + offset, len);
	ft_memmove(test_str, test_str + offset, len);
	TEST_ASSERT(!memcmp(test_str, ref_str, __slen2));

	memmove(ref_str + offset, ref_str, len);
	ft_memmove(test_str + offset, test_str, len);
	TEST_ASSERT(!memcmp(test_str, ref_str, __slen2));

	memmove(ref_str + offset, ref_str, 0);
	ft_memmove(test_str + offset, test_str, 0);
	TEST_ASSERT(!memcmp(test_str, ref_str, __slen2));

	TEST_PASS();
}

static int __test_ft_memset(void)
{
	char test_str[__slen2];
	char ref_str[__slen2];
	size_t len = __slen2 / 2;

	memcpy(ref_str, __s2, __slen2);
	memcpy(test_str, __s2, __slen2);

	memset(ref_str, 190, len);
	ft_memset(test_str, 190, len);
	TEST_ASSERT(!memcmp(test_str, ref_str, __slen2));

	memset(ref_str, -94, len);
	ft_memset(test_str, -94, len);
	TEST_ASSERT(!memcmp(test_str, ref_str, __slen2));

	memset(ref_str, 123456, len);
	ft_memset(test_str, 123456, len);
	TEST_ASSERT(!memcmp(test_str, ref_str, __slen2));

	memset(ref_str, -123456, len);
	ft_memset(test_str, -123456, len);
	TEST_ASSERT(!memcmp(test_str, ref_str, __slen2));

	memset(ref_str, 190, (0));
	ft_memset(test_str, 190, 0);
	TEST_ASSERT(!memcmp(test_str, ref_str, __slen2));

	TEST_PASS();
}

static int __test_ft_bzero(void)
{
	char test_str[32];
	char ref_str[32] = {0};

	memset(test_str, 127, sizeof(test_str));
	ft_bzero(test_str, sizeof(test_str));
	TEST_ASSERT(memcmp(test_str, ref_str, sizeof(test_str)) == 0);

	memset(test_str, 127, sizeof(test_str));
	memset(ref_str, 127, sizeof(ref_str));
	ft_bzero(test_str, 0);
	TEST_ASSERT(memcmp(test_str, ref_str, sizeof(test_str)) == 0);

	TEST_PASS();
}

static int __test_ft_memzcpy(void)
{
	char test_str[__slen2];
	char zero_str[__slen2];
	size_t len = __slen2 / 2;

	memset(zero_str, 0, sizeof(zero_str));

	memcpy(test_str, __s2, __slen2);
	ft_memzcpy(test_str, __s2, len, len / 2);
	TEST_ASSERT(!memcmp(test_str, __s2, len / 2));
	TEST_ASSERT(!memcmp(test_str + len / 2, zero_str, len - len / 2));

	memcpy(test_str, __s2, __slen2);
	ft_memzcpy(test_str, zero_str, len / 2, len);
	TEST_ASSERT(!memcmp(test_str, zero_str, len / 2));
	TEST_ASSERT(!memcmp(test_str + len / 2, __s2 + len / 2, len - len / 2));

	TEST_PASS();
}

static int __test_ft_itoa_base(void)
{
	char *res = NULL;
	intmax_t	ref_num;

	res = ft_itoa_base(0, 10);
	TEST_ASSERT(strcmp(res, "0") == 0);
	free(res);

	res = ft_itoa_base(0, 16);
	TEST_ASSERT(strcmp(res, "0") == 0);
	free(res);

	res = ft_itoa_base(1234, 10);
	TEST_ASSERT(strcmp(res, "1234") == 0);
	free(res);

	res = ft_itoa_base(-1234, 10);
	TEST_ASSERT(strcmp(res, "-1234") == 0);
	free(res);

	res = ft_itoa_base(-255, 16);
	TEST_ASSERT(strcmp(res, "ff") == 0);
	free(res);

	res = ft_itoa_base(255, 16);
	TEST_ASSERT(strcmp(res, "ff") == 0);
	free(res);

	res = ft_itoa_base(255, 2);
	TEST_ASSERT(strcmp(res, "11111111") == 0);
	free(res);

	res = ft_itoa_base(255, 8);
	TEST_ASSERT(strcmp(res, "377") == 0);
	free(res);

	res = ft_itoa_base(255, 12);
	TEST_ASSERT(strcmp(res, "193") == 0);
	free(res);

	res = ft_itoa_base(INTMAX_MAX, 10);
	ref_num = strtoimax(res, NULL, 10);
	TEST_ASSERT(ref_num == INTMAX_MAX);
	free(res);

	TEST_PASS();
}

static int __test_ft_itoa(void)
{
	char *res = NULL;
	intmax_t	ref_num;

	res = ft_itoa(0);
	TEST_ASSERT(strcmp(res, "0") == 0);
	free(res);

	res = ft_itoa(1234);
	TEST_ASSERT(strcmp(res, "1234") == 0);
	free(res);

	res = ft_itoa(-1234);
	TEST_ASSERT(strcmp(res, "-1234") == 0);
	free(res);

	res = ft_itoa(INTMAX_MAX);
	ref_num = strtoimax(res, NULL, 10);
	TEST_ASSERT(ref_num == INTMAX_MAX);
	free(res);

	TEST_PASS();
}

static int __test_ft_printf(void)
{
	char *test_str = NULL;
	char *ref_str = NULL;
	void *ptr = NULL;
	int	test_num;
	int	ref_num;

	test_num = ft_sprintf(&test_str, "%s", "apple\n");
	ref_num = asprintf(&ref_str, "%s", "apple\n");
	TEST_ASSERT(strcmp(test_str, ref_str) == 0);
	TEST_ASSERT(test_num == ref_num);
	free(test_str);
	free(ref_str);

	test_num = ft_sprintf(&test_str, "%.2s", "apple\n");
	ref_num = asprintf(&ref_str, "%.2s", "apple\n");
	TEST_ASSERT(strcmp(test_str, ref_str) == 0);
	TEST_ASSERT(test_num == ref_num);
	free(test_str);
	free(ref_str);

	test_num = ft_sprintf(&test_str, "%9s", "apple\n");
	ref_num = asprintf(&ref_str, "%9s", "apple\n");
	TEST_ASSERT(strcmp(test_str, ref_str) == 0);
	TEST_ASSERT(test_num == ref_num);
	free(test_str);
	free(ref_str);

	test_num = ft_sprintf(&test_str, "%d", 1244);
	ref_num = asprintf(&ref_str, "%d", 1244);
	TEST_ASSERT(strcmp(test_str, ref_str) == 0);
	TEST_ASSERT(test_num == ref_num);
	free(test_str);
	free(ref_str);

	test_num = ft_sprintf(&test_str, "%ld", LONG_MAX);
	ref_num = asprintf(&ref_str, "%ld", LONG_MAX);
	TEST_ASSERT(strcmp(test_str, ref_str) == 0);
	TEST_ASSERT(test_num == ref_num);
	free(test_str);
	free(ref_str);

	test_num = ft_sprintf(&test_str, "%lld", LLONG_MAX);
	ref_num = asprintf(&ref_str, "%lld", LLONG_MAX);
	TEST_ASSERT(strcmp(test_str, ref_str) == 0);
	TEST_ASSERT(test_num == ref_num);
	free(test_str);
	free(ref_str);

	test_num = ft_sprintf(&test_str, "%u", 1244u);
	ref_num = asprintf(&ref_str, "%u", 1244u);
	TEST_ASSERT(strcmp(test_str, ref_str) == 0);
	TEST_ASSERT(test_num == ref_num);
	free(test_str);
	free(ref_str);

	test_num = ft_sprintf(&test_str, "%lu", ULONG_MAX);
	ref_num = asprintf(&ref_str, "%lu", ULONG_MAX);
	TEST_ASSERT(strcmp(test_str, ref_str) == 0);
	TEST_ASSERT(test_num == ref_num);
	free(test_str);
	free(ref_str);

	test_num = ft_sprintf(&test_str, "%llu", ULLONG_MAX);
	ref_num = asprintf(&ref_str, "%llu", ULLONG_MAX);
	TEST_ASSERT(strcmp(test_str, ref_str) == 0);
	TEST_ASSERT(test_num == ref_num);
	free(test_str);
	free(ref_str);

	test_num = ft_sprintf(&test_str, "%zu", SIZE_T_MAX);
	ref_num = asprintf(&ref_str, "%zu", SIZE_T_MAX);
	TEST_ASSERT(strcmp(test_str, ref_str) == 0);
	TEST_ASSERT(test_num == ref_num);
	free(test_str);
	free(ref_str);

	test_num = ft_sprintf(&test_str, "%x", 0x1244);
	ref_num = asprintf(&ref_str, "%x", 0x1244);
	TEST_ASSERT(strcmp(test_str, ref_str) == 0);
	TEST_ASSERT(test_num == ref_num);
	free(test_str);
	free(ref_str);

	test_num = ft_sprintf(&test_str, "%p", ptr);
	ref_num = asprintf(&ref_str, "%p", ptr);
	TEST_ASSERT(strcmp(test_str, ref_str) == 0);
	TEST_ASSERT(test_num == ref_num);
	free(test_str);
	free(ref_str);

	test_num = ft_sprintf(&test_str, "%o", 01244);
	ref_num = asprintf(&ref_str, "%o", 01244);
	TEST_ASSERT(strcmp(test_str, ref_str) == 0);
	TEST_ASSERT(test_num == ref_num);
	free(test_str);
	free(ref_str);

	TEST_PASS();
}

static int __test_ft_str_isalpha(void)
{
	int res;

	res = ft_str_isalpha(NULL);
	TEST_ASSERT(res == 0);

	res = ft_str_isalpha("abcASDFacwqer31");
	TEST_ASSERT(res == 0);

	res = ft_str_isalpha("abASDcqwe");
	TEST_ASSERT(res == 1);

	TEST_PASS();
}

static int __test_ft_str_isalnum(void)
{
	int res;

	res = ft_str_isalnum(NULL);
	TEST_ASSERT(res == 0);

	res = ft_str_isalnum("abcAS2345123DFacwqer31");
	TEST_ASSERT(res == 1);

	res = ft_str_isalnum("abASDc#$%^qwe");
	TEST_ASSERT(res == 0);

	TEST_PASS();
}

static int __test_ft_str_isascii(void)
{
	int res;
	char test_str[] = {255, 127, 44, 45, 0};
	char test_str2[] = {2, 22, 55, 127, 44, 45, 0};

	res = ft_str_isascii(NULL);
	TEST_ASSERT(res == 0);

	res = ft_str_isascii(test_str);
	TEST_ASSERT(res == 0);

	res = ft_str_isascii(test_str2);
	TEST_ASSERT(res == 1);

	TEST_PASS();
}

static int __test_ft_str_isnum(void)
{
	int res;

	res = ft_str_isnum(NULL);
	TEST_ASSERT(res == 0);

	res = ft_str_isnum("abcAS2345123DFacwqer31");
	TEST_ASSERT(res == 0);

	res = ft_str_isnum("0123456789");
	TEST_ASSERT(res == 1);

	TEST_PASS();
}

static int __test_ft_str_ishex(void)
{
	int res;

	res = ft_str_ishex(NULL);
	TEST_ASSERT(res == 0);

	res = ft_str_ishex("1234123sczxvz");
	TEST_ASSERT(res == 0);

	res = ft_str_ishex("0123456789ABCDEFabcdef");
	TEST_ASSERT(res == 1);

	TEST_PASS();
}

static int __test_ft_str_isprint(void)
{
	int res;
	char test_str[] = {9, 1, 2, 3, 15, 127, 44, 45, 0};

	res = ft_str_isprint(NULL);
	TEST_ASSERT(res == 0);

	res = ft_str_isprint(test_str);
	TEST_ASSERT(res == 0);

	res = ft_str_isprint("0123456789ABCDEFabcdef");
	TEST_ASSERT(res == 1);

	TEST_PASS();
}

static int __test_ft_bytes_to_hex(void)
{
	char *test_str;
	char octets[] = {9, 1, 127, 3, 15, 0};
	char *ref_str = "09017f030f00";

	test_str = ft_bytes_to_hex(NULL, 20);
	TEST_ASSERT(test_str == NULL);

	test_str = ft_bytes_to_hex(octets, 0);
	TEST_ASSERT(test_str == NULL);

	test_str = ft_bytes_to_hex(octets, sizeof(octets));
	TEST_ASSERT(strcmp(test_str, ref_str) == 0);
	free(test_str);

	TEST_PASS();
}

static int __test_ft_hex_to_bytes(void)
{
	char ref_str[6] = {9, 1, 127, 3, 15, 0};
	char test_str[6] = {0};
	char *hex = "9017f030f00";

	ft_hex_to_bytes(test_str, hex, strlen(hex));
	TEST_ASSERT(memcmp(test_str, ref_str, sizeof(test_str)) == 0);

	TEST_PASS();
}

static int __test_ft_uint_to_bytes(void)
{
	uint32_t num = 3271454934;
	uint32_t zero = 0;
	char ref_str[4] = {194, 254, 112, 214};
	char buf[4] = {0};

	ft_uint_to_bytes(buf, num, sizeof(num));

	TEST_ASSERT(ft_memcmp(buf, ref_str, sizeof(buf)) == 0);

	bzero(buf, sizeof(buf));
	bzero(ref_str, sizeof(ref_str));
	ft_uint_to_bytes(buf, zero, sizeof(zero));
	TEST_ASSERT(ft_memcmp(buf, ref_str, sizeof(buf)) == 0);

	TEST_PASS();
}

static int __test_get_next_line(void)
{
	int fd;
	FILE *ref_stream;
	const char *file = "tests/files/libft/lines.txt";
	char *line = NULL;
	char *ref_line = NULL;
	ssize_t rbytes;
	ssize_t ref_rbytes;
	size_t linecapp = 0;

	ref_stream = fopen(file, "r");
	fd = open(file, O_RDONLY);
	TEST_ASSERT(fd >= 0);
	TEST_ASSERT(ref_stream != NULL);

	while ((rbytes = get_next_line(fd, &line)) > 0) {
		ref_rbytes = getline(&ref_line, &linecapp, ref_stream);

		TEST_ASSERT(ref_rbytes > 0);
		TEST_ASSERT(ref_rbytes == rbytes);

		if (ref_line && line) {
			TEST_ASSERT(strlen(ref_line) == strlen(line));
			TEST_ASSERT(strcmp(ref_line, line) == 0);
		}

		if (NULL != ref_line) {
			free(ref_line);
		}

		if (NULL != line) {
			free(line);
		}

		ref_line = NULL;
		linecapp = 0;
		line = NULL;
	}

	close(fd);
	fclose(ref_stream);

	TEST_PASS();
}

static int __test_ft_node(void)
{
	t_node *node;
	char *key = "some-key";
	char *content = "some-content";

	node = ft_node_create();
	TEST_ASSERT(node != NULL);

	ft_node_del(node);

	node = ft_node_new(key, content, strlen(content));
	TEST_ASSERT(node != NULL);
	TEST_ASSERT(node->key != NULL);
	TEST_ASSERT(strcmp(node->key, key) == 0);
	TEST_ASSERT(node->content != NULL);
	TEST_ASSERT(strcmp(node->content, content) == 0);

	ft_node_del(node);

	TEST_PASS();
}

// helper for __test_ft_list()
static int __func_for_test_ft_list(t_node *node, void *farg)
{
	if (NULL == node) {
		return (SSL_OK);
	}

	node->content = NULL;
	node->size = 0;

	return (SSL_OK);
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

	// get words, assume all words are unique (required for some tests)
	words = ft_strsplit(__s2, ' ');
	TEST_ASSERT(words != NULL);

	nwords = ft_2darray_len_null_terminated((void **)words);

	// expect to have at least 6 words
	TEST_ASSERT(nwords >= 6);

	if (NULL == words || nwords == 0) {
		TEST_PASS();
	}

	lst = NULL;

	lst_size = ft_lst_size(lst);
	TEST_ASSERT(lst_size == 0);

	// test list append
	idx = 0;
	while (idx < nwords) {
		node_key = ft_itoa(idx);
		node = ft_node_new(node_key, words[idx], strlen(words[idx]));
		TEST_ASSERT(node != NULL);

		ft_lst_append(&lst, node);
		TEST_ASSERT(lst != NULL);

		if (node_key != NULL) {
			free(node_key);
		}
		idx++;
	}

	// check list size
	lst_size = ft_lst_size(lst);
	TEST_ASSERT(lst_size == nwords);

	// inspect list node content and order
	idx = 0;
	tmp_node = lst;
	while (tmp_node != NULL) {
		node_key = ft_itoa(idx);

		TEST_ASSERT(idx < nwords);
		TEST_ASSERT(strcmp(tmp_node->key, node_key) == 0);
		TEST_ASSERT(strcmp((char *)tmp_node->content, words[idx]) == 0);

		if (node_key != NULL) {
			free(node_key);
		}

		tmp_node = tmp_node->next;
		idx++;
	}

	// check list delete first node
	ft_lst_del_first(&lst);
	lst_size = ft_lst_size(lst);
	TEST_ASSERT(lst_size == nwords-1);
	TEST_ASSERT(strcmp((char *)lst->content, words[1]) == 0);

	// check list delete last node
	ft_lst_del_last(&lst);
	lst_size = ft_lst_size(lst);
	TEST_ASSERT(lst_size == nwords - 2);

	// get last node pointer
	tmp_node = lst;
	while (tmp_node != NULL) {
		node = tmp_node;
		tmp_node = tmp_node->next;
	}

	// check if last node has been deleted
	TEST_ASSERT(strcmp((char *)node->content, words[nwords - 2]) == 0);

	// delete all list nodes
	ft_lst_del(lst);
	lst = NULL;

	// test list prepend
	idx = 0;
	while (idx < nwords) {
		node_key = ft_itoa(idx);
		node = ft_node_new(node_key, words[idx], strlen(words[idx]));
		TEST_ASSERT(node != NULL);

		ft_lst_prepend(&lst, node);

		// test first node content
		TEST_ASSERT(lst != NULL);
		TEST_ASSERT(strcmp(lst->key, node_key) == 0);
		TEST_ASSERT(strcmp((char *)lst->content, words[idx]) == 0);

		if (node_key != NULL) {
			free(node_key);
		}
		idx++;
	}

	// check list size
	lst_size = ft_lst_size(lst);
	TEST_ASSERT(lst_size == nwords);

	// select some node
	tmp_node = lst;
	while (tmp_node != NULL && strcmp(tmp_node->content, words[3]))
		tmp_node = tmp_node->next;

	// expect to find node
	TEST_ASSERT(tmp_node != NULL);

	// test list delete node
	ft_lst_del_one(&lst, tmp_node);

	// search for deleted node
	tmp_node = lst;
	while (tmp_node != NULL && strcmp(tmp_node->content, words[3]))
		tmp_node = tmp_node->next;

	// do not expect to find node
	TEST_ASSERT(tmp_node == NULL);

	// test list map (func sets content to null and size to 0)
	ft_lst_map(lst, NULL, __func_for_test_ft_list);

	// test if nodes have changed
	tmp_node = lst;
	while (tmp_node != NULL) {
		TEST_ASSERT(tmp_node->content == NULL);
		TEST_ASSERT(tmp_node->size == 0);
		tmp_node = tmp_node->next;
	}

	lst = NULL;

	ft_lst_prepend(&lst, ft_node_new("1", "1", 1));
	ft_lst_prepend(&lst, ft_node_new("2", "2", 1));
	ft_lst_prepend(&lst, ft_node_new("3", "3", 1));
	ft_lst_prepend(&lst, ft_node_new("4", "4", 1));

	ft_lst_rev(&lst);

	TEST_ASSERT(strcmp((char *)lst->content, "1") == 0);
	TEST_ASSERT(strcmp((char *)lst->next->content, "2") == 0);
	TEST_ASSERT(strcmp((char *)lst->next->next->content, "3") == 0);
	TEST_ASSERT(strcmp((char *)lst->next->next->next->content, "4") == 0);

	TEST_PASS();
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

	stack = ft_stack_create();
	TEST_ASSERT(stack != NULL);
	TEST_ASSERT(ft_stack_is_empty(stack));
	TEST_ASSERT(ft_stack_size(stack) == 0);

	words = ft_strsplit(__s2, ' ');
	nwords = ft_2darray_len_null_terminated((void **)words);
	TEST_ASSERT(words != NULL);
	TEST_ASSERT(nwords > 6);

	idx = 0;
	while (idx < nwords) {
		key = ft_itoa(idx);
		ft_stack_push(stack, key, words[idx], strlen(words[idx]));

		node = stack->top;
		TEST_ASSERT(node != NULL);
		TEST_ASSERT(strcmp(node->content, words[idx]) == 0);

		if (key != NULL) {
			free(key);
		}

		idx++;
	}

	TEST_ASSERT(ft_stack_size(stack) == nwords);

	node = ft_stack_peek(stack);
	TEST_ASSERT(node != NULL);
	TEST_ASSERT(!ft_stack_is_empty(stack));
	TEST_ASSERT(strcmp(node->content, words[nwords-1]) == 0);

	while (!ft_stack_is_empty(stack)) {
		if (--idx < 0) {
			break ;
		}

		content = ft_stack_pop(stack);
		TEST_ASSERT(strcmp(content, words[idx]) == 0);
	}

	TEST_ASSERT(ft_stack_is_empty(stack));
	TEST_ASSERT(ft_stack_size(stack) == 0);
	ft_stack_del(stack);

	TEST_PASS();
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

	queue = ft_queue_create();
	TEST_ASSERT(queue != NULL);
	TEST_ASSERT(ft_queue_is_empty(queue));
	TEST_ASSERT(ft_queue_size(queue) == 0);

	words = ft_strsplit(__s2, ' ');
	nwords = ft_2darray_len_null_terminated((void **)words);
	TEST_ASSERT(words != NULL);
	TEST_ASSERT(nwords > 6);

	idx = 0;
	while (idx < nwords) {
		key = ft_itoa(idx);
		ft_queue_enqueue(queue, key, words[idx], strlen(words[idx]));

		node = queue->last;
		TEST_ASSERT(node != NULL);
		TEST_ASSERT(strcmp(node->content, words[idx]) == 0);

		if (key != NULL) {
			free(key);
		}

		idx++;
	}

	TEST_ASSERT(ft_queue_size(queue) == nwords);

	node = ft_queue_first(queue);
	TEST_ASSERT(node != NULL);
	TEST_ASSERT(!ft_queue_is_empty(queue));
	TEST_ASSERT(strcmp(node->content, words[0]) == 0);

	idx = 0;
	while (!ft_queue_is_empty(queue)) {
		if (idx >= nwords) {
			break ;
		}

		content = ft_queue_dequeue(queue);
		TEST_ASSERT(strcmp(content, words[idx]) == 0);

		idx++;
	}

	TEST_ASSERT(ft_queue_is_empty(queue));
	TEST_ASSERT(ft_queue_size(queue) == 0);

	idx = 0;
	while (idx < nwords) {
		key = ft_itoa(idx);
		ft_queue_enqueue(queue, key, words[idx], strlen(words[idx]));

		if (key != NULL) {
			free(key);
		}

		idx++;
	}

	// select some node
	node = ft_queue_first(queue);
	while (node != NULL && strcmp(node->content, words[3]))
		node = node->next;

	// expect to find node
	TEST_ASSERT(node != NULL);

	// test queue delete node
	ft_queue_del_node(queue, node);

	// search for deleted node
	node = ft_queue_first(queue);
	while (node != NULL && strcmp(node->content, words[3]))
		node = node->next;

	// do not expect to find node
	TEST_ASSERT(node == NULL);

	ft_queue_del(queue);

	TEST_PASS();
}

static int __test_ft_tuple(void)
{
	t_tuple *tuple;
	const char *head_content = "head";
	const char *tail_content = "tail";
	size_t head_size = ft_strlen(head_content);
	size_t tail_size = ft_strlen(tail_content);

	tuple = ft_tuple_new((void *)head_content, head_size, (void *)tail_content, tail_size);
	TEST_ASSERT(tuple != NULL);
	TEST_ASSERT(tuple->__head != NULL);
	TEST_ASSERT(tuple->__tail != NULL);
	TEST_ASSERT(tuple->__head->content != NULL);
	TEST_ASSERT(tuple->__tail->content != NULL);
	TEST_ASSERT(ft_strcmp(tuple->__head->content, head_content) == 0);
	TEST_ASSERT(ft_strcmp(tuple->__tail->content, tail_content) == 0);
	TEST_ASSERT(tuple->__head->size == head_size);
	TEST_ASSERT(tuple->__tail->size == tail_size);
	ft_tuple_del(tuple);

	tuple = ft_tuple_new_with_f_del((void *)ft_strdup(head_content), head_size, (void *)ft_strdup(tail_content), tail_size, free);
	TEST_ASSERT(tuple != NULL);
	TEST_ASSERT(tuple->__head != NULL);
	TEST_ASSERT(tuple->__tail != NULL);
	TEST_ASSERT(tuple->__head->content != NULL);
	TEST_ASSERT(tuple->__tail->content != NULL);
	TEST_ASSERT(ft_strcmp(tuple->__head->content, head_content) == 0);
	TEST_ASSERT(ft_strcmp(tuple->__tail->content, tail_content) == 0);
	TEST_ASSERT(tuple->__head->size == head_size);
	TEST_ASSERT(tuple->__tail->size == tail_size);
	TEST_ASSERT(tuple->__head->f_del_content == free);
	TEST_ASSERT(tuple->__tail->f_del_content == free);
	ft_tuple_del(tuple);

	tuple = ft_tuple_new_with_f_del((void *)ft_strdup(head_content), head_size, (void *)ft_strdup(tail_content), tail_size, NULL);
	TEST_ASSERT(tuple->__head->f_del_content == NULL);
	TEST_ASSERT(tuple->__tail->f_del_content == NULL);
	ft_tuple_del_with_f_del(tuple, free);

	tuple = ft_tuple_new((void *)head_content, head_size, (void *)tail_content, tail_size);
	TEST_ASSERT(ft_tuple_get(tuple, 0) != NULL);
	TEST_ASSERT(ft_tuple_get(tuple, 1) != NULL);
	TEST_ASSERT(ft_strcmp(ft_tuple_get(tuple, 0), head_content) == 0);
	TEST_ASSERT(ft_strcmp(ft_tuple_get(tuple, 1), tail_content) == 0);
	ft_tuple_del(tuple);

	TEST_PASS();
}

static int __test_ft_htbl(void)
{
	t_htbl *htbl;
	t_node *node;
	void *content;
	char **words;
	char **test_content;
	int test_num;
	int nwords;
	int idx;

	words = ft_strsplit(__s2, ' ');
	nwords = ft_2darray_len_null_terminated((void **)words);
	TEST_ASSERT(words != NULL);
	TEST_ASSERT(nwords > 6);

	LIBFT_ALLOC(test_content, sizeof(char *) * (nwords + 1));

	idx = 0;
	while (idx < nwords) {
		asprintf(test_content + idx, "%s-%d", words[idx], idx);
		idx++;
	}

	htbl = ft_htbl_create(nwords);
	TEST_ASSERT(htbl != NULL);
	TEST_ASSERT(htbl->size > 0);
	TEST_ASSERT(htbl->arr != NULL);

	idx = 0;
	while (idx < nwords) {
		ft_htbl_add(htbl, words[idx], words[idx]);
		content = ft_htbl_get(htbl, words[idx]);
		TEST_ASSERT(content != NULL);
		TEST_ASSERT(strcmp(content, words[idx]) == 0);

		idx++;
	}

	idx = 0;
	while (idx < nwords) {
		content = test_content[idx];
		ft_htbl_assign(htbl, content, words[idx]);
		content = ft_htbl_get(htbl, words[idx]);
		TEST_ASSERT(content != NULL);
		TEST_ASSERT(strcmp(content, test_content[idx]) == 0);
		idx++;
	}

	test_num = htbl->size;
	ft_htbl_resize(htbl, 2 * LIBFT_HT_SIZE);
	TEST_ASSERT(htbl->size == 2 * LIBFT_HT_SIZE);

	idx = 0;
	while (idx < nwords) {
		content = ft_htbl_get(htbl, words[idx]);
		TEST_ASSERT(content != NULL);
		TEST_ASSERT(strcmp(content, test_content[idx]) == 0);
		idx++;
	}

	char **content_parts;
	int	nparts;
	int word_ht[nwords];

	bzero(word_ht, sizeof(word_ht));

	node = NULL;
	while ((node = ft_htbl_node_next(htbl, node)) != NULL) {
		TEST_ASSERT(node != NULL);
		TEST_ASSERT(node->key != NULL);
		TEST_ASSERT(node->content != NULL);

		content_parts = ft_strsplit(node->content, '-');
		TEST_ASSERT(content_parts != NULL);

		nparts = ft_2darray_len_null_terminated((void **)content_parts);
		TEST_ASSERT(nparts == 2);

		test_num = atoi(content_parts[1]);
		TEST_ASSERT(test_num < nwords);

		ft_2darray_del_null_terminated((void **)content_parts);

		word_ht[test_num] += 1;
	}
	idx = 0;
	while (idx < nwords) {
		TEST_ASSERT(word_ht[idx] == 1);
		idx++;
	}

	ft_htbl_del_key(htbl, words[3]);
	content = ft_htbl_get(htbl, words[3]);

	TEST_ASSERT(content == NULL);

	ft_htbl_del(htbl);
	ft_2darray_del_null_terminated((void **)words);
	ft_2darray_del_null_terminated((void **)test_content);

	TEST_PASS();
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

	words = ft_strsplit(__s2, ' ');
	nwords = ft_2darray_len_null_terminated((void **)words);
	TEST_ASSERT(words != NULL);
	TEST_ASSERT(nwords > 6);

	LIBFT_ALLOC(len_arr, sizeof(size_t *) * nwords);

	idx = 0;
	while (idx < nwords) {
		word = words[idx];
		len = strlen(word);

		word[len / 2] = 0;
		len_arr[idx] = len;

		idx++;
	}

	htbl = ft_htbl_create(nwords);
	TEST_ASSERT(htbl != NULL);
	TEST_ASSERT(htbl->size > 0);
	TEST_ASSERT(htbl->arr != NULL);

	idx = 0;
	while (idx < nwords) {
		ft_htbl_add_rawkey(htbl, words[idx], words[idx], len_arr[idx]);
		content = ft_htbl_get_rawkey(htbl, words[idx], len_arr[idx]);
		TEST_ASSERT(content != NULL);
		TEST_ASSERT(memcmp(content, words[idx], len_arr[idx]) == 0);

		idx++;
	}

	word = "test";

	idx = 0;
	while (idx < nwords) {
		ft_htbl_assign_rawkey(htbl, word, words[idx], len_arr[idx]);
		content = ft_htbl_get_rawkey(htbl, words[idx], len_arr[idx]);
		TEST_ASSERT(content != NULL);

		if (content) {
			TEST_ASSERT(memcmp(content, word, strlen(word)) == 0);
		}

		idx++;
	}

	ft_htbl_del(htbl);
	ft_2darray_del_null_terminated((void **)words);
	LIBFT_FREE(len_arr);

	TEST_PASS();
}

static int __test_ft_htbl_conversion(void)
{
	char *key;
	t_node *node;
	t_node *lst;
	t_stack *stack;
	t_queue *queue;
	t_htbl *htbl;
	char *word;
	char **words;
	int nwords;
	int idx;

	words = ft_strsplit(__s2, ' ');
	nwords = ft_2darray_len_null_terminated((void **)words);
	TEST_ASSERT(words != NULL);
	TEST_ASSERT(nwords > 6);

	lst = NULL;
	stack = ft_stack_create();
	queue = ft_queue_create();

	idx = 0;
	while (idx < nwords) {
		key = ft_itoa(idx);

		node = ft_node_new(key, words[idx], strlen(words[idx]));
		ft_lst_prepend(&lst, node);

		ft_queue_enqueue(queue, key, words[idx], strlen(words[idx]));

		ft_stack_push(stack, key, words[idx], strlen(words[idx]));

		LIBFT_FREE(key);
		idx++;
	}

	htbl = ft_lst_to_htable(lst);

	idx = 0;
	while (idx < nwords) {
		key = ft_itoa(idx);
		word = ft_htbl_get(htbl, key);

		TEST_ASSERT(word != NULL);

		if (word) {
			TEST_ASSERT(strcmp(word, words[idx]) == 0);
		}

		LIBFT_FREE(key);
		idx++;
	}

	ft_htbl_del(htbl);

	htbl = ft_queue_to_htable(queue);

	idx = 0;
	while (idx < nwords) {
		key = ft_itoa(idx);
		word = ft_htbl_get(htbl, key);

		TEST_ASSERT(word != NULL);

		if (word) {
			TEST_ASSERT(strcmp(word, words[idx]) == 0);
		}

		LIBFT_FREE(key);
		idx++;
	}

	ft_htbl_del(htbl);

	htbl = ft_stack_to_htable(stack);

	idx = 0;
	while (idx < nwords) {
		key = ft_itoa(idx);
		word = ft_htbl_get(htbl, key);

		TEST_ASSERT(word != NULL);

		if (word) {
			TEST_ASSERT(strcmp(word, words[idx]) == 0);
		}

		LIBFT_FREE(key);
		idx++;
	}

	ft_htbl_del(htbl);

	ft_lst_del(lst);
	ft_queue_del(queue);
	ft_stack_del(stack);

	ft_2darray_del_null_terminated((void **)words);

	TEST_PASS();
}

static int __test_ft_ntree_dfs_depth_func(t_node *node, const void *farg)
{
	char **key_parts;
	int cur_depth;
	int	expected_depth;
	int	nparts;

	if (NULL == node) {
		return (-1);
	}

	(void)farg;

	if (NULL != node->key) {
		key_parts = ft_strsplit(node->key, ':');

		if (NULL == key_parts) {
			return (-1);
		}

		nparts = ft_2darray_len_null_terminated((void **)key_parts);

		if (nparts == 2) {
			expected_depth = atoi(key_parts[1]);
			cur_depth = ft_ntree_dfs_cur_depth();
		}

		ft_2darray_del_null_terminated((void **)key_parts);

		if (nparts != 2 || expected_depth != cur_depth) {
			return (-2);
		}
	}

	return (0);
}

static int __test_ft_ntree_dfs_order_func(t_node *node, const void *farg)
{
	char ***words;

	if (NULL == node) {
		return (-1);
	}

	words = (char ***)farg;

	if (NULL != node->key) {
		if (**words == NULL) {
			return (-1);
		}

		if (strcmp(**words, node->key) != 0) {
			return (-1);
		}

		*words = *words + 1;
	}

	return (0);
}

static int __test_ft_ntree_helper_func(t_node *node, const void *farg)
{
	t_htbl *word_ht;
	void *content;
	int	*counter_ptr;

	if (NULL == node) {
		return (-1);
	}

	if (NULL != node->key) {
		word_ht = (t_htbl *)farg;
		content = ft_htbl_get(word_ht, node->key);

		counter_ptr = (int *)content;

		if (NULL == counter_ptr) {
			return (-3);
		}

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
	TEST_ASSERT(ntree != NULL);

	// filter out words
	// make tmp words array
	char **tmp;
	LIBFT_ALLOC(tmp, (nwords + 1) * sizeof(char *));

	nwords = 0;
	idx = 0;
	while (words[idx] != NULL) {
		// if word contains '{' symbol, get part of the word before that sytmbol
		if (ft_strchr(words[idx], '{')) {
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
	word_ht = ft_htbl_create(nwords);

	// create hash map for word counts
	int counter_arr[nwords];

	// populate words htable with word counts initialized to zero
	idx = 0;
	while (idx < nwords) {
		counter_arr[idx] = 0;
		ft_htbl_add(word_ht, counter_arr + idx, words[idx]);
		idx++;
	}

	// count words in ntree using words htable
	ret = ft_ntree_bfs(NULL, ntree, word_ht, __test_ft_ntree_helper_func);
	TEST_ASSERT(ret == 0);

	// check if all words are present in ntree exactly once
	idx = 0;
	while (idx < nwords)
		if (counter_arr[idx++] != 1) {
			break;
		}

	TEST_ASSERT(idx == nwords);

	//
	// test ntree DFS
	//

	// populate words htable with word counts initialized to zero
	idx = 0;
	while (idx < nwords) {
		counter_arr[idx] = 0;
		ft_htbl_add(word_ht, counter_arr + idx, words[idx]);
		idx++;
	}

	// count words in ntree using words htable
	ret = ft_ntree_dfs(NULL, ntree, word_ht, __test_ft_ntree_helper_func);
	TEST_ASSERT(ret == 0);

	// check if all words are present in ntree exactly once
	idx = 0;
	while (idx < nwords)
		if (counter_arr[idx++] != 1) {
			break;
		}

	TEST_ASSERT(idx == nwords);

	//
	// check word depth distribution within ntree using DFS
	//

	ret = ft_ntree_dfs(NULL, ntree, NULL, __test_ft_ntree_dfs_depth_func);
	TEST_ASSERT(ret == 0);

	//
	// check topology of ntree nodes by matching the order of words
	// in word array with order of words during DFS 
	//

	char **tmp_ptr = words;

	ret = ft_ntree_dfs(NULL, ntree, &tmp_ptr, __test_ft_ntree_dfs_order_func);
	TEST_ASSERT(ret == 0);

	//
	// test ntree "iterator"
	//

	t_node *iter;
	t_node *item;
	void *content;
	int *counter;

	// get iterator list
	iter = ft_ntree_iter(ntree);
	TEST_ASSERT(iter != NULL);

	// init counter array to zeros
	bzero(counter_arr, sizeof(counter_arr));

	// get counter pointer from word htable using node keys from iterator list
	// increment counter to count number of times each word is seen in iterator list
	node = iter;
	while (node != NULL) {
		item = node->content;

		if (item != NULL) {
			content = ft_htbl_get(word_ht, item->key);
			counter = (int *)content;

			if (counter != NULL) {
				*counter = *counter + 1;
			}
		}

		node = node->next;
	}

	ft_lst_del(iter);

	// check if all words are present in iterator list exactly once
	idx = 0;
	while (idx < nwords)
		if (counter_arr[idx++] != 1) {
			break;
		}

	// size of ntree
	size = ft_ntree_size(ntree);

	TEST_ASSERT(size == nwords);

	ft_2darray_del_null_terminated((void **)words);

	TEST_PASS();
}

struct s_mock_adapter_ctx {
	void	*source;
	void	*sink;
};

static ssize_t __mock_read_adapter(void *vctx, const void *buf, size_t bufsize)
{
	struct s_mock_adapter_ctx *ctx;

	ctx = vctx;
	ft_memcpy((void *)buf, ctx->sink, bufsize);

	return ((ssize_t)bufsize);
}

static ssize_t __mock_write_adapter(void *vctx, void *buf, size_t bufsize)
{
	struct s_mock_adapter_ctx *ctx;

	ctx = vctx;
	ft_memcpy(ctx->source, buf, bufsize);

	return ((ssize_t)bufsize);
}

static int __test_ft_buffer(void)
{
	t_ostring ref_content;
	t_buffer *buffer;
	const size_t buf_capacity = 1024;
	char buf[buf_capacity], sink[buf_capacity], source[buf_capacity];
	struct s_mock_adapter_ctx ctx = {.source = source, .sink = sink};
	ssize_t wbytes, rbytes;

	if (SSL_OK != file_read_all("tests/files/text/large.txt", &ref_content)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		TEST_FAIL();
	}
	buffer = ft_buffer_new(buf_capacity);
	TEST_ASSERT(buffer != NULL);
	TEST_ASSERT(ft_buffer_is_empty(buffer));
	TEST_ASSERT(ft_buffer_available(buffer) == buf_capacity);

	// write to empty buffer
	wbytes = ft_buffer_write(buffer, ref_content.content, 128);
	TEST_ASSERT(wbytes == 128);
	TEST_ASSERT(!ft_buffer_is_empty(buffer));
	TEST_ASSERT(!ft_buffer_is_full(buffer));
	TEST_ASSERT(ft_buffer_used(buffer) == 128);
	TEST_ASSERT(ft_buffer_available(buffer) == buf_capacity - 128);
	TEST_ASSERT(ft_memcmp(buffer->arr, ref_content.content, 128) == 0);
	TEST_ASSERT(ft_buffer_view(buffer) == (const char *)buffer->arr);

	// read from buffer
	rbytes = ft_buffer_read(buffer, buf, 128);
	TEST_ASSERT(rbytes == 128);
	TEST_ASSERT(ft_buffer_is_empty(buffer));
	TEST_ASSERT(!ft_buffer_is_full(buffer));
	TEST_ASSERT(ft_buffer_used(buffer) == 0);
	TEST_ASSERT(ft_buffer_available(buffer) == buf_capacity);
	TEST_ASSERT(ft_memcmp(buf, ref_content.content, 128) == 0);
	TEST_ASSERT(ft_buffer_view(buffer) == (const char *)buffer->arr);

	ft_buffer_reset(buffer);

	// write to buffer with adapter
	wbytes = ft_buffer_write_with_func(buffer, __mock_write_adapter, &ctx, 128);
	TEST_ASSERT(wbytes == 128);
	TEST_ASSERT(!ft_buffer_is_empty(buffer));
	TEST_ASSERT(!ft_buffer_is_full(buffer));
	TEST_ASSERT(ft_buffer_used(buffer) == 128);
	TEST_ASSERT(ft_buffer_available(buffer) == buf_capacity - 128);
	TEST_ASSERT(ft_memcmp(buffer->arr, ref_content.content, 128) == 0);
	TEST_ASSERT(ft_buffer_view(buffer) == (const char *)buffer->arr);

	// read from buffer with adapter
	rbytes = ft_buffer_read_with_func(buffer, __mock_read_adapter, &ctx, 128);
	TEST_ASSERT(rbytes == 128);
	TEST_ASSERT(ft_buffer_is_empty(buffer));
	TEST_ASSERT(!ft_buffer_is_full(buffer));
	TEST_ASSERT(ft_buffer_used(buffer) == 0);
	TEST_ASSERT(ft_buffer_available(buffer) == buf_capacity);
	TEST_ASSERT(ft_memcmp(buf, ref_content.content, 128) == 0);
	TEST_ASSERT(ft_buffer_view(buffer) == (const char *)buffer->arr);

	ft_buffer_reset(buffer);

	// multiple writes
	wbytes = ft_buffer_write(buffer, ref_content.content, 128);
	TEST_ASSERT(wbytes == 128);
	TEST_ASSERT(!ft_buffer_is_empty(buffer));
	TEST_ASSERT(!ft_buffer_is_full(buffer));

	wbytes = ft_buffer_write(buffer, ref_content.content + 128, 128);
	TEST_ASSERT(wbytes == 128);
	TEST_ASSERT(ft_memcmp(buffer->arr, ref_content.content, 2 * 128) == 0);
	TEST_ASSERT(!ft_buffer_is_empty(buffer));
	TEST_ASSERT(!ft_buffer_is_full(buffer));
	TEST_ASSERT(ft_buffer_used(buffer) == 2 * 128);
	TEST_ASSERT(ft_buffer_available(buffer) == buf_capacity - 2 * 128);
	TEST_ASSERT(ft_buffer_view(buffer) == (const char *)buffer->arr);

	// multiple reads
	rbytes = ft_buffer_read(buffer, buf, 128);
	TEST_ASSERT(rbytes == 128);
	TEST_ASSERT(!ft_buffer_is_empty(buffer));
	TEST_ASSERT(!ft_buffer_is_full(buffer));
	TEST_ASSERT(ft_buffer_used(buffer) == 128);
	TEST_ASSERT(ft_buffer_available(buffer) == buf_capacity - 128);
	TEST_ASSERT(ft_buffer_view(buffer) == (const char *)buffer->arr + 128);

	rbytes = ft_buffer_read(buffer, buf, 128);
	TEST_ASSERT(rbytes == 128);
	TEST_ASSERT(ft_memcmp(buf, ref_content.content + 128, 128) == 0);
	TEST_ASSERT(ft_buffer_is_empty(buffer));
	TEST_ASSERT(!ft_buffer_is_full(buffer));
	TEST_ASSERT(ft_buffer_used(buffer) == 0);
	TEST_ASSERT(ft_buffer_available(buffer) == buf_capacity);
	TEST_ASSERT(ft_buffer_view(buffer) == (const char *)buffer->arr);

	// try to write more than capacity
	wbytes = ft_buffer_write(buffer, ref_content.content, 2 * buf_capacity);
	TEST_ASSERT(wbytes == buf_capacity);
	TEST_ASSERT(!ft_buffer_is_empty(buffer));
	TEST_ASSERT(ft_buffer_is_full(buffer));
	TEST_ASSERT(ft_buffer_used(buffer) == buf_capacity);
	TEST_ASSERT(ft_buffer_available(buffer) == 0);

	// try to read more than used
	rbytes = ft_buffer_read(buffer, buf, 2 * buf_capacity);
	TEST_ASSERT(rbytes == buf_capacity);
	TEST_ASSERT(ft_memcmp(buf, ref_content.content, buf_capacity) == 0);
	TEST_ASSERT(ft_buffer_is_empty(buffer));
	TEST_ASSERT(!ft_buffer_is_full(buffer));
	TEST_ASSERT(ft_buffer_used(buffer) == 0);
	TEST_ASSERT(ft_buffer_available(buffer) == buf_capacity);

	// partially filled buffer, left pad = 128, right pad = cap - 256
	ssize_t trbytes = 0;
	ssize_t twbytes = 0;
	ssize_t left_pad = 0;
	ssize_t right_pad = buf_capacity;

	wbytes = ft_buffer_write(buffer, ref_content.content, 256);
	TEST_ASSERT(wbytes == 256);
	twbytes += wbytes;
	right_pad -= wbytes;
	rbytes = ft_buffer_read(buffer, buf, 128);
	TEST_ASSERT(rbytes == 128);
	trbytes += rbytes;
	left_pad += rbytes;

	// write size is less than right pad size
	wbytes = ft_buffer_write(buffer, ref_content.content + twbytes, 256);
	TEST_ASSERT(wbytes == 256);
	twbytes += wbytes;
	right_pad -= wbytes;
	TEST_ASSERT(ft_buffer_used(buffer) == twbytes - trbytes);

	// read from partially filled buffer
	rbytes = ft_buffer_read(buffer, buf + trbytes, 128);
	TEST_ASSERT(rbytes == 128);
	trbytes += rbytes;
	left_pad += rbytes;
	TEST_ASSERT(ft_buffer_used(buffer) == twbytes - trbytes);

	// write size is greater than right pad size but less than available
	const void *view_before = ft_buffer_view(buffer);
	wbytes = ft_buffer_write(buffer, ref_content.content + twbytes, buf_capacity - right_pad + 128);
	TEST_ASSERT(wbytes == buf_capacity - right_pad + 128);
	twbytes += wbytes;
	TEST_ASSERT(ft_buffer_used(buffer) == twbytes - trbytes);
	// must have left aligned buffer -> left pad = 0, right pad = buf_capacity - 128
	TEST_ASSERT(view_before == ft_buffer_view(buffer) + left_pad);
	// content of buffer must be as expected
	TEST_ASSERT(ft_memcmp(ft_buffer_view(buffer), ref_content.content + trbytes, twbytes - trbytes) == 0);
	// content of buf must be as expected
	TEST_ASSERT(ft_memcmp(buf, ref_content.content, trbytes) == 0);

	// write size is greater than right pad size and more than available
	ft_buffer_reset(buffer);
	trbytes = 0;
	twbytes = 0;
	left_pad = 0;
	right_pad = buf_capacity;
	wbytes = ft_buffer_write(buffer, ref_content.content + twbytes, buf_capacity);
	TEST_ASSERT(wbytes == buf_capacity);
	twbytes += wbytes;
	right_pad -= wbytes;
	rbytes = ft_buffer_read(buffer, buf + trbytes, 512);
	TEST_ASSERT(rbytes == 512);
	trbytes += rbytes;
	left_pad += rbytes;
	// try to write more than available
	view_before = ft_buffer_view(buffer);
	wbytes = ft_buffer_write(buffer, ref_content.content + twbytes, right_pad + buf_capacity);
	TEST_ASSERT(wbytes == right_pad + left_pad);
	twbytes += wbytes;
	TEST_ASSERT(ft_buffer_is_full(buffer));
	// must have left aligned buffer
	TEST_ASSERT(view_before == ft_buffer_view(buffer) + left_pad);
	// content of buffer must be as expected
	TEST_ASSERT(ft_memcmp(ft_buffer_view(buffer), ref_content.content + trbytes, twbytes - trbytes) == 0);
	// content of buf must be as expected
	TEST_ASSERT(ft_memcmp(buf, ref_content.content, trbytes) == 0);

	// try to read more than used
	ft_buffer_reset(buffer);
	trbytes = 0;
	twbytes = 0;
	wbytes = ft_buffer_write(buffer, ref_content.content + twbytes, 512);
	TEST_ASSERT(wbytes == 512);
	twbytes += wbytes;
	rbytes = ft_buffer_read(buffer, buf + trbytes, 128);
	TEST_ASSERT(rbytes == 128);
	trbytes += rbytes;
	rbytes = ft_buffer_read(buffer, buf + trbytes, 512);
	TEST_ASSERT(rbytes == twbytes - trbytes);
	TEST_ASSERT(ft_buffer_is_empty(buffer));
	// must have reset the buffer
	TEST_ASSERT(ft_buffer_used(buffer) == 0);
	TEST_ASSERT(ft_buffer_available(buffer) == buf_capacity);
	TEST_ASSERT(ft_buffer_view(buffer) == (const char *)buffer->arr);
	// content of buf must be as expected
	TEST_ASSERT(ft_memcmp(buf, ref_content.content, trbytes) == 0);

	ft_buffer_del(buffer);

	TEST_PASS();
}