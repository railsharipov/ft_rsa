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
static size_t __parse_string(const char *in, size_t inlen, char **res);
static size_t __parse_string_b(const char *in, size_t inlen, char *buf, size_t buf_size);
static size_t __parse_number_d(const char *in, size_t inlen, int *res);
static size_t __parse_number_u(const char *in, size_t inlen, unsigned int *res);
static size_t __parse_number_zd(const char *in, size_t inlen, ssize_t *res);
static size_t __parse_number_zu(const char *in, size_t inlen, size_t *res);
static size_t __parse_charset(const char *format, char *charset);
static size_t __parse_scanset(const char *in, size_t inlen, char **res, const char *charset);
static size_t __parse_scanset_b(const char *in, size_t inlen, char *buf, size_t buf_size, const char *charset);

int textutil_bnscanf(const char *in, size_t inlen, const char *format, ...)
{
	va_list	ap;
	int		matches;

	SSL_LOG(TRACE, "scanning in: mode=buffered");
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

	SSL_LOG(TRACE, "scanning in: mode=default");
	if (NULL == in || inlen == 0 || NULL == format) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (0);
	}

	va_start(ap, format);
	matches = __sscanf(in, inlen, format, ap, SCANF_MODE_DEFAULT);
	va_end(ap);

	return (matches);
}

typedef enum __e_scanf_len_mod {
	SCANF_LEN_MOD_NONE = 0,
	SCANF_LEN_MOD_LO_HH,
	SCANF_LEN_MOD_LO_H,
	SCANF_LEN_MOD_LO_L,
	SCANF_LEN_MOD_LO_LL,
	SCANF_LEN_MOD_LO_J,
	SCANF_LEN_MOD_LO_Z,
	SCANF_LEN_MOD_LO_T,
	SCANF_LEN_MOD_UP_L,
} __t_scanf_len_mod;

static int __sscanf(const char *in, size_t inlen, const char *format, va_list ap, enum e_scanf_mode mode)
{
	size_t	fpos = 0;
	size_t	inpos = 0;
	size_t	nbytes = 0;
	int		matches = 0;

	SSL_LOG(TRACE, "scanning using format string: '%s'", format);

	size_t format_len = ft_strlen(format);

	while (format[fpos] != '\0') {
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
			SSL_LOG(TRACE, "parsing format token");
			fpos++;

			bool has_arg = true;
			bool has_width = false;
			__t_scanf_len_mod length_mod = SCANF_LEN_MOD_NONE;
			size_t field_width = inlen - inpos;

			// Literal %.
			if (format[fpos] == '%') {
				SSL_LOG(TRACE, "found double %% token");
				if (inpos < inlen && in[inpos] == '%') {
					fpos++;
					inpos++;
				} else {
					SSL_LOG(TRACE, "no match for %%%% in input string: '%s'", in+inpos);
					break;
				}
			}

			// Argument existence modifier.
			if (format[fpos] == '*') {
				has_arg = false;
				fpos++;
			}

			// Field width modifier.
			if (ft_isdigit(format[fpos])) {
				has_width = true;
				size_t width = 0;
				nbytes = __parse_number_zu(format+fpos, format_len-fpos, &width);
				fpos += nbytes;
				field_width = MIN(width, field_width);
			}

			// Length modifier.
			if (format[fpos] == 'h') {
				fpos++;
				length_mod = SCANF_LEN_MOD_LO_H;
				if (format[fpos] == 'h') {
					fpos++;
					length_mod = SCANF_LEN_MOD_LO_HH;
				}
			}
			else if (format[fpos] == 'l') {
				fpos++;
				length_mod = SCANF_LEN_MOD_LO_L;
				if (format[fpos] == 'l') {
					fpos++;
					length_mod = SCANF_LEN_MOD_LO_LL;
				}
			}
			else if (format[fpos] == 'j') {
				fpos++;
				length_mod = SCANF_LEN_MOD_LO_J;
			}
			else if (format[fpos] == 'z') {
				fpos++;
				length_mod = SCANF_LEN_MOD_LO_Z;
			}
			else if (format[fpos] == 't') {
				fpos++;
				length_mod = SCANF_LEN_MOD_LO_T;
			}
			else if (format[fpos] == 'L') {
				fpos++;
				length_mod = SCANF_LEN_MOD_UP_L;
			}

			// Conversion.
			if (format[fpos] == 'n') {
				if (has_width) {
					SSL_LOG(WARN, "ignoring field width for %%n conversion");
				}
				if (length_mod != SCANF_LEN_MOD_LO_Z) {
					NOT_IMPLEMENTED("length modifier: anything but %z");
				}
				size_t *num = NULL;
				SSL_LOG(TRACE, "found %%n token");
				fpos++;
				if (has_arg) {
					num = va_arg(ap, size_t *);
				}
				SSL_LOG(TRACE, "arg: p=%p", num);
				if (NULL != num) *num = inpos;
			}
			else if (format[fpos] == 's') {
				if (length_mod != SCANF_LEN_MOD_NONE) {
					NOT_IMPLEMENTED("length modifier");
				}
				if (mode == SCANF_MODE_BUFFERED) {
					char *buf = NULL;
					size_t buf_size = 0;
					SSL_LOG(TRACE, "found %%s token");
					fpos++;
					if (has_arg) {
						buf = va_arg(ap, char *);
						buf_size = va_arg(ap, size_t);
					}
					SSL_LOG(TRACE, "buffer: p=%p, size=%zu", buf, buf_size);
					if ((nbytes = __parse_string_b(in+inpos, field_width, buf, buf_size)) == 0) {
						SSL_LOG(TRACE, "no match for %%s in input string: '%s'", in+inpos);
						buf[0] = '\0';
						break;
					}
					inpos += nbytes;
					SSL_LOG(TRACE, "string match: %s", buf ? buf : "no arg");
				}
				else {
					char **str = NULL;
					SSL_LOG(TRACE, "found %%s token");
					fpos++;
					if (has_arg) {
						str = va_arg(ap, char **);
					}
					SSL_LOG(TRACE, "arg: p=%p", str);
					if ((nbytes = __parse_string(in+inpos, field_width, str)) == 0) {
						SSL_LOG(TRACE, "no match for %%s in input string: '%s'", in+inpos);
						break;
					}
					inpos += nbytes;
					SSL_LOG(TRACE, "string match: v='%s'", str ? *str : "no arg");
				}
				matches++;
			}
			else if (format[fpos] == 'd') {
				SSL_LOG(TRACE, "found %%d token");
				fpos++;
				switch (length_mod) {
				case SCANF_LEN_MOD_NONE: {
					int *num = NULL;
					if (has_arg) num = va_arg(ap, int *);
					SSL_LOG(TRACE, "arg: p=%p", num);
					if ((nbytes = __parse_number_d(in+inpos, field_width, num)) == 0) {
						SSL_LOG(TRACE, "no match for %%d in input string: '%s'", in+inpos);
						goto label_exit;
					}
					inpos += nbytes;
					if (num) SSL_LOG(TRACE, "number match: v=%d", *num);
					matches++;
					break;
				}
				case SCANF_LEN_MOD_LO_Z: {
					ssize_t *num = NULL;
					if (has_arg) num = va_arg(ap, ssize_t *);
					SSL_LOG(TRACE, "arg: p=%p", num);
					if ((nbytes = __parse_number_zd(in+inpos, field_width, num)) == 0) {
						SSL_LOG(TRACE, "no match for %%d in input string: '%s'", in+inpos);
						goto label_exit;
					}
					inpos += nbytes;
					if (num) SSL_LOG(TRACE, "number match: v=%zd", *num);
					matches++;
					break;
				}
				case SCANF_LEN_MOD_LO_HH:
				case SCANF_LEN_MOD_LO_H:
				case SCANF_LEN_MOD_LO_L:
				case SCANF_LEN_MOD_LO_LL:
				case SCANF_LEN_MOD_LO_J:
				case SCANF_LEN_MOD_LO_T:
				case SCANF_LEN_MOD_UP_L: {
					NOT_IMPLEMENTED("length modifier");
				}}
			}
			else if (format[fpos] == 'u') {
				SSL_LOG(TRACE, "found %%d token");
				fpos++;
				switch (length_mod) {
				case SCANF_LEN_MOD_NONE: {
					unsigned int *num = NULL;
					if (has_arg) num = va_arg(ap, unsigned int *);
					SSL_LOG(TRACE, "arg: p=%p", num);
					if ((nbytes = __parse_number_u(in+inpos, field_width, num)) == 0) {
						SSL_LOG(TRACE, "no match for %%d in input string: '%s'", in+inpos);
						goto label_exit;
					}
					inpos += nbytes;
					if (num) SSL_LOG(TRACE, "number match: v=%u", *num);
					matches++;
					break;
				}
				case SCANF_LEN_MOD_LO_Z: {
					size_t *num = NULL;
					if (has_arg) num = va_arg(ap, size_t *);
					SSL_LOG(TRACE, "arg: p=%p", num);
					if ((nbytes = __parse_number_zu(in+inpos, field_width, num)) == 0) {
						SSL_LOG(TRACE, "no match for %%d in input string: '%s'", in+inpos);
						goto label_exit;
					}
					inpos += nbytes;
					if (num) SSL_LOG(TRACE, "number match: v=%zu", *num);
					matches++;
					break;
				}
				case SCANF_LEN_MOD_LO_HH:
				case SCANF_LEN_MOD_LO_H:
				case SCANF_LEN_MOD_LO_L:
				case SCANF_LEN_MOD_LO_LL:
				case SCANF_LEN_MOD_LO_J:
				case SCANF_LEN_MOD_LO_T:
				case SCANF_LEN_MOD_UP_L: {
					NOT_IMPLEMENTED("length modifier");
				}}
			}
			else if (format[fpos] == 'c') {
				if (length_mod != SCANF_LEN_MOD_NONE) {
					NOT_IMPLEMENTED("length modifier");
				}
				char *chr = NULL;
				SSL_LOG(TRACE, "found %%c token");
				fpos++;
				if (has_arg) {
					chr = va_arg(ap, char *);
				}
				SSL_LOG(TRACE, "arg: p=%p", chr);
				if (inpos >= inlen) {
					SSL_LOG(TRACE, "no match for %%c in input string");
					break;
				}
				if (chr) {
					*chr = in[inpos];
					SSL_LOG(TRACE, "char match: v=%c", *chr);
				}
				inpos++;
				matches++;
			}
			else if (format[fpos] == '[') {
				if (length_mod != SCANF_LEN_MOD_NONE) {
					NOT_IMPLEMENTED("length modifier");
				}
				char charset[256] = {0};
				SSL_LOG(TRACE, "found charset '%%[...]' token");
				if ((nbytes = __parse_charset(format+fpos, charset)) == 0) {
					SSL_LOG(ERROR, "Failed to parse charset '%%[...]' from format string: '%s'", format);
					break;
				}
				fpos += nbytes;
				if (mode == SCANF_MODE_BUFFERED) {
					char *buf = NULL;
					size_t buf_size = 0;
					if (has_arg) {
						buf = va_arg(ap, char *);
						buf_size = va_arg(ap, size_t);
					}
					SSL_LOG(TRACE, "buffer: p=%p, size=%zu", buf, buf_size);
					if ((nbytes = __parse_scanset_b(in+inpos, field_width, buf, buf_size, charset)) == 0) {
						SSL_LOG(TRACE, "no match for scanset '%%[...]' in input string: '%s'", in+inpos);
						break;
					}
					inpos += nbytes;
					SSL_LOG(TRACE, "scanset match: string: %s", buf ? buf : "no arg");
				}
				else {
					char **str = NULL;
					if (has_arg) {
						str = va_arg(ap, char **);
					}
					SSL_LOG(TRACE, "arg: p=%p", str);
					SSL_LOG(WARN, "inpos=%zu, field_width=%zu", inpos, field_width);
					if ((nbytes = __parse_scanset(in+inpos, field_width, str, charset)) == 0) {
						SSL_LOG(TRACE, "no match for scanset '%%[...]' in input string: '%s'", in+inpos);
						break;
					}
					inpos += nbytes;
					SSL_LOG(TRACE, "scanset match: string: v='%s'", str ? *str : "no arg");
				}
				matches++;
			}
			else if (format[fpos] == 'i') {
				NOT_IMPLEMENTED("conversion: %i");
			}
			else if (format[fpos] == 'o') {
				NOT_IMPLEMENTED("conversion: %o");
			}
			else if (format[fpos] == 'x') {
				NOT_IMPLEMENTED("conversion: %x");
			}
			else if (format[fpos] == 'X') {
				NOT_IMPLEMENTED("conversion: %X");
			}
			else if (format[fpos] == 'a') {
				NOT_IMPLEMENTED("conversion: %a");
			}
			else if (format[fpos] == 'A') {
				NOT_IMPLEMENTED("conversion: %A");
			}
			else if (format[fpos] == 'e') {
				NOT_IMPLEMENTED("conversion: %e");
			}
			else if (format[fpos] == 'E') {
				NOT_IMPLEMENTED("conversion: %E");
			}
			else if (format[fpos] == 'f') {
				NOT_IMPLEMENTED("conversion: %f");
			}
			else if (format[fpos] == 'F') {
				NOT_IMPLEMENTED("conversion: %F");
			}
			else if (format[fpos] == 'g') {
				NOT_IMPLEMENTED("conversion: %g");
			}
			else if (format[fpos] == 'G') {
				NOT_IMPLEMENTED("conversion: %G");
			}
			else if (format[fpos] == 'p') {
				NOT_IMPLEMENTED("conversion: %p");
			}
			else {
				SSL_LOG(ERROR, "Invalid conversion: '%%%c'", format[fpos]);
				break;
			}
		}
		else if (format[fpos] == in[inpos]) {
			fpos++;
			inpos++;
		}
		else {
			SSL_LOG(TRACE, "mismatch: format='%c', in='%c'", format[fpos], in[inpos]);
			break;
		}
	}

label_exit:
	va_end(ap);
	return (matches);
}

static size_t __parse_string(const char *in, size_t inlen, char **res)
{
	size_t start = 0;
	while (start < inlen && in[start] && ft_iseolws(in[start])) {
		start++;
	}
	size_t end = start;

	while (end < inlen && in[end] && !ft_iseolws(in[end])) {
		end++;
	}
	SSL_LOG(TRACE, "string: start=%zu, end=%zu, len=%zu", start, end, end - start);
	if (NULL != res) {
		*res = ft_strndup(in + start, end - start);
	}
	return (end - start);
}

static size_t __parse_string_b(const char *in, size_t inlen, char *buf, size_t buf_size)
{
	size_t start = 0;
	while (start < inlen && in[start] && ft_iseolws(in[start])) {
		start++;
	}
	size_t end = start;

	while (end < inlen && in[end] && !ft_iseolws(in[end])) {
		end++;
	}
	SSL_LOG(TRACE, "string: start=%zu, end=%zu, len=%zu", start, end, end - start);
	if (NULL != buf && buf_size > 0) {
		size_t len = MIN(end - start, buf_size - 1);
		ft_memcpy(buf, in + start, len);
		buf[len] = '\0';
	}
	return (end);
}

static size_t __parse_number_d(const char *in, size_t inlen, int *res)
{
	size_t start = 0;
	while (start < inlen && in[start] && ft_iseolws(in[start])) {
		start++;
	}
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
    if (NULL != res) {
    	*res = (int)ft_batoi_zd(in+start, end-start);
    }
    return (end);
}

static size_t __parse_number_u(const char *in, size_t inlen, unsigned int *res)
{
	size_t start = 0;
	while (start < inlen && in[start] && ft_iseolws(in[start])) {
		start++;
	}
	size_t end = start;

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
    if (NULL != res) {
    	*res = (unsigned int)ft_batoi_zu(in+start, end-start);
    }
    return (end);
}

static size_t __parse_number_zd(const char *in, size_t inlen, ssize_t *res)
{
	size_t start = 0;
	while (start < inlen && in[start] && ft_iseolws(in[start])) {
		start++;
	}
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
    if (NULL != res) {
   		*res = ft_batoi_zd(in+start, end-start);
    }
    return (end);
}

static size_t __parse_number_zu(const char *in, size_t inlen, size_t *res)
{
	size_t start = 0;
	while (start < inlen && in[start] && ft_iseolws(in[start])) {
		start++;
	}
	size_t end = start;

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
    if (NULL != res) {
   		*res = ft_batoi_zu(in+start, end-start);
    }
    return (end);
}

static size_t __parse_charset(const char *format, char *charset)
{
	size_t fpos = 0;
	int negate = 0;

	fpos++;
	if (format[fpos] == '^') {
		SSL_LOG(TRACE, "found negation in charset");
		negate = 1;
		fpos++;
	}
	if (format[fpos] == '[') {
		SSL_LOG(TRACE, "found opening bracket in charset");
		charset['['] = 1;
		fpos++;
	}
	while (format[fpos] != '\0' && format[fpos] != ']') {
		if (format[fpos+1] == '-' && format[fpos+2] != '\0' && format[fpos+2] != ']') {
			SSL_LOG(TRACE, "found range in charset: '%c-%c'", format[fpos], format[fpos+2]);
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
	return (fpos);
}

static size_t __parse_scanset(const char *in, size_t inlen, char **res, const char *charset)
{
	size_t pos = 0;
	while (pos < inlen) {
		if (!charset[(int)in[pos]]) {
			SSL_LOG(TRACE, "stop matching scanset at pos %zu, char: '%c'", pos, in[pos]);
			break;
		}
		pos++;
	}
	if (pos == 0) {
		SSL_LOG(TRACE, "no matching characters for scanset");
		if (NULL != res) *res = NULL;
		return (0);
	}
	if (NULL != res) {
		*res = ft_strndup(in, pos);
	}
	return (pos);
}

static size_t __parse_scanset_b(const char *in, size_t inlen, char *buf, size_t buf_size, const char *charset)
{
	size_t pos = 0;
	while (pos < inlen) {
		if (!charset[(int)in[pos]]) {
			SSL_LOG(TRACE, "stop matching scanset at pos %zu, char: '%c'", pos, in[pos]);
			break;
		}
		pos++;
	}
	if (pos == 0) {
		SSL_LOG(TRACE, "no matching characters for scanset");
		if (NULL != buf && buf_size > 0) buf[0] = '\0';
		return (0);
	}
	if (NULL != buf && buf_size > 0) {
		size_t len = MIN(pos, buf_size - 1);
		ft_memcpy(buf, in, len);
		buf[len] = '\0';
	}
	return (pos);
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
		ctx->done = 1;
		SSL_LOG(TRACE, "processing is complete");
		return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_DONE};
	}
	else {
		SSL_LOG(TRACE, "need output");
		return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_NEED_OUTPUT};
	}
}
