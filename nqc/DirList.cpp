/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * The Initial Developer of this code is David Baum.
 * Portions created by David Baum are Copyright (C) 1999 David Baum.
 * All Rights Reserved.
 *
 * Portions created by John Hansen are Copyright (C) 2005 John Hansen.
 * All Rights Reserved.
 *
 */
#include <cstring>
#include <strlutil.h>

#if defined(__MWERKS__) && (!__MACH__)
#include <stat.h>
#else
#include <sys/stat.h>
#endif
#include "DirList.h"

using std::strcat;
using std::strlen;
using std::size_t;

#ifdef DEBUG
using std::strcpy;
#endif

DirList::~DirList()
{
    Entry *e;

    while((e=fEntries.RemoveHead()) != 0)
        delete e;
}


void DirList::Add(const char *path)
{
    // ignore NULL or empty path
    if (path==0 || *path==0) return;

    Entry *e = new Entry(path);
    fEntries.InsertTail(e);
}


bool DirList::Find(const char *filename, char *pathname)
{
    struct stat stat_buf;

    size_t len = sizeof(pathname);
    if (strlcpy(pathname, filename, len) >= len) {
        return false;
    }

    if (stat(pathname, &stat_buf) == 0)
        return true;

    for(Entry *e = fEntries.GetHead(); e; e=e->GetNext()) {
        if (strlcpy(pathname, e->GetPath(), len) < len) {
            if (strlcat(pathname, filename, len) < len) {
                if (stat(pathname, &stat_buf) == 0) {
                    return true;
                }
            }
        }
    }

    return false;
}


DirList::Entry::Entry(const char *path)
{
    size_t length = strlen(path);

    fPath = new char[length+2]; // leave room for terminating delimiter
    strcpy(fPath, path);

    // should we append the delimiter?
    if (path[length-1] != DIR_DELIMITER) {
        fPath[length] = DIR_DELIMITER;
        fPath[length+1] = 0;
    }
}


DirList::Entry::~Entry()
{
    delete [] fPath;
}
