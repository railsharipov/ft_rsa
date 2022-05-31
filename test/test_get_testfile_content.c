#include <ft_ssl.h>
#include <ssl_test.h>
#include <ssl_io.h>

void	test_get_testfile_content(const char *testfile_path, t_ostring *ostring)
{
	int 	fd;
	ssize_t	nbytes;
	char	buf[IO_BUFSIZE];

	TEST_ASSERT(NULL != testfile_path);
	TEST_ASSERT(NULL != ostring);

	fd = open(testfile_path, O_RDONLY);

	TEST_ASSERT(fd > 2);

	ostring->content = NULL;
	ostring->size = 0;

	while ((nbytes = read(fd, buf, IO_BUFSIZE)) > 0)
	{
		SSL_REALLOC(ostring->content, ostring->size, ostring->size + nbytes);
		ft_memcpy(ostring->content + ostring->size, buf, nbytes);
		ostring->size += nbytes;
	}

	TEST_ASSERT(nbytes >= 0);
}
