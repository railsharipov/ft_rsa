#include <textutil.h>
#include <libft/string.h>
#include <libft/std.h>
#include <libft/list.h>

static int __parse_string(const char *octets, int olen, int opos, char **res);
static int __parse_number(const char *octets, int olen, int opos, ssize_t *res);
static int __parse_number_u(const char *octets, int olen, int opos, size_t *res);

int textutil_sscanf(const char *octets, int olen, char *format, ...)
{
	va_list	ap;
	int		fpos = 0;
	int		opos = 0;
	int		nbytes;
	void	*arg, *token;
	int		items_matched = 0;

	va_start(ap, format);
	
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
			token = NULL;
			arg = NULL;
			fpos++;

			if (format[fpos] == '%') {
				// valid but do nothing 
			}
			else if (format[fpos] == 's') {
				fpos++;
				if ((nbytes = __parse_string(octets, olen, opos, &token)) == 0) {
					break;
				}
				opos += nbytes;
				arg = va_arg(ap, char **);
				if (arg != NULL) {
					*(char **)arg = token;
				}
				items_matched++;
			}
			else if (format[fpos] == 'd') {
				fpos++;
				if ((nbytes = __parse_number(octets, olen, opos, &token)) == 0) {
					break;
				}
				opos += nbytes;
				arg = va_arg(ap, int *);
				if (arg != NULL) {
					*(int *)arg = *(ssize_t *)token;
				}
				SSL_FREE(token);
				items_matched++;
			}
			else if (format[fpos] == 'u') {
				fpos++;
				if ((nbytes = __parse_number_u(octets, olen, opos, &token)) == 0) {
					break;
				}
				opos += nbytes;
				arg = va_arg(ap, unsigned int *);
				if (arg != NULL) {
					*(unsigned int *)arg = *(size_t *)token;
				}
				SSL_FREE(token);
				items_matched++;
			}
			else if (format[fpos] == 'c') {
				fpos++;
				arg = va_arg(ap, char *);
				if (arg != NULL) {
					*(char *)arg = octets[opos];
				}
				opos++;
				items_matched++;
			}
			else if (format[fpos] == 'z') {
				fpos++;
				if (format[fpos] == 'd') {
					fpos++;
					if ((nbytes = __parse_number(octets, olen, opos, &token)) == 0) {
						break;
					}
					arg = va_arg(ap, ssize_t *);
					if (arg != NULL) {
						*(ssize_t *)arg = *(ssize_t *)token;
					}
					SSL_FREE(token);
					items_matched++;
				}
				else if (format[fpos] == 'u') {
					fpos++;
					if ((nbytes = __parse_number_u(octets, olen, opos, &token)) == 0) {
						break;
					}
					arg = va_arg(ap, size_t *);
					if (arg != NULL) {
						*(size_t *)arg = *(size_t *)token;
					}
					SSL_FREE(token);
					items_matched++;
				}
			}
			else {
				TEXTUTIL_LOG(ERROR, "Invalid specifier: %c", format[fpos]);
				break;
			}
			SSL_FREE(token);
		}

		if (format[fpos] == octets[opos]) {
			fpos++;
			opos++;
		}
		else {
			break;
		}
	}
	va_end(ap);

	return (items_matched);
}

int __parse_string(const char *octets, int olen, int opos, char **res)
{
	int	start = opos;
	int	end;

	*res = NULL;
	end = start;
	while (opos + end < olen && octets[opos + end] != ' ' && octets[opos + end] != '\0') {
		end++;
	}
	*res = ft_strndup(octets + start, end - start);

	return (end - start);
}

static int __parse_number(const char *octets, int olen, int opos, ssize_t *res)
{
    int start = opos;
    int end;

    while (start < olen && ft_isspace(octets[start])) {
        start++;
    }
    end = start;

    if (end < olen && (octets[end] == '+' || octets[end] == '-')) {
        end++;
    }
    if (end == olen || !ft_isdigit(octets[end])) {
        return (0);
    }
    while (end < olen && ft_isdigit(octets[end])) {
        end++;
    }
    *res = ft_atoi(&octets[start]);

    return (end - opos);
}

static int __parse_number_u(const char *octets, int olen, int opos, size_t *res)
{
    int start = opos;
    int end;

    while (start < olen && ft_isspace(octets[start])) {
        start++;
    }
    end = start;

    if (end == olen || !ft_isdigit(octets[end])) {
        return (0);
    }
    while (end < olen && ft_isdigit(octets[end])) {
        end++;
    }
    *res = ft_atoi_u(&octets[start]);

    return (end - opos);
}