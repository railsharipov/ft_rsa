#include <libft/string.h>
#include <libft/alloc.h>
#include <io.h>

static ssize_t	__swrite_delim(t_iodes *iodes, const char *buf, size_t nbytes)
{
	IO_LOG(TRACE, "entering with nbytes=%zu, delim=%d, lwidth=%d", nbytes, iodes->delim, iodes->lwidth);
	
	t_ostring	*osbuf;
	char		*obufptr;
	ssize_t		wbytes;
	ssize_t		offset;
	ssize_t		width;
	ssize_t		ix;

	osbuf = iodes->osbuf;

	if (NULL == osbuf) {
		IO_LOG(TRACE, "NULL osbuf");
		return (-1);
	}

	if (iodes->lwidth <= 0) {
		IO_LOG(TRACE, "setting default lwidth to 64");
		iodes->lwidth = 64;
	}

	width = (size_t)iodes->lwidth;
	offset = MIN(nbytes, (iodes->seek % width));
	wbytes = 0;
	
	IO_LOG(TRACE, "width=%zd, offset=%zd, current osbuf size=%zu", width, offset, osbuf->size);

	LIBFT_REALLOC(osbuf->content, osbuf->size, osbuf->size + 2*nbytes);
	obufptr = osbuf->content + osbuf->size;
	IO_LOG(TRACE, "reallocated osbuf, new size=%zu", osbuf->size + 2*nbytes);

	/* Finish last line if it's not [width] bytes long */
	if (offset) {
		IO_LOG(TRACE, "finishing last line with offset");
		for (; wbytes < (width - offset); nbytes--)
			obufptr[wbytes++] = *buf++;
		if (nbytes > 0) {
			obufptr[wbytes++] = iodes->delim;
		}
	}

	IO_LOG(TRACE, "writing %zu bytes", nbytes);
	while (nbytes/width > 0) {
		for (ix = 0; ix < width; ix++, nbytes--)
			obufptr[wbytes++] = *buf++;
		obufptr[wbytes++] = iodes->delim;
	}

	while (nbytes-- > 0)
		obufptr[wbytes++] = *buf++;

	osbuf->size += wbytes;
	iodes->seek += wbytes;
	
	IO_LOG(TRACE, "wrote %zd bytes, new osbuf size=%zu, seek=%zd", wbytes, osbuf->size, iodes->seek);
	return (wbytes);
}

static ssize_t __swrite(t_iodes *iodes, const char *buf, size_t nbytes)
{
	IO_LOG(TRACE, "entering with nbytes=%zu", nbytes);
	
	t_ostring	*osbuf;
	char		*obufptr;
	ssize_t		wbytes;

	osbuf = iodes->osbuf;

	if (NULL == osbuf) {
		IO_LOG(TRACE, "NULL osbuf");
		return (-1);
	}

	IO_LOG(TRACE, "current osbuf size=%zu", osbuf->size);
	LIBFT_REALLOC(osbuf->content, nbytes, nbytes + osbuf->size);
	obufptr = osbuf->content + osbuf->size;
	IO_LOG(TRACE, "reallocated osbuf, new size=%zu", nbytes + osbuf->size);

	wbytes = 0;
	while (wbytes < nbytes)
		obufptr[wbytes++] = *buf++;

	osbuf->size += wbytes;
	iodes->seek += wbytes;
	
	IO_LOG(TRACE, "wrote %zd bytes, new osbuf size=%zu, seek=%zd", wbytes, osbuf->size, iodes->seek);
	return (wbytes);
}

ssize_t	io_swrite(t_iodes *iodes, const char *buf, size_t nbytes)
{
	IO_LOG(TRACE, "entering function with iodes=%p, buf=%p, nbytes=%zu", iodes, buf, nbytes);
	
	ssize_t	wbytes;

	if (NULL == buf || NULL == iodes) {
		IO_LOG(TRACE, "NULL parameter detected - buf=%p, iodes=%p", buf, iodes);
		return (-1);
	}

	if (nbytes == 0) {
		IO_LOG(TRACE, "nbytes is 0, returning 0");
		return (0);
	}

	IO_LOG(TRACE, "iodes delim=%d", iodes->delim);
	
	if (iodes->delim) {
		IO_LOG(TRACE, "using delimiter-based write");
		wbytes = __swrite_delim(iodes, (char *)buf, nbytes);
	}
	else {
		IO_LOG(TRACE, "using standard write");
		wbytes = __swrite(iodes, (char *)buf, nbytes);
	}

	IO_LOG(TRACE, "returning %zd", wbytes);
	return (wbytes);
}
