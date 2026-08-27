#include <pwd.h>
#include <unistd.h>
#include <common.h>
#include <logger.h>
#include <args.h>
#include <cmd.h>
#include <des.h>
#include <base64.h>
#include <textutil.h>
#include <rand.h>

int	cmd_des_ecb(const t_cmd *cmd)
{
	// DES context
	t_des_ctx des_ctx = {0};
	uint8_t des_key[8] = {0};
	uint8_t des_salt[8] = {0};

	// I/O pipeline: Source -> Filters -> Sink

	// Source
	t_io_v2_stream *in = NULL;
	if (ft_htbl_has(cmd->opts, "-i")) {
		if (SSL_OK != io_v2_file_reader(&in, ft_htbl_get(cmd->opts, "-i"))) {
			SSL_LOG(ERROR, IO_INIT_ERROR);
			return (SSL_ERR);
		}
	} else {
		if (SSL_OK != io_v2_fd_reader(&in, 0)) {
			SSL_LOG(ERROR, IO_INIT_ERROR);
			return (SSL_ERR);
		}
	}
	// Sink
	t_io_v2_stream *out = NULL;
	if (ft_htbl_has(cmd->opts, "-o")) {
		if (SSL_OK != io_v2_file_writer(&out, ft_htbl_get(cmd->opts, "-o"))) {
			SSL_LOG(ERROR, IO_INIT_ERROR);
			return (SSL_ERR);
		}
	} else {
		if (SSL_OK != io_v2_fd_writer(&out, 1)) {
			SSL_LOG(ERROR, IO_INIT_ERROR);
			return (SSL_ERR);
		}
	}

	// Decrypt mode
	if (ft_htbl_has(cmd->opts, "-d")) {
		// Input filters
		if (ft_htbl_has(cmd->opts, "-a")) {
			// Input must be base64 decoded
			// source -> whitespace remover -> base64 decoder -> read()
			// Remove whitespace
			t_textutil_ctx *eolws_remover_ctx = NULL;
			t_io_v2_stream *eolws_remover_filter = NULL;
			SSL_ALLOC(eolws_remover_ctx, sizeof(t_textutil_ctx));
			*eolws_remover_ctx = (t_textutil_ctx){0};
			if (io_v2_filter_reader(&eolws_remover_filter, in, textutil_del_eolws_update, textutil_del_eolws_final, eolws_remover_ctx) < 0) {
				SSL_LOG(ERROR, IO_INIT_ERROR);
				return (SSL_ERR);
			}
			in = eolws_remover_filter;
			// Base64 decode
			t_b64_ctx *b64_ctx = NULL;
			t_io_v2_stream *b64_filter = NULL;
			SSL_ALLOC(b64_ctx, sizeof(t_b64_ctx));
			*b64_ctx = (t_b64_ctx){0};
			if (io_v2_filter_reader(&b64_filter, in, base64_decode_transform_update, base64_decode_transform_final, b64_ctx) < 0) {
				SSL_LOG(ERROR, IO_INIT_ERROR);
				return (SSL_ERR);
			}
			in = b64_filter;
		}

		// Output filters
		// write() -> des filter -> sink
		// DES decrypt
		if (ft_htbl_has(cmd->opts, "-k")) {
			// Key is provided by user
			char *des_hexkey = ft_htbl_get(cmd->opts, "-k");
			if (!ft_str_ishex(des_hexkey)) {
				SSL_LOG(ERROR, "key must be in hex format");
				return (SSL_ERR);
			}
			ft_hex_to_bytes(&des_key, des_hexkey, MIN(16, ft_strlen(des_hexkey)));
			if (ft_htbl_has(cmd->opts, "-n")) {
				// Dump vectors in hex format
				char *khex = ft_bytes_to_hex(des_key, sizeof(des_key));
				char *shex = ft_bytes_to_hex(des_salt, sizeof(des_salt));
				ft_printf("salt=%.16s\nkey=%.16s\n", shex, khex);
			}
		}
		else if (ft_htbl_has(cmd->opts, "-s")) {
			// Derive key from salt and password
			char *des_hexsalt = ft_htbl_get(cmd->opts, "-s");

			if (!ft_str_ishex(des_hexsalt)) {
				SSL_LOG(ERROR, "salt must be in hex format");
				return (SSL_ERR);
			}
			ft_hex_to_bytes(&des_salt, des_hexsalt, MIN(16, ft_strlen(des_hexsalt)));

			char des_pass[_PASSWORD_LEN+1] = {0};

			if (ft_htbl_has(cmd->opts, "-p")) {
				char *input = ft_htbl_get(cmd->opts, "-p");
				ft_strncpy(des_pass, input, _PASSWORD_LEN);
			} else {
				char *input = getpass("enter des-ecb encryption password: ");
				if (NULL == input) {
					SSL_LOG(ERROR, "bad password read");
					return (SSL_ERR);
				}
				ft_strncpy(des_pass, input, _PASSWORD_LEN);
				ft_bzero(input, _PASSWORD_LEN);
			}
			des_pass[_PASSWORD_LEN] = 0;

			if (SSL_OK != rand_openssl_v3_kdf(des_key, sizeof(des_key), NULL, 0, des_salt, sizeof(des_salt), (const uint8_t *)des_pass, ft_strlen(des_pass))) {
				SSL_LOG(ERROR, "failed to generate key from salt");
				return (SSL_ERR);
			}
			if (ft_htbl_has(cmd->opts, "-n")) {
				// Dump vectors in hex format
				char *khex = ft_bytes_to_hex(des_key, sizeof(des_key));
				char *shex = ft_bytes_to_hex(des_salt, sizeof(des_salt));
				ft_printf("salt=%.16s\nkey=%.16s\n", shex, khex);
			}
		}
		else {
			// No salt was provided which means the encryption must have a "Salted__" header.
			// We must read and parse that header to get the salt.
			// If header is not present we must fail the command.

			// First, we should get the user password.
			char des_pass[_PASSWORD_LEN+1] = {0};
			if (ft_htbl_has(cmd->opts, "-p")) {
				char *input = ft_htbl_get(cmd->opts, "-p");
				ft_strncpy(des_pass, input, _PASSWORD_LEN);
			} else {
				char *input = getpass("enter des-ecb encryption password: ");
				if (NULL == input) {
					SSL_LOG(ERROR, "bad password read");
					return (SSL_ERR);
				}
				ft_strncpy(des_pass, input, _PASSWORD_LEN);
				ft_bzero(input, _PASSWORD_LEN);
			}
			des_pass[_PASSWORD_LEN] = 0;

			// Second, read the header from encryption, then get the salt from header.
			// Header must be 2 DES blocks in size: 1st block must contain "Salted__" string, 2nd block - salt bytes.
			char des_salt_header[2*DES_BLOCK_SIZE] = {0};
			if (io_v2_read_all(in, des_salt_header, sizeof(des_salt_header)) != 2*DES_BLOCK_SIZE) {
				SSL_LOG(ERROR, "failed to read input");
				return (SSL_ERR);
			}
			ft_memcpy(des_salt, des_salt_header + DES_BLOCK_SIZE, DES_BLOCK_SIZE);
			// Derive the key from parsed salt and user password.
			if (SSL_OK != rand_openssl_v3_kdf(des_key, sizeof(des_key), NULL, 0, des_salt, sizeof(des_salt), (const uint8_t *)des_pass, ft_strlen(des_pass))) {
				SSL_LOG(ERROR, "failed to generate key from salt");
				return (SSL_ERR);
			}
			if (ft_htbl_has(cmd->opts, "-n")) {
				// Dump vectors in hex format
				char *khex = ft_bytes_to_hex(des_key, sizeof(des_key));
				char *shex = ft_bytes_to_hex(des_salt, sizeof(des_salt));
				ft_printf("salt=%.16s\nkey=%.16s\n", shex, khex);
			}
		}
		if (SSL_OK != des_ecb_decrypt_init(&des_ctx, des_key)) {
			SSL_LOG(ERROR, "des-ecb init error");
			return (SSL_ERR);
		}
		t_io_v2_stream *des_filter = NULL;
		if (SSL_OK != io_v2_filter_writer(&des_filter, out, des_ecb_decrypt_transform_update, des_ecb_decrypt_transform_final, &des_ctx)) {
			SSL_LOG(ERROR, IO_INIT_ERROR);
			return (SSL_ERR);
		}
		out = des_filter;
	}
	// Encrypt mode
	else {
		// Output filters
		if (ft_htbl_has(cmd->opts, "-a")) {
			// Base64 encode output
			// write() -> base64 encoder -> line breaker -> terminator -> sink
			// Add terminating newline
			t_textutil_ctx *terminator_ctx = NULL;
			t_io_v2_stream *terminator_filter = NULL;
			SSL_ALLOC(terminator_ctx, sizeof(t_textutil_ctx));
			*terminator_ctx = (t_textutil_ctx){.delim = '\n'};
			if (io_v2_filter_writer(&terminator_filter, out, NULL, textutil_terminator_final, terminator_ctx) < 0) {
				SSL_LOG(ERROR, IO_INIT_ERROR);
				return (SSL_ERR);
			}
			out = terminator_filter;
			// Break output into lines of 64 characters
			t_textutil_ctx *linebreak_ctx = NULL;
			t_io_v2_stream *linebreak_filter = NULL;
			SSL_ALLOC(linebreak_ctx, sizeof(t_textutil_ctx));
			*linebreak_ctx = (t_textutil_ctx){.delim = '\n', .line_width = 64};
			if (io_v2_filter_writer(&linebreak_filter, out, textutil_insert_delim_update, textutil_insert_delim_final, linebreak_ctx) < 0) {
				SSL_LOG(ERROR, IO_INIT_ERROR);
				return (SSL_ERR);
			}
			out = linebreak_filter;
			// Base64 encode
			t_b64_ctx *b64_ctx = NULL;
			t_io_v2_stream *b64_filter = NULL;
			SSL_ALLOC(b64_ctx, sizeof(t_b64_ctx));
			*b64_ctx = (t_b64_ctx){0};
			if (io_v2_filter_writer(&b64_filter, out, base64_encode_transform_update, base64_encode_transform_final, b64_ctx) < 0) {
				SSL_LOG(ERROR, IO_INIT_ERROR);
				return (SSL_ERR);
			}
			out = b64_filter;
		}

		// Input filters
		// source -> des filter -> read()
		// DES encrypt
		if (ft_htbl_has(cmd->opts, "-k")) {
			// Key is provided by user
			char *des_hexkey = ft_htbl_get(cmd->opts, "-k");
			if (!ft_str_ishex(des_hexkey)) {
				SSL_LOG(ERROR, "key must be in hex format");
				return (SSL_ERR);
			}
			ft_hex_to_bytes(&des_key, des_hexkey, MIN(16, ft_strlen(des_hexkey)));
			if (ft_htbl_has(cmd->opts, "-n")) {
				// Dump vectors in hex format
				char *khex = ft_bytes_to_hex(des_key, sizeof(des_key));
				char *shex = ft_bytes_to_hex(des_salt, sizeof(des_salt));
				ft_printf("salt=%.16s\nkey=%.16s\n", shex, khex);
			}
		}
		else if (ft_htbl_has(cmd->opts, "-s")) {
			// Derive the key from provided salt and user password.
			// Provided salt must be included in the encryption header.
			char *des_hexsalt = ft_htbl_get(cmd->opts, "-s");
			if (!ft_str_ishex(des_hexsalt)) {
				SSL_LOG(ERROR, "salt must be in hex format");
				return (SSL_ERR);
			}
			ft_hex_to_bytes(&des_salt, des_hexsalt, MIN(16, ft_strlen(des_hexsalt)));
			// Get user password
			char des_pass[_PASSWORD_LEN+1] = {0};
			if (ft_htbl_has(cmd->opts, "-p")) {
				char *input = ft_htbl_get(cmd->opts, "-p");
				ft_strncpy(des_pass, input, _PASSWORD_LEN);
			} else {
				char *input = getpass("enter des-ecb encryption password: ");
				if (NULL == input) {
					SSL_LOG(ERROR, "bad password read");
					return (SSL_ERR);
				}
				ft_strncpy(des_pass, input, _PASSWORD_LEN);
				ft_bzero(input, _PASSWORD_LEN);
			}
			des_pass[_PASSWORD_LEN] = 0;
			// Derive key from provided salt and user password.
			if (SSL_OK != rand_openssl_v3_kdf(des_key, sizeof(des_key), NULL, 0, des_salt, sizeof(des_salt), (const uint8_t *)des_pass, ft_strlen(des_pass))) {
				SSL_LOG(ERROR, "failed to generate key from salt");
				return (SSL_ERR);
			}
			if (ft_htbl_has(cmd->opts, "-n")) {
				// Dump vectors in hex format
				char *khex = ft_bytes_to_hex(des_key, sizeof(des_key));
				char *shex = ft_bytes_to_hex(des_salt, sizeof(des_salt));
				ft_printf("salt=%.16s\nkey=%.16s\n", shex, khex);
			}
		}
		else {
			// No salt was provided which means we need to generate random salt.
			// Generated salt must be included in the encryption header.
			// Get the user password.
			char des_pass[_PASSWORD_LEN+1] = {0};
			if (ft_htbl_has(cmd->opts, "-p")) {
				char *input = ft_htbl_get(cmd->opts, "-p");
				ft_strncpy(des_pass, input, _PASSWORD_LEN);
			} else {
				char *input = getpass("enter des-ecb encryption password: ");
				if (NULL == input) {
					SSL_LOG(ERROR, "bad password read");
					return (SSL_ERR);
				}
				ft_strncpy(des_pass, input, _PASSWORD_LEN);
				ft_bzero(input, _PASSWORD_LEN);
			}
			des_pass[_PASSWORD_LEN] = 0;
			// Generate random salt and derive key from generated salt and user password.
			if (SSL_OK != rand_useed((uint64_t *)&des_salt, sizeof(des_salt))) {
				SSL_LOG(ERROR, "failed to generate random salt");
				return (SSL_ERR);
			}
			if (SSL_OK != rand_openssl_v3_kdf(des_key, sizeof(des_key), NULL, 0, des_salt, sizeof(des_salt), (const uint8_t *)des_pass, ft_strlen(des_pass))) {
				SSL_LOG(ERROR, "failed to generate key from salt");
				return (SSL_ERR);
			}
			if (ft_htbl_has(cmd->opts, "-n")) {
				// Dump vectors in hex format
				char *khex = ft_bytes_to_hex(des_key, sizeof(des_key));
				char *shex = ft_bytes_to_hex(des_salt, sizeof(des_salt));
				ft_printf("salt=%.16s\nkey=%.16s\n", shex, khex);
			}
			// Write the encryption header.
			// Header must be 2 DES blocks in size: 1st block must contain "Salted__" string, 2nd block - salt bytes.
			char des_salt_header[2*DES_BLOCK_SIZE] = {0};
			ft_memcpy(des_salt_header, "Salted__", DES_BLOCK_SIZE);
			ft_memcpy(des_salt_header + DES_BLOCK_SIZE, des_salt, DES_BLOCK_SIZE);
			if (io_v2_write_all(out, des_salt_header, sizeof(des_salt_header)) != 2*DES_BLOCK_SIZE) {
				SSL_LOG(ERROR, "failed to write output");
				return (SSL_ERR);
			}
		}
		if (SSL_OK != des_ecb_encrypt_init(&des_ctx, des_key)) {
			SSL_LOG(ERROR, "des-ecb init error");
			return (SSL_ERR);
		}
		t_io_v2_stream *des_filter = NULL;
		if (SSL_OK != io_v2_filter_reader(&des_filter, in, des_ecb_encrypt_transform_update, des_ecb_encrypt_transform_final, &des_ctx)) {
			SSL_LOG(ERROR, IO_INIT_ERROR);
			return (SSL_ERR);
		}
		in = des_filter;
	}

	// Run the pipeline
	t_io_v2_pipe *pipe = NULL;
	if (io_v2_pipe_unidir(&pipe, in, out) < 0) {
		SSL_LOG(ERROR, IO_INIT_ERROR);
		return (SSL_ERR);
	}
	while (pipe->status == IO_V2_STATUS_PIPE_OK) {
		io_v2_pipe_pump(pipe);
	}
	if (pipe->status == IO_V2_STATUS_PIPE_ERROR) {
		SSL_LOG(ERROR, "i/o operation failed");
		return (SSL_ERR);
	}

	// Close all streams
	if (io_v2_close(in) < 0) {
		SSL_LOG(ERROR, IO_CLOSE_ERROR);
		return (SSL_ERR);
	}
	if (io_v2_close(out) < 0) {
		SSL_LOG(ERROR, IO_CLOSE_ERROR);
		return (SSL_ERR);
	}
	return (SSL_OK);
}
