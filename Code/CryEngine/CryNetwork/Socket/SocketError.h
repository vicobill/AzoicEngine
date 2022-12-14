// Copyright 2006-2021 Crytek GmbH / Crytek Group. All rights reserved.

#ifndef __SOCKETERROR_H__
#define __SOCKETERROR_H__

#pragma once

#include <CryNetwork/CrySocketError.h>

ESocketError OSErrorToSocketError(uint32 oserr);

#endif
