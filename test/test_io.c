#include <common.h>
#include <io.h>
#include <test.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef SSL_INCLUDE_TESTS

static int	__test_io_setup(void);
static int	__test_io_init(void);
static int	__test_io_read(void);
static int	__test_io_write(void);

static const char	*__lorem = "Cernantur iis sunt, voluptate export nulla \
	arbitror noster. Se nescius exercitation. Malis nescius o consectetur \
	est excepteur multos dolor excepteur quae id ut aute incididunt \
	domesticarum eu malis a voluptate ubi ita esse pariatur offendit de \
	quamquam quid qui commodo cohaerescant si do elit incurreret singulis, \
	est ita consectetur. Ut veniam constias deserunt, nisi cupidatat est esse \
	cillum. O id arbitrantur si eiusmod velit nostrud probant sed probant nisi \
	constias si eram pariatur ubi coniunctione, magna voluptate se probant, \
	iis eu tamen irure sunt id laborum tempor arbitror, excepteur ad noster. \
	Laboris enim constias doctrina. Elit nostrud in multos sint.";

static size_t	__lorem_size;

int	test_io(void)
{
	if (SSL_OK != __test_io_setup()) {
		TEST_LOG(ERROR, UNSPECIFIED_ERROR);
		TEST_FAIL();
	}

	return (
		__test_io_init()
		| __test_io_read()
		| __test_io_write()
	);
}

static int	__test_io_setup(void)
{
	__lorem_size = ft_strlen(__lorem);

	return (SSL_OK);
}

static int	__test_io_init(void)
{
	t_ostring	osbuf;
	t_iodes		iodes;

	io_init(&iodes, IO_READ|IO_FILE, "./Makefile");

	TEST_ASSERT(iodes.fd > 2);
	TEST_ASSERT(iodes.mode == IO_MODE_FILDES);

	io_init(&iodes, IO_READ|IO_STDIN);

	TEST_ASSERT(iodes.fd == STDIN_FILENO);
	TEST_ASSERT(iodes.mode == IO_MODE_FILDES);

	io_init(&iodes, IO_WRITE|IO_STDOUT);

	TEST_ASSERT(iodes.fd == STDOUT_FILENO);
	TEST_ASSERT(iodes.mode == IO_MODE_FILDES);

	osbuf.content = "some_content";
	osbuf.size = ft_strlen(osbuf.content);

	io_init(&iodes, IO_READ|IO_OSBUF, &osbuf);

	TEST_ASSERT(iodes.osbuf == &osbuf);
	TEST_ASSERT(iodes.mode == IO_MODE_OSBUF);

	io_init(&iodes, IO_WRITE|IO_OSBUF, &osbuf);

	TEST_ASSERT(iodes.osbuf == &osbuf);
	TEST_ASSERT(iodes.mode == IO_MODE_OSBUF);

	TEST_PASS();
}

static int	__test_io_read(void)
{
	t_ostring	osbuf;
	t_iodes		iodes;
	ssize_t		rbytes;
	char		buf[__lorem_size/10];
	char		test_buf[__lorem_size/10];
	ssize_t		test_rbytes;
	int			test_fd;

	osbuf.content = (char *)__lorem;
	osbuf.size = __lorem_size;

	io_init(&iodes, IO_READ|IO_OSBUF, &osbuf);
	rbytes = io_read(&iodes, buf, sizeof(buf));

	TEST_ASSERT(rbytes == __lorem_size/10);
	TEST_ASSERT(ft_strncmp(__lorem, buf, sizeof(buf)) == 0);

	test_fd = open("./Makefile", 0644);
	test_rbytes = read(test_fd, test_buf, sizeof(test_buf));

	io_init(&iodes, IO_READ|IO_FILE, "./Makefile");
	rbytes = io_read(&iodes, buf, sizeof(buf));

	TEST_ASSERT(test_rbytes > 0);
	TEST_ASSERT(rbytes > 0);
	TEST_ASSERT(rbytes == test_rbytes);
	TEST_ASSERT(ft_strncmp(test_buf, buf, sizeof(buf)) == 0);

	TEST_PASS();
}

static int	__test_io_write(void)
{
	t_ostring	osbuf;
	t_iodes		iodes;
	ssize_t		wbytes;

	osbuf.content = NULL;
	osbuf.size = 0;

	io_init(&iodes, IO_WRITE|IO_OSBUF, &osbuf);
	wbytes = io_write(&iodes, __lorem, __lorem_size);

	TEST_ASSERT(wbytes == __lorem_size);
	TEST_ASSERT(osbuf.size == __lorem_size);
	TEST_ASSERT(osbuf.content != NULL);
	TEST_ASSERT(ft_strncmp(__lorem, osbuf.content, osbuf.size) == 0);

	SSL_FREE(osbuf.content);
	osbuf.size = 0;

	io_init(&iodes, IO_WRITE|IO_OSBUF, &osbuf);

	iodes.delim = '&';
	iodes.lwidth = 1;

	wbytes = io_write(&iodes, __lorem, __lorem_size);

	TEST_ASSERT(wbytes == 2*__lorem_size);
	TEST_ASSERT(osbuf.size == 2*__lorem_size);
	TEST_ASSERT(osbuf.content != NULL);

	int check;
	int ix;

	check = 1;
	for (ix = 0; ix < wbytes; ix++) {
		if (2*ix < osbuf.size && __lorem[ix] != osbuf.content[2*ix]) {
			check = 0;
		}
	}
	TEST_ASSERT(check);

	TEST_PASS();
}

#endif
