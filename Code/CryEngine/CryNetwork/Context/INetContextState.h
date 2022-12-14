// Copyright 2011-2021 Crytek GmbH / Crytek Group. All rights reserved.

#ifndef __INETCONTEXTSTATE_H__
#define __INETCONTEXTSTATE_H__

#pragma once

struct INetContextState : public CMultiThreadRefCount
{
	virtual ~INetContextState() {}
	virtual void Die() = 0;
};

#endif // __INETCONTEXTSTATE_H__
