#include <stdarg.h>
#include <unistd.h>
#include <libft/string.h>
#include <libft/alloc.h>

char	*ft_strncat(char *s1, const char *s2, size_t n)
{
	size_t	i;
	size_t	j;

	if (NULL == s1 || NULL == s2)
		return (s1);

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

	if (NULL != s)
	{
		while (s[len])
			++len;
	}

	return (len);
}

int	ft_str_isdigit(const char *s)
{
	if (s == NULL)
		return (0);

	while (*s != 0)
		if (!ft_isdigit(*s++))
			return (0);

	return (1);
}

int	ft_str_isprint(const char *s)
{
	if (s == NULL)
		return (0);

	while (*s != 0)
		if (!ft_isprint(*s++))
			return (0);

	return (1);
}

static int __get_nwords(char const *s, char c);

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
		nwords = num_delims + 2;
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

	if (NULL == s)
		return (NULL);

	i = 0;
	slen = ft_strlen(s);

	while (i < slen)
	{
		if (s[i] == (char)c)
			return ((char *)(s + i));

		i++;
	}

	return (NULL);
}

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*res;
	size_t	s1len;
	size_t	s2len;

	if (NULL == s1 && NULL == s2)
		return (NULL);

	if (NULL == s1 || NULL == s2)
	{
		if (NULL == s1)
			return (ft_strdup(s2));
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

	if (NULL == s1 || NULL == s2)
		return (s1);

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
	if (s == NULL)
		return (0);

	while (*s != 0)
		if (!ft_isascii(*s++))
			return (0);

	return (1);
}

void	ft_strdel(char *s)
{
	if (NULL == s)
		return ;

	LIBFT_FREE(s);
}

char	*ft_strrchr(const char *s, int c)
{
	ssize_t	slen;

	if (NULL == s)
		return (NULL);

	slen = ft_strlen(s);

	while (--slen >= 0)
	{
		if (s[slen] == (char)c)
			return ((char *)(s + slen));
	}

	return (NULL);
}

int	ft_str_ishex(const char *s)
{
	if (s == NULL)
		return (0);

	while (*s != 0)
		if (!ft_ishex(*s++))
			return (0);

	return (1);
}

char	*ft_strlow(char *s)
{
	char	*s_low;
	size_t	idx;

	if (NULL == s)
		return (NULL);

	s_low = ft_strdup(s);

	idx = 0;
	while (s_low[idx] != '\0')
	{
		if (s_low[idx] >= 'A' && s_low[idx] <= 'Z')
			s_low[idx] = s_low[idx] + 32;

		idx++;
	}

	return (s_low);
}

int		ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t i;

	// no null check, same as libc strcmp

	i = 0;
	while ((i < n) && (s1[i] || s2[i]))
	{
		if (s1[i] != s2[i])
			return (s1[i] - s2[i]);
		i++;
	}
	return (0);
}

char	*ft_strcpy(char *dst, const char *src)
{
	size_t	i;

	if (NULL == dst || NULL == src)
		return (dst);

	i = 0;
	while (src[i] != 0)
	{
		dst[i] = src[i];
		++i;
	}
	dst[i] = 0;

	return (dst);
}

char	*ft_strndup(const char *s, size_t n)
{
	char	*sdup;
	size_t	len;
	size_t	slen;
	size_t	idx;

	if (NULL == s) {
		return (NULL);
	}
	slen = ft_strlen(s);
	len = MIN(n, slen);

	LIBFT_ALLOC(sdup, len + 1);

	if (NULL == sdup) {
		return (NULL);
	}
	idx = 0;
	while (idx < len) {
		sdup[idx] = s[idx];
		idx++;
	}
	sdup[len] = 0;

	return (sdup);
}

int	ft_str_isalpha(const char *s)
{
	if (s == NULL)
		return (0);

	while (*s != 0)
		if (!ft_isalpha(*s++))
			return (0);

	return (1);
}

int	ft_str_isalnum(const char *s)
{
	if (s == NULL)
		return (0);

	while (*s != 0)
		if (!ft_isalnum(*s++))
			return (0);

	return (1);
}

char *ft_strup(char *s)
{
	char *s_up;
	size_t idx;

	if (NULL == s)
		return (NULL);

	s_up = ft_strdup(s);

	idx = 0;
	while (s_up[idx] != '\0')
	{
		if (s_up[idx] >= 'a' && s_up[idx] <= 'z')
			s_up[idx] = s_up[idx] - 32;

		idx++;
	}

	return (s_up);
}

char	*ft_strrev(const char *s)
{
	char	*srev;
	size_t	len;
	int		idx;

	if (NULL == s)
		return (NULL);

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

	if (NULL == s)
		return (NULL);

	LIBFT_ALLOC(sdup, ft_strlen(s) + 1);

	if (NULL == sdup)
		return (NULL);

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

	if (NULL == s)
		return (NULL);

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
	size_t	slen;

	if (NULL == dst || NULL == src)
		return (dst);

	slen = ft_strlen(src);
	len = MIN(slen, len);

	i = 0;
	while (i < len)
	{
		dst[i] = src[i];
		i++;
	}
	dst[i] = 0;

	return (dst);
}

int		ft_strcmp(const char *s1, const char *s2)
{
	size_t i;

	// no null check, same as libc strcmp

	i = 0;
	while (s1[i] || s2[i])
	{
		if (s1[i] != s2[i])
			return (s1[i] - s2[i]);
		i++;
	}

	return (s1[i] - s2[i]);
}

int	ft_tolower(int c)
{
	if (c >= 65 && c <= 90)
		return (c + 32);

	return (c);
}

int	ft_toupper(int c)
{
	if (c >= 97 && c <= 122)
		return (c - 32);

	return (c);
}

int	ft_ishex(int c)
{
	if (ft_isdigit(c) || (c >= 65 && c <= 70) || (c >= 97 && c <= 102))
		return (1);

	return (0);
}

int	ft_iseol(int c)
{
	if ((c == 10) || (c == 13))
		return (1);

	return (0);
}

int	ft_isdigit(int c)
{
	if (c >= 48 && c <= 57)
		return (1);

	return (0);
}

int	ft_iswspace(int c)
{
	if ((c != 10) && ((c >= 9 && c < 13) || (c == 32)))
		return (1);

	return (0);
}

int	ft_isblank(int c)
{
	if ((c == 9) || (c == 32))
		return (1);

	return (0);
}

int	ft_iseolws(int c)
{
	if ((c >= 9 && c <= 13) || (c == 32))
		return (1);

	return (0);
}

int	ft_isascii(int c)
{
	if (c >= 0 && c <= 127)
		return (1);

	return (0);
}

int	ft_isprint(int c)
{
	if (c >= 32 && c <= 126)
		return (1);

	return (0);
}

int	ft_isalpha(int c)
{
	if ((c >= 65 && c <= 90) || (c >= 97 && c <= 122))
		return (1);

	return (0);
}

int	ft_isalnum(int c)
{
	if (ft_isalpha(c) || ft_isdigit(c))
		return (1);

	return (0);
}

void	ft_putstr_fd(int fd, char const *s)
{
	if (NULL != s && fd >= 0)
		write(fd, s, ft_strlen(s));
}

void	ft_putstr(char const *s)
{
	if (NULL != s)
		write(1, s, ft_strlen(s));
}