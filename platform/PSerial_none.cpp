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
#include "PSerial.h"

class PSerial_none : public PSerial
{
public:
	virtual	bool	Open(const char *) { return false; }
	virtual void	Close() {}
	virtual long	Write(const void *, long ) { return 0; }
	virtual long	Read(void *, long ) { return 0; }
	virtual bool	SetSpeed(int , int) { return false; }

private:
};


PSerial* PSerial::NewSerial()
{
	return new PSerial_none();
}


const char *PSerial::GetDefaultName()
{
	return "none";
}
