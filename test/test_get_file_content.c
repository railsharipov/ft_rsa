#include <sys/fcntl.h>
#include <unistd.h>
#include <ssl.h>
#include <test.h>
#include <io.h>

int	test_get_file_content(const char *testfile_path, t_ostring *ostring)
{
	int 	fd;
	ssize_t	nbytes;
	char	buf[IO_BUFSIZE];

	if (NULL == testfile_path || NULL == ostring)
		return (TEST_LOG(ERROR, INVALID_INPUT_ERROR));

	if ((fd = open(testfile_path, O_RDONLY)) < 0)
		return (TEST_LOG(ERROR, UNSPECIFIED_ERROR));

	ostring->content = NULL;
	ostring->size = 0;

	while ((nbytes = read(fd, buf, IO_BUFSIZE)) > 0)
	{
		SSL_REALLOC(ostring->content, ostring->size, ostring->size + nbytes);
		ft_memcpy(ostring->content + ostring->size, buf, nbytes);
		ostring->size += nbytes;
	}

	if (nbytes < 0)
		return (TEST_LOG(ERROR, UNSPECIFIED_ERROR));

	return (SSL_OK);
}
