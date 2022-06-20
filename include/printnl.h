#ifndef PRINTNL_H
# define PRINTNL_H

# include <stdio.h>

# define printf_dec_format(x)	_Generic((x), \
	char: "%c", \
	const char: "%c", \
	signed char: "%hhd", \
	const signed char: "%hhd", \
	unsigned char: "%hhu", \
	const unsigned char: "%hhu", \
	signed short: "%hd", \
	const signed short: "%hd", \
	unsigned short: "%hu", \
	const unsigned short: "%hu", \
	signed int: "%d", \
	const signed int: "%d", \
	unsigned int: "%u", \
	const unsigned int: "%u", \
	long int: "%ld", \
	const long int: "%ld", \
	unsigned long int: "%lu", \
	const unsigned long int: "%lu", \
	long long int: "%lld", \
	const long long int: "%lld", \
	unsigned long long int: "%llu", \
	const unsigned long long int: "%llu", \
	float: "%f", \
	const float: "%f", \
	double: "%f", \
	const double: "%f", \
	long double: "%Lf", \
	const long double: "%Lf", \
	char *: "%s", \
	const char *: "%s", \
	void *: "%p", \
	const void *: "%p")

# define print(x)		printf(printf_dec_format(x), x)
# define printnl(x)		printf(printf_dec_format(x), x), printf("\n");

#endif
