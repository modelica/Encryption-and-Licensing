/*
    Copyright (C) 2015 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the BSD style license.

     This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    BSD_License.txt file for more details.

    You should have received a copy of the BSD_License.txt file
    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
*/

#ifndef MLLE_CR_DECRYPT_H_
#define MLLE_CR_DECRYPT_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Decrypt the data pointed to by in to out, where in_len is the length of the data pointed to by in.
 *
 * The data pointed to by in is assumed to consist of IV, encrypted data and HMAC, in that order.
 * The buffer pointed to by out must be large enough to hold the decrypted data, which is always shorter than in_len.
 * The in and out buffers may be the same.
 *
 * Returns the length of the decrypted data, or -1 on an error.
 */
int mlle_cr_decrypt(char* in,
                    int in_len,
                    char* out);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MLLE_CR_DECRYPT_H_ */
