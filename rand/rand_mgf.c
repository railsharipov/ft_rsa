// #include <ft_ssl.h>
// #include <ssl_hash.h>
// #include <ssl_rand.h>
//
// int rand_mgf(char *source, int slen, char *mask, int mlen)
// {
//   t_hash      hash;
//   t_io        hin;
//   int         tbytes;
//   char        buf[slen+sizeof(int)];
//   static int  state;
//
//   if ((NULL == source) || (NULL == mask))
//     return (SSL_ERR);
//
//   if ((state == 0) && (SSL_OK != rand_mseed(&state, 384)))
//     return (SSL_ERR);
//
//   io_init(&hin);
//   hin.flag = (IO_INPUT | IO_STRING);
//   hin.input = buf;
//   hin.insize = slen + sizeof(int);
//   if (NULL == (hin.func = io_func(&hin)))
//     return (SSL_ERROR("hash input error"));
//
//   state = ft_hash(&state, sizeof(int));
//   ft_memcpy(buf, source, slen);
//
//   tbytes = 0;
//   while (tbytes < mlen)
//   {
//     state += tbytes;
//     ft_memcpy(buf+slen, &state, sizeof(int));
//     io_init_buf(&hin);
//     hash_init(&hash);
//     hash.flag = SHA256;
//     if (SSL_OK != hash_sha256(&hash, &hin, NULL, 0))
//       return (SSL_ERROR("hash error"));
//     ft_memcpy(mask+tbytes, hash.hash, MIN(32, mlen-tbytes));
//     tbytes += 32;
//   }
//   return (SSL_OK);
// }
