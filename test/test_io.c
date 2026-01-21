#include <common.h>
#include <io.h>
#include <test.h>
#include <unistd.h>
#include <fcntl.h>

static int	__test_io_setup(void);
static int	__test_io_init(void);
// static int	__test_io_read(void);
// static int	__test_io_write(void);

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
		// | __test_io_read()
		// | __test_io_write()
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

	io_fopen(&iodes, IO_READ|IO_FILE, "./Makefile");

	TEST_ASSERT(iodes.fd > 2);
	TEST_ASSERT(iodes.mode == IO_MODE_FILDES);

	io_fopen(&iodes, IO_READ|IO_STDIN, NULL);

	TEST_ASSERT(iodes.fd == STDIN_FILENO);
	TEST_ASSERT(iodes.mode == IO_MODE_FILDES);

	io_fopen(&iodes, IO_WRITE|IO_STDOUT, NULL);

	TEST_ASSERT(iodes.fd == STDOUT_FILENO);
	TEST_ASSERT(iodes.mode == IO_MODE_FILDES);

	osbuf.content = (unsigned char *)"some_content";
	osbuf.size = ft_strlen((char *)osbuf.content);

	io_osbuf(&iodes, IO_READ|IO_OSBUF, &osbuf);

	TEST_ASSERT(iodes.osbuf == &osbuf);
	TEST_ASSERT(iodes.mode == IO_MODE_OSBUF);

	io_osbuf(&iodes, IO_WRITE|IO_OSBUF, &osbuf);

	TEST_ASSERT(iodes.osbuf == &osbuf);
	TEST_ASSERT(iodes.mode == IO_MODE_OSBUF);

	TEST_PASS();
}

