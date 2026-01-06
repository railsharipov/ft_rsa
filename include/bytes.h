#ifndef SSL_BYTES_H
# define SSL_BYTES_H

# include <common.h>
# include <io.h>

int bytes_stream(t_io_v2_stream **stream, const t_ostring *ostring, t_io_v2_flag flags);
int bytes_reader(t_io_v2_stream **stream, const t_ostring *ostring);
int bytes_writer(t_io_v2_stream **stream, const t_ostring *ostring);

#endif