#include <libft.h>
#include <libft_v2.h>

typedef enum e_prf_fmt_kind
{
	PF_FMT_NONE = 0,
	PF_FMT_LO_I,
	PF_FMT_LO_D,
	PF_FMT_LO_O,
	PF_FMT_LO_U,
	PF_FMT_LO_X,
	PF_FMT_UP_X,
	PF_FMT_LO_P,
	PF_FMT_LO_S,
	PF_FMT_LO_C,
} t_prf_fmt_kind;

typedef struct s_prf_ctx
{
	t_prf_fmt_kind fmt_kind;
	uint32_t flag;
	char *buf;
	size_t bufsize;
	int	bufpos;
	t_io_v2_stream *stream;
	int fd;
	int fwid;
	int prec;
	int written;
	int seen;
} t_prf_ctx;

#define PRF_FLAG_FMT_MASK ((0xFFFFFFFF<<26)>>26)
#define PRF_FLAG_LEN_MASK (((0xFFFFFFFF<<20)>>26)<<6)

typedef enum e_prf_flag {
	PF_SPACE = (1 << 0),
	PF_PLUS	 = (1 << 1),
	PF_MINUS = (1 << 2),
	PF_ZERO	 = (1 << 3),
	PF_SHARP = (1 << 4),
	PF_PREC	 = (1 << 5),
	PF_Z	 = (1 << 6),
	PF_J	 = (1 << 7),
	PF_LL	 = (1 << 8),
	PF_L	 = (1 << 9),
	PF_H	 = (1 << 10),
	PF_HH	 = (1 << 11),
} t_prf_flag;

#define FT_PRINTF_BUFSIZE 4096

typedef void (*__t_func_prf_write)(t_prf_ctx *ctx, const char *s, int slen);
typedef void (*__t_func_prf_flush)(t_prf_ctx *ctx);

static void __prf_core(t_prf_ctx *ctx, const char *fmt, va_list ap, __t_func_prf_write f_write, __t_func_prf_flush f_flush);

static void	__prf_fwrite(t_prf_ctx *ctx, const char *s, int slen);
static void	__prf_dwrite(t_prf_ctx *ctx, const char *s, int slen);
static void	__prf_dflush(t_prf_ctx *ctx);
static void	__prf_bwrite(t_prf_ctx *ctx, const char *s, int slen);
static void	__prf_swrite(t_prf_ctx *ctx, const char *s, int slen);

int ft_printf(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	char buf[FT_PRINTF_BUFSIZE];
	t_prf_ctx ctx = { .fd = 1, .buf = buf, .bufsize = sizeof(buf), };
	__prf_core(&ctx, fmt, ap, __prf_dwrite, __prf_dflush);
	va_end(ap);
	return (ctx.written);
}

int ft_vprintf(const char *fmt, va_list ap)
{
	char buf[FT_PRINTF_BUFSIZE];
	t_prf_ctx ctx = { .fd = 1, .buf = buf, .bufsize = sizeof(buf), };
	__prf_core(&ctx, fmt, ap, __prf_dwrite, __prf_dflush);
	return (ctx.written);
}

int ft_fprintf(t_io_v2_stream *stream, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	t_prf_ctx ctx = { .stream = stream, };
	__prf_core(&ctx, fmt, ap, __prf_fwrite, NULL);
	va_end(ap);
	return (ctx.written);
}

int ft_vfprintf(t_io_v2_stream *stream, const char *fmt, va_list ap)
{
	t_prf_ctx ctx = { .stream = stream, };
	__prf_core(&ctx, fmt, ap, __prf_fwrite, NULL);
	return (ctx.written);
}

int ft_dprintf(int fd, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	char buf[FT_PRINTF_BUFSIZE];
	t_prf_ctx ctx = { .fd = fd, .buf = buf, .bufsize = sizeof(buf), };
	__prf_core(&ctx, fmt, ap, __prf_dwrite, __prf_dflush);
	va_end(ap);
	return (ctx.written);
}

int ft_vdprintf(int fd, const char *fmt, va_list ap)
{
	char buf[FT_PRINTF_BUFSIZE];
	t_prf_ctx ctx = { .fd = fd, .buf = buf, .bufsize = sizeof(buf), };
	__prf_core(&ctx, fmt, ap, __prf_dwrite, __prf_dflush);
	return (ctx.written);
}

int ft_sprintf(char *buf, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	t_prf_ctx ctx = { .buf = buf, .bufsize = INT_MAX, };
	__prf_core(&ctx, fmt, ap, __prf_bwrite, NULL);
	va_end(ap);
	return (ctx.written);
}

int ft_vsprintf(char *buf, const char *fmt, va_list ap)
{
	t_prf_ctx ctx = { .buf = buf, .bufsize = INT_MAX, };
	__prf_core(&ctx, fmt, ap, __prf_bwrite, NULL);
	return (ctx.written);
}

int ft_snprintf(char *buf, size_t size, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	t_prf_ctx ctx = { .buf = buf, .bufsize = size };
	__prf_core(&ctx, fmt, ap, __prf_bwrite, NULL);
	va_end(ap);
	return (ctx.seen);
}

int ft_vsnprintf(char *buf, size_t size, const char *fmt, va_list ap)
{
	t_prf_ctx ctx = { .buf = buf, .bufsize = size };
	__prf_core(&ctx, fmt, ap, __prf_bwrite, NULL);
	return (ctx.seen);
}

int ft_asprintf(char **s, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	t_prf_ctx ctx = { .buf = NULL, .bufsize = 0, };
	__prf_core(&ctx, fmt, ap, __prf_swrite, NULL);
	if (NULL != s) *s = ctx.buf;
	va_end(ap);
	return (ctx.written);
}

int ft_vasprintf(char **s, const char *fmt, va_list ap)
{
	t_prf_ctx ctx = { .buf = NULL, .bufsize = 0, };
	__prf_core(&ctx, fmt, ap, __prf_swrite, NULL);
	if (NULL != s) *s = ctx.buf;
	return (ctx.written);
}

static void __prf_core(t_prf_ctx *ctx, const char *fmt, va_list ap, __t_func_prf_write f_write, __t_func_prf_flush f_flush)
{
	if (NULL != fmt) {
		int i = 0;
		while (fmt[i] != '\0') {
			char conv = 0;
			int start = i;

			ctx->fmt_kind = PF_FMT_NONE;
			ctx->flag = 0;
			ctx->prec = 0;
		 	ctx->fwid = 0;

			while (fmt[i] && fmt[i] != '%') i++;

			if (i > start) {
				f_write(ctx, fmt + start, i - start);
			}

			if (fmt[i] == '%' && fmt[++i]) {
				// Literal %.
				if (fmt[i] == '%') {
					i++;
					f_write(ctx, "%", 1);
					continue;
				}

				// Optional flags.
				while (fmt[i] != '\0') {
					if (fmt[i] == '#')        ctx->flag |= PF_SHARP;
					else if (fmt[i] == '0')   ctx->flag |= PF_ZERO;
					else if (fmt[i] == '-')   ctx->flag |= PF_MINUS;
					else if (fmt[i] == '+')   ctx->flag |= PF_PLUS;
					else if (fmt[i] == ' ')   ctx->flag |= PF_SPACE;
					else break;
					i++;
				}

				// Optional field width.
				if (fmt[i] >= '1' && fmt[i] <= '9') {
					intmax_t n = ft_atoi_zd(fmt + i);
					ctx->fwid = (n > INT_MAX) ? INT_MAX : (int)n;
					while (fmt[i] >= '0' && fmt[i] <= '9') i++;
				}
				else if (fmt[i] == '*') {
					ctx->fwid = va_arg(ap, int);
					if (ctx->fwid < 0) {
						ctx->flag |= PF_MINUS;
						ctx->fwid = (ctx->fwid == INT_MIN) ? INT_MAX : -ctx->fwid;
					}
					i++;
				}

				// Optional precision.
				if (fmt[i] == '.') {
					ctx->flag |= PF_PREC;
					i++;
					if (fmt[i] >= '0' && fmt[i] <= '9') {
						intmax_t n = ft_atoi_zd(fmt + i);
						ctx->prec = (n > INT_MAX) ? INT_MAX : (int)n;
						while (fmt[i] >= '0' && fmt[i] <= '9') i++;
					}
					else if (fmt[i] == '*') {
						ctx->prec = (int)va_arg(ap, int);
						if (ctx->prec < 0) {
							ctx->prec = 0;
							ctx->flag &= ~PF_PREC;
						}
						i++;
					}
				}

				// Optional length.
				if (fmt[i] == 'h' && fmt[i+1] == 'h')      { ctx->flag |= PF_HH; i += 2; }
				else if (fmt[i] == 'h')                    { ctx->flag |= PF_H;  i += 1; }
				else if (fmt[i] == 'l' && fmt[i+1] == 'l') { ctx->flag |= PF_LL; i += 2; }
				else if (fmt[i] == 'l')                    { ctx->flag |= PF_L;  i += 1; }
				else if (fmt[i] == 'j')                    { ctx->flag |= PF_J;  i += 1; }
				else if (fmt[i] == 'z')                    { ctx->flag |= PF_Z;  i += 1; }

				// Conversion.
				if (fmt[i]) {
					conv = fmt[i];
				} else {
					break;
				}
				i++;

				// Conversion kind.
				if (conv == 'i') ctx->fmt_kind = PF_FMT_LO_I;
				else if (conv == 'd') ctx->fmt_kind = PF_FMT_LO_D;
				else if (conv == 'o') ctx->fmt_kind = PF_FMT_LO_O;
				else if (conv == 'u') ctx->fmt_kind = PF_FMT_LO_U;
				else if (conv == 'x') ctx->fmt_kind = PF_FMT_LO_X;
				else if (conv == 'X') ctx->fmt_kind = PF_FMT_UP_X;
				else if (conv == 'p') ctx->fmt_kind = PF_FMT_LO_P;
				else if (conv == 's') ctx->fmt_kind = PF_FMT_LO_S;
				else if (conv == 'c') ctx->fmt_kind = PF_FMT_LO_C;
				else ctx->fmt_kind = PF_FMT_NONE;

				// Handle flags.
				t_prf_fmt_kind kind = ctx->fmt_kind;

				if (kind == PF_FMT_LO_P) {
					// Handle as unsigned integer with `l` length form.
					ctx->flag &= ~PRF_FLAG_LEN_MASK;
					ctx->flag |= PF_L;
				}
				if (ctx->flag & PF_PLUS) {
					// Plus overrides space.
					ctx->flag &= (~PF_SPACE);
				}
				if (ctx->flag & PF_MINUS) {
					// Minus overrides zero.
					ctx->flag &= (~PF_ZERO);
				}
				if (ctx->flag & PF_PREC) {
					// Precision overrides zero.
					ctx->flag &= (~PF_ZERO);
				}

				// Process.
				switch (kind) {
				case PF_FMT_LO_I:
				case PF_FMT_LO_D: {
					intmax_t val = 0;
					if (ctx->flag & PF_HH)       val = (short)va_arg(ap, int);
					else if (ctx->flag & PF_H)   val = (char)va_arg(ap, int);
					else if (ctx->flag & PF_LL)  val = va_arg(ap, long long);
					else if (ctx->flag & PF_L)   val = va_arg(ap, long);
					else if (ctx->flag & PF_J)   val = va_arg(ap, intmax_t);
					else if (ctx->flag & PF_Z)   val = va_arg(ap, size_t);
					else                        val = va_arg(ap, int);

					// [left spaces][sign][precision zeros / 0-flag zeros][digits][right spaces]
					char digits[32];
					int ndigs = 0;
					uintmax_t mag = (val < 0) ? (uintmax_t)0 - (uintmax_t)val : (uintmax_t)val;
					char sign = (val < 0) ? '-' : (ctx->flag & PF_PLUS) ? '+' : (ctx->flag & PF_SPACE) ? ' ' : 0;
					int zeros, len, pad;

					int pos = 32;
					if (!(mag == 0 && ctx->flag & PF_PREC && ctx->prec == 0)) {   // "%.0d" of 0 prints no digits
						do {
							digits[--pos] = (char)('0' + mag % 10);
							mag /= 10;
						} while (mag != 0);
					}
					ndigs = 32 - pos;

					zeros = (ctx->prec > ndigs) ? ctx->prec - ndigs : 0;
					len = (sign ? 1 : 0) + zeros + ndigs;
					if (ctx->flag & PF_ZERO && ctx->fwid > len) {
						zeros += ctx->fwid - len;
						len = ctx->fwid;
					}
					pad = (ctx->fwid > len) ? ctx->fwid - len : 0;

					if (!(ctx->flag & PF_MINUS)) for (int j = 0; j < pad; j++) f_write(ctx, " ", 1);
					if (sign) f_write(ctx, &sign, 1);
					for (int j = 0; j < zeros; j++) f_write(ctx, "0", 1);
					f_write(ctx, digits + pos, ndigs);
					if (ctx->flag & PF_MINUS) for (int j = 0; j < pad; j++) f_write(ctx, " ", 1);
					break;
				}
				case PF_FMT_LO_O:
				case PF_FMT_LO_P:
				case PF_FMT_LO_X:
				case PF_FMT_UP_X:
				case PF_FMT_LO_U: {
					uintmax_t val = 0;
					if (ctx->flag & PF_HH)       val = (unsigned short)va_arg(ap, unsigned int);
					else if (ctx->flag & PF_H)   val = (unsigned char)va_arg(ap, unsigned int);
					else if (ctx->flag & PF_LL)  val = va_arg(ap, unsigned long long);
					else if (ctx->flag & PF_L)   val = va_arg(ap, unsigned long);
					else if (ctx->flag & PF_J)   val = va_arg(ap, uintmax_t);
					else if (ctx->flag & PF_Z)   val = va_arg(ap, size_t);
					else                        val = va_arg(ap, unsigned int);

					// [left spaces][prefix][precision zeros / 0-flag zeros][digits][right spaces]
					char digits[32];
					int ndigs = 0;
					uintmax_t mag = val;
					uintmax_t base = 10;
					const char *prefix = "";
					int prefix_len = 0;
					char first_hex_ch = (kind == PF_FMT_UP_X) ? 'A' : 'a';
					int zeros, len, pad;

					if (kind == PF_FMT_LO_X || kind == PF_FMT_UP_X) {
						if (ctx->flag & PF_SHARP && mag != 0) {
							prefix = (kind == PF_FMT_UP_X) ? "0X" : "0x";
							prefix_len = 2;
						}
						base = 16;
					}
					else if (kind == PF_FMT_LO_P) {
						prefix = "0x";
						prefix_len = 2;
						base = 16;
					}
					else if (kind == PF_FMT_LO_O) {
						if (ctx->flag & PF_SHARP) {
							prefix = "0";
							prefix_len = 1;
						}
						base = 8;
					}

					int pos = 32;
					if (!(mag == 0 && ctx->flag & PF_PREC && ctx->prec == 0)) {   // "%.0d" of 0 prints no digits
						do {
							uintmax_t d = mag % base;
							digits[--pos] = (d < 10) ? '0' + d : first_hex_ch + d-10;
							mag /= base;
						} while (mag != 0);
					}
					ndigs = 32 - pos;

					zeros = (ctx->prec > ndigs) ? ctx->prec - ndigs : 0;
					if (kind == PF_FMT_LO_O) {
						// If there are already precision zeros, '0' prefix is not added.
						zeros = MAX(zeros, prefix_len);
					}

					len = prefix_len + zeros + ndigs;
					if (ctx->flag & PF_ZERO && ctx->fwid > len) {
						zeros += ctx->fwid - len;
						len = ctx->fwid;
					}
					pad = (ctx->fwid > len) ? ctx->fwid - len : 0;

					if (!(ctx->flag & PF_MINUS)) for (int j = 0; j < pad; j++) f_write(ctx, " ", 1);
					if (prefix_len) f_write(ctx, prefix, prefix_len);
					for (int j = 0; j < zeros; j++) f_write(ctx, "0", 1);
					f_write(ctx, digits + pos, ndigs);
					if (ctx->flag & PF_MINUS) for (int j = 0; j < pad; j++) f_write(ctx, " ", 1);
					break;
				}
				case PF_FMT_LO_S: {
					char *s = va_arg(ap, char *);
					int len;
					int pad;

					if (NULL == s) {
						s = "(null)";
					}
					if (ctx->flag & PF_PREC) {
						len = 0;
						for (int j = 0; j < ctx->prec && fmt[j]; j++) len++;
					} else {
						len = ft_strlen(s);
					}
					pad = (ctx->fwid > len) ? ctx->fwid - len : 0;

					if (!(ctx->flag & PF_MINUS)) for (int j = 0; j < pad; j++) f_write(ctx, " ", 1);
					f_write(ctx, s, len);
					if (ctx->flag & PF_MINUS) for (int j = 0; j < pad; j++) f_write(ctx, " ", 1);
					break;
				}
				case PF_FMT_LO_C: {
					char c = (char)va_arg(ap, int);
					size_t len;
					int pad;

					len = 1;
					pad = MAX(ctx->fwid - len, 0);

					if (!(ctx->flag & PF_MINUS)) for (int j = 0; j < pad; j++) f_write(ctx, " ", 1);
					f_write(ctx, &c, 1);
					if (ctx->flag & PF_MINUS) for (int j = 0; j < pad; j++) f_write(ctx, " ", 1);
					break;
				}
				case PF_FMT_NONE: {
					f_write(ctx, &conv, 1);
					break;
				}}
			}
		}
	}
	// Flush buffer.
	if (NULL != f_flush) f_flush(ctx);
}

static void	__prf_fwrite(t_prf_ctx *ctx, const char *s, int slen)
{
	ssize_t wbytes = io_v2_write_all(ctx->stream, s, slen);
	if (wbytes != slen) ctx->written = -1;
	if (ctx->written != -1) {
		if (INT_MAX - ctx->written < slen) ctx->written = -1;
		else ctx->written += slen;
	}
	ctx->seen = ctx->written;
}

static void	__prf_dwrite(t_prf_ctx *ctx, const char *s, int slen)
{
	if (slen > ctx->bufsize) {
		write(ctx->fd, ctx->buf, ctx->bufpos);
		write(ctx->fd, s, slen);
		ctx->bufpos = 0;
	}
	else if (ctx->bufpos + slen > ctx->bufsize) {
		write(ctx->fd, ctx->buf, ctx->bufpos);
		for (int i = 0; i < slen; i++) ctx->buf[i] = s[i];
		ctx->bufpos = slen;
	}
	else {
		for (int i = 0; i < slen; i++) ctx->buf[i+ctx->bufpos] = s[i];
		ctx->bufpos += slen;
	}
	if (ctx->written != -1) {
		if (INT_MAX - ctx->written < slen) ctx->written = -1;
		else ctx->written += slen;
	}
	ctx->seen = ctx->written;
}

static void	__prf_dflush(t_prf_ctx *ctx)
{
	write(ctx->fd, ctx->buf, ctx->bufpos);
}

static void	__prf_bwrite(t_prf_ctx *ctx, const char *s, int slen)
{
	int left = ctx->bufsize - ctx->bufpos;
	if (left) {
		int tw = MIN(slen, left-1);
		for (int i = 0; i < tw; i++) ctx->buf[i+ctx->bufpos] = s[i];
		ctx->bufpos += tw;
		ctx->buf[ctx->bufpos] = 0;
		if (ctx->written != -1) {
			if (INT_MAX - ctx->written < tw) ctx->written = -1;
			else ctx->written += tw;
		}
	}
	ctx->seen += slen;
}

static void	__prf_swrite(t_prf_ctx *ctx, const char *s, int slen)
{
	if (ctx->bufpos + slen+1 > ctx->bufsize) {
		size_t newsize = ctx->bufsize + slen+1 + FT_PRINTF_BUFSIZE;
		LIBFT_REALLOC(ctx->buf, ctx->bufsize, newsize);
		ctx->bufsize = newsize;
	}
	for (int i = 0; i < slen; i++) ctx->buf[i+ctx->bufpos] = s[i];
	ctx->bufpos += slen;
	ctx->buf[ctx->bufpos] = 0;

	if (ctx->written != -1) {
		if (INT_MAX - ctx->written < slen) ctx->written = -1;
		else ctx->written += slen;
	}
	ctx->seen = ctx->written;
}
