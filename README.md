# ft_rsa
Simple crypto library. RSA, ASN.1, PEM, DER, DES, Base64, SHAx, MD5

## About
The goals of this project:
* Pseudo-random number generator
* Password based key derivation
* RSA key generation
* RSA crypt
* RSA utilities
* DES crypt
* Message digest
* Base64 encoding
* ASN.1 tree
* DER encoding
* PEM encoding
* OpenSSL style flags

## Requirements
* GNU C
* MacOS High Sierra or higher
* Ubuntu 16.04 or higher

## Compile
```
make -j
```

## Usage
```
./ft_ssl -h
```

## Usage examples
Generate RSA private key:
```
./ft_ssl genrsa -o <key> 2048
```
Check RSA key and print its components:
```
./ft_ssl rsa -in <key> -check -text -noout
```
Encrypt message using RSA algorithm:
```
./ft_ssl rsautl -in <message> -out <cipher> -inkey <key>
```

## References
* LibTomMath, Multi-Precision Math: Tom St Denis, Mads Rasmussen, Greg Rose
* ITU-T X.660: General procedures and top arcs of the international object identifier tree
* ITU-T X.690: Abstract Syntax Notation One (ASN.1)
* FIPS 180-4: Secure Hash Standard
* FIPS 186-4: Digital Signature Standard
* RFC 5280: Internet X.509 Public Key Infrastructure Certificate
* RFC 3447: PKCS #1: RSA Cryptography Specifications Version 2.1
* RFC 8017: PKCS #1: RSA Cryptography Specifications Version 2.2
* RFC 5208: PKCS #8: Private-Key Information Syntax Specification Version 1.2
* RFC 5958: Asymmetric Key Packages
* RFC 7468: Textual Encodings of PKIX, PKCS, and CMS Structures
* RFC 6234: US Secure Hash Algorithms (SHA and SHA-based HMAC and HKDF)
* RFC 1321: The MD5 Message-Digest Algorithm
* FIPS 46-3: Data Encryption Standard (DES)
* RFC 1829: The ESP DES-CBC Transform
