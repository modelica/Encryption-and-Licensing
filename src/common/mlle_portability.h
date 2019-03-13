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

#ifndef MLLE_PORTABILITY_H_
#define MLLE_PORTABILITY_H_
#define _CRT_SECURE_NO_WARNINGS

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#include <io.h>
#  undef X509_NAME 
#  undef X509_EXTENSIONS 
#  undef PKCS7_ISSUER_AND_SERIAL 
#  undef OCSP_REQUEST 
#  undef OCSP_RESPONSE

#ifdef _MSC_VER 
#define strcasecmp(s1, s2) _stricmp(s1, s2)
#if _MSC_VER < 1900
#define snprintf sprintf_s
#endif
#define strtok_r(s, d, c) strtok_s(s, d, c)
#define getcwd(p, l) _getcwd(p, l)
#define strdup(s) _strdup(s)
#define close(fd) _close(fd)

#define _CRT_SECURE_NO_WARNINGS
#endif

#define PATH_MAX 260

#else /* _WIN32 */
#include <unistd.h>
#include <strings.h>
#endif /* _WIN32 */

#endif /* MLLE_PORTABILITY_H_ */
