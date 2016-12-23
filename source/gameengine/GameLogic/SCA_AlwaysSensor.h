/*
 *
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Contributor(s): none yet.
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file SCA_AlwaysSensor.h
 *  \ingroup gamelogic
 */

#ifndef __SCA_ALWAYSSENSOR_H__
#define __SCA_ALWAYSSENSOR_H__

#include "SCA_ISensor.h"

class SCA_AlwaysSensor : public SCA_ISensor
{
	Py_Header

private:
	bool m_alwaysresult;

public:
	SCA_AlwaysSensor(class SCA_EventManager *eventmgr,
	                 SCA_IObject *gameobj);
	virtual ~SCA_AlwaysSensor();

	virtual CValue *GetReplica();
	virtual bool Evaluate();
	virtual bool IsPositiveTrigger();
	virtual void Init();
};

#endif  //__SCA_ALWAYSSENSOR_H__
