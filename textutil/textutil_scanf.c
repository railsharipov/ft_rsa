#include <textutil.h>
#include <libft/string.h>
#include <libft/std.h>
#include <libft/list.h>

enum e_scanf_mode
{
	SCANF_MODE_DEFAULT,
	SCANF_MODE_BUFFERED,
};

static int __scanf(const char *octets, size_t olen, const char *format, va_list ap, enum e_scanf_mode mode);
static size_t __parse_string(const char *octets, size_t olen, size_t opos, char **res);
static size_t __parse_string_b(const char *octets, size_t olen, size_t opos, char *buf, size_t buf_size);
static size_t __parse_number(const char *octets, size_t olen, size_t opos, ssize_t *res);
static size_t __parse_number_u(const char *octets, size_t olen, size_t opos, size_t *res);
static size_t __parse_charset(const char *format, size_t fpos, char *charset);
static size_t __parse_scanset(const char *octets, size_t olen, size_t opos, char **res, const char *charset);

int textutil_sbscanf(const char *octets, size_t olen, const char *format, ...)
{
	va_list	ap;
	int		matches;

	va_start(ap, format);
	matches = __scanf(octets, olen, format, ap, SCANF_MODE_BUFFERED);
	va_end(ap);

	return (matches);
}

int textutil_sscanf(const char *octets, size_t olen, const char *format, ...)
{
	va_list	ap;
	int		matches;

	va_start(ap, format);
	matches = __scanf(octets, olen, format, ap, SCANF_MODE_DEFAULT);
	va_end(ap);

	return (matches);
}

static int __scanf(const char *octets, size_t olen, const char *format, va_list ap, enum e_scanf_mode mode)
{
	size_t	fpos = 0;
	size_t	opos = 0;
	size_t	nbytes;
	void	*arg;
	int		matches = 0;

	TEXTUTIL_LOG(TRACE, "Scanning using format string: '%s'", format);
	
	while (format[fpos] != '\0' && opos < olen) {
		if (ft_iseolws(format[fpos])) {
			while (ft_iseolws(format[fpos])) {
				fpos++;
			}
			while (ft_iseolws(octets[opos])) {
				opos++;
			}
			continue;
		}

		if (format[fpos] == '%') {
			TEXTUTIL_LOG(TRACE, "Parsing format token");
			arg = NULL;
			fpos++;

			if (format[fpos] == '%') {
				TEXTUTIL_LOG(TRACE, "Found double %% token");
				if (octets[opos] == '%') {
					fpos++;
					opos++;
				} else {
					break;
				}
			}
			else if (format[fpos] == 's') {
				if (mode == SCANF_MODE_BUFFERED) {
					char	*buf;
					size_t	buf_size;
					TEXTUTIL_LOG(TRACE, "Found %%s token");
					fpos++;
					buf = va_arg(ap, char *);
					buf_size = va_arg(ap, size_t);
					if (buf != NULL && buf_size > 0) {
						if ((nbytes = __parse_string_b(octets, olen, opos, buf, buf_size)) == 0) {
							TEXTUTIL_LOG(TRACE, "No match for %%s in input string: '%s'", octets);
							break;
						}
						opos += nbytes;
						TEXTUTIL_LOG(TRACE, "String buffer: p=%p, size=%zu", buf, buf_size);
					} else {
						TEXTUTIL_LOG(ERROR, "Invalid buffer for %%s: p=%p, size=%zu", buf, buf_size);
						break;
					}
				} else {
					char	*str;
					TEXTUTIL_LOG(TRACE, "Found %%s token");
					fpos++;
					if ((nbytes = __parse_string(octets, olen, opos, &str)) == 0) {
						TEXTUTIL_LOG(TRACE, "No match for %%s in input string: '%s'", octets);
						break;
					}
					opos += nbytes;
					arg = va_arg(ap, char **);
					TEXTUTIL_LOG(TRACE, "Arg: p=%p", arg);
					if (arg != NULL) {
						*(char **)arg = str;
						TEXTUTIL_LOG(TRACE, "String: v='%s'", *(char **)arg);
					} else {
						SSL_FREE(str);
					}
				}
				matches++;
			}
			else if (format[fpos] == 'd') {
				ssize_t	num;
				TEXTUTIL_LOG(TRACE, "Found %%d token");
				fpos++;
				if ((nbytes = __parse_number(octets, olen, opos, &num)) == 0) {
					TEXTUTIL_LOG(TRACE, "No match for %%d in input string: '%s'", octets);
					break;
				}
				opos += nbytes;
				arg = va_arg(ap, int *);
				TEXTUTIL_LOG(TRACE, "Arg: p=%p", arg);
				if (arg != NULL) {
					*(int *)arg = (int)num;
					TEXTUTIL_LOG(TRACE, "Number: v=%d", *(int *)arg);
				}
				matches++;
			}
			else if (format[fpos] == 'u') {
				size_t	num;
				TEXTUTIL_LOG(TRACE, "Found %%u token");
				fpos++;
				if ((nbytes = __parse_number_u(octets, olen, opos, &num)) == 0) {
					TEXTUTIL_LOG(TRACE, "No match for %%u in input string: '%s'", octets);
					break;
				}
				opos += nbytes;
				arg = va_arg(ap, unsigned int *);
				TEXTUTIL_LOG(TRACE, "Arg: p=%p", arg);
				if (arg != NULL) {
					*(unsigned int *)arg = (unsigned int)num;
					TEXTUTIL_LOG(TRACE, "Number: v=%u", *(unsigned int *)arg);
				}
				matches++;
			}
			else if (format[fpos] == 'c') {
				TEXTUTIL_LOG(TRACE, "Found %%c token");
				fpos++;
				arg = va_arg(ap, char *);
				TEXTUTIL_LOG(TRACE, "Arg: p=%p", arg);
				if (arg != NULL) {
					*(char *)arg = octets[opos];
					TEXTUTIL_LOG(TRACE, "Char: v=%c", *(char *)arg);
				}
				opos++;
				matches++;
			}
			else if (format[fpos] == 'z') {
				ssize_t	num;
				TEXTUTIL_LOG(TRACE, "Found %%z token");
				fpos++;
				if (format[fpos] == 'd') {
					TEXTUTIL_LOG(TRACE, "Found %%zd token");
					fpos++;
					if ((nbytes = __parse_number(octets, olen, opos, &num)) == 0) {
						TEXTUTIL_LOG(TRACE, "No match for %%zd in input string: '%s'", octets);
						break;
					}
					arg = va_arg(ap, ssize_t *);
					TEXTUTIL_LOG(TRACE, "Arg: p=%p", arg);
					if (arg != NULL) {
						*(ssize_t *)arg = (ssize_t)num;
						TEXTUTIL_LOG(TRACE, "Number: v=%zd", *(ssize_t *)arg);
					}
					matches++;
				}
				else if (format[fpos] == 'u') {
					size_t	num;
					TEXTUTIL_LOG(TRACE, "Found %%zu token");
					fpos++;
					if ((nbytes = __parse_number_u(octets, olen, opos, &num)) == 0) {
						TEXTUTIL_LOG(TRACE, "No match for %%zu in input string: '%s'", octets);
						break;
					}
					arg = va_arg(ap, size_t *);
					TEXTUTIL_LOG(TRACE, "Arg: p=%p", arg);
					if (arg != NULL) {
						*(size_t *)arg = num;
						TEXTUTIL_LOG(TRACE, "Number: v=%zu", *(size_t *)arg);
					}
					matches++;
				}
			}
			else if (format[fpos] == '[') {
				char	*str;
				char	charset[256] = {0};
				TEXTUTIL_LOG(TRACE, "Found charset '%%[...]' token");
				if ((nbytes = __parse_charset(format, fpos, charset)) == 0) {
					TEXTUTIL_LOG(ERROR, "Failed to parse charset '%%[...]' from format string: '%s'", format);
					break;
				}
				fpos += nbytes;
				if ((nbytes = __parse_scanset(octets, olen, opos, &str, charset)) == 0) {
					TEXTUTIL_LOG(TRACE, "No match for scanset '%%[...]' in input string: '%s'", octets);
					break;
				}
				opos += nbytes;
				arg = va_arg(ap, char **);
				if (arg != NULL) {
					*(char **)arg = str;
					TEXTUTIL_LOG(TRACE, "Scanset: v='%s'", *(char **)arg);
				} else {
					SSL_FREE(str);
				}
				matches++;
			}
			else {
				TEXTUTIL_LOG(ERROR, "Invalid specifier: '%%%c'", format[fpos]);
				break;
			}
		}
		else if (format[fpos] == octets[opos]) {
			fpos++;
			opos++;
		}
		else {
			TEXTUTIL_LOG(TRACE, "Mismatch: format='%c', octets='%c'", format[fpos], octets[opos]);
			break;
		}
	}
	va_end(ap);

	return (matches);
}

static size_t __parse_string(const char *octets, size_t olen, size_t opos, char **res)
{
	size_t	start = opos;
	size_t	end;

	*res = NULL;
	end = start;
	while (opos + end < olen && octets[opos + end] != ' ' && octets[opos + end] != '\0') {
		end++;
	}
	*res = ft_strndup(octets + start, end - start);

	return (end - start);
}

static size_t __parse_string_b(const char *octets, size_t olen, size_t opos, char *buf, size_t buf_size)
{
	size_t	start = opos;
	size_t	end;
	size_t	len;

	end = start;
	while (opos + end < olen && octets[opos + end] != ' ' && octets[opos + end] != '\0') {
		end++;
	}
	len = MIN(end - start, buf_size - 1);
	ft_memcpy(buf, octets + start, len);
	buf[len] = '\0';

	return (len);
}

static size_t __parse_number(const char *octets, size_t olen, size_t opos, ssize_t *res)
{
    size_t start = opos;
    size_t end = start;

    if (end < olen && (octets[end] == '+' || octets[end] == '-')) {
        end++;
    }
    if (end == olen || !ft_isdigit(octets[end])) {
        return (0);
    }
    while (end < olen && ft_isdigit(octets[end])) {
        end++;
    }
    *res = ft_atoi(octets + start);

    return (end - opos);
}

static size_t __parse_number_u(const char *octets, size_t olen, size_t opos, size_t *res)
{
    size_t start = opos;
    size_t end;

    while (start < olen && ft_iseolws(octets[start])) {
        start++;
    }
    end = start;

    if (end == olen || !ft_isdigit(octets[end])) {
        return (0);
    }
    while (end < olen && ft_isdigit(octets[end])) {
        end++;
    }
    *res = ft_atoi_u(octets + start);

    return (end - opos);
}

static size_t __parse_charset(const char *format, size_t fpos, char *charset)
{
	size_t	start = fpos;
	int		negate = 0;

	fpos++;
	if (format[fpos] == '^') {
		TEXTUTIL_LOG(TRACE, "Found negation in charset");
		negate = 1;
		fpos++;
	}
	if (format[fpos] == '[') {
		TEXTUTIL_LOG(TRACE, "Found opening bracket in charset");
		charset['['] = 1;
		fpos++;
	}
	while (format[fpos] != '\0' && format[fpos] != ']') {
		if (format[fpos+1] == '-' && format[fpos+2] != '\0' && format[fpos+2] != ']') {
			TEXTUTIL_LOG(TRACE, "Found range in charset: '%c-%c'", format[fpos], format[fpos+2]);
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
		TEXTUTIL_LOG(ERROR, "Unterminated charset '%%[...]' in format string: '%s' at pos %zu, char: '%c'", format, fpos, format[fpos]);
		return (0);
	}
	if (negate) {
		for (size_t i = 0; i < 256; i++) {
			charset[i] = ~charset[i] & 0x1;
		}
	}
	return (fpos - start);
}

static size_t __parse_scanset(const char *octets, size_t olen, size_t opos, char **res, const char *charset)
{
	size_t	start = opos;
	size_t	end = start;

	*res = NULL;
	while (end < olen) {
		if (!charset[(int)octets[end]]) {
			TEXTUTIL_LOG(TRACE, "No match for scanset at pos %zu, char: '%c'", end, octets[end]);
			break;
		}
		end++;
	}
	if (end == start) {
		TEXTUTIL_LOG(TRACE, "No matching characters for scanset");
		return (0);
	}
	*res = ft_strndup(octets + start, end - start);

	return (end - start);
}