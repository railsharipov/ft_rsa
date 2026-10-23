# ASN1 module

The top-level module grammar: [itu](https://www.itu.int/rec/dologin_pub.asp?lang=e&id=T-REC-X.680-202102-I!!PDF-E&type=items)

## 1. Module structure

```asn1
ModuleDefinition ::=
    ModuleIdentifier
    DEFINITIONS
    EncodingReferenceDefault
    TagDefault
    ExtensionDefault
    "::="
    BEGIN
        ModuleBody
        EncodingControlSections
    END
```

The defaults are optional:

```asn1
EncodingReferenceDefault ::=
    encodingreference INSTRUCTIONS
  | empty

TagDefault ::=
    EXPLICIT TAGS
  | IMPLICIT TAGS
  | AUTOMATIC TAGS
  | empty

ExtensionDefault ::=
    EXTENSIBILITY IMPLIED
  | empty
```

If the tagging default is absent, `EXPLICIT TAGS` applies. `EXTENSIBILITY IMPLIED` establishes an extensibility default; it is not simply a flag saying that every declaration is extensible. [itu](https://www.itu.int/rec/dologin_pub.asp?lang=e&id=T-REC-X.680-202102-I!!PDF-E&type=items)

## 2. Module identifier

```asn1
ModuleIdentifier ::=
    modulereference DefinitiveIdentification

DefinitiveIdentification ::=
    DefinitiveOID
  | DefinitiveOID DefinitiveIRI
  | empty

DefinitiveOID ::=
    "{" DefinitiveObjIdComponentList "}"

DefinitiveObjIdComponentList ::=
    DefinitiveObjIdComponent
  | DefinitiveObjIdComponent DefinitiveObjIdComponentList

DefinitiveObjIdComponent ::=
    NameForm
  | DefinitiveNumberForm
  | DefinitiveNameAndNumberForm

DefinitiveNumberForm ::= number

DefinitiveNameAndNumberForm ::=
    identifier "(" DefinitiveNumberForm ")"

NameForm ::= identifier
```

`DefinitiveIRI` supplies an additional IRI identification using the standard’s IRI notation; it is a separate lexical/value-notation dependency.

For example:

```asn1
ExampleModule { iso(1) member-body(2) 999 }
DEFINITIONS AUTOMATIC TAGS ::= BEGIN
    ...
END
```

The definitive identification is optional, so a module can begin with just its name followed by `DEFINITIONS`. [itu](https://www.itu.int/rec/dologin_pub.asp?lang=e&id=T-REC-X.680-202102-I!!PDF-E&type=items)

## 3. Module body

```asn1
ModuleBody ::=
    Exports Imports AssignmentList
  | empty

AssignmentList ::=
    Assignment
  | AssignmentList Assignment
```

Assignments are adjacent; there is no general semicolon separator between them. `Exports` and `Imports` have their own terminating semicolons when written. [itu](https://www.itu.int/rec/dologin_pub.asp?lang=e&id=T-REC-X.680-202102-I!!PDF-E&type=items)

## 4. Exports

```asn1
Exports ::=
    EXPORTS SymbolsExported ";"
  | EXPORTS ALL ";"
  | empty

SymbolsExported ::=
    SymbolList
  | empty

SymbolList ::=
    Symbol
  | SymbolList "," Symbol

Symbol ::=
    Reference
  | ParameterizedReference
```

The important distinction is: [itu](https://www.itu.int/rec/dologin_pub.asp?lang=e&id=T-REC-X.680-202102-I!!PDF-E&type=items)

| Syntax | Meaning |
|---|---|
| No `EXPORTS` clause | All eligible locally defined symbols are exported |
| `EXPORTS ALL;` | Explicitly exports all eligible locally defined symbols |
| `EXPORTS;` | Exports no symbols |
| `EXPORTS A, B;` | Exports the listed symbols |

`Reference` covers the relevant assignment-name categories; `ParameterizedReference` comes from X.683.

## 5. Imports

```asn1
Imports ::=
    IMPORTS SymbolsImported ";"
  | empty

SymbolsImported ::=
    SymbolsFromModuleList
  | empty

SymbolsFromModuleList ::=
    SymbolsFromModule
  | SymbolsFromModuleList SymbolsFromModule

SymbolsFromModule ::=
    SymbolList FROM GlobalModuleReference

GlobalModuleReference ::=
    modulereference AssignedIdentifier

AssignedIdentifier ::=
    ObjectIdentifierValue
  | DefinedValue
  | empty
```

There is no extra comma between successive `SymbolsFromModule` groups. Commas separate names within each group. [itu](https://www.itu.int/rec/dologin_pub.asp?lang=e&id=T-REC-X.680-202102-I!!PDF-E&type=items)

For example:

```asn1
IMPORTS
    Count, Status FROM CommonTypes
    Header FROM ProtocolHeaders;
```

`ObjectIdentifierValue` and `DefinedValue` depend on the value grammar.

## 6. Assignment alternatives

```asn1
Assignment ::=
    TypeAssignment
  | ValueAssignment
  | XMLValueAssignment
  | ValueSetTypeAssignment
  | ObjectClassAssignment
  | ObjectAssignment
  | ObjectSetAssignment
  | ParameterizedAssignment
```

This is where the module grammar hands off to type/value definitions, information-object definitions, and parameterization. [itu](https://www.itu.int/rec/dologin_pub.asp?lang=e&id=T-REC-X.680-202102-I!!PDF-E&type=items)

## 7. Encoding control sections

At the end of the module, before `END`, optional encoding-control sections can appear:

```asn1
EncodingControlSections ::=
    EncodingControlSection EncodingControlSections
  | empty

EncodingControlSection ::=
    ENCODING-CONTROL encodingreference EncodingInstructionAssignmentList
```

The instruction-assignment contents depend on the encoding-instruction notation being used; they are not ordinary type/value assignments. [itu](https://www.itu.int/rec/dologin_pub.asp?lang=e&id=T-REC-X.680-202102-I!!PDF-E&type=items)

## 8. Complete ordinary example

```asn1
Messages DEFINITIONS AUTOMATIC TAGS ::= BEGIN

    EXPORTS Message, defaultMessage;

    IMPORTS
        Count FROM CommonTypes;

    Message ::= SEQUENCE {
        count Count,
        enabled BOOLEAN DEFAULT TRUE
    }

    defaultMessage Message ::= {
        count 10,
        enabled TRUE
    }

END
```
