#include <file.h>
#include <libft/std.h>
#include <libft/alloc.h>

int	file_new(t_file **file, const char *path)
{
	FILE_LOG(TRACE, "new file with path=%s", path ? path : "");

	if (NULL == file) {
		FILE_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (NULL == path) {
		FILE_LOG(ERROR, "file path is not specified");
		return (SSL_ERR);
	}

	SSL_ALLOC(*file, sizeof(t_file));
	(*file)->path = ft_strdup(path);

	return (SSL_OK);
}