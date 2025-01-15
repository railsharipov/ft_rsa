#include <sys/fcntl.h>
#include <unistd.h>
#include <common.h>
#include <test.h>
#include <io.h>

int	test_get_file_content(const char *testfile_path, t_ostring *ostring)
{
	int 	fd;
	ssize_t	nbytes;
	char	buf[IO_BUFSIZE];

	if (NULL == testfile_path || NULL == ostring) {
		TEST_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	if ((fd = open(testfile_path, O_RDONLY)) < 0) {
		TEST_LOG(ERROR, UNSPECIFIED_ERROR);
		return (SSL_ERR);
	}

	ostring->content = NULL;
	ostring->size = 0;

	while ((nbytes = read(fd, buf, IO_BUFSIZE)) > 0) {
		SSL_REALLOC(ostring->content, ostring->size, ostring->size + nbytes);
		ft_memcpy(ostring->content + ostring->size, buf, nbytes);
		ostring->size += nbytes;
	}

	if (nbytes < 0) {
		TEST_LOG(ERROR, UNSPECIFIED_ERROR);
		return (SSL_ERR);
	}

	return (SSL_OK);
}
