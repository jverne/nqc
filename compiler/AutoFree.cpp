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
#include "AutoFree.h"

AutoFreeGroup& GetAutoFreeGroup()
{
    static AutoFreeGroup afGroup;

    return afGroup;
}


AutoFreeGroup::AutoFreeGroup()
{
    head_.next_ = &tail_;
    head_.prev_ = 0;
    tail_.next_ = 0;
    tail_.prev_ = &head_;
}


void* AutoFreeGroup::allocate(size_t n)
{
    // allocate memory plus overhead for link
    Link *link = (Link *) ::operator new(n + sizeof(Link));

    // link memory into pool list
    link->next_ = head_.next_;
    link->prev_ = &head_;
    link->next_->prev_ = link;
    link->prev_->next_ = link;

    // return pointer to memory past the link
    return (void *)((char *)link + sizeof(Link));
}


void AutoFreeGroup::free(void *ptr)
{
    if (ptr==0) return;

    // determine pointer to link
    Link *link = (Link *)((char *)ptr - sizeof(Link));

    dispose(link);
}


void AutoFreeGroup::dispose(Link *link)
{
    // unlink memory from pool list
    link->prev_->next_ = link->next_;
    link->next_->prev_ = link->prev_;

    // free the memory
    ::operator delete((void*)link);
}


void AutoFreeGroup::freeAll()
{
    Link *link;

    while((link = head_.next_)->next_ != 0) {
        dispose(link);
    }
}
