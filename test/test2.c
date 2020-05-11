#include <ft_ssl.h>

int main(int ac, char **av)
{
  t_io io;
  t_io io2;

  if (ac != 2)
    return 0;

  int fd = open(av[1], O_RDONLY);

  if (fd < 0)
    perror(NULL);
  else
  {
    io_init_multi(&io, &io2, NULL);

    io.stream.fd = fd;
    io.flag |= IO_FILE;
    io_setbuf(&io, RD_BUF);

    _io_read(&io, 0);

    io2.flag |= IO_STRING;
    io2.input = io.output;
    io2.insize = io.outsize;
    io_setbuf(&io2, RD_BUF);
    io2.outlimit = 8;

    printf("io2 bytes = %zd\n", _io_read(&io2, 0));

    write(1, io2.output, io2.outsize);
    write(1, "\n", 1);
  }

  return 0;
}
