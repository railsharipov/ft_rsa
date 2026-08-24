#include <textutil.h>
#include <logger.h>
#include <libft.h>

typedef int (*__t_search_func)(int);

static int __del_with_f_search(const char *in, size_t inlen, char **out, size_t *outlen, __t_search_func f)
{
	char	*res;
	size_t   ix, iy;

	if (NULL == in || NULL == out || NULL == outlen) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	SSL_ALLOC(res, inlen + 1);
	ix = 0;
	iy = 0;
	while (ix < inlen) {
		if (!f(in[ix])) {
			res[iy] = in[ix];
			iy++;
		}
		ix++;
	}
	res[iy] = '\0';
	*out = res;
	*outlen = iy;

	return (SSL_OK);
}

int textutil_del_blank(const char *in, size_t inlen, char **out, size_t *outlen)
{
	return (__del_with_f_search(in, inlen, out, outlen, ft_isblank));
}

int textutil_del_eol(const char *in, size_t inlen, char **out, size_t *outlen)
{
	return (__del_with_f_search(in, inlen, out, outlen, ft_iseol));
}

int textutil_del_eolws(const char *in, size_t inlen, char **out, size_t *outlen)
{
	return (__del_with_f_search(in, inlen, out, outlen, ft_iseolws));
}

int textutil_del_wspace(const char *in, size_t inlen, char **out, size_t *outlen)
{
	return (__del_with_f_search(in, inlen, out, outlen, ft_iswspace));
}

static const int LF = 0xA;

// Delete empty lines in character array

int textutil_del_empty_lines(const char *in, size_t inlen, char **out, size_t *outlen)
{
	char	*res;
  	size_t   ix, iy;

	if (NULL == in || NULL == out || NULL == outlen) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	SSL_ALLOC(res, inlen + 1);
	ix = 0;
	iy = 0;
	while (ix < inlen) {
		if (ft_iseol(in[ix])) {
			res[iy++] = LF;
			while (ix < inlen && ft_iseol(in[ix])) {
				ix++;
			}
		}
		else {
			res[iy++] = in[ix++];
		}
	}
	res[iy] = '\0';
	*out = res;
	*outlen = iy;

	return (SSL_OK);
}

int	textutil_insert_delim(const char *in, size_t inlen, char **out, size_t *outlen, int delim, int step)
{
	char	*res;
	size_t	ix, iy;

	if (NULL == in || NULL == out || NULL == outlen || inlen == 0 || step <= 0) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	SSL_ALLOC(res, inlen * 2 + 1);

	for (ix = 0, iy = 0; ix < inlen; ix++) {
		if (ix != 0 && ix % (size_t)step == 0) {
			res[iy++] = delim;
		}
		res[iy++] = in[ix];
	}
	res[iy] = '\0';

	*out = res;
	*outlen = iy;

	return (SSL_OK);
}

ssize_t textutil_find(const char *octets, size_t olen, const char *pattern, size_t patlen)
{
	unsigned int pathash, winhash;
	unsigned char *oct, *pat;
	int mod;
	size_t ix;

	if (NULL == pattern || NULL == octets) {
		return (-1);
	}
	pat = (unsigned char *)pattern;
	oct = (unsigned char *)octets;

	if (patlen > olen) {
		return (-1);
	}
	if (patlen == 0) {
		return (0);
	}
	mod = 0;
	pathash = 0;
	winhash = 0;

	for (ix = 0; ix < patlen; ix++) {
		mod = (mod != 0) ? ((mod * RK_ALPHA) % RK_PRIME) : (1);
		pathash = (pathash * RK_ALPHA + pat[ix]) % RK_PRIME;
		winhash = (winhash * RK_ALPHA + oct[ix]) % RK_PRIME;
	}
	for (ix = 0; ix < olen-patlen; ix++) {
		if (winhash == pathash) {
			if (!ft_memcmp(oct + ix, pat, patlen)) {
				return (ix);
			}
		}
		winhash = (winhash + RK_PRIME) - (oct[ix] * mod) % RK_PRIME;
		winhash = (RK_ALPHA * winhash + oct[ix+patlen]) % RK_PRIME;
	}
	if (winhash == pathash) {
		if (!ft_memcmp(oct + ix, pat, patlen)) {
			return (ix);
		}
	}
	return (-1);
}

ssize_t textutil_findf(const char *octets, size_t olen, const char *format, ...)
{
	va_list	ap;
	char	*pattern;
	size_t	patlen;
	int		idx;

	va_start(ap, format);
	patlen = ft_vsprintf(&pattern, format, ap);
	va_end(ap);

	idx = textutil_find(octets, olen, pattern, patlen);
	SSL_FREE(pattern);

	return (idx);
}

int textutil_replace(const char *in, size_t inlen, char **out, size_t *outlen, const char *pat, size_t patlen, const char *rep, size_t replen)
{
	char	*res;
	size_t	idx, end, reslen;

	if (NULL == in || NULL == out || NULL == outlen || NULL == pat || NULL == rep) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (patlen > inlen) {
		return (SSL_OK);
	}
	if ((idx = textutil_find(in, inlen, pat, patlen)) < 0) {
		return (SSL_OK);
	}
	end = idx + patlen;
	reslen = inlen - patlen + replen;
	SSL_ALLOC(res, reslen + 1);
	ft_memcpy(res, in, idx);
	ft_memcpy(res + idx, rep, replen);
	ft_memcpy(res + idx + replen, in + end, inlen - end);
	res[reslen] = '\0';

	*out = res;
	*outlen = reslen;

	return (SSL_OK);
}

ssize_t textutil_seek(const char *in, size_t inlen, const char *pat, size_t patlen)
{
	if (NULL == in || NULL == pat) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (0);
	}
	if (patlen == 0 || patlen > inlen) {
		return (0);
	}
	if (ft_strneq(in, pat, patlen)) {
		return (patlen);
	}
	return (0);
}

ssize_t textutil_seekf(const char *in, size_t inlen, const char *format, ...)
{
	va_list	ap;
	char	*pat;
	size_t	patlen;
	int		match;

	if (NULL == in || NULL == format) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (0);
	}
	va_start(ap, format);
	patlen = ft_vsprintf(&pat, format, ap);
	va_end(ap);

	match = 0;
	if (patlen > 0 && patlen <= inlen) {
		match = ft_strncmp(in, pat, patlen) == 0;
	}
	SSL_FREE(pat);

	return (match ? (ssize_t)patlen : 0);
}

ssize_t textutil_bseekf(const char *in, size_t inlen, const char *format, ...)
{
	va_list	ap;
	char	*pat;
	size_t	patlen;
	int		match;

	if (NULL == in || NULL == format) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (0);
	}
	va_start(ap, format);
	patlen = ft_vsprintf(&pat, format, ap);
	va_end(ap);

	match = 0;
	if (patlen > 0 && patlen <= inlen) {
		match = ft_strncmp(in, pat, patlen) == 0;
	}
	SSL_FREE(pat);

	return (match ? (ssize_t)patlen : 0);
}

enum e_scanf_mode
{
	SCANF_MODE_DEFAULT,
	SCANF_MODE_BUFFERED,
};

static int __sscanf(const char *in, size_t inlen, const char *format, va_list ap, enum e_scanf_mode mode);
static size_t __parse_string(const char *in, size_t inlen, size_t inpos, char **res);
static size_t __parse_string_b(const char *in, size_t inlen, size_t inpos, char *buf, size_t buf_size);
static size_t __parse_number(const char *in, size_t inlen, size_t inpos, ssize_t *res);
static size_t __parse_number_u(const char *in, size_t inlen, size_t inpos, size_t *res);
static size_t __parse_charset(const char *format, size_t fpos, char *charset);
static size_t __parse_scanset(const char *in, size_t inlen, size_t inpos, char **res, const char *charset);
static size_t __parse_scanset_b(const char *in, size_t inlen, size_t inpos, char *buf, size_t buf_size, const char *charset);

int textutil_bnscanf(const char *in, size_t inlen, const char *format, ...)
{
	va_list	ap;
	int		matches;

	SSL_LOG(TRACE, "Scanning in: mode=buffered");
	if (NULL == in || inlen == 0 || NULL == format) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (0);
	}

	va_start(ap, format);
	matches = __sscanf(in, inlen, format, ap, SCANF_MODE_BUFFERED);
	va_end(ap);

	return (matches);
}

int textutil_sscanf(const char *in, size_t inlen, const char *format, ...)
{
	va_list	ap;
	int		matches;

	SSL_LOG(TRACE, "Scanning in: mode=default");
	if (NULL == in || inlen == 0 || NULL == format) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (0);
	}

	va_start(ap, format);
	matches = __sscanf(in, inlen, format, ap, SCANF_MODE_DEFAULT);
	va_end(ap);

	return (matches);
}

static int __sscanf(const char *in, size_t inlen, const char *format, va_list ap, enum e_scanf_mode mode)
{
	size_t	fpos = 0;
	size_t	inpos = 0;
	size_t	nbytes;
	void	*arg;
	int		matches = 0;

	SSL_LOG(TRACE, "Scanning using format string: '%s'", format);

	while (format[fpos] != '\0' && inpos < inlen) {
		if (ft_iseolws(format[fpos])) {
			while (ft_iseolws(format[fpos])) {
				fpos++;
			}
			while (ft_iseolws(in[inpos])) {
				inpos++;
			}
			continue;
		}

		if (format[fpos] == '%') {
			SSL_LOG(TRACE, "Parsing format token");
			arg = NULL;
			fpos++;

			if (format[fpos] == '%') {
				SSL_LOG(TRACE, "Found double %% token");
				if (in[inpos] == '%') {
					fpos++;
					inpos++;
				} else {
					break;
				}
			}
			else if (format[fpos] == 's') {
				if (mode == SCANF_MODE_BUFFERED) {
					char	*buf;
					size_t	buf_size;
					SSL_LOG(TRACE, "Found %%s token");
					fpos++;
					buf = va_arg(ap, char *);
					buf_size = va_arg(ap, size_t);
					SSL_LOG(TRACE, "Buffer: p=%p, size=%zu", buf, buf_size);
					if (buf != NULL && buf_size > 0) {
						if ((nbytes = __parse_string_b(in, inlen, inpos, buf, buf_size)) == 0) {
							SSL_LOG(TRACE, "No match for %%s in input string: '%s'", in);
							buf[0] = '\0';
							break;
						}
						inpos += nbytes;
						SSL_LOG(TRACE, "String match: %s", buf);
					} else {
						SSL_LOG(ERROR, "Invalid buffer for %%s: p=%p, size=%zu", buf, buf_size);
						break;
					}
				} else {
					char	*str;
					SSL_LOG(TRACE, "Found %%s token");
					fpos++;
					if ((nbytes = __parse_string(in, inlen, inpos, &str)) == 0) {
						SSL_LOG(TRACE, "No match for %%s in input string: '%s'", in);
						break;
					}
					inpos += nbytes;
					arg = va_arg(ap, char **);
					SSL_LOG(TRACE, "Arg: p=%p", arg);
					if (arg != NULL) {
						*(char **)arg = str;
						SSL_LOG(TRACE, "String match: v='%s'", *(char **)arg);
					} else {
						SSL_FREE(str);
					}
				}
				matches++;
			}
			else if (format[fpos] == 'd') {
				ssize_t	num;
				SSL_LOG(TRACE, "Found %%d token");
				fpos++;
				if ((nbytes = __parse_number(in, inlen, inpos, &num)) == 0) {
					SSL_LOG(TRACE, "No match for %%d in input string: '%s'", in);
					break;
				}
				inpos += nbytes;
				arg = va_arg(ap, int *);
				SSL_LOG(TRACE, "Arg: p=%p", arg);
				if (arg != NULL) {
					*(int *)arg = (int)num;
					SSL_LOG(TRACE, "Number match: v=%d", *(int *)arg);
				}
				matches++;
			}
			else if (format[fpos] == 'u') {
				size_t	num;
				SSL_LOG(TRACE, "Found %%u token");
				fpos++;
				if ((nbytes = __parse_number_u(in, inlen, inpos, &num)) == 0) {
					SSL_LOG(TRACE, "No match for %%u in input string: '%s'", in);
					break;
				}
				inpos += nbytes;
				arg = va_arg(ap, unsigned int *);
				SSL_LOG(TRACE, "Arg: p=%p", arg);
				if (arg != NULL) {
					*(unsigned int *)arg = (unsigned int)num;
					SSL_LOG(TRACE, "Number match: v=%u", *(unsigned int *)arg);
				}
				matches++;
			}
			else if (format[fpos] == 'c') {
				SSL_LOG(TRACE, "Found %%c token");
				fpos++;
				arg = va_arg(ap, char *);
				SSL_LOG(TRACE, "Arg: p=%p", arg);
				if (arg != NULL) {
					*(char *)arg = in[inpos];
					SSL_LOG(TRACE, "Char match: v=%c", *(char *)arg);
				}
				inpos++;
				matches++;
			}
			else if (format[fpos] == 'z') {
				ssize_t	num;
				SSL_LOG(TRACE, "Found %%z token");
				fpos++;
				if (format[fpos] == 'd') {
					SSL_LOG(TRACE, "Found %%zd token");
					fpos++;
					if ((nbytes = __parse_number(in, inlen, inpos, &num)) == 0) {
						SSL_LOG(TRACE, "No match for %%zd in input string: '%s'", in);
						break;
					}
					arg = va_arg(ap, ssize_t *);
					SSL_LOG(TRACE, "Arg: p=%p", arg);
					if (arg != NULL) {
						*(ssize_t *)arg = (ssize_t)num;
						SSL_LOG(TRACE, "Number match: v=%zd", *(ssize_t *)arg);
					}
					matches++;
				}
				else if (format[fpos] == 'u') {
					size_t	num;
					SSL_LOG(TRACE, "Found %%zu token");
					fpos++;
					if ((nbytes = __parse_number_u(in, inlen, inpos, &num)) == 0) {
						SSL_LOG(TRACE, "No match for %%zu in input string: '%s'", in);
						break;
					}
					arg = va_arg(ap, size_t *);
					SSL_LOG(TRACE, "Arg: p=%p", arg);
					if (arg != NULL) {
						*(size_t *)arg = num;
						SSL_LOG(TRACE, "Number match: v=%zu", *(size_t *)arg);
					}
					matches++;
				}
			}
			else if (format[fpos] == '[') {
				char	charset[256] = {0};
				SSL_LOG(TRACE, "Found charset '%%[...]' token");
				if ((nbytes = __parse_charset(format, fpos, charset)) == 0) {
					SSL_LOG(ERROR, "Failed to parse charset '%%[...]' from format string: '%s'", format);
					break;
				}
				fpos += nbytes;
				if (mode == SCANF_MODE_BUFFERED) {
					char	*buf;
					size_t	buf_size;
					buf = va_arg(ap, char *);
					buf_size = va_arg(ap, size_t);
					SSL_LOG(TRACE, "Buffer: p=%p, size=%zu", buf, buf_size);
					if (buf != NULL && buf_size > 0) {
						if ((nbytes = __parse_scanset_b(in, inlen, inpos, buf, buf_size, charset)) == 0) {
							SSL_LOG(TRACE, "No match for scanset '%%[...]' in input string: '%s'", in);
							break;
						}
						inpos += nbytes;
						SSL_LOG(TRACE, "Scanset match: string: %s", buf);
					} else {
						SSL_LOG(ERROR, "Invalid buffer for %%s: p=%p, size=%zu", buf, buf_size);
						break;
					}
				} else {
					char	*str;
					if ((nbytes = __parse_scanset(in, inlen, inpos, &str, charset)) == 0) {
						SSL_LOG(TRACE, "No match for scanset '%%[...]' in input string: '%s'", in);
						break;
					}
					inpos += nbytes;
					arg = va_arg(ap, char **);
					SSL_LOG(TRACE, "Arg: p=%p", arg);
					if (arg != NULL) {
						*(char **)arg = str;
						SSL_LOG(TRACE, "Scanset match: string: v='%s'", *(char **)arg);
					} else {
						SSL_FREE(str);
					}
				}
				matches++;
			}
			else {
				SSL_LOG(ERROR, "Invalid specifier: '%%%c'", format[fpos]);
				break;
			}
		}
		else if (format[fpos] == in[inpos]) {
			fpos++;
			inpos++;
		}
		else {
			SSL_LOG(TRACE, "Mismatch: format='%c', in='%c'", format[fpos], in[inpos]);
			break;
		}
	}
	va_end(ap);

	return (matches);
}

static size_t __parse_string(const char *in, size_t inlen, size_t inpos, char **res)
{
	size_t	start = inpos;
	size_t	end = start;

	*res = NULL;
	while (end < inlen && in[end] && !ft_iseolws(in[end])) {
		end++;
	}
	SSL_LOG(TRACE, "String: start=%zu, end=%zu, len=%zu", start, end, end - start);
	*res = ft_strndup(in + start, end - start);

	return (end - start);
}

static size_t __parse_string_b(const char *in, size_t inlen, size_t inpos, char *buf, size_t buf_size)
{
	size_t	start = inpos;
	size_t	end = start;
	size_t	len;

	while (end < inlen && in[end] && !ft_iseolws(in[end])) {
		end++;
	}
	len = MIN(end - start, buf_size - 1);
	SSL_LOG(TRACE, "String: start=%zu, end=%zu, len=%zu", start, end, len);
	ft_memcpy(buf, in + start, len);
	buf[len] = '\0';

	return (len);
}

static size_t __parse_number(const char *in, size_t inlen, size_t inpos, ssize_t *res)
{
    size_t start = inpos;
    size_t end = start;

    if (end < inlen && (in[end] == '+' || in[end] == '-')) {
        end++;
    }
    if (end == inlen || !ft_isdigit(in[end])) {
        return (0);
    }
    while (end < inlen && ft_isdigit(in[end])) {
        end++;
    }
    *res = ft_atoi(in + start);

    return (end - inpos);
}

static size_t __parse_number_u(const char *in, size_t inlen, size_t inpos, size_t *res)
{
    size_t start = inpos;
    size_t end;

    while (start < inlen && ft_iseolws(in[start])) {
        start++;
    }
    end = start;

    if (end == inlen || !ft_isdigit(in[end])) {
        return (0);
    }
    while (end < inlen && ft_isdigit(in[end])) {
        end++;
    }
    *res = ft_atoi_u(in + start);

    return (end - inpos);
}

static size_t __parse_charset(const char *format, size_t fpos, char *charset)
{
	size_t	start = fpos;
	int		negate = 0;

	fpos++;
	if (format[fpos] == '^') {
		SSL_LOG(TRACE, "Found negation in charset");
		negate = 1;
		fpos++;
	}
	if (format[fpos] == '[') {
		SSL_LOG(TRACE, "Found opening bracket in charset");
		charset['['] = 1;
		fpos++;
	}
	while (format[fpos] != '\0' && format[fpos] != ']') {
		if (format[fpos+1] == '-' && format[fpos+2] != '\0' && format[fpos+2] != ']') {
			SSL_LOG(TRACE, "Found range in charset: '%c-%c'", format[fpos], format[fpos+2]);
			for (char c = format[fpos]; c <= format[fpos+2]; c++) {
				charset[(int)c] = 1;
			}
			fpos += 3;
		} else {
			charset[(int)format[fpos]] = 1;
			fpos++;
		}
	}
	if (format[fpos] == ']') {
		fpos++;
	} else {
		SSL_LOG(ERROR, "Unterminated charset '%%[...]' in format string: '%s' at pos %zu, char: '%c'", format, fpos, format[fpos]);
		return (0);
	}
	if (negate) {
		for (size_t i = 0; i < 256; i++) {
			charset[i] = ~charset[i] & 0x1;
		}
	}
	return (fpos - start);
}

static size_t __parse_scanset(const char *in, size_t inlen, size_t inpos, char **res, const char *charset)
{
	size_t	start = inpos;
	size_t	end = start;

	*res = NULL;
	while (end < inlen) {
		if (!charset[(int)in[end]]) {
			SSL_LOG(TRACE, "Stop matching scanset at pos %zu, char: '%c'", end, in[end]);
			break;
		}
		end++;
	}
	if (end == start) {
		SSL_LOG(TRACE, "No matching characters for scanset");
		return (0);
	}
	*res = ft_strndup(in + start, end - start);

	return (end - start);
}

static size_t __parse_scanset_b(const char *in, size_t inlen, size_t inpos, char *buf, size_t buf_size, const char *charset)
{
	size_t	start = inpos;
	size_t	end = start;
	size_t	len;

	while (end < inlen) {
		if (!charset[(int)in[end]]) {
			SSL_LOG(TRACE, "Stop matching scanset at pos %zu, char: '%c'", end, in[end]);
			break;
		}
		end++;
	}
	if (end == start) {
		SSL_LOG(TRACE, "No matching characters for scanset");
		return (0);
	}
	len = MIN(end - start, buf_size - 1);
	ft_memcpy(buf, in + start, len);
	buf[len] = '\0';

	return (end - start);
}

static t_transform_result __del_update_with_f_search(void *vctx, const void *in, size_t insize, void *out, size_t outsize, __t_search_func f)
{
	t_textutil_ctx *ctx = vctx;

	SSL_LOG(TRACE, "input size %zu, output size %zu", insize, outsize);

	if (NULL == in || NULL == out) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (t_transform_result){.status = TRANSFORM_ERROR};
	}
	if (ctx->done) {
		SSL_LOG(TRACE, "already complete");
		return (t_transform_result){.status = TRANSFORM_DONE};
	}
	const uint8_t *textin = in;
	uint8_t *textout = out;
	size_t i = 0;
	size_t j = 0;

	if (insize == 0) {
		SSL_LOG(TRACE, "need input");
		return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_NEED_INPUT};
	}
	while (i < insize && j < outsize) {
		if (f(textin[i])) {
			i++;
		} else {
			if (j < outsize) {
				textout[j++] = textin[i++];
			} else {
				break;
			}
		}
	}
	if (i <= insize) {
		SSL_LOG(TRACE, "need output");
		return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_NEED_OUTPUT};
	}
	SSL_LOG(TRACE, "update is ok");
	return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_OK};
}

static t_transform_result __del_final_with_f_search(
	void *vctx, const void *in, size_t insize, void *out, size_t outsize, __t_search_func f)
{
	t_textutil_ctx *ctx = vctx;

	SSL_LOG(TRACE, "input size %zu, output size %zu", insize, outsize);

	if (NULL == in || NULL == out) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (t_transform_result){.status = TRANSFORM_ERROR};
	}
	if (ctx->done) {
		SSL_LOG(TRACE, "already complete");
		return (t_transform_result){.status = TRANSFORM_DONE};
	}
	const uint8_t *textin = in;
	uint8_t *textout = out;
	size_t i = 0;
	size_t j = 0;

	if (insize == 0) {
		SSL_LOG(TRACE, "no input, done");
		return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_DONE};
	}
	while (i < insize) {
		if (f(textin[i])) {
			i++;
		} else {
			if (j < outsize) {
				textout[j++] = textin[i++];
			} else {
				break;
			}
		}
	}
	if (i <= insize) {
		SSL_LOG(TRACE, "need output");
		return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_NEED_OUTPUT};
	} else {
		SSL_LOG(TRACE, "processing is complete");
		return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_DONE};
	}
}

t_transform_result textutil_del_eolws_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	SSL_LOG(TRACE, "running delete eolws text transform update");
	return (__del_update_with_f_search(vctx, in, insize, out, outsize, ft_iseolws));
}

t_transform_result textutil_del_eolws_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	SSL_LOG(TRACE, "running delete eolws text transform final");
	return (__del_final_with_f_search(vctx, in, insize, out, outsize, ft_iseolws));
}

static inline size_t __get_insert_pos(size_t offset, size_t line_width);
static inline size_t __get_new_offset(size_t offset, size_t line_width, size_t pos);

t_transform_result textutil_insert_delim_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	t_textutil_ctx *ctx = vctx;

	SSL_LOG(TRACE, "input size %zu, output size %zu", insize, outsize);

	if (NULL == in || NULL == out || NULL == ctx) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (t_transform_result){.status = TRANSFORM_ERROR};
	}
	if (ctx->line_width == 0) {
		SSL_LOG(ERROR, "invalid line length: %zu", ctx->line_width);
		return (t_transform_result){.status = TRANSFORM_ERROR};
	}
	if (ctx->done) {
		SSL_LOG(TRACE, "already complete");
		return (t_transform_result){.status = TRANSFORM_DONE};
	}
	const uint8_t *textin = in;
	uint8_t *textout = out;
	size_t i = 0;
	size_t j = 0;

	if (insize == 0) {
		SSL_LOG(TRACE, "need input");
		return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_NEED_INPUT};
	}
	if (outsize == 0) {
		SSL_LOG(TRACE, "need output");
		return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_NEED_OUTPUT};
	}
	size_t insert_pos = __get_insert_pos(ctx->offset, ctx->line_width);

	while (i < insize && j < outsize) {
		if (i == insert_pos) {
			textout[j++] = ctx->delim;
			insert_pos = i + ctx->line_width;
		}
		if (j < outsize) {
			textout[j++] = textin[i++];
		}
	}
	ctx->offset = __get_new_offset(ctx->offset, ctx->line_width, i);
	SSL_LOG(TRACE, "update is ok");
	return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_OK};
}

static inline size_t __get_insert_pos(size_t offset, size_t line_width) {
	if (offset == line_width) {
		return (0);
	} else {
		return (line_width - offset%line_width);
	}
}

static inline size_t __get_new_offset(size_t offset, size_t line_width, size_t pos) {
	if (offset + pos > line_width) {
		return ((offset + pos) % line_width);
	} else {
		return (offset + pos);
	}
}

t_transform_result textutil_insert_delim_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	t_textutil_ctx *ctx = vctx;

	SSL_LOG(TRACE, "input size %zu, output size %zu", insize, outsize);

	if (NULL == in || NULL == out || NULL == ctx) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (t_transform_result){.status = TRANSFORM_ERROR};
	}
	if (ctx->line_width == 0) {
		SSL_LOG(ERROR, "invalid line length: %zu", ctx->line_width);
		return (t_transform_result){.status = TRANSFORM_ERROR};
	}
	if (ctx->done) {
		SSL_LOG(TRACE, "already complete");
		return (t_transform_result){.status = TRANSFORM_DONE};
	}

	const uint8_t *textin = in;
	uint8_t *textout = out;
	size_t i = 0;
	size_t j = 0;

	size_t insert_pos = __get_insert_pos(ctx->offset, ctx->line_width);

	while (i < insize && j < outsize) {
		if (i == insert_pos) {
			textout[j++] = ctx->delim;
			insert_pos = i + ctx->line_width;
		}
		if (j < outsize) {
			textout[j++] = textin[i++];
		}
	}
	ctx->offset = __get_new_offset(ctx->offset, ctx->line_width, i);

	if (i >= insize) {
		SSL_LOG(TRACE, "processing is complete");
		return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_DONE};
	}
	else {
		SSL_LOG(TRACE, "need output");
		return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_NEED_OUTPUT};
	}
}

t_transform_result textutil_terminator_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	t_textutil_ctx *ctx = vctx;

	SSL_LOG(TRACE, "input size %zu, output size %zu", insize, outsize);

	if (NULL == in || NULL == out || NULL == ctx) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (t_transform_result){.status = TRANSFORM_ERROR};
	}
	if (ctx->done) {
		SSL_LOG(TRACE, "already complete");
		return (t_transform_result){.status = TRANSFORM_DONE};
	}
	const uint8_t *textin = in;
	uint8_t *textout = out;
	size_t i = 0;
	size_t j = 0;

	while (i < insize && j < outsize) {
		textout[j++] = textin[i++];
	}
	if (j < outsize) {
		textout[j++] = ctx->delim;
		SSL_LOG(TRACE, "processing is complete");
		return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_DONE};
	}
	else {
		SSL_LOG(TRACE, "need output");
		return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_NEED_OUTPUT};
	}
}
