# ASN1 Type

Below is the ASN.1 **type grammar**, including every alternative of `Type`, the built-in type forms, references, tagging, constraints at the type level, and component/alternative lists. [itu](https://www.itu.int/rec/dologin_pub.asp?lang=e&id=T-REC-X.680-202102-I!!PDF-E&type=items)

## 1. Top-level classification

```asn1
Type ::=
    BuiltinType
  | ReferencedType
  | ConstrainedType

BuiltinType ::=
    BitStringType
  | BooleanType
  | CharacterStringType
  | ChoiceType
  | DateType
  | DateTimeType
  | DurationType
  | EmbeddedPDVType
  | EnumeratedType
  | ExternalType
  | InstanceOfType
  | IntegerType
  | IRIType
  | NullType
  | ObjectClassFieldType
  | ObjectIdentifierType
  | OctetStringType
  | PrefixedType
  | RealType
  | RelativeIRIType
  | RelativeOIDType
  | SequenceType
  | SequenceOfType
  | SetType
  | SetOfType
  | TimeType
  | TimeOfDayType

ReferencedType ::=
    DefinedType
  | UsefulType
  | SelectionType
  | TypeFromObject
  | ValueSetFromObjects
```

These categories are grammar classifications. In particular, `BuiltinType` includes constructed and prefixed types, not just primitive types. [itu](https://www.itu.int/rec/dologin_pub.asp?lang=e&id=T-REC-X.680-202102-I!!PDF-E&type=items)

## 2. Simple built-in types

```asn1
BooleanType ::= BOOLEAN

RealType ::= REAL

OctetStringType ::= OCTET STRING

NullType ::= NULL

ObjectIdentifierType ::= OBJECT IDENTIFIER

RelativeOIDType ::= RELATIVE-OID

IRIType ::= OID-IRI

RelativeIRIType ::= RELATIVE-OID-IRI

EmbeddedPDVType ::= EMBEDDED PDV

ExternalType ::= EXTERNAL

TimeType ::= TIME

DateType ::= DATE

TimeOfDayType ::= TIME-OF-DAY

DateTimeType ::= DATE-TIME

DurationType ::= DURATION
```

These productions specify the type notation; the corresponding value notations are separate. [itu](https://www.itu.int/rec/dologin_pub.asp?lang=e&id=T-REC-X.680-202102-I!!PDF-E&type=items)

## 3. Integer, bit string, and enumerated types

### Integer

```asn1
IntegerType ::=
    INTEGER
  | INTEGER "{" NamedNumberList "}"

NamedNumberList ::=
    NamedNumber
  | NamedNumberList "," NamedNumber

NamedNumber ::=
    identifier "(" SignedNumber ")"
  | identifier "(" DefinedValue ")"

SignedNumber ::=
    number
  | "-" number
```

### Bit string

```asn1
BitStringType ::=
    BIT STRING
  | BIT STRING "{" NamedBitList "}"

NamedBitList ::=
    NamedBit
  | NamedBitList "," NamedBit

NamedBit ::=
    identifier "(" number ")"
  | identifier "(" DefinedValue ")"
```

### Enumerated

```asn1
EnumeratedType ::=
    ENUMERATED "{" Enumerations "}"

Enumerations ::=
    RootEnumeration
  | RootEnumeration "," "..." ExceptionSpec
  | RootEnumeration "," "..." ExceptionSpec "," AdditionalEnumeration

RootEnumeration ::= Enumeration

AdditionalEnumeration ::= Enumeration

Enumeration ::=
    EnumerationItem
  | EnumerationItem "," Enumeration

EnumerationItem ::=
    identifier
  | NamedNumber
```

`DefinedValue` and `ExceptionSpec` are dependencies on the value/reference and exception grammars. [itu](https://www.itu.int/rec/dologin_pub.asp?lang=e&id=T-REC-X.680-202102-I!!PDF-E&type=items)

## 4. `SEQUENCE` and `SET`

```asn1
SequenceType ::=
    SEQUENCE "{" "}"
  | SEQUENCE "{" ExtensionAndException OptionalExtensionMarker "}"
  | SEQUENCE "{" ComponentTypeLists "}"

SetType ::=
    SET "{" "}"
  | SET "{" ExtensionAndException OptionalExtensionMarker "}"
  | SET "{" ComponentTypeLists "}"
```

Both use the following component-list grammar:

```asn1
ComponentTypeLists ::=
    RootComponentTypeList
  | RootComponentTypeList ","
      ExtensionAndException
      ExtensionAdditions
      OptionalExtensionMarker
  | RootComponentTypeList ","
      ExtensionAndException
      ExtensionAdditions
      ExtensionEndMarker ","
      RootComponentTypeList
  | ExtensionAndException
      ExtensionAdditions
      ExtensionEndMarker ","
      RootComponentTypeList
  | ExtensionAndException
      ExtensionAdditions
      OptionalExtensionMarker

RootComponentTypeList ::= ComponentTypeList

ComponentTypeList ::=
    ComponentType
  | ComponentTypeList "," ComponentType

ComponentType ::=
    NamedType
  | NamedType OPTIONAL
  | NamedType DEFAULT Value
  | COMPONENTS OF Type

NamedType ::= identifier Type
```

Extension additions are:

```asn1
ExtensionAdditions ::=
    "," ExtensionAdditionList
  | empty

ExtensionAdditionList ::=
    ExtensionAddition
  | ExtensionAdditionList "," ExtensionAddition

ExtensionAddition ::=
    ComponentType
  | ExtensionAdditionGroup

ExtensionAdditionGroup ::=
    "[[" VersionNumber ComponentTypeList "]]"

VersionNumber ::=
    number ":"
  | empty

ExtensionEndMarker ::= "," "..."

ExtensionAndException ::= "..." ExceptionSpec

OptionalExtensionMarker ::=
    "," "..."
  | empty
```

Here, `COMPONENTS OF` is an entry in the component list and accepts a general `Type` operand. `Value` after `DEFAULT` is another grammar dependency. [itu](https://www.itu.int/rec/dologin_pub.asp?lang=e&id=T-REC-X.680-202102-I!!PDF-E&type=items)

## 5. `SEQUENCE OF` and `SET OF`

The unconstrained forms are:

```asn1
SequenceOfType ::=
    SEQUENCE OF Type
  | SEQUENCE OF NamedType

SetOfType ::=
    SET OF Type
  | SET OF NamedType
```

For example, these are distinct syntactic forms:

```asn1
SEQUENCE OF INTEGER
SEQUENCE OF item INTEGER
```

Constraints written before `OF` are handled by `TypeWithConstraint`, shown below—not by these productions. [itu](https://www.itu.int/rec/dologin_pub.asp?lang=e&id=T-REC-X.680-202102-I!!PDF-E&type=items)

## 6. `CHOICE`

```asn1
ChoiceType ::= CHOICE "{" AlternativeTypeLists "}"

AlternativeTypeLists ::=
    RootAlternativeTypeList
  | RootAlternativeTypeList ","
      ExtensionAndException
      ExtensionAdditionAlternatives
      OptionalExtensionMarker

RootAlternativeTypeList ::= AlternativeTypeList

AlternativeTypeList ::=
    NamedType
  | AlternativeTypeList "," NamedType

ExtensionAdditionAlternatives ::=
    "," ExtensionAdditionAlternativesList
  | empty

ExtensionAdditionAlternativesList ::=
    ExtensionAdditionAlternative
  | ExtensionAdditionAlternativesList "," ExtensionAdditionAlternative

ExtensionAdditionAlternative ::=
    NamedType
  | ExtensionAdditionAlternativesGroup

ExtensionAdditionAlternativesGroup ::=
    "[[" VersionNumber AlternativeTypeList "]]"
```

Unlike a `SEQUENCE`/`SET` component, a `CHOICE` alternative is a `NamedType`; it does not take component-level `OPTIONAL`, `DEFAULT`, or `COMPONENTS OF`. [itu](https://www.itu.int/rec/dologin_pub.asp?lang=e&id=T-REC-X.680-202102-I!!PDF-E&type=items)

## 7. Character string types

```asn1
CharacterStringType ::=
    RestrictedCharacterStringType
  | UnrestrictedCharacterStringType

RestrictedCharacterStringType ::=
    BMPString
  | GeneralString
  | GraphicString
  | IA5String
  | ISO646String
  | NumericString
  | PrintableString
  | TeletexString
  | T61String
  | UniversalString
  | UTF8String
  | VideotexString
  | VisibleString

UnrestrictedCharacterStringType ::= CHARACTER STRING
```

`T61String` and `ISO646String` are alternative names associated with `TeletexString` and `VisibleString`, respectively. [itu](https://www.itu.int/rec/dologin_pub.asp?lang=e&id=T-REC-X.680-202102-I!!PDF-E&type=items)

## 8. Useful types

```asn1
UsefulType ::= typereference
```

This production is subject to the semantic restriction that the name identifies one of the useful types specified by X.680:

```asn1
GeneralizedTime
UTCTime
ObjectDescriptor
```

Thus `UsefulType` does not mean an arbitrary user-defined type merely because its lexical form is `typereference`. [itu](https://www.itu.int/rec/dologin_pub.asp?lang=e&id=T-REC-X.680-202102-I!!PDF-E&type=items)

## 9. Defined-type references

```asn1
DefinedType ::=
    ExternalTypeReference
  | typereference
  | ParameterizedType
  | ParameterizedValueSetType

ExternalTypeReference ::=
    modulereference "." typereference
```

The parameterized forms, defined in X.683, have this shape:

```asn1
ParameterizedType ::=
    SimpleDefinedType ActualParameterList

ParameterizedValueSetType ::=
    SimpleDefinedType ActualParameterList

SimpleDefinedType ::=
    ExternalTypeReference
  | typereference
```

`ActualParameterList` is a dependency on the parameterization grammar. The same surface shape can require semantic information to distinguish a parameterized type from a parameterized value-set type. [itu](https://www.itu.int/rec/dologin_pub.asp?lang=e&id=T-REC-X.680-202102-I!!PDF-E&type=items)

## 10. Selection types

```asn1
SelectionType ::= identifier "<" Type
```

For example:

```asn1
Payload ::= CHOICE {
    count INTEGER,
    text UTF8String
}

SelectedCount ::= count < Payload
```

This selects the type of the identified `CHOICE` alternative. [itu](https://www.itu.int/rec/dologin_pub.asp?lang=e&id=T-REC-X.680-202102-I!!PDF-E&type=items)

## 11. Prefixed types and tagging

```asn1
PrefixedType ::=
    TaggedType
  | EncodingPrefixedType

TaggedType ::=
    Tag Type
  | Tag IMPLICIT Type
  | Tag EXPLICIT Type

Tag ::= "[" Class ClassNumber "]"

Class ::=
    UNIVERSAL
  | APPLICATION
  | PRIVATE
  | empty

ClassNumber ::=
    number
  | DefinedValue
```

Encoding prefixes use:

```asn1
EncodingPrefixedType ::= EncodingPrefix Type

EncodingPrefix ::=
    "[" EncodingReference EncodingInstruction "]"

EncodingReference ::=
    encodingreference
  | empty
```

`EncodingInstruction` depends on the encoding-instruction notation being used. Importantly, both tagged and encoding-prefixed forms wrap `Type`, so their operands are not restricted to built-ins. [2050] [itu](https://www.itu.int/rec/dologin_pub.asp?lang=e&id=T-REC-X.680-202102-I!!PDF-E&type=items)

## 12. Constrained types

```asn1
ConstrainedType ::=
    Type Constraint
  | TypeWithConstraint

TypeWithConstraint ::=
    SET Constraint OF Type
  | SET SizeConstraint OF Type
  | SEQUENCE Constraint OF Type
  | SEQUENCE SizeConstraint OF Type
  | SET Constraint OF NamedType
  | SET SizeConstraint OF NamedType
  | SEQUENCE Constraint OF NamedType
  | SEQUENCE SizeConstraint OF NamedType

Constraint ::= "(" ConstraintSpec ExceptionSpec ")"

ConstraintSpec ::=
    SubtypeConstraint
  | GeneralConstraint

SubtypeConstraint ::= ElementSetSpecs

SizeConstraint ::= SIZE Constraint

ExceptionSpec ::=
    "!" ExceptionIdentification
  | empty

ExceptionIdentification ::=
    SignedNumber
  | DefinedValue
  | Type ":" Value
```

The remaining dependencies here are `ElementSetSpecs`, which expands into the subtype/set-expression grammar, and `GeneralConstraint`, which is defined in X.682. This is where ranges, unions, intersections, contents constraints, component constraints, and other constraint forms enter—not as additional alternatives of `Type`. [itu](https://www.itu.int/rec/dologin_pub.asp?lang=e&id=T-REC-X.680-202102-I!!PDF-E&type=items)

## 13. Information-object-related type forms

These branches involve the information-object grammar of X.681:

```asn1
ObjectClassFieldType ::=
    DefinedObjectClass "." FieldName

InstanceOfType ::=
    INSTANCE OF DefinedObjectClass

TypeFromObject ::=
    ReferencedObjects "." FieldName

ValueSetFromObjects ::=
    ReferencedObjects "." FieldName
```

The meanings and permitted field categories differ even where the surface productions look identical. `DefinedObjectClass`, `ReferencedObjects`, and `FieldName` require the X.681 information-object grammar and semantic rules. [itu](https://www.itu.int/rec/dologin_pub.asp?lang=e&id=T-REC-X.680-202102-I!!PDF-E&type=items)

## Scope of this grammar

Every `Type` branch is represented above, but this is not a standalone parser grammar for all ASN.1. Completing every dependency recursively would also require:

- The complete value and defined-value grammar.
- The complete constraint grammar from X.680 and X.682.
- Information-object notation from X.681.
- Parameterization from X.683.
- Lexical rules and encoding-instruction notation.

Also, `empty` means an empty token sequence. By contrast, quoted `"..."`, `"[["`, and `"]]"` above are actual ASN.1 syntax, not editorial omissions. Syntax alone does not establish validity; the standard adds semantic restrictions to these productions. [itu](https://www.itu.int/rec/dologin_pub.asp?lang=e&id=T-REC-X.680-202102-I!!PDF-E&type=items)

# Dependency graph

The graph follows X.680’s type classification; dependencies on information objects and parameterization come from X.681 and X.683. [itu](https://www.itu.int/rec/dologin_pub.asp?lang=e&id=T-REC-X.680-202102-I!!PDF-E&type=items)

## 1. Overall graph

```text
Type                                                   tokens: ∅
│
├── BuiltinType                                        tokens: ∅
│   │
│   ├── PrefixedType                                   tokens: ∅
│   │   │
│   │   ├── TaggedType                                 tokens: [, ],
│   │   │                                                  IMPLICIT, EXPLICIT
│   │   │   ├── Tag
│   │   │   │   ├── Class                              tokens: UNIVERSAL,
│   │   │   │   │                                          APPLICATION, PRIVATE
│   │   │   │   └── ClassNumber                        tokens: number,
│   │   │   │                                              defined-value reference
│   │   │   └── ↻ Type
│   │   │
│   │   └── EncodingPrefixedType                       tokens: [, ],
│   │       │                                              encoding reference,
│   │       │                                              encoding instructions
│   │       └── ↻ Type
│   │
│   ├── SequenceType                                   tokens: SEQUENCE, {, }
│   │   └── ComponentTypeLists                         → graph 2
│   │
│   ├── SetType                                        tokens: SET, {, }
│   │   └── ComponentTypeLists                         → graph 2
│   │
│   ├── ChoiceType                                     tokens: CHOICE, {, }
│   │   └── AlternativeTypeLists                       → graph 3
│   │
│   ├── SequenceOfType                                 tokens: SEQUENCE, OF
│   │   ├── ↻ Type
│   │   └── NamedType                                  tokens: identifier
│   │       └── ↻ Type
│   │
│   ├── SetOfType                                      tokens: SET, OF
│   │   ├── ↻ Type
│   │   └── NamedType                                  tokens: identifier
│   │       └── ↻ Type
│   │
│   ├── IntegerType                                    → graph 4
│   ├── BitStringType                                  → graph 4
│   ├── EnumeratedType                                 → graph 4
│   ├── CharacterStringType                            → graph 5
│   │
│   ├── InstanceOfType                                 tokens: INSTANCE, OF
│   │   └── DefinedObjectClass                         [X.681 dependency]
│   │
│   ├── ObjectClassFieldType                           tokens: .
│   │   ├── DefinedObjectClass                         [X.681 dependency]
│   │   └── FieldName                                  [X.681 dependency]
│   │
│   └── Simple built-in types                          → graph 5
│
├── ReferencedType                                     tokens: ∅
│   │
│   ├── DefinedType                                    tokens: ∅
│   │   ├── Local type reference                       tokens: typereference
│   │   ├── ExternalTypeReference                      tokens: modulereference,
│   │   │                                                  ., typereference
│   │   ├── ParameterizedType                          tokens: reference, {, }
│   │   │   └── ActualParameterList                    [X.683 dependency]
│   │   └── ParameterizedValueSetType                  tokens: reference, {, }
│   │       └── ActualParameterList                    [X.683 dependency]
│   │
│   ├── UsefulType                                     tokens: GeneralizedTime,
│   │                                                      UTCTime, ObjectDescriptor
│   │
│   ├── SelectionType                                  tokens: identifier, <
│   │   └── ↻ Type
│   │
│   ├── TypeFromObject                                 tokens: .
│   │   ├── ReferencedObjects                          [X.681 dependency]
│   │   └── FieldName                                  [X.681 dependency]
│   │
│   └── ValueSetFromObjects                            tokens: .
│       ├── ReferencedObjects                          [X.681 dependency]
│       └── FieldName                                  [X.681 dependency]
│
└── ConstrainedType                                    tokens: ∅
    │
    ├── Type Constraint                               tokens: ∅
    │   ├── ↻ Type
    │   └── Constraint                                tokens: (, )
    │       ├── ConstraintSpec                         [constraint grammar]
    │       └── ExceptionSpec                          tokens: optional !
    │
    └── TypeWithConstraint                             tokens: SEQUENCE or SET,
        │                                                  OF
        ├── Constraint                                tokens: (, )
        │   OR
        ├── SizeConstraint                            tokens: SIZE
        │   └── Constraint                            tokens: (, )
        │
        └── Element
            ├── ↻ Type
            └── NamedType                             tokens: identifier
                └── ↻ Type
```

The paired branches under `SequenceOfType`, `SetOfType`, and the constrained element represent alternatives, not two simultaneously required children. [itu](https://www.itu.int/rec/dologin_pub.asp?lang=e&id=T-REC-X.680-202102-I!!PDF-E&type=items)

## 2. `SEQUENCE` / `SET` component dependencies

```text
ComponentTypeLists                                    tokens: commas separating
│                                                         root/extension sections
├── RootComponentTypeList                              tokens: ∅
│   └── ComponentTypeList                              tokens: ,
│       └── ComponentType
│           │
│           ├── NamedType                              tokens: identifier
│           │   └── ↻ Type
│           │
│           ├── NamedType OPTIONAL                     tokens: OPTIONAL
│           │   └── NamedType                          tokens: identifier
│           │       └── ↻ Type
│           │
│           ├── NamedType DEFAULT Value                tokens: DEFAULT
│           │   ├── NamedType                          tokens: identifier
│           │   │   └── ↻ Type
│           │   └── Value                              [value grammar]
│           │
│           └── COMPONENTS OF Type                     tokens: COMPONENTS, OF
│               └── ↻ Type
│
├── ExtensionAndException                              tokens: ...
│   └── ExceptionSpec                                  tokens: optional !
│
├── ExtensionAdditions                                 tokens: ,
│   └── ExtensionAdditionList                          tokens: ,
│       └── ExtensionAddition                          tokens: ∅
│           ├── ComponentType                          [same branches as above]
│           └── ExtensionAdditionGroup                 tokens: [[, ]]
│               ├── VersionNumber                      tokens: optional number, :
│               └── ComponentTypeList                  [same list as above]
│
└── Extension-ending marker                            tokens: optional , ...
```

This is a dependency graph rather than the complete list of permitted root/extension-section arrangements. An extension group contains component-list entries; it does not introduce another nested `SEQUENCE` or `SET` type. [itu](https://www.itu.int/rec/dologin_pub.asp?lang=e&id=T-REC-X.680-202102-I!!PDF-E&type=items)

## 3. `CHOICE` alternative dependencies

```text
AlternativeTypeLists                                  tokens: section-separating ,
│
├── RootAlternativeTypeList                            tokens: ∅
│   └── AlternativeTypeList                            tokens: ,
│       └── NamedType                                  tokens: identifier
│           └── ↻ Type
│
├── ExtensionAndException                              tokens: ...
│   └── ExceptionSpec                                  tokens: optional !
│
├── ExtensionAdditionAlternatives                      tokens: ,
│   └── ExtensionAdditionAlternativesList              tokens: ,
│       └── ExtensionAdditionAlternative               tokens: ∅
│           ├── NamedType                              tokens: identifier
│           │   └── ↻ Type
│           └── ExtensionAdditionAlternativesGroup     tokens: [[, ]]
│               ├── VersionNumber                      tokens: optional number, :
│               └── AlternativeTypeList                tokens: ,
│                   └── NamedType                      tokens: identifier
│                       └── ↻ Type
│
└── OptionalExtensionMarker                            tokens: optional , ...
```

There is no component-level `OPTIONAL`, `DEFAULT`, or `COMPONENTS OF` branch here: an alternative is a `NamedType`. [itu](https://www.itu.int/rec/dologin_pub.asp?lang=e&id=T-REC-X.680-202102-I!!PDF-E&type=items)

## 4. Built-ins with named items

```text
IntegerType                                           tokens: INTEGER,
│                                                         optional {, }
└── NamedNumberList                                    tokens: ,
    └── NamedNumber                                    tokens: identifier, (, )
        └── Number source
            ├── SignedNumber                           tokens: number, optional -
            └── DefinedValue                           [value-reference grammar]


BitStringType                                         tokens: BIT, STRING,
│                                                         optional {, }
└── NamedBitList                                       tokens: ,
    └── NamedBit                                       tokens: identifier, (, )
        └── Position source
            ├── number                                 tokens: number
            └── DefinedValue                           [value-reference grammar]


EnumeratedType                                        tokens: ENUMERATED, {, }
└── Enumerations                                       tokens: ,
    ├── RootEnumeration                               tokens: ∅
    │   └── Enumeration                               tokens: ,
    │       └── EnumerationItem                       tokens: ∅
    │           ├── identifier                        tokens: identifier
    │           └── NamedNumber                       tokens: identifier, (, )
    │               └── SignedNumber / DefinedValue
    │
    ├── Extension marker                              tokens: ...
    │   └── ExceptionSpec                             tokens: optional !
    │
    └── AdditionalEnumeration                         tokens: ∅
        └── Enumeration                               [same branches as above]
```

Integer named numbers and bit-string named positions are not nested types; their dependencies terminate in numbers or value references. [itu](https://www.itu.int/rec/dologin_pub.asp?lang=e&id=T-REC-X.680-202102-I!!PDF-E&type=items)

## 5. Terminal built-in type forms

These introduce no nested `Type` operand. Constraints and tags can still be applied through the outer recursive forms. [itu](https://www.itu.int/rec/dologin_pub.asp?lang=e&id=T-REC-X.680-202102-I!!PDF-E&type=items)

| Grammar node | Type-specific tokens |
|---|---|
| `BooleanType` | `BOOLEAN` |
| `RealType` | `REAL` |
| `OctetStringType` | `OCTET STRING` |
| `NullType` | `NULL` |
| `ObjectIdentifierType` | `OBJECT IDENTIFIER` |
| `RelativeOIDType` | `RELATIVE-OID` |
| `IRIType` | `OID-IRI` |
| `RelativeIRIType` | `RELATIVE-OID-IRI` |
| `EmbeddedPDVType` | `EMBEDDED PDV` |
| `ExternalType` | `EXTERNAL` |
| `TimeType` | `TIME` |
| `DateType` | `DATE` |
| `TimeOfDayType` | `TIME-OF-DAY` |
| `DateTimeType` | `DATE-TIME` |
| `DurationType` | `DURATION` |

The character-string branch adds one classification level:

```text
CharacterStringType                                   tokens: ∅
│
├── RestrictedCharacterStringType                      tokens: one of
│       BMPString
│       GeneralString
│       GraphicString
│       IA5String
│       ISO646String
│       NumericString
│       PrintableString
│       TeletexString
│       T61String
│       UniversalString
│       UTF8String
│       VideotexString
│       VisibleString
│
└── UnrestrictedCharacterStringType                    tokens: CHARACTER, STRING
```

`GeneralizedTime`, `UTCTime`, and `ObjectDescriptor` are under `UsefulType`, not this character-string grammar branch, despite their string-related semantics. [itu](https://www.itu.int/rec/dologin_pub.asp?lang=e&id=T-REC-X.680-202102-I!!PDF-E&type=items)

## 6. An actual outermost-to-innermost path

For:

```asn1
count [0] IMPLICIT SEQUENCE SIZE (1..10) OF INTEGER OPTIONAL
```

the nesting is:

```text
ComponentType: NamedType OPTIONAL                      own tokens: OPTIONAL
    ↓
NamedType                                             own tokens: count
    ↓
Type → BuiltinType → PrefixedType → TaggedType         own tokens: ∅
    ↓
TaggedType                                            own syntax: [0] IMPLICIT
    ↓
Type → ConstrainedType → TypeWithConstraint            own tokens: ∅
    ↓
TypeWithConstraint                                    own tokens: SEQUENCE, OF
    ├── SizeConstraint                                own tokens: SIZE
    │   └── Constraint                                own delimiters: (, )
    │       └── value-range constraint                 own syntax: 1..10
    ↓
Type → BuiltinType → IntegerType                       own tokens: INTEGER
```

Here the named component is outermost, tagging wraps the collection type, the size constraint belongs to the collection, and `INTEGER` is the innermost element type. This illustrates why “grammar level” and “source-token order” are related but not identical: some constructs place their syntax after their operand. [itu](https://www.itu.int/rec/dologin_pub.asp?lang=e&id=T-REC-X.680-202102-I!!PDF-E&type=items)
