#include <unistd.h>
#include <file.h>
#include <libft/std.h>
#include <libft/alloc.h>

void	file_del(t_file *file)
{
	FILE_LOG(TRACE, "file del with file=%p", file);

	if (NULL == file) {
		FILE_LOG(ERROR, INVALID_INPUT_ERROR);
		return ;
	}
	if (file->path != NULL) {
		SSL_FREE(file->path);
	}
	ft_bzero(file, sizeof(t_file));
	SSL_FREE(file);

	return ;
}