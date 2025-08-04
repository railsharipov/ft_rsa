#include <common.h>
#include <textutil.h>
#include <test.h>

static int	__test_textutil_find(void);
static int	__test_textutil_findf(void);
static int	__test_textutil_seek(void);
static int	__test_textutil_seekf(void);
static int	__test_textutil_del_wspace(void);
static int	__test_textutil_del_blank(void);
static int	__test_textutil_del_eolws(void);
static int	__test_textutil_del_eol(void);
static int	__test_textutil_del_empty_lines(void);
static int	__test_textutil_insert_delim(void);
static int	__test_textutil_sscanf(void);

static const char	*__lorem = "Cernantur iis sunt, voluptate export nulla \
arbitror noster. Se nescius exercitation. Malis nescius o consectetur \
est excepteur multos dolor excepteur quae id ut aute incididunt \
domesticarum eu malis a voluptate ubi\t\tita esse pariatur offendit de \
quamquam quid qui commodo cohaerescant si do elit incurreret singulis, \
est ita consectetur. Ut veniam\nconstias deserunt, nisi cupidatat est esse \
cillum. O id arbitrantur si\n\n\neiusmod velit nostrud probant sed probant nisi \
constias si eram pariatur ubi coniunctione, magna voluptate se probant, \
iis eu tamen irure sunt id laborum\n\ntempor arbitror, excepteur ad noster. \
Laboris enim constias doctrina. Elit nostrud in multos sint.";

int	test_textutil(void)
{
	return (
		__test_textutil_find()
		| __test_textutil_findf()
		| __test_textutil_seek()
		| __test_textutil_seekf()
		| __test_textutil_del_wspace()
		| __test_textutil_del_blank()
		| __test_textutil_del_eolws()
		| __test_textutil_del_eol()
		| __test_textutil_del_empty_lines()
		| __test_textutil_insert_delim()
		| __test_textutil_sscanf()
	);
}

static int	__test_textutil_find(void)
{
	char	*str;
	int		pos, ref_pos;

	pos = textutil_find(__lorem, ft_strlen(__lorem), __lorem, ft_strlen(__lorem));
	TEST_ASSERT(pos == 0);

	str = "Cernantur";
	pos = textutil_find(__lorem, ft_strlen(__lorem), str, ft_strlen(str));
	TEST_ASSERT(pos == 0);

	str = "Elit";
	pos = textutil_find(__lorem, ft_strlen(__lorem), str, ft_strlen(str));
	ref_pos = ft_strlen(__lorem) - ft_strlen("Elit nostrud in multos sint.");
	TEST_ASSERT(pos == ref_pos);

	str = "Where-banana";
	pos = textutil_find(__lorem, ft_strlen(__lorem), str, ft_strlen(str));
	TEST_ASSERT(pos == -1);

	str = "sint.WhereBANANA";
	pos = textutil_find(__lorem, ft_strlen(__lorem), str, ft_strlen(str));
	TEST_ASSERT(pos == -1);

	str = "";
	pos = textutil_find(__lorem, ft_strlen(__lorem), str, ft_strlen(str));
	TEST_ASSERT(pos == 0);

	str = NULL;
	pos = textutil_find(__lorem, ft_strlen(__lorem), str, 0);
	TEST_ASSERT(pos == -1);

	TEST_PASS();
}

static int	__test_textutil_findf(void)
{
	char	*str;
	int		pos, ref_pos;

	str = "Cernantur";
	pos = textutil_findf(__lorem, ft_strlen(__lorem), "%s", str);
	TEST_ASSERT(pos == 0);

	str = "iis sunt, voluptate";
	pos = textutil_findf(__lorem, ft_strlen(__lorem), "Cernantur %s", str);
	TEST_ASSERT(pos == 0);

	str = "Elit";
	pos = textutil_findf(__lorem, ft_strlen(__lorem), "%s nostrud", str);
	ref_pos = ft_strlen(__lorem) - ft_strlen("Elit nostrud in multos sint.");
	TEST_ASSERT(pos == ref_pos);

	str = "Where-banana";
	pos = textutil_findf(__lorem, ft_strlen(__lorem), "Cernantur iis sunt, %s", str);
	TEST_ASSERT(pos == -1);

	str = "";
	pos = textutil_findf(__lorem, ft_strlen(__lorem), "%s", str);
	TEST_ASSERT(pos == 0);

	str = NULL;
	pos = textutil_findf(__lorem, ft_strlen(__lorem), "%s", str);
	TEST_ASSERT(pos == -1);

	TEST_PASS();
}

static int	__test_textutil_seek(void)
{
	char	*str;
	int		pos, seek;

	seek = textutil_seek(__lorem, ft_strlen(__lorem), __lorem, ft_strlen(__lorem));
	TEST_ASSERT(seek == ft_strlen(__lorem));

	str = "Cernantur";
	seek = textutil_seek(__lorem, ft_strlen(__lorem), str, ft_strlen(str));
	TEST_ASSERT(seek == ft_strlen(str));

	str = "Elit";
	pos = textutil_find(__lorem, ft_strlen(__lorem), str, ft_strlen(str));
	seek = textutil_seek(__lorem + pos, ft_strlen(__lorem) - pos, str, ft_strlen(str));
	TEST_ASSERT(seek == ft_strlen(str));

	str = "Where-banana";
	seek = textutil_seek(__lorem, ft_strlen(__lorem), str, ft_strlen(str));
	TEST_ASSERT(seek == 0);

	str = "sint.WhereBANANA";
	seek = textutil_seek(__lorem, ft_strlen(__lorem), str, ft_strlen(str));
	TEST_ASSERT(seek == 0);

	str = "";
	seek = textutil_seek(__lorem, ft_strlen(__lorem), str, ft_strlen(str));
	TEST_ASSERT(seek == 0);

	str = NULL;
	seek = textutil_seek(__lorem, ft_strlen(__lorem), str, 0);
	TEST_ASSERT(seek == 0);

	TEST_PASS();
}

static int	__test_textutil_seekf(void)
{
	char	*str;
	int		seek, pos;

	str = "Cernantur";
	seek = textutil_seekf(__lorem, ft_strlen(__lorem), "%s", str);
	TEST_ASSERT(seek == ft_strlen(str));

	str = "iis sunt, voluptate";
	seek = textutil_seekf(__lorem, ft_strlen(__lorem), "Cernantur %s", str);
	TEST_ASSERT(seek == ft_strlen("Cernantur ") + ft_strlen(str));

	str = "Elit";
	pos = textutil_findf(__lorem, ft_strlen(__lorem), "%s nostrud", str);
	TEST_ASSERT(pos > 0);
	seek = textutil_seekf(__lorem + pos, ft_strlen(__lorem) - pos, "%s nostrud", str);
	TEST_ASSERT(seek == ft_strlen(str) + ft_strlen(" nostrud"));

	str = "Where-banana";
	seek = textutil_seekf(__lorem, ft_strlen(__lorem), "Cernantur iis sunt, %s", str);
	TEST_ASSERT(seek == 0);

	str = "";
	seek = textutil_seekf(__lorem, ft_strlen(__lorem), "%s", str);
	TEST_ASSERT(seek == 0);

	str = NULL;
	seek = textutil_seekf(__lorem, ft_strlen(__lorem), "%s", str);
	TEST_ASSERT(seek == 0);

	TEST_PASS();
}

static int	__test_textutil_del_wspace(void)
{
	char	lorem_no_ws[1024] = {0};
	char 	*str;
	size_t	len;
	int		ret;

	len = 0;
	for (int i = 0; i < ft_strlen(__lorem) && len < sizeof(lorem_no_ws); i++) {
		if (!ft_iswspace(__lorem[i])) {
			lorem_no_ws[len++] = __lorem[i];
		}
	}

	ret = textutil_del_wspace(__lorem, ft_strlen(__lorem), &str, &len);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(len == ft_strlen(lorem_no_ws));
	TEST_ASSERT(ft_memcmp(str, lorem_no_ws, len) == 0);

	TEST_PASS();
}

static int	__test_textutil_del_blank(void)
{
	char	lorem_no_blank[1024] = {0};
	char 	*str;
	size_t	len;
	int		ret;

	len = 0;
	for (int i = 0; i < ft_strlen(__lorem) && len < sizeof(lorem_no_blank); i++) {
		if (!ft_isblank(__lorem[i])) {
			lorem_no_blank[len++] = __lorem[i];
		}
	}

	ret = textutil_del_blank(__lorem, ft_strlen(__lorem), &str, &len);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(len == ft_strlen(lorem_no_blank));
	TEST_ASSERT(ft_memcmp(str, lorem_no_blank, len) == 0);

	TEST_PASS();
}

static int	__test_textutil_del_eolws(void)
{
	char	lorem_no_ws[1024] = {0};
	char 	*str;
	size_t	len;
	int		ret;

	len = 0;
	for (int i = 0; i < ft_strlen(__lorem) && len < sizeof(lorem_no_ws); i++) {
		if (!ft_iseolws(__lorem[i])) {
			lorem_no_ws[len++] = __lorem[i];
		}
	}

	ret = textutil_del_eolws(__lorem, ft_strlen(__lorem), &str, &len);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(len == ft_strlen(lorem_no_ws));
	TEST_ASSERT(ft_memcmp(str, lorem_no_ws, len) == 0);

	TEST_PASS();
}

static int	__test_textutil_del_eol(void)
{
	char	lorem_no_eol[1024] = {0};
	char 	*str;
	size_t	len;
	int		ret;

	TEST_ASSERT(sizeof(lorem_no_eol) > ft_strlen(__lorem) && "buf can't be smaller than the length of the original string");

	len = 0;
	for (int i = 0; i < ft_strlen(__lorem) && len < sizeof(lorem_no_eol); i++) {
		if (!ft_iseol(__lorem[i])) {
			lorem_no_eol[len++] = __lorem[i];
		}
	}

	ret = textutil_del_eol(__lorem, ft_strlen(__lorem), &str, &len);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(len == ft_strlen(lorem_no_eol));
	TEST_ASSERT(ft_memcmp(str, lorem_no_eol, len) == 0);

	TEST_PASS();
}

static int	__test_textutil_del_empty_lines(void)
{
	char	lorem_with_empty_lines[] = "Cernantur iis sunt, voluptate export nulla \
arbitror noster. \n\n\n\nSe nescius exercitation.\n\n\n\n";
	char	lorem_without_empty_lines[] = "Cernantur iis sunt, voluptate export nulla \
arbitror noster. \nSe nescius exercitation.\n";
	char 	*str;
	size_t	len;
	int		ret;

	ret = textutil_del_empty_lines(lorem_with_empty_lines, ft_strlen(lorem_with_empty_lines), &str, &len);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(len == ft_strlen(lorem_without_empty_lines));
	TEST_ASSERT(ft_memcmp(str, lorem_without_empty_lines, len) == 0);

	TEST_PASS();
}

static int	__test_textutil_insert_delim(void)
{
	char	buf[2048] = {0};
	int		step = 16;
	char 	*str_no_delim, *str_with_delim;
	size_t	len_no_delim, len_with_delim;
	int		ret;

	ret = textutil_del_eol(__lorem, ft_strlen(__lorem), &str_no_delim, &len_no_delim);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(sizeof(buf) > 2 * len_no_delim && "buf can't be smaller than 2x the length of the original string");

	for (size_t i = 0, j = 0; i < len_no_delim; i++) {
		if (i != 0 && i % (size_t)step == 0) {
			buf[j++] = '\n';
		}
		buf[j++] = str_no_delim[i];
	}
	
	ret = textutil_insert_delim(str_no_delim, len_no_delim, &str_with_delim, &len_with_delim, '\n', step);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(len_with_delim == ft_strlen(buf));
	TEST_ASSERT(ft_strncmp(str_with_delim, buf, len_with_delim) == 0);

	TEST_PASS();
}

static int	__test_textutil_sscanf(void)
{
	int		matches;
	
	char	*s1;
	matches = textutil_sscanf(__lorem, ft_strlen(__lorem), "%s", &s1);
	TEST_ASSERT(matches == 1);
	TEST_ASSERT(s1 != NULL);
	TEST_ASSERT(ft_streq(s1, "Cernantur"));
	
	char	str2[] = "Test-string 10";
	char	*s2;
	int		i2;
	matches = textutil_sscanf(str2, ft_strlen(str2), "%s %d", &s2, &i2);
	TEST_ASSERT(matches == 2);
	TEST_ASSERT(s2 != NULL);
	TEST_ASSERT(ft_streq(s2, "Test-string"));
	TEST_ASSERT(i2 == 10);

	char			str3[] = "Test 10 10 T 10 10";
	char			*s3;
	int				i3;
	unsigned int	u3;
	char			c3;
	ssize_t			zd3;
	size_t			zu3;

	matches = textutil_sscanf(str3, ft_strlen(str3), "%s %d %u %c %zd %zu", &s3, &i3, &u3, &c3, &zd3, &zu3);
	TEST_ASSERT(matches == 6);
	TEST_ASSERT(s3 != NULL);
	TEST_ASSERT(ft_streq(s3, "Test"));
	TEST_ASSERT(i3 == 10);
	TEST_ASSERT(u3 == 10);
	TEST_ASSERT(c3 == 'T');
	TEST_ASSERT(zd3 == 10);
	TEST_ASSERT(zu3 == 10);

	char	str4[] = "Test-string-214-#R@#$R(&): 10";
	char	*s4;
	int		i4;
	matches = textutil_sscanf(str4, ft_strlen(str4), "%[^:]: %d", &s4, &i4);
	TEST_ASSERT(matches == 2);
	TEST_ASSERT(s4 != NULL);
	TEST_ASSERT(ft_streq(s4, "Test-string-214-#R@#$R(&)"));
	TEST_ASSERT(i4 == 10);

	TEST_PASS();
}