#ifndef ASN_1_PRIVATE_H
#define ASN_1_PRIVATE_H

#include <asn1_v2.h>

static inline void __asn1_v2_oid_arc_delete_adapter(void *vctx) { asn1_v2_oid_arc_delete((t_asn_v2_oid_arc *)vctx); }
static inline void __asn1_v2_symbol_delete_adapter(void *p)		{ asn1_v2_symbol_delete((t_asn_v2_symbol *)p); }
static inline void __asn1_v2_tag_delete_adapter(void *p)		{ asn1_v2_tag_delete((t_asn_v2_tag *)p); }
static inline void __asn1_v2_value_delete_adapter(void *p)		{ asn1_v2_value_delete((t_asn_v2_value *)p); }
static inline void __asn1_v2_constraint_delete_adapter(void *p)	{ asn1_v2_constraint_delete((t_asn_v2_constraint *)p); }
static inline void __asn1_v2_enum_delete_adapter(void *p)		{ asn1_v2_enum_delete((t_asn_v2_enum *)p); }
static inline void __asn1_v2_type_delete_adapter(void *p)	 	{ asn1_v2_type_delete((t_asn_v2_type *)p); }
static inline void __asn1_v2_component_delete_adapter(void *p)	{ asn1_v2_component_delete((t_asn_v2_component *)p); }
static inline void __asn1_v2_module_delete_adapter(void *p)		{ asn1_v2_module_delete((t_asn_v2_module *)p); }

static void *__asn1_v2_oid_arc_copy_adapter(const void *vctx)
{
	t_asn_v2_oid_arc *dst = asn1_v2_oid_arc_create();
	asn1_v2_oid_arc_copy((t_asn_v2_oid_arc *)vctx, dst);
	return (dst);
}
static inline void *__asn1_v2_symbol_copy_adapter(const void *p)
{
	t_asn_v2_symbol *dst = asn1_v2_symbol_create();
	asn1_v2_symbol_copy((t_asn_v2_symbol *)p, dst);
	return (dst);
}
static inline void *__asn1_v2_tag_copy_adapter(const void *p)
{
	t_asn_v2_tag *dst = asn1_v2_tag_create();
	asn1_v2_tag_copy((t_asn_v2_tag *)p, dst);
	return (dst);
}
static inline void *__asn1_v2_constraint_copy_adapter(const void *p)
{
	t_asn_v2_constraint *dst = asn1_v2_constraint_create();
	asn1_v2_constraint_copy((const t_asn_v2_constraint *)p, dst);
	return (dst);
}
static inline void *__asn1_v2_enum_copy_adapter(const void *p)
{
	t_asn_v2_enum *dst = asn1_v2_enum_create();
	asn1_v2_enum_copy((const t_asn_v2_enum *)p, dst);
	return (dst);
}
static inline void *__asn1_v2_value_copy_adapter(const void *p)
{
	t_asn_v2_value *dst = asn1_v2_value_create();
	asn1_v2_value_copy((const t_asn_v2_value *)p, dst);
	return (dst);
}
static inline void *__asn1_v2_type_copy_adapter(const void *p)
{
	t_asn_v2_type *dst = asn1_v2_type_create();
	asn1_v2_type_copy((const t_asn_v2_type *)p, dst);
	return (dst);
}
static inline void *__asn1_v2_component_copy_adapter(const void *p)
{
	t_asn_v2_component *dst = asn1_v2_component_create();
	asn1_v2_component_copy((const t_asn_v2_component *)p, dst);
	return (dst);
}
static inline void *__asn1_v2_module_copy_adapter(const void *p)
{
	t_asn_v2_module *dst = asn1_v2_module_create();
	asn1_v2_module_copy((const t_asn_v2_module *)p, dst);
	return (dst);
}

static inline void __der_v2_tag_delete_adapter(void *p) { der_v2_tag_delete((t_der_v2_tag *)p); };
static inline void __der_v2_value_delete_adapter(void *p) { der_v2_value_delete((t_der_v2_value *)p); };
static inline void __der_v2_type_delete_adapter(void *p) { der_v2_type_delete((t_der_v2_type *)p); };
static inline void __der_v2_component_delete_adapter(void *p) { der_v2_component_delete((t_der_v2_component *)p); };

#endif
