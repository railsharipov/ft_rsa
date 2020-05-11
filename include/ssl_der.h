#ifndef SSL_DER_H
# define SSL_DER_H

typedef struct	s_der
{
	char		*content;
	int			size;
}				t_der;

struct s_iasn;

t_der	*der_init(void);
void	der_del(t_der *);
void	der_clear(t_der *);
int		der_append_sequence(t_der *, void *, int);
int		der_append_bool(t_der *, void *, int);
int		der_append_int(t_der *, void *, int);
int		der_append_ostring(t_der *, void *, int);
int		der_append_bitstring(t_der *, void *, int);
int		der_append_oid(t_der *, void *, int);
int		der_append_null(t_der *, void *, int);
int		der_append_len(char *, int);
int		der_read_sequence(struct s_iasn *, char **, int *);
int		der_read_bool(struct s_iasn *, char **, int *);
int		der_read_int(struct s_iasn *, char **, int *);
int		der_read_ostring(struct s_iasn *, char **, int *);
int		der_read_bitstring(struct s_iasn *, char **, int *);
int		der_read_oid(struct s_iasn *, char **, int *);
int		der_read_null(struct s_iasn *, char **, int *);
int		der_read_len(char **, int *, int *);

#endif
