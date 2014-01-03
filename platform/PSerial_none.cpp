/*
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.
 *
 * The Initial Developer of this code is David Baum.
 * Portions created by David Baum are Copyright (C) 2002 David Baum.
 * All Rights Reserved.
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
