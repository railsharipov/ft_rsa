#ifndef SSL_MAP_H
# define SSL_MAP_H

# define MAP_ASN_OID_TREE \
"1:iso{ "\
	"1.2:member-body{ "\
		"1.2.840:us{ "\
			"1.2.840.113549:rsadsi{ "\
				"1.2.840.113549.1:pkcs{ "\
					"1.2.840.113549.1.1:pkcs-1{ "\
						"1.2.840.113549.1.1.1:rsaEncryption "\
						"1.2.840.113549.1.1.2:md2withRSAEncryption "\
						"1.2.840.113549.1.1.4:md5withRSAEncryption "\
						"1.2.840.113549.1.1.5:sha1WithRSAEncryption "\
					"} "\
					"1.2.840.113549.1.3:pkcs-3{ "\
						"1.2.840.113549.1.3.1:dhKeyAgreement "\
					"} "\
					"1.2.840.113549.1.5:pkcs-5{ "\
						"1.2.840.113549.1.5.1:pbeWithMD2AndDES-CBC "\
						"1.2.840.113549.1.5.3:pbeWithMD5AndDES-CBC "\
					"} "\
					"1.2.840.113549.1.7:pkcs-7{ "\
						"1.2.840.113549.1.7.1:data "\
						"1.2.840.113549.1.7.2:signedData "\
						"1.2.840.113549.1.7.3:envelopedData "\
						"1.2.840.113549.1.7.4:signedAndEnvelopedData "\
						"1.2.840.113549.1.7.5:digestData "\
						"1.2.840.113549.1.7.6:encryptedData "\
					"} "\
					"1.2.840.113549.1.9:pkcs-9{ "\
						"1.2.840.113549.1.9.1:emailAddress "\
						"1.2.840.113549.1.9.2:unstructuredName "\
						"1.2.840.113549.1.9.3:contentType "\
						"1.2.840.113549.1.9.4:messageDigest "\
						"1.2.840.113549.1.9.5:signingTime "\
						"1.2.840.113549.1.9.6:countersignature "\
						"1.2.840.113549.1.9.7:challengePassword "\
						"1.2.840.113549.1.9.8:unstructuredAddress "\
						"1.2.840.113549.1.9.9:extendedCertificateAttributes "\
					"} "\
					"1.2.840.113549.1.11:pkcs-11 "\
					"1.2.840.113549.1.12:pkcs-12{ "\
						"1.2.840.113549.1.12.1:pkcs-12PbeIds{ "\
							"1.2.840.113549.1.12.1.1:pbeWithSHA1And128BitRC4 "\
							"1.2.840.113549.1.12.1.2:pbeWithSHA1And40BitRC4 "\
							"1.2.840.113549.1.12.1.3:pbeWithSHA1And3-KeyTripleDES-CBC "\
							"1.2.840.113549.1.12.1.4:pbeWithSHA1And2-KeyTripleDES-CBC "\
							"1.2.840.113549.1.12.1.5:pbeWithSHA1And128BitRC2-CBC "\
							"1.2.840.113549.1.12.1.6:pbeWithSHA1And40BitRC2-CBC "\
						"} "\
					"} "\
				"} "\
				"1.2.840.113549.2:digestAlgorithm{ "\
					"1.2.840.113549.2.2:md2 "\
					"1.2.840.113549.2.4:md4 "\
					"1.2.840.113549.2.5:md5 "\
				"} "\
				"1.2.840.113549.3:encryptionAlgorithm{ "\
					"1.2.840.113549.3.2:rc2-cbc "\
					"1.2.840.113549.3.7:des-ede3-cbc "\
					"1.2.840.113549.3.8:RC5CBC "\
					"1.2.840.113549.3.9:RC5CBCPAD "\
				"} "\
			"} "\
		"} "\
	"} "\
"}"

# define MAP_RSA_PRIVATE_KEY \
"sequence:RSA_PRIVATE_KEY{ "\
	"int:version "\
	"int:modulus "\
	"int:publicExponent "\
	"int:privateExponent "\
	"int:prime1 "\
	"int:prime2 "\
	"int:exponent1 "\
	"int:exponent2 "\
	"int:coefficient "\
"}"

# define MAP_X509_PUBLIC_KEY \
"sequence:PUBLIC_KEY{ "\
	"sequence:algorithmId{ :"\
		"objectId:rsaEncryption "\
		"null:NULL "\
	"} "\
	"bitstring:BITSTRING{ "\
		"sequence:SEQUENCE{ "\
			"int:modulus "\
			"int:publicExponent "\
		"} "\
	"} "\
"}"

#endif
