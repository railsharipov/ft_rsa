#include <common.h>
#include <logger.h>
#include <args.h>
#include <cmd.h>
#include <json.h>
#include <asn1.h>
#include <der.h>
#include <pem.h>
#include <base64.h>
#include <textutil.h>
#include <io.h>
#include <file.h>
#include <libft_v2.h>

int	cmd_dev(const t_cmd *cmd)
{
	t_htbl_v2 *htbl = ft_htbl_v2_create(1024*1024);

	t_ostring ostring = {0};
	ft_ostr_init_with_capacity(&ostring, 1024*1024);
	if (SSL_OK != file_read_all("tests/files/text/medium.txt", &ostring)) {
		SSL_LOG(ERROR, "file read error");
		return (SSL_ERR);
	}

	char *text = ft_ostr_to_cstr(&ostring, 0, ostring.size);
	char **words = ft_strsplit(text, '\n');

	for (size_t idx = 0; words[idx] != NULL && idx < 10; idx++) {
		char **subwords = ft_strsplit(words[idx], '"');
		for (size_t sidx = 0; subwords[sidx] != NULL && sidx < 10; sidx++) {
			ft_htbl_v2_set(htbl, subwords[sidx], subwords[sidx]);
		}
	}

	char *dumps = ft_htbl_v2_dumps(htbl);
	io_v2_write_all(io_v2_stdout_writer, dumps, ft_strlen(dumps));

	return (SSL_OK);
}
