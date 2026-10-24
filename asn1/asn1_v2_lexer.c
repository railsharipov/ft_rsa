#include <common.h>
#include <logger.h>
#include <asn1.h>
#include <libft.h>
#include <textutil.h>

typedef enum e_asn_v2_token_kind {
	ASN_V2_TOKEN_KIND_SINGLE_CHAR = 1,
	ASN_V2_TOKEN_KIND_KEYWORD,
	ASN_V2_TOKEN_KIND_NAME,
	ASN_V2_TOKEN_KIND_COMMENT,
	ASN_V2_TOKEN_KIND_NUMBER,
	ASN_V2_TOKEN_KIND_BSTRING,
	ASN_V2_TOKEN_KIND_HSTRING,
	ASN_V2_TOKEN_KIND_CSTRING,
	ASN_V2_TOKEN_KIND_ASSIGNMENT,
	ASN_V2_TOKEN_KIND_RANGE,
	ASN_V2_TOKEN_KIND_ELLIPSIS,
	ASN_V2_TOKEN_KIND_LVER_BRACKET,
	ASN_V2_TOKEN_KIND_RVER_BRACKET,
} t_asn_v2_token_kind;

const char *RESERVED_KEYWORDS[] = {
	"ABSENT", "ENCODED", "INTERSECTION", "SEQUENCE", "ABSTRACT-SYNTAX", "ENCODING-CONTROL", "ISO646String",
	"SET", "ALL", "END", "MAX", "SETTINGS", "APPLICATION", "ENUMERATED", "MIN", "SIZE",
	"AUTOMATIC", "EXCEPT", "MINUS-INFINITY", "STRING", "BEGIN", "EXPLICIT", "NOT-A-NUMBER", "SYNTAX",
	"BIT", "EXPORTS", "NULL", "T61String", "BMPString", "EXTENSIBILITY", "NumericString", "TAGS",
	"BOOLEAN", "EXTERNAL", "OBJECT", "TeletexString", "BY", "FALSE", "ObjectDescriptor", "TIME",
	"CHARACTER", "FROM", "OCTET", "TIME-OF-DAY", "CHOICE", "GeneralizedTime", "OF", "TRUE",
	"CLASS", "GeneralString", "OID-IRI", "TYPE-IDENTIFIER", "COMPONENT", "GraphicString", "OPTIONAL", "UNION",
	"COMPONENTS", "IA5String", "PATTERN", "UNIQUE", "CONSTRAINED", "IDENTIFIER", "PDV", "UNIVERSAL",
	"CONTAINING", "IMPLICIT", "PLUS-INFINITY", "UniversalString", "DATE", "IMPLIED", "PRESENT", "UTCTime",
	"DATE-TIME", "IMPORTS", "PrintableString", "UTF8String", "DEFAULT", "INCLUDES", "PRIVATE", "VideotexString",
	"DEFINITIONS", "INSTANCE", "REAL", "VisibleString", "DURATION", "INSTRUCTIONS", "RELATIVE-OID", "WITH",
	"EMBEDDED", "INTEGER", "RELATIVE-OID-IRI",
};
const size_t RESERVED_KEYWORD_COUNT = sizeof(RESERVED_KEYWORDS)/sizeof(*RESERVED_KEYWORDS);

const char SINGLE_CHAR_LEXES[] = {
	'{', '}', '<', '>', ',', '.', '/', '(', ')', '[', ']', '-', ':', '=', '"', '\'', ';', '@', '|', '!', '^'
};
const size_t SINGLE_CHAR_LEX_COUNT = sizeof(SINGLE_CHAR_LEXES)/sizeof(*SINGLE_CHAR_LEXES);
