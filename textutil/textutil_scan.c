#include <textutil.h>
#include <libft.h>
#include <logger.h>

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
