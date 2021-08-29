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

This project also includes:
* ```bnum``` library (big number implementation)
* ```libft``` library (partial implementation of ```libc``` functions)

## Requirements
* GNU C
* MacOS High Sierra or higher
* Ubuntu 16.04 or higher

## Limitations
```bnum``` library supports ``big ints`` up to ```8192 bits```. This is because ```big int```'s
values are stored in stack memory. Since ```bnum``` library is used mainly for RSA operations
and RSA keysize is usually less than ```8192 bits```, this limitation should not be important in practice.

During DES crypt operations on a file, whole file is read into memory,
which implies system memory limitations for large files.

Although standard recommends that number of trials in Miller-Rabin prime check should change depending
on number size, this implemetation uses fixed number of trials, specifically 20 (defined by ```RM_TRIALS```). 
This results in lower performance when prime checks are performed on 4096 bit or larger numbers.

## For 42School buddies
This project does not follow 'Norminette' code formatting requirements. Crypto involves a lot of math, 
so making 25-liners will not improve readability in any way. In my opinion, 'Norminette' is only good for
Tier 2 and lower projects, but that's another story ;)

## Compile
```make```

## Usage
Usage is quite similar to openssl, although some flags are different

```
Digest commands: md5, sha1, sha224, sha256, sha384, sha512, sha512/224, sha512/256

    <file>              read message from <file>
    -p                  read message from stdin
    -r                  reverse output format
    -q                  quiet mode
    -s <string>         input <string>

DES cipher commands: des-ecb, des-cbc

    -a                  base64-encode output / base64-decode input (depending on DES mode)
    -i <file>           read message from <file>
    -o <file>           write output to <file>
    -e                  DES encryption mode (default)
    -d                  DES decryption mode
    -k <key>            hex key input
    -s <salt>           hex salt input
    -p <pass>           password input

Base64 commands: base64

    -i <file>           read message from <file>
    -o <file>           write output to <file>
    -e                  base64 encode mode (default)
    -d                  base64 decode mode
    -b <num>            set output line width to <num>

RSA commands:

    genrsa (generate RSA private keys):
    
    -rand <files>       set rand source as list of <files>, separated by colon
    -o <keyfile>        output generated key to a <keyfile>
    <keysize>           set RSA key size to <keysize> (shall be last argument)
    
    rsa (perform operation on RSA keys):
    
    -in <keyfile>       read input key from <keyfile>
    -out <keyfile>      output key to <keyfile>
    -inform <PEM|DER>   set format of the input key, default is PEM-encoded
    -outform <PEM|DER>  set format of the output key, default is PEM-encoded
    -pubin              input key is a X.509 public key
    -pubout             output key as X.509 public key
    -check              check RSA private key
    -passin:<pass>      set <pass> as password for input key
    -passout:<pass>     set <pass> as password for output key
    -des                encrypt output key using DES-CBC (key outform shall be PEM)
    -text               print input key components
    -noout              do not output key
    -modulus            print input key modulus
    
    rsautl (perform RSA crypt):
    
    -in <file>          read message/cipher form <file>
    -out <file>         write output to a <file>
    -inkey <keyfile>    read RSA key form <keyfile>
    -encrypt            RSA encryption mode (default)
    -decrypt            RSA decryption mode
    -pubin              input key is a X.509 public key
    -hexdump            hexdump output
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
