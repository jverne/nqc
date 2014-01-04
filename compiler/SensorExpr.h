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
