#include <ft_ssl.h>
#include <ssl_der.h>
#include <ssl_io.h>

static ssize_t	__read_octets_indef(t_iodes *in, t_iodes *out);

ssize_t	der_read_octets_indef(char **content, size_t *size, t_iodes *iodes)
{
	t_iodes		temp_iodes;
	t_ostring	osbuf;
	ssize_t		rbytes;

	if (NULL == iodes || NULL == content || NULL == size)
		return (-1);

	if (SSL_OK != io_init(&temp_iodes, IO_WRITE|IO_OSBUF, &osbuf))
		return (-1);

	rbytes = __read_octets_indef(iodes, &temp_iodes);

	if (rbytes < 0)
		util_ostr_clean(&osbuf);

	*content = util_ostr_get_content(&osbuf);
	*size = util_ostr_get_size(&osbuf);
	
	io_close(&temp_iodes);

	return (rbytes);
}

static ssize_t	__read_octets_indef(t_iodes *in, t_iodes *out)
{
	char		octets[2];
	ssize_t		rbytes;
	ssize_t		tbytes;
	int			read_null;

	read_null = SSL_FALSE;
	tbytes = 0;

	while (io_read(in, octets, 1) == 1)
	{
		rbytes = 1;

		if (octets[0] == 0)
		{
			if (io_read(in, octets+1, 1) < 1)
				break ;

			rbytes += 1;

			if (octets[1] == 0)
				read_null = SSL_TRUE;
		}

		tbytes += rbytes;

		if (read_null)
			break ;

		if (io_write(out, octets, rbytes) != rbytes)
			break ;
	}

	return (read_null ? tbytes : -1);
}
