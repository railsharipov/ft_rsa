#include "libft.h"

void	ft_2darray_del(void **arr, int arr_len)
{
	if (NULL == arr) {
		return ;
	}

	while (arr_len > 0)
		LIBFT_FREE(arr[arr_len--]);

	LIBFT_FREE(arr);
}

void 	ft_2darray_del_null_terminated(void **arr)
{
	size_t	idx;

	if (NULL == arr) {
		return ;
	}

	idx = 0;
	while (NULL != arr[idx]) {
		LIBFT_FREE(arr[idx]);
		idx++;
	}

	LIBFT_FREE(arr);
}

int 	ft_2darray_len_null_terminated(void **arr)
{
	int	len;

	if (NULL == arr) {
		return (0);
	}

	len = 0;
	while (NULL != *arr++)
		len++;

	return (len);
}

char	*ft_2darray_strjoin(char **str_arr, size_t str_arr_len, const char *delim)
{
	char	*joined_str;
	int		joined_size;
	int		delim_size;
	int		ix;
	size_t	slen;
	char	*p;

	if (NULL == str_arr || str_arr_len == 0) {
		return (NULL);
	}

	joined_size = 0;
	delim_size = ft_strlen(delim);

	ix = 0;
	while (ix < str_arr_len)
		joined_size += ft_strlen(str_arr[ix++]);

	joined_size += delim_size * (str_arr_len-1);

	LIBFT_ALLOC(joined_str, joined_size+1);
	p = joined_str;

	ix = 0;
	while (ix < str_arr_len-1) {
		slen = ft_strlen(str_arr[ix]);
		ft_memcpy(p, str_arr[ix], slen);
		ft_memcpy(p + slen, delim, delim_size);

		p += slen + delim_size;
		ix++;
	}

	ft_strcpy(p, str_arr[str_arr_len-1]);

	return (joined_str);
}

/****************************************************************************/

static int	__is_blank(char c)
{
	if ((c >= 9 && c <= 13) || c == 32) {
		return (1);
	}
	return (0);
}

ssize_t	ft_atoi(const char *str)
{
	int		i;
	ssize_t	nb;
	ssize_t	neg;

	nb = 0;

	i = 0;
	while (__is_blank(str[i])) {
		i++;
	}
	neg = (str[i] == '-') ? -1 : 1;

	if (str[i] == '-' || str[i] == '+') {
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9' && str[i] != 0) {
		nb = nb * 10 + str[i++] - 48;
	}
	return (neg * nb);
}

size_t	ft_atoi_u(const char *str)
{
	int		i;
	size_t	nb;

	nb = 0;

	i = 0;
	while (__is_blank(str[i])) {
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9' && str[i] != 0) {
		nb = nb * 10 + str[i++] - 48;
	}

	return (nb);
}

/****************************************************************************/

uint32_t	ft_uint_bswap32(uint32_t x)
{
	return (
		((x & 0xFF000000) >> 24)
		| ((x & 0x00FF0000) >> 8)
		| ((x & 0x0000FF00) << 8)
		| ((x & 0x000000FF) << 24)
	);
}

uint64_t	ft_uint_bswap64(uint64_t x)
{
	return (
		((x & 0xFF00000000000000) >> 56)
		| ((x & 0x00FF000000000000) >> 40)
		| ((x & 0x0000FF0000000000) >> 24)
		| ((x & 0x000000FF00000000) >> 8)
		| ((x & 0x00000000FF000000) << 8)
		| ((x & 0x0000000000FF0000) << 24)
		| ((x & 0x000000000000FF00) << 40)
		| ((x & 0x00000000000000FF) << 56)
	);
}

uint128_t	ft_uint_bswap128(uint128_t x)
{
	uint64_t	lo;
	uint64_t	hi;

	lo = (uint64_t)x;
	hi = (uint64_t)(x >> 64);

	lo = ft_uint_bswap64(lo);
	hi = ft_uint_bswap64(hi);

	return (((uint128_t)lo) << 64) | hi;
}

/****************************************************************************/

t_buffer *ft_buffer_new(size_t capacity)
{
	t_buffer *buffer;

	if (capacity == 0) {
		return (NULL);
	}
	LIBFT_ALLOC(buffer, sizeof(t_buffer));
	LIBFT_ALLOC(buffer->arr, capacity);
	buffer->capacity = capacity;
	buffer->write_pos = 0;
	buffer->read_pos = 0;

	return (buffer);
}

void ft_buffer_reset(t_buffer *buffer)
{
	if (buffer == NULL) {
		return ;
	}
	buffer->read_pos = 0;
	buffer->write_pos = 0;
}

void ft_buffer_del(t_buffer *buffer)
{
	if (buffer == NULL) {
		return ;
	}
	LIBFT_FREE(buffer->arr);
	LIBFT_FREE(buffer);
}

const char *ft_buffer_view(t_buffer *buffer)
{
	if (NULL == buffer) {
		return (NULL);
	}
	if (!ft_buffer_is_valid(buffer)) {
		return (NULL);
	}
	return ((const char *)buffer->arr + buffer->read_pos);
}

size_t ft_buffer_used(t_buffer *buffer)
{
	if (NULL == buffer) {
		LIBFT_LOG(ERROR, "invalid buffer");
		return (0);
	}
	if (buffer->write_pos < buffer->read_pos) {
		return (0);
	}
	return (buffer->write_pos - buffer->read_pos);
}

size_t ft_buffer_available(t_buffer *buffer)
{
	if (NULL == buffer) {
		LIBFT_LOG(ERROR, "invalid buffer");
		return (0);
	}
	size_t used = ft_buffer_used(buffer);
	if (buffer->capacity < used) {
		LIBFT_LOG(ERROR, "buffer capacity is smaller than buffer used");
		return (0);
	}
	return (buffer->capacity - used);
}

int ft_buffer_is_empty(t_buffer *buffer)
{
	return (ft_buffer_used(buffer) == 0);
}

int ft_buffer_is_full(t_buffer *buffer)
{
	return (ft_buffer_available(buffer) == 0);
}

ssize_t ft_buffer_read(t_buffer *buffer, void *buf, size_t nbytes)
{
	size_t	used;
	ssize_t rbytes;

	if (NULL == buffer || NULL == buf) {
		LIBFT_LOG(ERROR, "invalid input");
		return (-1);
	}
	if (!ft_buffer_is_valid(buffer)) {
		LIBFT_LOG(ERROR, "invalid buffer");
		return (-1);
	}
	if (nbytes == 0) {
		return (0);
	}
	used = ft_buffer_used(buffer);
	if (used == 0) {
		return (0);
	}
	rbytes = MIN(nbytes, used);
	ft_memcpy(buf, (char *)buffer->arr + buffer->read_pos, rbytes);
	buffer->read_pos += rbytes;

	if (buffer->read_pos >= buffer->write_pos) {
		ft_buffer_reset(buffer);
	}
	return (rbytes);
}

ssize_t ft_buffer_write(t_buffer *buffer, const void *buf, size_t nbytes)
{
	ssize_t wbytes;
	size_t	available;

	if (NULL == buffer || NULL == buf) {
		LIBFT_LOG(ERROR, "invalid input");
		return (-1);
	}
	if (!ft_buffer_is_valid(buffer)) {
		LIBFT_LOG(ERROR, "invalid buffer");
		return (-1);
	}
	if (nbytes == 0) {
		return (0);
	}
	available = ft_buffer_available(buffer);
	if (available == 0) {
		return (0);
	}
	wbytes = (nbytes > available) ? available : nbytes;

	if (wbytes > buffer->capacity - buffer->write_pos) {
		ft_buffer_left_align(buffer);
	}
	ft_memcpy((char *)buffer->arr + buffer->write_pos, buf, wbytes);
	buffer->write_pos += wbytes;

	return (wbytes);
}

ssize_t ft_buffer_read_with_func(t_buffer *buffer, t_func_buffer_read read, void *vctx, size_t nbytes)
{
	ssize_t rbytes;
	size_t	used;

	if (NULL == buffer || NULL == read) {
		LIBFT_LOG(ERROR, "invalid input");
		return (-1);
	}
	if (!ft_buffer_is_valid(buffer)) {
		LIBFT_LOG(ERROR, "invalid buffer");
		return (-1);
	}
	if (nbytes == 0) {
		return (0);
	}
	used = ft_buffer_used(buffer);
	if (used == 0) {
		return (0);
	}
	rbytes = read(vctx, (char *)buffer->arr + buffer->read_pos, MIN(nbytes, used));
	if (rbytes < 0) {
		return (-1);
	}
	buffer->read_pos += rbytes;

	if (buffer->read_pos >= buffer->write_pos) {
		ft_buffer_reset(buffer);
	}
	return (rbytes);
}

ssize_t ft_buffer_write_with_func(t_buffer *buffer, t_func_buffer_write write, void *vctx, size_t nbytes)
{
	ssize_t wbytes;
	size_t available;

	if (NULL == buffer || NULL == write) {
		LIBFT_LOG(ERROR, "invalid input");
		return (-1);
	}
	if (!ft_buffer_is_valid(buffer)) {
		LIBFT_LOG(ERROR, "invalid input");
		return (-1);
	}
	if (nbytes == 0) {
		return (0);
	}
	available = ft_buffer_available(buffer);
	if (available == 0) {
		return (0);
	}
	wbytes = MIN(nbytes, available);

	if (wbytes > buffer->capacity - buffer->write_pos) {
		ft_buffer_left_align(buffer);
	}
	wbytes = write(vctx, (char *)buffer->arr + buffer->write_pos, wbytes);
	if (wbytes < 0) {
		return (-1);
	}
	buffer->write_pos += wbytes;

	return (wbytes);
}

ssize_t ft_buffer_transfer(t_buffer *src, t_buffer *dst)
{
	if (NULL == src || NULL == dst) {
		return (-1);
	}
	if (!ft_buffer_is_valid(src) || !ft_buffer_is_valid(dst)) {
		return (-1);
	}
	size_t src_used = ft_buffer_used(src);
	size_t dst_available = ft_buffer_available(dst);
	size_t can_transfer = MIN(src_used, dst_available);

	size_t dst_right_pad = dst->capacity - dst->write_pos;
	if (can_transfer > dst_right_pad) {
		ft_buffer_left_align(dst);
	}
	ft_memcpy(dst->arr + dst->write_pos, src->arr + src->read_pos, can_transfer);

	return (can_transfer);
}

t_transform_result ft_buffer_transform(t_buffer *src, t_buffer *dst, t_func_transform transform, void *transform_ctx)
{
	if (NULL == src || NULL == dst || NULL == transform) {
		LIBFT_LOG(ERROR, "invalid input");
		return (t_transform_result){.status = TRANSFORM_ERROR};
	}
	if (!ft_buffer_is_valid(src) || !ft_buffer_is_valid(dst)) {
		LIBFT_LOG(ERROR, "invalid input");
		return (t_transform_result){.status = TRANSFORM_ERROR};
	}
	size_t src_used = ft_buffer_used(src);
	size_t dst_right_pad = dst->capacity - dst->write_pos;

	t_transform_result result = transform(
		transform_ctx,
		src->arr + src->read_pos, src_used,
		dst->arr + dst->write_pos, dst_right_pad
	);

	switch (result.status) {
	case TRANSFORM_ERROR:
		return result;
	case TRANSFORM_OK:
	case TRANSFORM_NEED_INPUT:
	case TRANSFORM_NEED_OUTPUT:
	case TRANSFORM_DONE:
		src->read_pos += result.consumed;
		if (src->read_pos >= src->write_pos) {
			ft_buffer_reset(src);
		}
		dst->write_pos += result.produced;
		return result;
	default:
		return (t_transform_result){.status = TRANSFORM_ERROR};
	}
}

int	ft_buffer_is_valid(t_buffer *buffer)
{
	if (NULL == buffer) {
		LIBFT_LOG(DEBUG, "invalid buffer");
		return (0);
	}
	if (NULL == buffer->arr) {
		LIBFT_LOG(DEBUG, "invalid buffer");
		return (0);
	}
	if (buffer->capacity == 0) {
		LIBFT_LOG(DEBUG, "invalid buffer state");
		return (0);
	}
	if (buffer->read_pos > buffer->capacity) {
		LIBFT_LOG(DEBUG, "invalid buffer state");
		return (0);
	}
	if (buffer->write_pos > buffer->capacity) {
		LIBFT_LOG(DEBUG, "invalid buffer state");
		return (0);
	}
	if (buffer->read_pos > buffer->write_pos) {
		LIBFT_LOG(DEBUG, "invalid buffer state");
		return (0);
	}
	return (1);
}

size_t ft_buffer_left_align(t_buffer *buffer)
{
	if (NULL == buffer) {
		LIBFT_LOG(ERROR, "invalid buffer");
		return (0);
	}
	size_t shift = buffer->read_pos;

	if (buffer->read_pos > 0) {
		ft_memmove(buffer->arr, buffer->arr + shift, ft_buffer_used(buffer));
		buffer->write_pos -= shift;
		buffer->read_pos = 0;
	}
	return (shift);
}

/****************************************************************************/

static const int	COL = 16;

static const char	A[16] = {
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

static const int	B[128] = {
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
	48,	48,	48,	48,	48,	48,	48,	48,	48,	48,	0,	0,	0,	0,	0,	0,
	0,	55,	55,	55,	55,	55,	55,	55,	55,	55,	55,	55,	55,	55,	55,	55,
	55,	55,	55,	55,	55,	55,	55,	55,	55,	55,	55,	0,	0,	0,	0,	0,
	0,	87,	87,	87,	87,	87,	87,	87,	87,	87,	87,	87,	87,	87,	87,	87,
	87,	87,	87,	87,	87,	87,	87,	87,	87,	87,	87,	0,	0,	0,	0,	0,
};

void	ft_bytes_xor(void *res, void *bytes1, void *bytes2, size_t size)
{
	uint8_t	*rptr;
	uint8_t	*b1ptr;
	uint8_t	*b2ptr;
	size_t	i;

	rptr = (uint8_t *)res;
	b1ptr = (uint8_t *)bytes1;
	b2ptr = (uint8_t *)bytes2;

	i = 0;
	while (i < size) {
		rptr[i] = b1ptr[i] ^ b2ptr[i];
		i++;
	}
}

uint64_t	ft_bytes_to_uint(void *bytes, size_t size)
{
	uint8_t		*ptr;
	uint64_t	num;
	size_t		idx;

	num = 0;
	size = MIN(size, sizeof(uint64_t));
	ptr = (uint8_t *)bytes;

	idx = 0;
	while (idx < size) {
		num <<= 8;
		num |= (uint64_t)ptr[idx++];
	}
	return (num);
}

void	ft_bytes_lshift(void *bytes, size_t size, int shift)
{
	unsigned char	*ptr;
	unsigned int	tmp;
	unsigned int	rem;
	size_t			idx;

	if ((NULL == bytes) || (shift <= 0)) {
		return ;
	}
	ptr = (unsigned char *)bytes + size-1;
	rem = 0;
	idx = 0;

	while (idx++ < size) {
		tmp = *ptr;
		*ptr = (*ptr << shift) | rem;
		rem = tmp >> (CHAR_BIT-shift);
		ptr--;
	}
}

void	ft_bytes_rshift(void *bytes, size_t size, int shift)
{
	unsigned char	*ptr;
	unsigned int	tmp;
	unsigned int	rem;
	size_t			idx;

	if ((NULL == bytes) || (shift <= 0)) {
		return ;
	}
	ptr = (unsigned char *)bytes;
	rem = 0;
	idx = 0;

	while (idx < size) {
		tmp = ptr[idx];
		ptr[idx] = (ptr[idx] >> shift) | rem;
		rem = tmp << (CHAR_BIT-shift);
		idx++;
	}
}

void	ft_bytes_write_hex(int fd, void *ptr, size_t size)
{
	unsigned char		*octets;
	uint32_t	bytes;
	int			ix;
	int			iy;

	if ((size <= 0) || (NULL == ptr) || (fd < 0)) {
		return ;
	}

	octets = (unsigned char *)(ptr);
	bytes = 0;

	ix = 0;
	while (ix < size) {
		ft_fprintf(fd, "%.4x -", bytes);

		iy = 0;
		while ((iy < COL) && (ix < size)) {
			ft_fprintf(fd, " %.2x", octets[ix++]);
			iy++;
		}
		ix -= iy;
		while (iy++ <= COL) {
			ft_fprintf(fd, "   ");
		}
		iy = 0;
		while ((iy < COL) && (ix < size)) {
			if (ft_isprint(octets[ix])) {
				ft_fprintf(fd, "%c", octets[ix]);
			} else {
				ft_fprintf(fd, ".");
			}
			iy++;
			ix++;
		}
		ft_fprintf(fd, "\n");
		bytes += iy;
	}
}

char	*ft_bytes_dumps_hex(void *bytes, size_t size, int cols, int del)
{
	unsigned char	*octets;
	char	hex[size*4];
	size_t	hexlen, ix;

	if (NULL == bytes || size == 0) {
		return (NULL);
	}

	octets = (unsigned char *)(bytes);
	hexlen = 0;
	ix = 0;
	while (ix < size && hexlen < sizeof(hex)) {
		if (del != 0) {
			hexlen += ft_snprintf(hex+hexlen, sizeof(hex)-hexlen, "%.2x%c", octets[ix++], del);
		}
		else {
			hexlen += ft_snprintf(hex+hexlen, sizeof(hex)-hexlen, "%.2x", octets[ix++]);
		}

		if (cols != 0) {
			if ((ix < size) && (ix % cols == 0)) {
				hexlen += ft_snprintf(hex+hexlen, sizeof(hex)-hexlen, "\n");
			}
		}
	}
	hex[sizeof(hex)-1] = '\0';

	return (ft_strdup(hex));
}

void	ft_bytes_dumpb_hex(void *bytes, size_t size, int cols, int del, char *buf, size_t bufsize)
{
	char	*hex;

	if (NULL == buf || bufsize == 0) {
		return ;
	}
	if (NULL == bytes || size == 0) {
		buf[0] = '\0';
		return ;
	}

	hex = ft_bytes_dumps_hex(bytes, size, cols, del);
	ft_strncpy(buf, hex, bufsize-1);
	buf[bufsize-1] = '\0';
	LIBFT_FREE(hex);
}

void	ft_bytes_print_bits(void *ptr, size_t size)
{
	size_t			i;
	size_t			j;

	i = 0;
	while (i < size) {
		j = 0;
		while (j < 8) {
			if ((((unsigned char *)ptr)[i] << j) & (1 << 7)) {
				write(1, "1", 1);
			}
			else
				write(1, "0", 1);
			j++;
		}
		write(1, " ", 1);
		if ((i + 1) % 8 == 0) {
			write(1, "\n", 1);
		}
		i++;
	}
	if (i % 8 != 0) {
		write(1, "\n", 1);
	}
}

void	ft_bytes_reverse_bits(void *ptr, size_t size)
{
	unsigned char	*p;
	unsigned char	num;
	size_t			i;
	size_t			j;

	p = ptr;
	i = 0;
	while (i < size) {
		num = 0;
		j = 0;
		while (j < 8) {
			if (p[i] & (1 << j)) {
				num |= (1 << (7 - j));
			}
			j++;
		}
		p[i] = num;
		i++;
	}
}

char	*ft_bytes_to_hex_upper(const void *bin, size_t binsize)
{
	char	*hex;
	size_t	ix;

	hex = ft_bytes_to_hex(bin, binsize);

	if (hex != NULL) {
		ix = 0;
		while (hex[ix] != '\0') {
			hex[ix] = ft_toupper(hex[ix]);
			ix++;
		}
	}
	return (hex);
}

char	*ft_bytes_to_hex(const void *bin, size_t binsize)
{
	unsigned char	*bptr;
	char			*hptr;
	char			*hex;
	size_t			hexsize;
	size_t			ix;

	if (NULL == bin || binsize == 0) {
		return (NULL);
	}
	hexsize = 2*binsize;
	LIBFT_ALLOC(hex, hexsize+1);
	hptr = hex;
	bptr = (unsigned char *)bin;

	for (ix = 0; ix < binsize; ix++) {
		*hptr++ = A[bptr[ix]>>4];
		*hptr++ = A[bptr[ix]&0xF];
	}
	*hptr = 0;

	return (hex);
}

void	ft_hex_to_bytes(void *bin, const char *hex, size_t hexsize)
{
	unsigned char	*buf;
	unsigned char	*ptr;
	size_t			bufsize;
	ssize_t			ix;

	if ((NULL == hex) || (NULL == bin)) {
		return ;
	}

	bufsize = 2 * NBITS_TO_NBYTES(4 * hexsize);
	LIBFT_ALLOC(buf, bufsize);
	ft_bzero(buf, bufsize);

	ptr = (unsigned char *)buf+bufsize-1;

	for (ix = hexsize-1; ix >= 0; ix--)
		*ptr-- = hex[ix] - B[hex[ix] & 0x7F];

	ptr = (unsigned char *)buf;

	for (ix = 0; ix < bufsize/2; ix++) {
		((unsigned char *)bin)[ix] = *ptr++ << 4;
		((unsigned char *)bin)[ix] |= *ptr++;
	}
	LIBFT_FREE(buf);
}

/****************************************************************************/

char	*ft_strncat(char *s1, const char *s2, size_t n)
{
	size_t	i;
	size_t	j;

	if (NULL == s1 || NULL == s2) {
		return (s1);
	}

	i = 0;
	while (s1[i] != 0)
		i++;

	j = 0;
	while (s2[j] != 0 && j < n)
		s1[i++] = s2[j++];

	s1[i] = 0;
	return (s1);
}

char	*ft_strjoin_multi(int n, ...)
{
	char	*res;
	char	*s;
	size_t	slen;
	va_list arg;
	int		idx;

	if (n <= 0) {
		return (NULL);
	}
	va_start(arg, n);
	slen = 0;

	idx = 0;
	while (idx < n) {
		s = va_arg(arg, char *);
		slen += ft_strlen(s);
		idx++;
	}
	va_end(arg);

	LIBFT_ALLOC(res, slen+1);

	va_start(arg, n);
	slen = 0;

	idx = 0;
	while (idx < n) {
		s = va_arg(arg, char *);
		res = ft_strcat(res, s);
		idx++;
	}
	va_end(arg);

	return (res);
}

size_t		ft_strlen(const char *s)
{
	size_t len;

	len = 0;

	if (NULL != s) {
		while (s[len])
			++len;
	}

	return (len);
}

int	ft_str_isnum(const char *s)
{
	if (s == NULL) {
		return (0);
	}

	while (*s != 0)
		if (!ft_isdigit(*s++)) {
			return (0);
		}

	return (1);
}

int	ft_str_isprint(const char *s)
{
	if (s == NULL) {
		return (0);
	}

	while (*s != 0)
		if (!ft_isprint(*s++)) {
			return (0);
		}

	return (1);
}

char *ft_str_strip(const char *s)
{
	size_t	start;
	size_t	slen;
	size_t	idx;

	if (NULL == s) {
		return (NULL);
	}
	slen = ft_strlen(s);
	idx = 0;
	while (idx < slen) {
		if (!ft_iswspace(s[idx])) {
			break;
		}
		idx++;
	}
	start = idx;
	while (idx < slen) {
		if (ft_iswspace(s[idx])) {
			break;
		}
		idx++;
	}
	return (ft_strsub(s, start, idx - start));
}

char	**ft_strsplit(char const *s, char c)
{
	int		idx;
	int		word_idx;
	int		start_idx;
	int		nwords;
	int 	num_delims;
	char	**arr;

	if (NULL == s) {
		return (NULL);
	}
	num_delims = 0;
	idx = 0;

	while (s[idx] != 0) {
		if (s[idx] == c) {
			num_delims++;
		}
		idx++;
	}

	if (idx == 0) {
		nwords = 0;
	} else if (num_delims == 0) {
		nwords = 1;
	} else {
		nwords = num_delims + 1;
	}

	LIBFT_ALLOC(arr, sizeof(char *) * (nwords + 1));

	idx = 0;
	start_idx = 0;
	word_idx = 0;

	while (word_idx < nwords) {
		while (s[idx] == c && s[idx] != 0) {
			idx++;
		}
		start_idx = idx;
		while (s[idx] != c && s[idx] != 0) {
			idx++;
		}
		arr[word_idx] = ft_strsub(s, start_idx, (size_t)(idx - start_idx));
		word_idx++;
	}
	arr[word_idx] = NULL;

	return (arr);
}

char	*ft_strchr(const char *s, int c)
{
	size_t	i;
	size_t	slen;

	if (NULL == s) {
		return (NULL);
	}

	i = 0;
	slen = ft_strlen(s);

	while (i < slen) {
		if (s[i] == (char)c) {
			return ((char *)(s + i));
		}

		i++;
	}

	return (NULL);
}

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*res;
	size_t	s1len;
	size_t	s2len;

	if (NULL == s1 && NULL == s2) {
		return (NULL);
	}

	if (NULL == s1 || NULL == s2) {
		if (NULL == s1) {
			return (ft_strdup(s2));
		}
		else
			return (ft_strdup(s1));
	}

	s1len = ft_strlen(s1);
	s2len = ft_strlen(s2);

	LIBFT_ALLOC(res, s1len+s2len+1);

	ft_memcpy(res, s1, s1len);
	ft_memcpy(res + s1len, s2, s2len);
	res[s1len + s2len] = 0;

	return (res);
}

char	*ft_strcat(char *s1, const char *s2)
{
	int	i;
	int j;

	if (NULL == s1 || NULL == s2) {
		return (s1);
	}

	i = 0;
	while (s1[i] != 0)
		i++;

	j = 0;
	while (s2[j] != 0)
		s1[i++] = s2[j++];

	s1[i] = 0;

	return (s1);
}

int	ft_str_isascii(const char *s)
{
	if (s == NULL) {
		return (0);
	}

	while (*s != 0)
		if (!ft_isascii(*s++)) {
			return (0);
		}

	return (1);
}

void	ft_strdel(char *s)
{
	if (NULL == s) {
		return ;
	}

	LIBFT_FREE(s);
}

char	*ft_strrchr(const char *s, int c)
{
	ssize_t	slen;

	if (NULL == s) {
		return (NULL);
	}

	slen = ft_strlen(s);

	while (--slen >= 0) {
		if (s[slen] == (char)c) {
			return ((char *)(s + slen));
		}
	}

	return (NULL);
}

int	ft_str_ishex(const char *s)
{
	if (s == NULL) {
		return (0);
	}

	while (*s != 0)
		if (!ft_ishex(*s++)) {
			return (0);
		}

	return (1);
}

char	*ft_strlow(const char *s)
{
	char	*s_low;
	size_t	idx;

	if (NULL == s) {
		return (NULL);
	}

	s_low = ft_strdup(s);

	idx = 0;
	while (s_low[idx] != '\0') {
		if (s_low[idx] >= 'A' && s_low[idx] <= 'Z') {
			s_low[idx] = s_low[idx] + 32;
		}

		idx++;
	}

	return (s_low);
}

int		ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t i;

	// no null check, same as libc strcmp
	i = 0;
	while (i < n && s1[i] && s2[i]) {
		if (s1[i] != s2[i]) {
			return (s1[i] - s2[i]);
		}
		i++;
	}
	return (i < n ? s1[i] - s2[i] : 0);
}

char	*ft_strcpy(char *dst, const char *src)
{
	size_t	i;

	if (NULL == dst || NULL == src) {
		return (dst);
	}

	i = 0;
	while (src[i] != 0) {
		dst[i] = src[i];
		++i;
	}
	dst[i] = 0;

	return (dst);
}

char	*ft_strndup(const char *s, size_t len)
{
	char	*sdup;
	size_t	idx;

	if (NULL == s) {
		return (NULL);
	}

	LIBFT_ALLOC(sdup, len + 1);
	if (NULL == sdup) {
		return (NULL);
	}

	idx = 0;
	while (idx < len && s[idx]) {
		sdup[idx] = s[idx];
		idx++;
	}
	while (idx < len) {
		sdup[idx] = 0;
		idx++;
	}

	return (sdup);
}

int	ft_str_isalpha(const char *s)
{
	if (s == NULL) {
		return (0);
	}

	while (*s != 0)
		if (!ft_isalpha(*s++)) {
			return (0);
		}

	return (1);
}

int	ft_str_isalnum(const char *s)
{
	if (s == NULL) {
		return (0);
	}

	while (*s != 0)
		if (!ft_isalnum(*s++)) {
			return (0);
		}

	return (1);
}

char *ft_strup(const char *s)
{
	char *s_up;
	size_t idx;

	if (NULL == s) {
		return (NULL);
	}

	s_up = ft_strdup(s);

	idx = 0;
	while (s_up[idx] != '\0') {
		if (s_up[idx] >= 'a' && s_up[idx] <= 'z') {
			s_up[idx] = s_up[idx] - 32;
		}

		idx++;
	}

	return (s_up);
}

char *ft_strnup(const char *s, size_t n)
{
	char *s_up;
	size_t idx;

	if (NULL == s) {
		return (NULL);
	}
	s_up = ft_strndup(s, n);

	idx = 0;
	while (idx < n && s_up[idx] != '\0') {
		if (s_up[idx] >= 'a' && s_up[idx] <= 'z') {
			s_up[idx] = s_up[idx] - 32;
		}
		idx++;
	}

	return (s_up);
}

char	*ft_strrev(const char *s)
{
	char	*srev;
	size_t	len;
	int		idx;

	if (NULL == s) {
		return (NULL);
	}

	len = ft_strlen(s);
	LIBFT_ALLOC(srev, len + 1);

	idx = len-1;
	while (*s != 0)
		srev[idx--] = *s++;

	srev[len] = 0;

	return (srev);
}

char	*ft_strdup(const char *s)
{
	char	*sdup;
	char	*ref;

	if (NULL == s) {
		return (NULL);
	}

	LIBFT_ALLOC(sdup, ft_strlen(s) + 1);

	if (NULL == sdup) {
		return (NULL);
	}

	ref = sdup;

	while (*s != '\0')
		*sdup++ = *s++;

	*sdup = '\0';

	return (ref);
}

char	*ft_strsub(char const *s, unsigned int start, size_t len)
{
	size_t	i;
	char	*subs;

	if (NULL == s) {
		return (NULL);
	}

	LIBFT_ALLOC(subs, len + 1);

	i = 0;
	while (i < len)
		subs[i++] = s[start++];

	subs[i] = 0;

	return (subs);
}

char	*ft_strncpy(char *dst, const char *src, size_t len)
{
	size_t	i;

	if (NULL == dst || NULL == src) {
		return (dst);
	}

	i = 0;
	while (i < len && src[i]) {
		dst[i] = src[i];
		i++;
	}
	while (i < len) {
		dst[i] = 0;
		i++;
	}
	return (dst);
}

int		ft_strcmp(const char *s1, const char *s2)
{
	size_t i;

	// no null check, same as libc strcmp

	i = 0;
	while (s1[i] || s2[i]) {
		if (s1[i] != s2[i]) {
			return (s1[i] - s2[i]);
		}
		i++;
	}

	return (s1[i] - s2[i]);
}

int	ft_streq(const char *a, const char *b)
{
	if (a == NULL && b == NULL) {
		return (1);
	}
	if (a == NULL || b == NULL) {
		return (0);
	}
	return (ft_strcmp(a, b) == 0 ? 1 : 0);
}

int	ft_strneq(const char *a, const char *b, size_t n)
{
	return (ft_strncmp(a, b, n) == 0 ? 1 : 0);
}

int	ft_tolower(int c)
{
	if (c >= 65 && c <= 90) {
		return (c + 32);
	}

	return (c);
}

int	ft_toupper(int c)
{
	if (c >= 97 && c <= 122) {
		return (c - 32);
	}

	return (c);
}

int	ft_ishex(int c)
{
	if (ft_isdigit(c) || (c >= 65 && c <= 70) || (c >= 97 && c <= 102)) {
		return (1);
	}

	return (0);
}

int	ft_iseol(int c)
{
	if ((c == 10) || (c == 13)) {
		return (1);
	}

	return (0);
}

int	ft_isdigit(int c)
{
	if (c >= 48 && c <= 57) {
		return (1);
	}

	return (0);
}

int	ft_iswspace(int c)
{
	if ((c != 10) && ((c >= 9 && c < 13) || (c == 32))) {
		return (1);
	}

	return (0);
}

int	ft_isblank(int c)
{
	if ((c == 9) || (c == 32)) {
		return (1);
	}

	return (0);
}

int	ft_iseolws(int c)
{
	if ((c >= 9 && c <= 13) || (c == 32)) {
		return (1);
	}

	return (0);
}

int	ft_isascii(int c)
{
	if (c >= 0 && c <= 127) {
		return (1);
	}

	return (0);
}

int	ft_isprint(int c)
{
	if (c >= 32 && c <= 126) {
		return (1);
	}

	return (0);
}

int	ft_isalpha(int c)
{
	if ((c >= 65 && c <= 90) || (c >= 97 && c <= 122)) {
		return (1);
	}

	return (0);
}

int	ft_isalnum(int c)
{
	if (ft_isalpha(c) || ft_isdigit(c)) {
		return (1);
	}

	return (0);
}

void	ft_putstr_fd(int fd, char const *s)
{
	if (NULL != s && fd >= 0) {
		write(fd, s, ft_strlen(s));
	}
}

void	ft_putstr(char const *s)
{
	if (NULL != s) {
		write(1, s, ft_strlen(s));
	}
}

/****************************************************************************/

int	read_to_buf(int fd, char *buf, int size, int *done)
{
	char	c;
	int		rbytes;
	int		tbytes;

	tbytes = 0;
	while (tbytes < size) {
		c = 0;

		if (0 > (rbytes = read(fd, &c, 1))) {
			return (-1);
		}
		*buf++ = c;
		tbytes += rbytes;

		if (rbytes == 0 || c == '\n') {
			*done = 1;
			break;
		}
	}
	*buf = 0;

	return (tbytes);
}

int	get_next_line(int fd, char **line)
{
	char	buf[LIBFT_BUFF + 1];
	int		rbytes;
	int		tbytes;
	int		done;

	done = 0;

	if ((NULL == line) || (fd < 0)) {
		return (-1);
	}
	LIBFT_ALLOC(*line, 1);

	tbytes = 0;
	while ((rbytes = read_to_buf(fd, buf, LIBFT_BUFF, &done)) > 0) {
		LIBFT_REALLOC(*line, tbytes+1, tbytes+rbytes+1);
		ft_strcat(*line, buf);
		tbytes += rbytes;

		if (done) {
			break ;
		}
	}
	if (rbytes < 0) {
		return (-1);
	}
	return (tbytes);
}

/****************************************************************************/

uint32_t ft_hash(const unsigned char *key, size_t keysize)
{
	uint32_t  hash;

	hash = 0;
	while (keysize--) {
		hash += *key++;
		hash += hash << 10;
		hash ^= hash >> 6;
	}
	hash += hash << 3;
	hash ^= hash >> 11;
	hash += hash << 15;

	return (hash);
}

/****************************************************************************/

static uint32_t	__calculate_hash(const char *key);
static t_node	*__create_htable_node(const char *key, void *content, uint32_t hash, t_func_content_del f_del);
static void 	__add_node_to_htable(t_htbl *htbl, const char *key, void *content, t_func_content_del f_del);
static void 	__assign_content_to_node(t_node *node, void *content, t_func_content_del f_del);
static t_node	*__get_node_from_htable(t_htbl *htbl, const char *key);
static t_node	*__get_node_from_list(t_node *list, const char *key);
static int		__get_htable_array_idx(t_htbl *htbl, uint32_t hash);
static void		__del_htable_array(t_htbl *htbl, t_func_content_del f_del);
static void		__copy_node_to_htable(t_htbl *htbl, t_node *node, t_func_content_copy f_copy);

void *ft_htbl_create(int size)
{
	t_htbl	*htbl;

	LIBFT_ALLOC(htbl, sizeof(t_htbl));

	htbl->size = MAX(LIBFT_HT_SIZE, CEIL_TO_MULTIPLE(size, LIBFT_HT_SIZE));

	LIBFT_ALLOC(htbl->arr, htbl->size * sizeof(void *));

	return (htbl);
}

void	(ft_htbl_add)(t_htbl *htbl, void *content, const char *key)
{
	if (NULL == htbl) {
		return ;
	}
	__add_node_to_htable(htbl, key, content, NULL);
}

void	ft_htbl_add_with_f_del(t_htbl *htbl, void *content, const char *key, t_func_content_del f_del)
{
	if (NULL == htbl) {
		return ;
	}
	__add_node_to_htable(htbl, key, content, f_del);
}

int		ft_htbl_has(t_htbl *htbl, const char *key)
{
	if (NULL == htbl) {
		return (0);
	}
	return (__get_node_from_htable(htbl, key) != NULL);
}

void	*ft_htbl_get(t_htbl *htbl, const char *key)
{
	t_node	*node;

	if (NULL == htbl) {
		return (NULL);
	}
	node = __get_node_from_htable(htbl, key);

	if (NULL == node) {
		return (NULL);
	}
	return (node->content);
}

void	(ft_htbl_assign)(t_htbl *htbl, void *content, const char *key)
{
	t_node	*node;

	if (NULL == htbl) {
		return ;
	}
	node = __get_node_from_htable(htbl, key);

	if (NULL == node) {
		return ;
	}
	__assign_content_to_node(node, content, NULL);
}

void	ft_htbl_assign_with_f_del(t_htbl *htbl, void *content, const char *key, t_func_content_del f_del)
{
	t_node	*node;

	if (NULL == htbl) {
		return ;
	}
	node = __get_node_from_htable(htbl, key);

	if (NULL == node) {
		return ;
	}
	__assign_content_to_node(node, content, f_del);
}

void	(ft_htbl_del_key)(t_htbl *htbl, const char *key)
{
	t_node		*node;
	t_node		*list;
	uint32_t	hash;
	int 		idx;

	if (NULL == htbl) {
		return;
	}
	hash = __calculate_hash(key);
	idx = __get_htable_array_idx(htbl, hash);

	list = htbl->arr[idx];
	node = __get_node_from_list(list, key);

	if (node != NULL) {
		ft_lst_del_one(&list, node);
	}
	htbl->arr[idx] = list;
}

void	ft_htbl_del_key_with_f_del(t_htbl *htbl, const char *key, t_func_content_del f_del)
{
	t_node		*node;
	t_node		*list;
	uint32_t	hash;
	int 		idx;

	if (NULL == htbl) {
		return;
	}
	hash = __calculate_hash(key);
	idx = __get_htable_array_idx(htbl, hash);

	list = htbl->arr[idx];
	node = __get_node_from_list(list, key);

	if (node != NULL) {
		ft_lst_del_one_with_f_del(&list, node, f_del);
	}
	htbl->arr[idx] = list;
}

void ft_htbl_resize(t_htbl *htbl, int size)
{
	t_htbl new_htbl;
	t_node *node;

	if (NULL == htbl || size <= 0) {
		return;
	}

	new_htbl.size = MAX(LIBFT_HT_SIZE, CEIL_TO_MULTIPLE(size, LIBFT_HT_SIZE));

	if (new_htbl.size <= htbl->size) {
		return;
	}

	LIBFT_ALLOC(new_htbl.arr, new_htbl.size * sizeof(void *));

	node = ft_htbl_node_next(htbl, NULL);

	while (node != NULL) {
		__add_node_to_htable(&new_htbl, node->key, node->content, node->f_del_content);
		node = ft_htbl_node_next(htbl, node);
	}

	__del_htable_array(htbl, NULL);

	htbl->arr = new_htbl.arr;
	htbl->size = new_htbl.size;
}

static uint32_t	__calculate_hash(const char *key)
{
	return (ft_hash((unsigned char *)key, ft_strlen(key)));
}

static int	__get_htable_array_idx(t_htbl *htbl, uint32_t hash)
{
	return ((int)(hash % (uint32_t)htbl->size));
}

static void	__add_node_to_htable(t_htbl *htbl, const char *key, void *content, t_func_content_del f_del)
{
	t_node		*list;
	t_node		*node;
	uint32_t	hash;
	int 		idx;

	hash = __calculate_hash(key);
	idx = __get_htable_array_idx(htbl, hash);
	list = htbl->arr[idx];

	if (NULL != list) {
		node = __get_node_from_list(list, key);

		if (NULL != node) {
			__assign_content_to_node(node, content, f_del);
		} else {
			ft_lst_prepend(&list, __create_htable_node(key, content, hash, f_del));
			htbl->arr[idx] = list;
		}
	} else {
		htbl->arr[idx] = __create_htable_node(key, content, hash, f_del);
	}
}

static t_node	*__create_htable_node(const char *key, void *content, uint32_t hash, t_func_content_del f_del)
{
	t_node	*node;

	node = ft_node_new_with_f_del(key, content, sizeof(t_node), f_del);
	node->hash = hash;

	return (node);
}

static void 	__assign_content_to_node(t_node *node, void *content, t_func_content_del f_del)
{
	if (node->f_del_content) {
		node->f_del_content(node->content);
	}
	node->content = content;
	node->f_del_content = f_del;
}

static t_node	*__get_node_from_list(t_node *list, const char *key)
{
	t_node		*node;

	node = list;

	while (NULL != node) {
		if (ft_strcmp(node->key, key) == 0) {
			return (node);
		}
		node = node->next;
	}
	return (NULL);
}

static t_node	*__get_node_from_htable(t_htbl *htbl, const char *key)
{
	t_node		*list;
	uint32_t	hash;

	hash = __calculate_hash(key);

	list = htbl->arr[hash % (uint32_t)htbl->size];

	if (NULL == list) {
		return (NULL);
	}
	return (__get_node_from_list(list, key));
}

void	(ft_htbl_del)(t_htbl *htbl)
{
	if (NULL == htbl) {
		return;
	}
	__del_htable_array(htbl, NULL);
	LIBFT_FREE(htbl);
}

void	ft_htbl_del_with_f_del(t_htbl *htbl, t_func_content_del f_del)
{
	if (NULL == htbl) {
		return;
	}
	__del_htable_array(htbl, f_del);
	LIBFT_FREE(htbl);
}

static void	__del_htable_array(t_htbl *htbl, t_func_content_del f_del)
{
	t_node *node;
	t_node *tmp;
	int idx;

	if (htbl->arr == NULL) {
		return;
	}
	idx = 0;
	while (idx < htbl->size) {
		node = htbl->arr[idx];

		while (NULL != node) {
			tmp = node;
			node = node->next;

			if (f_del) {
				ft_node_del_with_f_del(tmp, f_del);
			} else {
				(ft_node_del)(tmp);
			}
		}
		idx++;
	}
	LIBFT_FREE(htbl->arr);
	htbl->arr = NULL;
	htbl->size = 0;
}

t_node	*ft_htbl_node_next(t_htbl *htbl, t_node *node)
{
	t_node	*item;
	int	idx;

	if (NULL == htbl) {
		return (NULL);
	}
	if (node != NULL && node->next != NULL) {
		return (node->next);
	}

	if (node != NULL) {
		idx = (int)(node->hash % (uint32_t)htbl->size) + 1;
	} else {
		idx = 0;
	}

	while (idx < htbl->size) {
		item = htbl->arr[idx];

		if (NULL != item) {
			return (item);
		}
		idx++;
	}
	return (NULL);
}

void	ft_htbl_dump(t_htbl *htbl)
{
	t_node *node;
	int idx;

	if (NULL == htbl) {
		ft_printf("hash table: (null)\n");
		return ;
	}
	ft_printf("hash table: {size: %d, arr: %p}\n", htbl->size, htbl->arr);

	if (htbl->arr != NULL && htbl->size > 0) {
		idx = 0;
		while (idx < htbl->size) {
			node = htbl->arr[idx];
			while (node != NULL) {
				ft_printf("\tnode at idx: %d, addr: %p: {key: %s, hash: %llu}\n", idx, node, node->key, node->hash);
				node = node->next;
			}
			idx++;
		}
	}
}

t_htbl	*ft_htbl_copy(t_htbl *htbl)
{
	t_htbl	*copy;

	if (NULL == htbl) {
		return (NULL);
	}
	copy = ft_htbl_create(htbl->size);
	ft_htbl_merge(copy, htbl);
	return (copy);
}

t_htbl	*ft_htbl_copy_with_f_copy(t_htbl *htbl, t_func_content_copy f_copy)
{
	t_htbl	*copy;

	if (NULL == htbl) {
		return (NULL);
	}
	copy = ft_htbl_create(htbl->size);
	ft_htbl_merge_with_f_copy(copy, htbl, f_copy);
	return (copy);
}

void	ft_htbl_merge(t_htbl *htbl_to, t_htbl *htbl_from)
{
	t_node	*node;

	if (htbl_from == NULL || htbl_to == NULL) {
		return;
	}
	if (htbl_from->size > htbl_to->size) {
		ft_htbl_resize(htbl_to, htbl_from->size);
	}
	node = ft_htbl_node_next(htbl_from, NULL);

	while (node != NULL) {
		__copy_node_to_htable(htbl_to, node, NULL);
		node = ft_htbl_node_next(htbl_from, node);
	}
}

void	ft_htbl_merge_with_f_copy(t_htbl *htbl_to, t_htbl *htbl_from, t_func_content_copy f_copy)
{
	t_node	*node;

	if (htbl_from == NULL || htbl_to == NULL) {
		return;
	}
	if (htbl_from->size > htbl_to->size) {
		ft_htbl_resize(htbl_to, htbl_from->size);
	}
	node = ft_htbl_node_next(htbl_from, NULL);
	while (node != NULL) {
		__copy_node_to_htable(htbl_to, node, f_copy);
		node = ft_htbl_node_next(htbl_from, node);
	}
}


static void	__copy_node_to_htable(t_htbl *htbl, t_node *node, t_func_content_copy f_copy)
{
	if (f_copy != NULL) {
		__add_node_to_htable(htbl, node->key, f_copy(node->content), node->f_del_content);
	}
	else {
		__add_node_to_htable(htbl, node->key, node->content, node->f_del_content);
	}
}

/****************************************************************************/

static char		*__to_cstring(char *raw, size_t rawsize);

void	ft_htbl_add_rawkey(t_htbl *htbl, void *content, const void *rawkey, size_t rksize)
{
	char	*cskey;

	cskey = __to_cstring((char *)rawkey, rksize);
	(ft_htbl_add)(htbl, content, (const char *)cskey);
	LIBFT_FREE(cskey);
}

void	ft_htbl_add_rawkey_with_f_del(t_htbl *htbl, void *content, const void *rawkey, size_t rksize, t_func_content_del f_del)
{
	char	*cskey;

	cskey = __to_cstring((char *)rawkey, rksize);
	ft_htbl_add_with_f_del(htbl, content, (const char *)cskey, f_del);
	LIBFT_FREE(cskey);
}

void	*ft_htbl_get_rawkey(t_htbl *htbl, const void *rawkey, size_t rksize)
{
	char	*cskey;
	void	*content;

	cskey = __to_cstring((char *)rawkey, rksize);
	content = ft_htbl_get(htbl, (const char *)cskey);
	LIBFT_FREE(cskey);

	return (content);
}

void	ft_htbl_assign_rawkey(t_htbl *htbl, void *content, const void *rawkey, size_t rksize)
{
	char	*cskey;

	cskey = __to_cstring((char *)rawkey, rksize);
	(ft_htbl_assign)(htbl, content, (const char *)cskey);
	LIBFT_FREE(cskey);
}

void	ft_htbl_assign_rawkey_with_f_del(t_htbl *htbl, void *content, const void *rawkey, size_t rksize, t_func_content_del f_del)
{
	char	*cskey;

	cskey = __to_cstring((char *)rawkey, rksize);
	ft_htbl_assign_with_f_del(htbl, content, (const char *)cskey, f_del);
	LIBFT_FREE(cskey);
}

static char	*__to_cstring(char *raw, size_t rawsize)
{
	char *cskey;
	size_t ix;

	LIBFT_ALLOC(cskey, rawsize + 1);
	ix = 0;

	while (ix < rawsize) {
		if (raw[ix] == 0) {
			cskey[ix] = '=';
		} else {
			cskey[ix] = raw[ix];
		}
		ix++;
	}
	cskey[rawsize] = 0;

	return (cskey);
}

/****************************************************************************/

void	__tostr(char *str, intmax_t nb, intmax_t mult)
{
	int	i;

	i = 0;

	if (nb < 0) {
		str[i++] = '-';
	}
	if (nb < 0) {
		nb = -nb;
	}
	while (mult) {
		str[i++] = nb / mult + 48;
		nb = nb % mult;
		mult = mult / 10;
	}
	str[i] = 0;
}

char	*ft_itoa(intmax_t n)
{
	intmax_t	nb;
	intmax_t	mult;
	char		*str;
	int			len;

	mult = 1;
	len = (n < 0) ? 2 : 1;
	nb = n;

	while (n / 10) {
		mult = mult * 10;
		n = n / 10;
		len++;
	}
	LIBFT_ALLOC(str, len + 1);
	__tostr(str, nb, mult);

	return (str);
}

/****************************************************************************/

static int	__space_needed(intmax_t value, int base)
{
	int	len;

	len = 0;
	while (value) {
		len++;
		value = value / base;
	}
	return (len);
}

void	__digit_arr(uint8_t *arr, intmax_t val, int base, int len)
{
	if (val < 0) {
		val = -val;
	}
	while (len) {
		arr[len-1] = (uint8_t)(val % base);
		val = val / base;
		len--;
	}
}

void	__base_str(char *s, uint8_t *arr, int len, int is_neg)
{
	uint8_t	*p;
	uint8_t	digit;

	p = (uint8_t *)s;

	if (is_neg) {
		*p++ = '-';
	}

	while (len--) {
		digit = *arr++;

		if (digit >= 0 && digit <= 9) {
			*p = digit + 48;
		}
		else if (digit >= 10 && digit <= 15)
			*p = digit + 87;

		p++;
	}
	*p = 0;
}

char	*ft_itoa_base(intmax_t value, int base)
{
	int		is_neg;
	int		len;
	uint8_t	arr[64];
	char	*s;

	len = 0;
	is_neg = 0;

	if (base > 16) {
		return (ft_strdup("n/a"));
	}
	if (value == 0) {
		return (ft_strdup("0"));
	}
	if (base != 10 && value < 0) {
		value = -value;
	}
	if (base == 10 && value < 0) {
		is_neg = 1;
	}
	len = __space_needed(value, base);
	__digit_arr(arr, value, base, len);

	LIBFT_ALLOC(s, len+1 + (int)(is_neg)); // extra for hypen symbol
	__base_str(s, arr, len, is_neg);

	return (s);
}

/****************************************************************************/

t_node	*ft_lst_append(t_node **lst, t_node *node)
{
	t_node	*tmp;

	if (NULL == lst || NULL == node) {
		return (NULL);
	}
	if (NULL == *lst) {
		*lst = node;
		return (node);
	}
	tmp = *lst;

	while (tmp->next) {
		tmp = tmp->next;
	}
	tmp->next = node;

	return (node);
}

t_node	*ft_lst_prepend(t_node **lst, t_node *node)
{
	if (NULL == node || NULL == lst) {
		return (NULL);
	}
	node->next = *lst;
	*lst = node;

	return (node);
}

t_node	*ft_lst_concat(t_node *lst_a, t_node *lst_b)
{
	if (NULL == lst_a) {
		return (lst_b);
	}
	if (NULL == lst_b) {
		return (lst_a);
	}
	while (lst_a->next) {
		lst_a = lst_a->next;
	}
	lst_a->next = lst_b;

	return (lst_a);
}

void	ft_lst_del_first(t_node **lst)
{
	t_node	*tmp;

	if ((NULL == lst) || (NULL == *lst)) {
		return ;
	}
	tmp = *lst;
	*lst = (*lst)->next;
	ft_node_del(tmp);
}

void	ft_lst_del_first_with_f_del(t_node **lst, t_func_content_del f_del)
{
	t_node	*tmp;

	if ((NULL == lst) || (NULL == *lst)) {
		return ;
	}
	tmp = *lst;
	*lst = (*lst)->next;
	ft_node_del_with_f_del(tmp, f_del);
}

void	ft_lst_del_last(t_node **lst)
{
	t_node	*cur;
	t_node	*prev;

	if ((NULL == lst) || (NULL == *lst)) {
		return ;
	}
	prev = NULL;
	cur = *lst;

	while (cur->next) {
		prev = cur;
		cur = cur->next;
	}
	if (NULL == prev) {
		*lst = NULL;
	} else {
		prev->next = NULL;
	}
	ft_node_del(cur);
}

void	ft_lst_del_last_with_f_del(t_node **lst, t_func_content_del f_del)
{
	t_node	*cur;
	t_node	*prev;

	if ((NULL == lst) || (NULL == *lst)) {
		return ;
	}
	prev = NULL;
	cur = *lst;

	while (cur->next) {
		prev = cur;
		cur = cur->next;
	}
	if (NULL == prev) {
		*lst = NULL;
	} else {
		prev->next = NULL;
	}
	ft_node_del_with_f_del(cur, f_del);
}

void	ft_lst_del_one(t_node **lst, t_node *node)
{
	t_node	*cur;
	t_node	*prev;

	if (NULL == lst || NULL == *lst || NULL == node) {
		return ;
	}
	prev = NULL;
	cur = *lst;

	while ((NULL != cur) && (node != cur)) {
		prev = cur;
		cur = cur->next;
	}
	if (NULL == cur) {
		return ;
	}
	if (NULL == prev) {
		*lst = cur->next;
	} else {
		prev->next = cur->next;
	}
	ft_node_del(cur);
}

void	ft_lst_del_one_with_f_del(t_node **lst, t_node *node, t_func_content_del f_del)
{
	t_node	*cur;
	t_node	*prev;

	if (NULL == lst || NULL == *lst || NULL == node) {
		return ;
	}
	prev = NULL;
	cur = *lst;

	while ((NULL != cur) && (node != cur)) {
		prev = cur;
		cur = cur->next;
	}
	if (NULL == cur) {
		return ;
	}
	if (NULL == prev) {
		*lst = cur->next;
	} else {
		prev->next = cur->next;
	}
	ft_node_del_with_f_del(cur, f_del);
}

size_t	ft_lst_size(t_node *lst)
{
	size_t	size;

	size = 0;
	while (lst) {
		size++;
		lst = lst->next;
	}

	return (size);
}

void	ft_lst_rev(t_node **lst)
{
	t_node	*prev;
	t_node	*cur;
	t_node	*next;

	if (NULL == lst || NULL == *lst) {
		return ;
	}

	prev = NULL;
	cur = *lst;

	while (cur) {
		next = cur->next;
		cur->next = prev;
		prev = cur;
		cur = next;
	}
	*lst = prev;
}

void	ft_lst_del(t_node *lst)
{
	t_node	*tmp;

	while (lst) {
		tmp = lst;
		lst = lst->next;
		ft_node_del(tmp);
	}
}

void	ft_lst_del_with_f_del(t_node *lst, t_func_content_del f_del)
{
	t_node	*tmp;

	while (lst) {
		tmp = lst;
		lst = lst->next;
		ft_node_del_with_f_del(tmp, f_del);
	}
}

int	ft_lst_map(t_node *lst, void *farg, int (*f)(t_node *, void *))
{
	if (NULL == f) {
		return (-1);
	}
	while (lst) {
		if (f(lst, farg) < 0) {
			return (-1);
		}
		lst = lst->next;
	}
	return (0);
}

t_htbl	*ft_lst_to_htable(t_node *lst)
{
	t_htbl	*htbl;

	if (NULL == lst) {
		return (NULL);
	}
	htbl = ft_htbl_create(ft_lst_size(lst));

	while (lst) {
		ft_htbl_add(htbl, lst->content, lst->key);
		lst = lst->next;
	}

	return (htbl);
}

/****************************************************************************/

#define FT_LOG_CRIT_LOG_PREFIX	"[crit ] "
#define FT_LOG_ERROR_LOG_PREFIX	"[error] "
#define FT_LOG_WARN_LOG_PREFIX	"[warn ] "
#define FT_LOG_INFO_LOG_PREFIX	"[info ] "
#define FT_LOG_DEBUG_LOG_PREFIX	"[debug] "
#define FT_LOG_TRACE_LOG_PREFIX	"[trace] "
#define FT_LOG_ALL_LOG_PREFIX	""

static const char *__lvl_prefixes[] = {
	[LIBFT_LOG_LEVEL_ALWAYS] = FT_LOG_ALL_LOG_PREFIX,
	[LIBFT_LOG_LEVEL_CRIT] = FT_LOG_CRIT_LOG_PREFIX,
	[LIBFT_LOG_LEVEL_ERROR] = FT_LOG_ERROR_LOG_PREFIX,
	[LIBFT_LOG_LEVEL_WARN] = FT_LOG_WARN_LOG_PREFIX,
	[LIBFT_LOG_LEVEL_INFO] = FT_LOG_INFO_LOG_PREFIX,
	[LIBFT_LOG_LEVEL_DEBUG] = FT_LOG_DEBUG_LOG_PREFIX,
	[LIBFT_LOG_LEVEL_TRACE] = FT_LOG_TRACE_LOG_PREFIX,
};
static const size_t __lvl_prefixes_size = sizeof(__lvl_prefixes) / sizeof(__lvl_prefixes[0]);

static const char *__colored_lvl_prefixes[] = {
	[LIBFT_LOG_LEVEL_ALWAYS] = TXT_B_WHITE(FT_LOG_ALL_LOG_PREFIX),
	[LIBFT_LOG_LEVEL_CRIT] = TXT_MAGEN(FT_LOG_CRIT_LOG_PREFIX),
	[LIBFT_LOG_LEVEL_ERROR] = TXT_B_RED(FT_LOG_ERROR_LOG_PREFIX),
	[LIBFT_LOG_LEVEL_WARN] = TXT_YELL(FT_LOG_WARN_LOG_PREFIX),
	[LIBFT_LOG_LEVEL_INFO] = TXT_BLUE(FT_LOG_INFO_LOG_PREFIX),
	[LIBFT_LOG_LEVEL_DEBUG] = TXT_CYAN(FT_LOG_DEBUG_LOG_PREFIX),
	[LIBFT_LOG_LEVEL_TRACE] = TXT_GRAY(FT_LOG_TRACE_LOG_PREFIX),
};
static const size_t __colored_lvl_prefixes_size = sizeof(__colored_lvl_prefixes) / sizeof(__colored_lvl_prefixes[0]);

#define __DEBUG_MSG(TXT, COLORED)	(COLORED ? TXT_YELL(TXT) : TXT)
#define __LVL_PREFIX(LVL, COLORED)	(COLORED ? __colored_lvl_prefixes[LVL%__colored_lvl_prefixes_size] : __lvl_prefixes[LVL%__lvl_prefixes_size])

static int __log(const char *func_name, const char *file_name, int line_number, t_logger *logger, uint8_t level, const char *fmt, va_list va_arg);

static int __default_log_writer(const char *mes) {
	return (ft_printf("%@%s\n", mes));
}
static t_logger __default_logger = {
	.log_writer = __default_log_writer,
	.log_level_thres = LIBFT_LOG_LEVEL_INFO,
	.debug_info_thres = LIBFT_LOG_LEVEL_DEBUG,
	.is_ansi_colored = 0,
};

int	ft_log_log(const char *func_name, const char *file_name, int line_number, t_logger *logger, uint8_t level, const char *fmt, ...)
{
	va_list	va_arg;
	int		ret;

	if (NULL == logger) {
		logger = &__default_logger;
	}
	if (NULL == logger || NULL == logger->log_writer || NULL == fmt) {
		return (0);
	}
	if (level > logger->log_level_thres) {
		return (0);
	}
	va_start(va_arg, fmt);

	ret = __log(func_name, file_name, line_number, logger, level, fmt, va_arg);

	va_end(va_arg);

	return (ret);
}

int	ft_log_va_log(const char *func_name, const char *file_name, int line_number, t_logger *logger, uint8_t level, const char *fmt, va_list va_arg)
{
	if (NULL == logger) {
		logger = &__default_logger;
	}
	if (NULL == logger || NULL == logger->log_writer || NULL == fmt) {
		return (0);
	}
	if (level > logger->log_level_thres) {
		return (0);
	}
	return (__log(func_name, file_name, line_number, logger, level, fmt, va_arg));
}

t_logger *ft_log_get_default_logger(void)
{
	return (&__default_logger);
}

static int	__log(const char *func_name, const char *file_name, int line_number, t_logger *logger, uint8_t level, const char *fmt, va_list va_arg)
{
	const char	*lvl_prefix;
	char	*full_mes;
	char	*debug_mes;
	char	*mes;
	int		ret;

	lvl_prefix = __LVL_PREFIX(level, logger->is_ansi_colored);
	debug_mes = NULL;
	mes = NULL;

	if (level <= logger->debug_info_thres) {
		if (NULL != file_name) {
			ft_sprintf(&debug_mes, __DEBUG_MSG(" (%s:%d)", logger->is_ansi_colored), file_name, line_number);
		} else {
			debug_mes = ft_strdup("");
		}
	}
	ft_vsprintf(&mes, fmt, va_arg);

	if (NULL != func_name) {
		full_mes = ft_strjoin_multi(5, lvl_prefix, func_name, ": ", mes, debug_mes);
	} else {
		full_mes = ft_strjoin_multi(4, lvl_prefix, mes, debug_mes);
	}
	ret = logger->log_writer(full_mes);

	LIBFT_FREE(debug_mes);
	LIBFT_FREE(full_mes);
	LIBFT_FREE(mes);

	return (ret);
}

/****************************************************************************/

static t_node	*__lookup(void *memptr);
static int		__push(const char *memkey, void *memptr, size_t memsize);
static void		__clear(void);

static t_node	*__memlist;
int				global_libft_alloc_error;

void	*ft_malloc(const char *memkey, size_t memsize)
{
	void	*memptr;
	t_node	*lookup_node;

	global_libft_alloc_error = LIBFT_MEM_OK;

	memptr = malloc(memsize);
	ft_bzero(memptr, memsize);

	if (ENOMEM == errno) {
		global_libft_alloc_error = LIBFT_MEM_FATAL;
		return (NULL);
	}

	if (NULL != (lookup_node = __lookup(memptr))) {
		global_libft_alloc_error = LIBFT_MEM_LEAK;
		return (NULL);
	}

	if (LIBFT_MEM_OK != __push(memkey, memptr, memsize)) {
		global_libft_alloc_error = LIBFT_MEM_FATAL;
		return (NULL);
	}

	return (memptr);
}

void	ft_free(const char *memkey, void *memptr)
{
	t_node	*node;
	t_node	*prevnode;

	global_libft_alloc_error = LIBFT_MEM_OK;

	node = __memlist;
	prevnode = NULL;

	while (node) {
		if (node->content == memptr) {
			break ;
		}
		prevnode = node;
		node = node->next;
	}

	if (NULL == node) {
		global_libft_alloc_error = LIBFT_MEM_DOUBLE_FREE;
		return ;
	}

	if (NULL == prevnode) {
		__memlist = node->next;
	}
	else
		prevnode->next = node->next;

	free(node->content);
	free(node);
}

void	ft_free_all(void)
{
	__clear();
}

static t_node *__lookup(void *memptr)
{
	t_node *node;

	node = __memlist;

	while (node) {
		if (node->content == memptr) {
			return (node);
		}

		node = node->next;
	}

	return (NULL);
}

static int __push(const char *memkey, void *memptr, size_t memsize)
{
	t_node *node;

	if (NULL == (node = malloc(sizeof(t_node)))) {
		return (LIBFT_MEM_FATAL);
	}

	node->key = (char *)memkey;
	node->content = memptr;
	node->size = memsize;
	node->next = __memlist;
	__memlist = node;

	return (LIBFT_MEM_OK);
}

static void __clear(void)
{
	t_node *node;

	while (__memlist) {
		node = __memlist;
		__memlist = __memlist->next;
		free(node);
	}
}

/****************************************************************************/

void	ft_bzero(void *s, size_t n)
{
	if ((NULL == s) || (n == 0)) {
		return ;
	}

	ft_memset(s, 0, n);
}

void	*ft_memdup(void *src, size_t srcsize)
{
	void	*dst;

	if (NULL == src) {
		return (NULL);
	}

	LIBFT_ALLOC(dst, srcsize);
	ft_memcpy(dst, src, srcsize);

	return (dst);
}

int	ft_memcmp(const void *p1, const void *p2, size_t n)
{
	unsigned char	*octets_1;
	unsigned char	*octets_2;
	size_t			i;

	octets_1 = (unsigned char *)p1;
	octets_2 = (unsigned char *)p2;

	i = 0;
	while (i < n) {
		if (octets_1[i] != octets_2[i]) {
			return (octets_1[i] - octets_2[i]);
		}
		i++;
	}
	return (0);
}

void	*ft_memcpy(void *dst, const void *src, size_t srcsize)
{
	size_t			ix;
	unsigned char	*p1;
	unsigned char	*p2;

	ix = 0;
	p1 = (unsigned char *)dst;
	p2 = (unsigned char *)src;

	while (ix < srcsize) {
		p1[ix] = p2[ix];
		ix++;
	}

	return (dst);
}

// Copy destsize bytes from src to dest.
// If destsize is bigger than srcsize set extra bytes (destsize - srcsize) to zero
void	*ft_memzcpy(void *dst, const void *src, size_t dstsize, size_t srcsize)
{
	if (dstsize > srcsize) {
		ft_memcpy(dst, src, srcsize);
		ft_memset(dst + srcsize, 0, dstsize - srcsize);
	} else {
		ft_memcpy(dst, src, dstsize);
	}
	return (dst);
}

void	*ft_memset(void *buf, int byte, size_t len)
{
	size_t	i;

	i = 0;
	while (i < len) {
		((uint8_t *)buf)[i++] = (uint8_t)byte;
	}
	return (buf);
}

void	*ft_memmove(void *dst, const void *src, size_t size)
{
	unsigned char	*p1;
	unsigned char	*p2;
	size_t	ix;

	p1 = (unsigned char *)dst;
	p2 = (unsigned char *)src;
	ix = 0;

	// Check if destination address is "after" source address.
	// If so, copy in reverse order
	if (dst >= src) {
		while (size--)
			p1[size] = p2[size];
	} else {
		while (ix < size) {
			p1[ix] = p2[ix];
			ix++;
		}
	}
	return (dst);
}

/****************************************************************************/

void	(ft_node_init)(t_node *node)
{
	ft_bzero(node, sizeof(t_node));
}

void	ft_node_init_with_f_del(t_node *node, t_func_content_del f_del)
{
	ft_bzero(node, sizeof(t_node));
	node->f_del_content = f_del;
}

t_node *(ft_node_create)(void)
{
	t_node	*node;

	LIBFT_ALLOC(node, sizeof(t_node));

	return (node);
}

t_node *ft_node_create_with_f_del(t_func_content_del f_del)
{
	t_node	*node;

	LIBFT_ALLOC(node, sizeof(t_node));
	node->f_del_content = f_del;

	return (node);
}

t_node	*(ft_node_new)(const char *key, void *content, size_t size)
{
	t_node	*node;

	node = ft_node_create();

	if (NULL == node) {
		return (NULL);
	}

	node->key = ft_strdup(key);
	node->content = content;
	node->size = size;

	return (node);
}

t_node	*ft_node_new_with_f_del(const char *key, void *content, size_t size, t_func_content_del f_del)
{
	t_node	*node;

	node = ft_node_create_with_f_del(f_del);

	if (NULL == node) {
		return (NULL);
	}

	node->key = ft_strdup(key);
	node->content = content;
	node->size = size;

	return (node);
}

void	(ft_node_del)(t_node *node)
{
	if (NULL != node) {
		if (NULL != node->f_del_content) {
			node->f_del_content(node->content);
		}
		LIBFT_FREE(node->key);
		LIBFT_FREE(node);
	}
}

void ft_node_del_with_f_del(t_node *node, t_func_content_del f_del)
{
	if (NULL != node) {
		if (NULL != f_del) {
			f_del(node->content);
		}
		LIBFT_FREE(node->key);
		LIBFT_FREE(node);
	}
}

int   ft_node_is_parent(t_node *node)
{
	if (NULL == node) {
		return (0);
	}

	return (NULL != node->nodes);
}

/****************************************************************************/

static int	__calc_size_func(t_node *node, const void *farg);
static void	__del_node_recur(t_node *node, t_func_content_del f_del);
static int	__iter_func(t_node *node, const void *ptr);
static int	__htable_func(t_node *node, const void *farg);
static void	__f_print_default(t_node *node, int space);
static int __dfs_recur(t_node *node, const void *farg, int (*f)(t_node *, const void *), t_node **res);
static void	__print_recur(t_node *node, int *space, void (*f_print)(t_node *, int));
static t_node *__create_recur(char **key, int *idx);

static int __depth;

int	ft_ntree_size(t_node *node)
{
	int	size;

	size = 0;
	ft_ntree_bfs(NULL, node, &size, __calc_size_func);

	return (size);
}

void	ft_ntree_del(t_node *ntree, t_func_content_del f_del)
{
	if (NULL == ntree) {
		return ;
	}
	__del_node_recur(ntree, f_del);
}

t_node	*ft_ntree_iter(t_node *ntree)
{
	t_queue	*queue;
	t_node	*iter;

	if (NULL == ntree || NULL == (queue = ft_queue_create())) {
		return (NULL);
	}
	ft_ntree_bfs(NULL, ntree, queue, __iter_func);

	iter = ft_queue_first(queue);
	LIBFT_FREE(queue);

	return (iter);
}

t_htbl	*ft_ntree_to_htable(t_node *node)
{
	t_htbl	*htbl;

	if (NULL == node) {
		return (NULL);
	}

	htbl = ft_htbl_create(ft_ntree_size(node));
	ft_ntree_bfs(NULL, node, htbl, __htable_func);

	return (htbl);
}

void	ft_ntree_print(t_node *ntree, t_func_ntree_print f_print)
{
	int	space;

	space = 0;

	if (NULL == f_print) {
		f_print = __f_print_default;
	}

	__print_recur(ntree, &space, f_print);
}

t_node *ft_ntree_construct(const char *map)
{
	// Construct ntree from a map.
	//
	// map: 	"colors{ RGB{ 25 27 30 } RGBA{ 32 35 40 0.5 } } sizes{ S M L }"
	//
	// result:
	//							colors         ------>       sizes
	//						/			\					/  \   \
	//					RGB	  			RGBA				S   M   L
	//				/	|	\		/	|	\	\
	//			   25	27	30		32	35	40	0.5

	t_node *ntree;
	char **keys;
	int idx;

	if (NULL == map) {
		return (NULL);
	}

	idx = 0;
	keys = ft_strsplit(map, ' ');

	if (NULL == keys) {
		return (NULL);
	}

	ntree = __create_recur(keys, &idx);
	ft_2darray_del_null_terminated((void **)keys);

	return (ntree);
}

int	ft_ntree_bfs(t_node **res, t_node *node, const void *farg, t_func_ntree_map f)
{
	t_queue	*queue;
	t_node	*child_node;
	t_node	*result_node;
	int		ret;

	if (NULL == node || NULL == f) {
		return (-1);
	}
	queue = ft_queue_create();
	result_node = NULL;

	while (node) {
		ft_queue_enqueue(queue, NULL, node, 0);
		node = node->next;
	}
	while (!ft_queue_is_empty(queue)) {
		node = ft_queue_dequeue(queue);

		if (ft_node_is_parent(node)) {
			child_node = node->nodes;

			while (NULL != child_node) {
				ft_queue_enqueue(queue, NULL, child_node, 0);
				child_node = child_node->next;
			}
		}
		ret = f(node, farg);

		if (ret < 0) {
			break ;
		} else if (ret == 1) {
			result_node = node;
			break ;
		}
	}
	if (res != NULL) {
		*res = result_node;
	}
	ft_queue_del(queue);

	return (ret);
}

int	ft_ntree_dfs(t_node **res, t_node *ntree, const void *farg, t_func_ntree_map f)
{
	t_node	*node;
	int		ret;

	__depth = 0;

	if ((NULL == ntree) || (NULL == f)) {
		return (-1);
	}

	ret = __dfs_recur(ntree, farg, f, &node);

	if (res != NULL) {
		*res = node;
	}

	return (ret);
}

int ft_ntree_dfs_cur_depth(void)
{
	return (__depth);
}

static int __dfs_recur(t_node *node, const void *farg, t_func_ntree_map f, t_node **res)
{
	int ret;

	if (NULL == node) {
		*res = NULL;
		return (0);
	}

	if ((ret = f(node, farg)) != 0) {
		*res = (ret == 1) ? node : NULL;
		return (ret);
	}

	__depth++;

	ret = __dfs_recur(node->nodes, farg, f, res);

	__depth--;

	if (ret == 0) {
		ret = __dfs_recur(node->next, farg, f, res);
	}

	return (ret);
}

static int	__calc_size_func(t_node *node, const void *farg)
{
	int	*size;

	(void)node;
	size = (int *)farg;
	*size = *size + 1;

	return (0);
}

static void	__del_node_recur(t_node *node, t_func_content_del f_del)
{
	if (NULL == node) {
		return ;
	}
	__del_node_recur(node->nodes, f_del);
	__del_node_recur(node->next, f_del);
	ft_node_del_with_f_del(node, f_del);
}

static int	__iter_func(t_node *node, const void *ptr)
{
	t_queue	*queue;

	queue = (t_queue *)(ptr);
	ft_queue_enqueue(queue, NULL, node, 0);

	return (0);
}

static int	__htable_func(t_node *node, const void *farg)
{
	ft_htbl_add((t_htbl *)farg, node->content, node->key);

	return (0);
}

static void	__f_print_default(t_node *node, int space)
{
	(void)space;

	if (NULL != node) {
		if (ft_node_is_parent(node)) {
			ft_printf("%s\n", node->key, node->content);
		}
		else
			ft_printf("%-25.25s %p\n", node->key, node->content);
	}
}

static void	__print_recur(t_node *node, int *space, void (*f_print)(t_node *, int))
{
	if (NULL == node) {
		return ;
	}

	f_print(node, *space);

	(*space)++;

	__print_recur(node->nodes, space, f_print);

	(*space)--;

	__print_recur(node->next, space, f_print);
}

static t_node *__create_recur(char **key, int *idx)
{
	t_node	*node;
	char	**childkey;

	if (NULL == key[*idx]) {
		return (NULL);
	}

	if (!ft_strcmp(key[*idx], "}")) {
		(*idx)++;
		return (NULL);
	}

	node = ft_node_new(key[(*idx)++], NULL, 0);

	if (ft_strchr(node->key, '{')) {
		childkey = ft_strsplit(node->key, '{');
		LIBFT_FREE(node->key);

		if (NULL != childkey) {
			node->key = ft_strdup(childkey[0]);
		}
		else
			node->key = NULL;

		ft_2darray_del_null_terminated((void **)childkey);

		node->nodes = __create_recur(key, idx);
	}

	node->next = __create_recur(key, idx);

	return (node);
}

/****************************************************************************/

t_ostring	*ft_ostr_new(void)
{
	t_ostring	*ostring;

	LIBFT_ALLOC(ostring, sizeof(t_ostring));
	if (NULL == ostring) {
		return (NULL);
	}
	ft_ostr_init(ostring);

	return (ostring);
}

t_ostring	*ft_ostr_new_with_capacity(size_t capacity)
{
	t_ostring	*ostring;

	LIBFT_ALLOC(ostring, sizeof(t_ostring));
	if (NULL == ostring) {
		return (NULL);
	}
	ft_ostr_init_with_capacity(ostring, capacity);

	return (ostring);
}

t_ostring	*ft_ostr_create_from_cstr(const char *cstr)
{
	size_t slen = ft_strlen(cstr);
	t_ostring *ostring = ft_ostr_new_with_capacity(slen);
	if (NULL == ostring) {
		return (NULL);
	}
	ft_memcpy(ostring->content, cstr, slen);
	ostring->size = slen;

	return (ostring);
}

void 	ft_ostr_init(t_ostring *ostring)
{
	if (NULL == ostring) {
		return ;
	}
	ostring->content = NULL;
	ostring->size = 0;
	ostring->capacity = 0;
}

void 	ft_ostr_init_with_capacity(t_ostring *ostring, size_t capacity)
{
	if (NULL == ostring) {
		return ;
	}
	LIBFT_ALLOC(ostring->content, capacity);
	ostring->size = 0;
	ostring->capacity = capacity;
}

void	ft_ostr_init_with_content(t_ostring *ostring, void *content, size_t size)
{
	if (NULL == ostring) {
		return;
	}
	ft_ostr_init(ostring);
	if (content) {
		ostring->content = content;
		ostring->size = size;
		ostring->capacity = size;
	}
}

void	ft_ostr_del(t_ostring *ostring)
{
	if (NULL == ostring) {
		return ;
	}
	LIBFT_FREE(ostring->content);
	LIBFT_FREE(ostring);
}

void	ft_ostr_clear(t_ostring *ostring)
{
	if (NULL == ostring) {
		return ;
	}
	LIBFT_FREE(ostring->content);
	ostring->content = NULL;
	ostring->size = 0;
	ostring->capacity = 0;
}

t_ostring	*ft_ostr_dup(const t_ostring *ostring)
{
	t_ostring *dup = ft_ostr_new_with_capacity(ostring->size);
	if (NULL == dup) {
		return (NULL);
	}
	ft_memcpy(dup->content, ostring->content, ostring->size);
	dup->size = ostring->size;
	dup->capacity = dup->size;

	return (dup);
}

t_ostring	*ft_ostr_appendf(t_ostring *ostring, const char *fmt, ...)
{
	char *cstr;
	va_list	ap;

	if (NULL == ostring) {
		return (NULL);
	}
	cstr = NULL;
	va_start(ap, fmt);
	ft_vsprintf(&cstr, fmt, ap);
	va_end(ap);

	if (NULL == ft_ostr_append_cstr(ostring, cstr)) {
		LIBFT_FREE(cstr);
		return (NULL);
	}
	LIBFT_FREE(cstr);

	return (ostring);
}

t_ostring	*ft_ostr_append_cstr(t_ostring *ostring, const char *cstr)
{
	if (NULL == ostring) {
		return (NULL);
	}
	return (ft_ostr_append(ostring, (void *)cstr, ft_strlen(cstr)));
}

t_ostring	*ft_ostr_append_ostr(t_ostring *ostring, const t_ostring *ostr)
{
	if (NULL == ostring || NULL == ostr) {
		return (NULL);
	}
	return (ft_ostr_append(ostring, (void *)ostr->content, ostr->size));
}

t_ostring	*ft_ostr_append(t_ostring *ostring, void *content, size_t size)
{
	if (NULL == ostring) {
		return (NULL);
	}
	if (NULL != content) {
		if (NULL == ostring->content) {
			ft_ostr_init_with_capacity(ostring, size);
		}
		if (ostring->size + size > ostring->capacity) {
			LIBFT_REALLOC(ostring->content, ostring->capacity, ostring->capacity + size);
			if (NULL == ostring->content) {
				return (NULL);
			}
			ostring->capacity += size;
		}
		ft_memcpy(ostring->content + ostring->size, content, size);
		ostring->size += size;
	}

	return (ostring);
}

t_ostring	*ft_ostr_prepend(t_ostring *ostring, void *content, size_t size)
{
	if (NULL == ostring) {
		return (NULL);
	}
	if (NULL != content) {
		if (NULL == ostring->content) {
			ft_ostr_init_with_capacity(ostring, size);
		}
		if (ostring->size + size > ostring->capacity) {
			void *new_content = NULL;
			LIBFT_ALLOC(new_content, ostring->capacity + size);
			if (NULL == new_content) {
				return (NULL);
			}
			ft_memcpy(new_content, content, size);
			ft_memcpy(new_content + size, ostring->content, ostring->size);
			ostring->content = new_content;
			ostring->size += size;
			ostring->capacity += size;
		}
		else {
			ft_memcpy(ostring->content + size, ostring->content, ostring->size);
			ft_memcpy(ostring->content, content, size);
			ostring->size += size;
		}
	}
	return (ostring);
}

char 	*ft_ostr_to_cstr(const t_ostring *ostring, size_t idx, size_t len)
{
	char *str;

	if (NULL == ostring) {
		return (NULL);
	}
	if (idx >= ostring->size) {
		return (NULL);
	}
	len = MIN(len, ostring->size - idx);

	LIBFT_ALLOC(str, len + 1);
	if (NULL == str) {
		return (NULL);
	}
	ft_memcpy(str, ostring->content + idx, len);
	str[len] = '\0';

	return (str);
}

/****************************************************************************/

static void	__l_arg_zero(t_pf *l);
static t_pf	*__l_arg_new(int fd);

static void __parse(t_pf *data, const char *s, va_list *arg);
static void __format(t_pf *data, va_list *arg);
static void	__get_data(t_pf *data, const char *s, int *i);
static void	__oper(t_pf *data, char *s, size_t *i, char op);

static void __flags(char *len, char *flag, char *type);
static int	__is_flag(char c, char *flag);
static int	__is_len_flag(char c, char next, char *len, int *i);
static void	__format_di(t_pf *data, char len, va_list *arg);
static void	__format_oux(t_pf *data, char len, va_list *arg);
static void	__format_c(t_pf *data, char len, va_list *arg);
static void	__format_s(t_pf *data, char len, va_list *arg);

static void	__out(t_pf *data, const char *s, int size);

static void	__print_c(t_pf *data, char *src, size_t size);
static void	__print_di(t_pf *data, intmax_t n, char flag, size_t i);
static void	__print_s(t_pf *data, char *src, size_t i);
static void	__print_wc(t_pf *data, unsigned int c);
static void	__print_ws(t_pf *data, unsigned int *src);
static void	__print_oux(t_pf *data, uintmax_t n, char flag, size_t i);
static void	__print_else(t_pf *data, char flag, size_t i);

static void	__di_to_s(t_pf *data, char *s, intmax_t n, size_t *i);
static void	__di_str_size(t_pf *data, intmax_t n, size_t size);
static void	__di_size(t_pf *data, intmax_t n);

static void	__oux_to_s(t_pf *data, char *s, uintmax_t n, size_t *i);
static void	__oux_size(t_pf *data, uintmax_t n);
static void	__oux_str_size(t_pf *data, uintmax_t n, size_t size, char type);

static int	__ws_len(unsigned int *s);

static void	__utf_8_size(unsigned int *ws, int size, int *chars);
static void	__encode_utf_8(unsigned int *ws, char *s, int chars, int j);
static char	*__convert_to_utf_8(unsigned int *ws, int size, int prec);

int ft_printf(const char *format, ...)
{
	int num;
	va_list arg;
	t_pf *data;

	num = 0;
	va_start(arg, format);

	if (NULL == (data = __l_arg_new(STDOUT_FILENO))) {
		return (-1);
	}
	if (format) {
		__parse(data, format, &arg);
	}
	va_end(arg);
	num = data->num;
	free(data);

	return (num);
}

int ft_vprintf(const char *format, va_list arg)
{
	int num;
	t_pf *data;

	num = 0;

	if (NULL == (data = __l_arg_new(STDOUT_FILENO))) {
		return (-1);
	}
	if (format) {
		__parse(data, format, &arg);
	}
	va_end(arg);
	num = data->num;
	free(data);

	return (num);
}

int ft_fprintf(int fd, const char *format, ...)
{
	int num;
	va_list arg;
	t_pf *data;

	num = 0;
	va_start(arg, format);

	if ((fd < 0) || (NULL == (data = __l_arg_new(fd)))) {
		return (-1);
	}
	if (format) {
		__parse(data, format, &arg);
	}
	va_end(arg);
	num = data->num;
	free(data);

	return (num);
}

int ft_vfprintf(int fd, const char *format, va_list arg)
{
	int num;
	t_pf *data;

	num = 0;

	if ((fd < 0) || (NULL == (data = __l_arg_new(fd)))) {
		return (-1);
	}
	if (format) {
		__parse(data, format, &arg);
	}
	num = data->num;
	free(data);

	return (num);
}

int ft_sprintf(char **buf, const char *format, ...)
{
	int num;
	va_list arg;
	t_pf *data;

	num = 0;
	va_start(arg, format);

	if (NULL == buf) {
		return (-1);
	}
	*buf = NULL;
	data = __l_arg_new(-1);
	if (NULL == data) {
		return (-1);
	}
	data->buf = buf;

	if (format) {
		__parse(data, format, &arg);
	}
	va_end(arg);
	num = data->num;
	free(data);

	return (num);
}

int ft_snprintf(char *buf, size_t size, const char *format, ...)
{
	int num;
	va_list arg;
	t_pf *data;
	char *tmp_buf;

	va_start(arg, format);

	if (NULL == buf || size < 1) {
		return (-1);
	}
	data = __l_arg_new(-1);
	if (NULL == data) {
		return (-1);
	}
	tmp_buf = NULL;
	data->buf = &tmp_buf;

	if (format) {
		__parse(data, format, &arg);
	}
	va_end(arg);

	ft_strncpy(buf, tmp_buf, size-1);
	num = (int)ft_strlen(buf);

	if (tmp_buf) {
		free(tmp_buf);
	}
	free(data);

	return (num);
}

int ft_vsprintf(char **buf, const char *format, va_list arg)
{
	int num;
	t_pf *data;

	num = 0;

	if (NULL == buf) {
		return (-1);
	}
	*buf = NULL;
	data = __l_arg_new(-1);
	if (NULL == data) {
		return (-1);
	}
	data->buf = buf;

	if (format) {
		__parse(data, format, &arg);
	}
	num = data->num;
	free(data);

	return (num);
}

static void	__l_arg_zero(t_pf *l)
{
	if (l) {
		l->flag = 0;
		l->len = 0;
		l->fwid = 0;
		l->prec = -1;
		l->type = -1;
		l->size = 0;
		l->str_size = 0;
		l->space = 0;
		l->base = 10;
	}
}

static t_pf	*__l_arg_new(int fd)
{
	t_pf	*new;

	if (NULL == (new = malloc(sizeof(t_pf)))) {
		return (NULL);
	}
	ft_bzero(new, sizeof(t_pf));

	new->fd = fd;
	new->prec = -1;
	new->type = -1;
	new->base = 10;

	return (new);
}

static void __flags(char *len, char *flag, char *type)
{
	if (ft_strchr("DOUCS", *type) && (*len = PF_L)) {
		*type += 32;
	}
	if (!(*type == 'x' || *type == 'X' || *type == 'o')) {
		*flag &= (~PF_SHARP);
	}
	if (*type == 'p') {
		*flag = (*flag & (~PF_SPACE) & (~PF_PLUS) & (~PF_PREC)) | PF_SHARP;
		*len = PF_L;
	}
	if (ft_strchr("diouxXp", *type) && (*flag & PF_MINUS || *flag & PF_PREC)) {
		*flag &= (~PF_ZERO);
	}
	if (!(*type == 'd' || *type == 'i')) {
		*flag &= (~PF_SPACE);
		*flag &= (~PF_PLUS);
	}
	if (*flag & PF_PLUS) {
		*flag &= (~PF_SPACE);
	}
}

static void __format(t_pf *data, va_list *arg)
{
	__flags(&(data->len), &(data->flag), &(data->type));

	if (data->type == 'x' || data->type == 'X' || data->type == 'p') {
		data->base = 16;
	} else if (data->type == 'o') {
		data->base = 8;
	}
	if (data->type == '%') {
		__out(data, "%", 1);
	} else if (data->type == '@') {
		data->fd = STDERR_FILENO;
	} else if (ft_strchr("id", data->type)) {
		__format_di(data, data->len, arg);
	} else if (ft_strchr("ouxXp", data->type)) {
		__format_oux(data, data->len, arg);
	} else if ((data->type == 's') || (data->type == 'q')) {
		__format_s(data, data->len, arg);
	} else if (data->type == 'c') {
		__format_c(data, data->len, arg);
	} else {
		__print_else(data, data->flag, 0);
	}
}

static void	__parse(t_pf *data, const char *s, va_list *arg)
{
	int	i;
	int	start;

	start = 0;

	i = 0;
	while (s[i]) {
		start = i;

		while (s[i] && s[i] != '%')
			i++;

		if (i > start) {
			__out(data, s + start, i - start);
		}

		if (s[i] == '%' && s[++i]) {
			if (s[i] == '%' || s[i] == '@') {
				data->type = s[i];
			} else {
				__get_data(data, s, &i);
			}
			__format(data, arg);
		}

		__l_arg_zero(data);

		if (s[i]) {
			i++;
		}
	}
}

static int	__is_flag(char c, char *flag)
{
	char	check;

	check = *flag;

	if (ft_strchr("#0 -+'", c)) {
		(c == '#') ? (check |= PF_SHARP) : 0;
		(c == '0') ? (check |= PF_ZERO) : 0;
		(c == '-') ? (check |= PF_MINUS) : 0;
		(c == '+') ? (check |= PF_PLUS) : 0;
		(c == 32) ? (check |= PF_SPACE) : 0;
		(c == 39) ? (check |= PF_APOS) : 0;
		*flag |= check;

		return (1);
	}
	return (0);
}

static int	__is_len_flag(char c, char next, char *len, int *i)
{
	char	check;

	check = 0;

	if (!ft_strchr("hljz", c)) {
		return (0);
	}
	if (c == 'h' && next == 'h' && (*i += 1)) {
		check |= PF_HH;
	} else if (c == 'h') {
		check |= PF_H;
	}
	if (c == 'l' && next == 'l' && (*i += 1)) {
		check |= PF_LL;
	} else if (c == 'l') {
		check |= PF_L;
	}
	(c == 'j') ? check |= PF_J : 0;
	(c == 'z') ? check |= PF_Z : 0;

	if (check > *len) {
		*len = check;
	}
	return (1);
}

static void	__get_data(t_pf *data, const char *s, int *i)
{
	if (s[*i] == 0) {
		return ;
	}
	if (__is_flag(s[*i], &(data->flag))) {
		*i += 1;
	} else if (s[*i] >= '0' && s[*i] <= '9') {
		data->fwid = (int)ft_atoi(s + *i);

		while (s[*i] && s[*i] >= '0' && s[*i] <= '9') {
			*i += 1;
		}
	} else if (s[*i] == '.' && (*i += 1)) {
		data->flag |= PF_PREC;
		data->prec = (int)ft_atoi(s + *i);

		while (s[*i] && s[*i] >= '0' && s[*i] <= '9') {
			*i += 1;
		}
	} else if (s[*i] && __is_len_flag(s[*i], s[*i + 1], &(data->len), i)) {
		*i += 1;
	} else {
		data->type = s[*i];
		return ;
	}
	__get_data(data, s, i);
}

static void	__oper(t_pf *data, char *s, size_t *i, char op)
{
	size_t	j;

	j = 0;

	if (op == '+' && data->flag & PF_PLUS) {
		s[*i + j++] = '+';
	} else if (op == '+' && data->flag & PF_SPACE) {
		s[*i + j++] = ' ';
	} else if (op == '-') {
		s[*i + j++] = '-';
	} else if (op == '0' && data->space > 0) {
		while ((int)j < data->space) {
			s[*i + j++] = '0';
		}
	} else if (op == ' ' && data->space > 0) {
		while ((int)j < data->space) {
			s[*i + j++] = ' ';
		}
	} else if (op == 'p' && data->prec > (int)data->size) {
		while ((int)j < data->prec - (int)data->size) {
			s[*i + j++] = '0';
		}
	} else if (op == 'x' && ft_strchr("xXp", data->type) && (j = 2)) {
		ft_strcpy(s + *i, (data->type == 'X' ? "0X" : "0x"));
	}
	*i += j;
}

static void	__format_di(t_pf *data, char len, va_list *arg)
{
	intmax_t	val;

	val = 0;

	if (!(data->flag & PF_PREC)) {
		data->prec = 1;
	}

	if (!len) {
		val = (int)va_arg(*arg, int);
	}
	else if (len & PF_HH)
		val = (signed char)va_arg(*arg, int);
	else if (len & PF_H)
		val = (short)va_arg(*arg, int);
	else if (len & PF_LL)
		val = (long long)va_arg(*arg, long long);
	else if (len & PF_L)
		val = (long)va_arg(*arg, long);
	else if (len & PF_J)
		val = va_arg(*arg, intmax_t);
	else if (len & PF_Z)
		val = (size_t)va_arg(*arg, size_t);

	__print_di(data, val, data->flag, 0);
}

static void	__format_oux(t_pf *data, char len, va_list *arg)
{
	uintmax_t	val;

	val = 0;

	if (!len) {
		val = (unsigned int)va_arg(*arg, unsigned int);
	}
	else if (len & PF_HH)
		val = (unsigned char)va_arg(*arg, unsigned int);
	else if (len & PF_H)
		val = (unsigned short)va_arg(*arg, unsigned int);
	else if (len & PF_LL)
		val = (unsigned long long)va_arg(*arg, unsigned long long);
	else if (len & PF_L)
		val = (unsigned long)va_arg(*arg, unsigned long);
	else if (len & PF_J)
		val = va_arg(*arg, uintmax_t);
	else if (len & PF_Z)
		val = (size_t)va_arg(*arg, size_t);

	__print_oux(data, val, data->flag, 0);
}

static void	__format_c(t_pf *data, char len, va_list *arg)
{
	unsigned int	wc;
	char			c;

	wc = 0;
	c = 0;

	if (len & PF_L) {
		wc = va_arg(*arg, unsigned int);
		__print_wc(data, wc);
	} else {
		c = (char)va_arg(*arg, int);
		__print_c(data, &c, 1);
	}
}

static void	__format_s(t_pf *data, char len, va_list *arg)
{
	unsigned int	*ws;
	char			*s;

	s = NULL;
	ws = NULL;

	if (len & PF_L) {
		ws = va_arg(*arg, unsigned int *);
	}
	else
		s = va_arg(*arg, char *);

	if (!s && !ws && !(data->flag & PF_PREC)) {
		if (!(data->flag & PF_PREC)) {
			__out(data, "(null)", 6);
		}
		return ;
	}
	if (len & PF_L) {
		__print_ws(data, ws);
	}
	else
		__print_s(data, s, 0);
}

static void	__out(t_pf *data, const char *s, int size)
{
	char	*ptr;

	if (data->buf) {
		ptr = malloc(data->num+size+1);

		if (ENOMEM == errno) {
			perror("error :");
			return ;
		}
		if (NULL != *(data->buf)) {
			ft_memcpy(ptr, *(data->buf), data->num);
			free(*(data->buf));
		}
		ft_memcpy(ptr + data->num, s, size);
		ptr[data->num+size] = '\0';

		*(data->buf) = ptr;
		data->num += size;

	} else if (data->fd >= 0) {
		write(data->fd, s, size);
		data->num += size;
	}
}

static void	__print_c(t_pf *data, char *src, size_t size)
{
	char	*s;
	size_t	i;

	i = 0;
	data->str_size = size;
	data->space = data->fwid - size;

	if (data->fwid > (int)data->str_size) {
		data->str_size = (size_t)data->fwid;
	}
	if (!(s = malloc(data->str_size + 1))) {
		return ;
	}
	if (!(data->flag & PF_MINUS)) {
		__oper(data, s, &i, (data->flag & PF_ZERO) ? ('0') : (' '));
	}
	if (src) {
		ft_memcpy(s + i, src, size);
		i += size;
	}
	if (data->flag & PF_MINUS) {
		__oper(data, s, &i, (' '));
	}
	s[i] = 0;
	__out(data, s, i);
	free(s);
}

static void	__di_to_s(t_pf *data, char *s, intmax_t n, size_t *i)
{
	int	j;

	j = data->size;

	while (--j >= 0) {
		if (n < 0) {
			s[*i + j] = -1 * (n % data->base) + 48;
		} else {
			s[*i + j] = n % data->base + 48;
		}
		n = n / data->base;
	}
	*i += data->size;
}

static void	__di_str_size(t_pf *data, intmax_t n, size_t size)
{
	if ((int)size > data->prec) {
		data->str_size += size;
	} else {
		data->str_size += (size_t)data->prec;
	}
	if (n < 0 || data->flag & PF_SPACE || data->flag & PF_PLUS) {
		data->str_size += 1;
	}
	data->space = data->fwid - (int)(data->str_size);

	if (data->str_size < (size_t)data->fwid) {
		data->str_size = (size_t)data->fwid;
	}
}

static void	__di_size(t_pf *data, intmax_t n)
{
	if (n == 0 && data->prec == 0) {
		data->size = 0;
	} else if (n == 0) {
		data->size = 1;
	}
	while (n) {
		data->size += 1;
		n = n / (intmax_t)(data->base);
	}
}

static void	__print_di(t_pf *data, intmax_t n, char flag, size_t i)
{
	char	*s;

	__di_size(data, n);
	__di_str_size(data, n, data->size);

	if (!(s = malloc(data->str_size + 1))) {
		return ;
	}
	if (flag & PF_ZERO) {
		__oper(data, s, &i, (n < 0) ? ('-') : ('+'));
	}
	if (!(flag & PF_MINUS)) {
		__oper(data, s, &i, (flag & PF_ZERO) ? ('0') : (' '));
	}
	if (!(flag & PF_ZERO)) {
		__oper(data, s, &i, (n < 0) ? ('-') : ('+'));
	}
	__oper(data, s, &i, ('p'));
	__di_to_s(data, s, n, &i);

	if (flag & PF_MINUS) {
		__oper(data, s, &i, (' '));
	}
	s[i] = 0;
	__out(data, s, i);
	free(s);
}

static void	__oux_to_s(t_pf *data, char *s, uintmax_t n, size_t *i)
{
	uintmax_t	val;
	int			j;

	val = 0;
	j = data->size;

	while (--j >= 0) {
		val = n % (uintmax_t)(data->base);

		if (val <= 9) {
			s[*i + j] = val + 48;
		} else if (val >= 10 && val <= 15) {
			s[*i + j] = val + ((data->type == 'X') ? 55 : 87);
		}
		n = n / (uintmax_t)(data->base);
	}
	*i += data->size;
}

static void	__oux_str_size(t_pf *data, uintmax_t n, size_t size, char type)
{
	if (data->flag & PF_SHARP && type == 'o' && data->prec <= (int)size) {
		data->prec = size + 1;
	} else if (data->flag & PF_SHARP && ft_strchr("xX", type) && n) {
		data->str_size += 2;
	} else if (type == 'p') {
		data->str_size += 2;
	}
	if ((int)size > data->prec) {
		data->str_size += size;
	} else {
		data->str_size += (size_t)data->prec;
	}
	data->space = data->fwid - (int)(data->str_size);

	if (data->str_size < (size_t)data->fwid) {
		data->str_size = (size_t)data->fwid;
	}
}

static void	__oux_size(t_pf *data, uintmax_t n)
{
	if (n == 0 && data->prec == 0) {
		data->size = 0;
	} else if (n == 0) {
		data->size = 1;
	}
	if (data->type == 'o' && n == 0 && !(data->flag & PF_PREC)) {
		data->flag &= (~PF_SHARP);
	}
	while (n) {
		data->size += 1;
		n = n / (uintmax_t)(data->base);
	}
}

static void	__print_oux(t_pf *data, uintmax_t n, char flag, size_t i)
{
	char	*s;

	__oux_size(data, n);
	__oux_str_size(data, n, data->size, data->type);

	if (!(s = malloc(data->str_size + 1))) {
		return ;
	}
	if (flag & PF_ZERO && ((n != 0 && flag & PF_SHARP) || data->type == 'p')) {
		__oper(data, s, &i, ('x'));
	}
	if (!(flag & PF_MINUS)) {
		__oper(data, s, &i, (flag & PF_ZERO) ? ('0') : (' '));
	}
	if (!(flag & PF_ZERO) && ((n != 0 && flag & PF_SHARP) || data->type == 'p')) {
		__oper(data, s, &i, ('x'));
	}
	__oper(data, s, &i, ('p'));
	__oux_to_s(data, s, n, &i);

	if (flag & PF_MINUS) {
		__oper(data, s, &i, (' '));
	}
	s[i] = 0;
	__out(data, s, i);
	free(s);
}

static void	__print_s(t_pf *data, char *src, size_t i)
{
	char	*temp;
	char	*s;

	data->size = ft_strlen(src);

	if (data->prec < (int)data->size && data->flag & PF_PREC) {
		data->size = data->prec;
	}
	data->str_size = data->size;
	data->space = data->fwid - (int)data->str_size;

	if (data->fwid > (int)data->str_size) {
		data->str_size = data->fwid;
	}
	if (!(s = malloc(data->str_size + 1))) {
		return ;
	}
	if (!(data->flag & PF_MINUS)) {
		__oper(data, s, &i, (data->flag & PF_ZERO) ? ('0') : (' '));
	}
	if (src) {
		ft_memzcpy(s + i, src, data->str_size - i, data->size);

		if (data->type == 'q') {
			// 'q' means uppercase format
			temp = ft_strnup(s + i, data->size);
			ft_strcpy(s + i, temp);
			free(temp);
		}
		i += data->size;
	}
	if (data->flag & PF_MINUS) {
		__oper(data, s, &i, (' '));
	}
	s[i] = 0;
	__out(data, s, i);
	free(s);
}

static void	__print_wc(t_pf *data, unsigned int c)
{
	char	*s;

	s = __convert_to_utf_8(&c, 1, data->prec);
	__print_c(data, s, c ? ft_strlen(s) : 1);

	if (s) {
		free(s);
	}
}

static int	__ws_len(unsigned int *s)
{
	int len;

	len = 0;
	while (s && s[len])
		len++;

	return (len);
}

static void	__print_ws(t_pf *data, unsigned int *src)
{
	char *s;

	data->size = __ws_len(src);

	if (data->prec < (int)data->size && data->flag & PF_PREC) {
		data->size = data->prec;
	}

	s = __convert_to_utf_8(src, data->size, data->prec);
	__print_s(data, s, 0);

	if (s) {
		free(s);
	}
}

static void	__utf_8_size(unsigned int *ws, int size, int *chars)
{
	int	i;

	i = 0;
	while (ws && i < size) {
		if (ws[i] < 0x80) {
			*chars += 1;
		} else if (ws[i] < 0x800) {
			*chars += 2;
		} else if (ws[i] < 0x10000) {
			*chars += 3;
		} else if (ws[i] <= 0x10FFFF) {
			*chars += 4;
		}
		i++;
	}
}

static void	__encode_utf_8(unsigned int *ws, char *s, int chars, int j)
{
	int	tmp;

	tmp = 0;
	while (j < chars) {
		tmp = j;

		if (*ws < 0x80) {
			s[j++] = (char)(*ws);
		} else if (*ws < 0x800) {
			s[j++] = (char)(((*ws >> 6) & 0x3F) | 0xC0);
		} else if (*ws < 0x10000) {
			s[j++] = (char)(((*ws >> 12) & 0x3F) | 0xE0);
		} else if (*ws <= 0x10FFFF) {
			s[j++] = (char)(((*ws >> 18) & 0x3F) | 0xF0);
			s[j++] = (char)(((*ws >> 12) & 0x3F) | 0x80);
		}
		if (*ws >= 0x800) {
			s[j++] = (char)(((*ws >> 6) & 0x3F) | 0x80);
		}
		if (*ws >= 0x80) {
			s[j++] = (char)((*ws & 0x3F) | 0x80);
		}
		ws++;
	}
	s[(j == chars) ? j : tmp] = 0;
}

static char	*__convert_to_utf_8(unsigned int *ws, int size, int prec)
{
	int		chars;
	char	*s;

	chars = 0;

	if (ws) {
		__utf_8_size(ws, size, &chars);
	}
	if (prec > 0 && prec < chars) {
		chars = prec;
	}
	if (chars && (s = malloc(chars + 1))) {
		__encode_utf_8(ws, s, chars, 0);
		return (s);
	}
	return (NULL);
}

static void	__print_else(t_pf *data, char flag, size_t i)
{
	char	*s;

	if (data->fwid > 1) {
		data->str_size = data->fwid;
	} else {
		data->str_size = 1;
	}
	data->space = data->fwid - 1;

	if (!(s = malloc(data->str_size + 1))) {
		return ;
	}
	if (!(flag & PF_MINUS)) {
		__oper(data, s, &i, (flag & PF_ZERO) ? ('0') : (' '));
	}
	s[i++] = data->type;

	if (flag & PF_MINUS) {
		__oper(data, s, &i, (' '));
	}
	s[i] = 0;
	__out(data, s, i);
	free(s);
}

/****************************************************************************/

t_queue	*ft_queue_create(void)
{
	t_queue	*new;

	LIBFT_ALLOC(new, sizeof(t_queue));

	return (new);
}

void	ft_queue_enqueue(t_queue *queue, const char *key, void *content, size_t size)
{
	t_node	*node;

	if (NULL == queue) {
		return ;
	}
	node = ft_node_new(key, content, size);

	if (NULL == queue->last) {
		queue->last = node;
		queue->first = node;
	} else {
		queue->last->next = node;
		queue->last = node;
	}
}

void	ft_queue_enqueue_node(t_queue *queue, t_node *node)
{
	if (NULL == queue) {
		return ;
	}
	if (NULL == queue->last) {
		queue->last = node;
		queue->first = node;
	} else {
		queue->last->next = node;
		queue->last = node;
	}
}

void	*ft_queue_dequeue(t_queue *queue)
{
	t_node	*node;
	void	*content;

	if ((NULL == queue) || (NULL == queue->first)) {
		return (NULL);
	}
	node = queue->first;

	if (queue->first == queue->last) {
		queue->first = NULL;
		queue->last = NULL;
	} else {
		queue->first = queue->first->next;
	}
	content = node->content;
	LIBFT_FREE(node->key);
	LIBFT_FREE(node);

	return (content);
}

t_node	*ft_queue_dequeue_node(t_queue *queue)
{
	t_node	*node;

	if ((NULL == queue) || (NULL == queue->first)) {
		return (NULL);
	}
	node = queue->first;

	if (queue->first == queue->last) {
		queue->first = NULL;
		queue->last = NULL;
	} else {
		queue->first = queue->first->next;
	}

	return (node);
}

t_node	*ft_queue_first(t_queue *queue)
{
	if (NULL == queue) {
		return (NULL);
	}
	return (queue->first);
}

t_node	*ft_queue_last(t_queue *queue)
{
	if (NULL == queue) {
		return (NULL);
	}
	return (queue->last);
}

int		ft_queue_is_empty(t_queue *queue)
{
	if ((NULL == queue) || (NULL == queue->first)) {
		return (1);
	}
	return (0);
}

int		ft_queue_size(t_queue *queue)
{
	if ((NULL == queue) || (NULL == queue->first)) {
		return (0);
	}
	return (ft_lst_size(queue->first));
}

void	ft_queue_del(t_queue *queue)
{
	if (NULL == queue) {
		return ;
	}
	ft_lst_del(ft_queue_first(queue));
	LIBFT_FREE(queue);
}

void	ft_queue_del_with_f_del(t_queue *queue, t_func_content_del f_del)
{
	if (NULL == queue) {
		return ;
	}
	ft_lst_del_with_f_del(ft_queue_first(queue), f_del);
	LIBFT_FREE(queue);
}

void	ft_queue_del_node(t_queue *queue, t_node *node)
{
	t_node *cur;
	t_node *prev;

	if (NULL == queue || NULL == queue->first || NULL == node) {
		return ;
	}
	prev = NULL;
	cur = queue->first;

	while ((NULL != cur) && (node != cur)) {
		prev = cur;
		cur = cur->next;
	}
	if (NULL == cur) {
		return ;
	}
	if (NULL == prev) {
		queue->first = cur->next;
	} else {
		prev->next = cur->next;
	}
	if (cur == queue->last) {
		queue->last = prev;
	}
	ft_node_del(cur);
}

void	ft_queue_del_node_with_f_del(t_queue *queue, t_node *node, t_func_content_del f_del)
{
	t_node *cur;
	t_node *prev;

	if (NULL == queue || NULL == queue->first || NULL == node) {
		return ;
	}
	prev = NULL;
	cur = queue->first;

	while ((NULL != cur) && (node != cur)) {
		prev = cur;
		cur = cur->next;
	}
	if (NULL == cur) {
		return ;
	}
	if (NULL == prev) {
		queue->first = cur->next;
	} else {
		prev->next = cur->next;
	}
	if (cur == queue->last) {
		queue->last = prev;
	}
	ft_node_del_with_f_del(cur, f_del);
}

t_htbl	*ft_queue_to_htable(t_queue *queue)
{
	if (NULL == queue) {
		return (NULL);
	}
	return (ft_lst_to_htable(ft_queue_first(queue)));
}

t_node	*ft_queue_pop_list(t_queue *queue)
{
	t_node	*node;

	node = ft_queue_first(queue);
	queue->first = NULL;
	queue->last = NULL;

	return (node);
}

/****************************************************************************/

t_stack	*ft_stack_create(void)
{
	t_stack	*stack;

	LIBFT_ALLOC(stack, sizeof(t_stack));

	return (stack);
}

void	*ft_stack_pop(t_stack *stack)
{
	t_node	*node;
	void	*content;

	if ((NULL == stack) || (NULL == stack->top)) {
		return (NULL);
	}
	node = stack->top;
	content = node->content;

	stack->top = stack->top->next;

	LIBFT_FREE(node->key);
	LIBFT_FREE(node);

	return (content);
}

t_node	*ft_stack_pop_node(t_stack *stack)
{
	t_node	*node;

	if ((NULL == stack) || (NULL == stack->top)) {
		return (NULL);
	}
	node = stack->top;
	stack->top = stack->top->next;

	return (node);
}

void	ft_stack_push(t_stack *stack, const char *key, void *content, size_t size)
{
	if (NULL == stack) {
		return ;
	}
	ft_lst_prepend(&stack->top, ft_node_new(key, content, size));
}

void	ft_stack_push_node(t_stack *stack, t_node *node)
{
	if (NULL == stack) {
		return ;
	}
	ft_lst_prepend(&stack->top, node);
}

t_node	*ft_stack_peek(t_stack *stack)
{
	if (NULL == stack) {
		return (NULL);
	}
	return (stack->top);
}

int		ft_stack_is_empty(t_stack *stack)
{
	if ((NULL == stack) || (NULL == stack->top)) {
		return (1);
	}
	return (0);
}

int		ft_stack_size(t_stack *stack)
{
	if ((NULL == stack) || (NULL == stack->top)) {
		return (0);
	}
	return (ft_lst_size(stack->top));
}

void	ft_stack_del(t_stack *stack)
{
	if (NULL == stack) {
		return ;
	}
	ft_lst_del(stack->top);
	LIBFT_FREE(stack);
}

void	ft_stack_del_with_f_del(t_stack *stack, t_func_content_del f_del)
{
	if (NULL == stack) {
		return ;
	}
	ft_lst_del_with_f_del(stack->top, f_del);
	LIBFT_FREE(stack);
}

t_htbl	*ft_stack_to_htable(t_stack *stack)
{
	if ((NULL == stack) || (NULL == stack->top)) {
		return (NULL);
	}
	return (ft_lst_to_htable(stack->top));
}

t_node	*ft_stack_pop_list(t_stack *stack)
{
	t_node	*node;

	node = ft_stack_peek(stack);
	stack->top = NULL;

	return (node);
}

/****************************************************************************/

static t_tuple	*__create_tuple(void);

t_tuple *ft_tuple_new(void *head, size_t head_size, void *tail, size_t tail_size)
{
	t_tuple *tuple;

	tuple = __create_tuple();
	tuple->__head = ft_node_new(NULL, head, head_size);
	tuple->__tail = ft_node_new(NULL, tail, tail_size);
	return (tuple);
}

t_tuple *ft_tuple_new_with_f_del(void *head, size_t head_size, void *tail, size_t tail_size, t_func_content_del f_del)
{
	t_tuple *tuple;

	tuple = __create_tuple();
	tuple->__head = ft_node_new_with_f_del(NULL, head, head_size, f_del);
	tuple->__tail = ft_node_new_with_f_del(NULL, tail, tail_size, f_del);
	return (tuple);
}

void	ft_tuple_del(t_tuple *tuple)
{
	if (NULL == tuple) {
		return ;
	}
	ft_node_del(tuple->__head);
	ft_node_del(tuple->__tail);
	LIBFT_FREE(tuple);
}

void 	ft_tuple_del_with_f_del(t_tuple *tuple, t_func_content_del f_del)
{
	if (NULL == tuple) {
		return ;
	}
	ft_node_del_with_f_del(tuple->__head, f_del);
	ft_node_del_with_f_del(tuple->__tail, f_del);
	LIBFT_FREE(tuple);
}

void 	*ft_tuple_get(t_tuple *tuple, int index)
{
	if (NULL == tuple) {
		return (NULL);
	}
	if (index == 0) {
		if (NULL == tuple->__head) {
			return (NULL);
		}
		return (tuple->__head->content);
	}
	if (NULL == tuple->__tail) {
		return (NULL);
	}
	return (tuple->__tail->content);
}

static t_tuple	*__create_tuple(void)
{
	t_tuple *tuple;

	LIBFT_ALLOC(tuple, sizeof(t_tuple));
	ft_bzero(tuple, sizeof(t_tuple));
	return (tuple);
}

/****************************************************************************/

int		ft_uint_lmbit(uint64_t num, int bits)
{
	bits = MIN(bits, 8 * sizeof(num));

	for (int i = bits; i > 0; i--) {
		if ((((uint64_t)1)<<(i-1)) & num) {
			return (i);
		}
	}
	return (0);
}

int		ft_uint_rmbit(uint64_t num, int bits)
{
	for (int i = 1; i <= bits; i++) {
		if ((1<<(i-1)) & num) {
			return (i);
		}
	}
	return (0);
}

void	ft_uint_print_bits(uint64_t num, int numbits, int bit_split)
{
	int	j;

	j = 0;
	while (j < numbits) {
		if ((num << j) & ((uint64_t)1 << (numbits - 1))) {
			write(1, "1", 1);
		}
		else
			write(1, "0", 1);
		j++;
		if (j % bit_split == 0) {
			write(1, " ", 1);
		}
	}
	write(1, "\n", 1);
}

void	ft_uint_to_bytes(void *bytes, uint64_t uint, int uint_size)
{
	uint8_t	*buf;
	int		idx;
	int		shift;

	if (!bytes) {
		return ;
	}
	buf = (unsigned char *)bytes;
	idx = 0;
	while (idx < uint_size) {
		shift = (uint_size - idx - 1) * 8;
		buf[idx] = (unsigned char)((uint >> shift) & 0xff);
		idx++;
	}
}

static int __log_writer(const char *mes) {
	return (ft_printf("%@%s\n", mes));
}

static t_logger __logger = {
	.log_writer = __log_writer,
	.log_level_thres = LIBFT_LOG_LEVEL_INFO,
	.debug_info_thres = LIBFT_LOG_LEVEL_DEBUG,
	.is_ansi_colored = 1,
};

t_logger *ft_get_logger(void)
{
	return (&__logger);
}
