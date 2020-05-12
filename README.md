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

## Compile
```make```

## Usage
Usage is quite similar to openssl, although some flags are different

```
Digest commands: md5, sha1, sha224, sha256, sha384, sha512, sha512/224, sha512/256
    <file>              read from <file>
    -p                  read from IO_STDIN
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
