# ft_rsa
Simple crypto library. RSA, ASN.1, PEM, DER, DES, Base64, SHAx, MD5

## About
This goal of this project is implementing following algorithms:
1. RSA crypt
2. DES crypt
2. Message digest (hashing)
3. Base64 encoding
4. ASN.1 tree
5. DER encoding
6. PEM encoding

This project also includes:
1. bnum library (big number implementation)
2. libft library (partial implementation of libc functions)
3. text parser

## Requirements
GNU C

MacOS High Sierra or higher

Ubuntu 16.04 or higher

## Limitations
```bnum``` library supports ``big ints`` up to ```8192 bits```. This is because ```big int```'s
values are stored in stack memory. Since ```bnum``` library is used mainly for RSA operations
and RSA keysize is usually less than ```8192 bits```, this limitation should not be important in practice.

During DES crypt operations on a file, whole file is read into memory,
which implies system memory limitations for large files.

## Compile
```make```

## Usage
Usage is quite similar to openssl, although some flags are different

```
Digest commands: md5, sha1, sha224, sha256, sha384, sha512, sha512/224, sha512/256

    <file>              read from <file>
    -p                  read from stdin
    -r                  reverse output format
    -q                  quiet mode
    -s <string>         input <string>

DES cipher commands: des-ecb, des-cbc

    -a                  encode/decode the input/output to base64
    -i <file>           read from <file>
    -o <file>           write output to <file>
    -e                  encrypt mode
    -d                  decrypt mode
    -k <key>            hex key (des)
    -s <salt>           hex salt (des)
    -p <pass>           password (des)

Base64 commands: base64

    -i <file>           encode file contents
    -o <file>           output to file
    -b <num>            set output line width to num

RSA commands:

    genrsa (generate RSA private keys):
    
    -rand <files>       set rand source as list of files, each separated by semicolon
    -o <file>           output generated key to a file
    <number>            set RSA key size (shall be last argument)
    
    rsa (perform operation on RSA keys):
    
    -in <file>          input RSA key file
    -out <file>         output RSA key to file
    -inform <PEM|DER>   set format of the input key, default is PEM-encoded
    -outform <PEM|DER>  set format of the output key, default is PEM-encoded
    -pubin              input key is a public RSA key
    -pubout             output public RSA key
    -check              check RSA private key
    -passin:<pass>      set pass as password for input key
    -passout:<pass>     set pass as password for output key
    -des                encrypt output key using DES, works only with PEM-encoded keys
    -text               print key components
    -noout              do not output key
    -modulus            print key modulus
    
    rsautl (perform RSA crypt):
    
    -in <file>          encrypt file contents
    -out <file>         output to a file
    -inkey <file>       RSA key input
    -encrypt            RSA encryption mode
    -decrypt            RSA decryption mode
    -pubin              input key file is public RSA key
    -hexdump            hexdump output
```
## Usage examples
Generate RSA private key:
```
./ft_ssl genrsa -o key 2048
```
Check RSA key and print its contents:
```
./ft_ssl rsa -in key -check -text -noout
```
Encrypt message using RSA algorithm:
```
./ft_ssl rsautl -in message -out cipher -inkey key
```
## References
1.	ITU-T X.660: General procedures and top arcs of the international object identifier tree
2.	ITU-T X.690: Abstract Syntax Notation One (ASN.1)
3.	FIPS 180-4: Secure Hash Standard
4.	FIPS 186-4: Digital Signature Standard
5.	RFC 5280: Internet X.509 Public Key Infrastructure Certificate
6.	RFC 3447: PKCS #1: RSA Cryptography Specifications Version 2.1
7.	RFC 8017: PKCS #1: RSA Cryptography Specifications Version 2.2
8.	RFC 5208: PKCS #8: Private-Key Information Syntax Specification Version 1.2
9.	RFC 5958: Asymmetric Key Packages
10.	RFC 7468: Textual Encodings of PKIX, PKCS, and CMS Structures
11.	RFC 6234: US Secure Hash Algorithms (SHA and SHA-based HMAC and HKDF)
12.	RFC 1321: The MD5 Message-Digest Algorithm
13.	FIPS 46-3: Data Encryption Standard (DES)
14.	RFC 1829: The ESP DES-CBC Transform
