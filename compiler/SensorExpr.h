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
 * Portions created by David Baum are Copyright (C) 2000 David Baum.
 * All Rights Reserved.
 *
 * Portions created by John Hansen are Copyright (C) 2005 John Hansen.
 * All Rights Reserved.
 *
 */

/*
 * This class is a kludge that is necessary to allow SENSOR_x to be used
 * interchangeably with the index of the sensor.  In these cases, a valid
 * expression is either a constant or an EA of source 2.  The SensorExpr()
 * class checks for these conditions and evaluates to the actual index of
 * the sensor.
 */

#ifndef __SensorExpr_h
#define __SensorExpr_h

#include "NodeExpr.h"


class SensorExpr : public NodeExpr
{
public:
		SensorExpr(Expr *e) : NodeExpr(e) {}

	virtual Expr*		Clone(Mapping *b) const;

	virtual RCX_Value	EmitAny_(Bytecode &b) const;

	virtual bool		PromiseConstant() const	{ return true; }
	virtual bool		Evaluate(int &) const;
};


#endif
