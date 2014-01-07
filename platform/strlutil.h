/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef __strlutil_h
#define __strlutil_h

#ifdef __cplusplus
extern "C" {
#endif
    
size_t strlcpy(char *dst, const char *src, size_t size);
size_t strlcat(char *dst, const char *src, size_t size);

#ifdef __cplusplus
}
#endif

 #endif // __strlutil_h