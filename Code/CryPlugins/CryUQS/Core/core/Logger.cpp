// Copyright 2016-2021 Crytek GmbH / Crytek Group. All rights reserved.

#include "StdAfx.h"
#include "Logger.h"

// *INDENT-OFF* - <hard to read code and declarations due to inconsistent indentation>

namespace UQS
{
	namespace Core
	{

		//===================================================================================
		//
		// CLogger
		//
		//===================================================================================

		int CLoggerIndentation::s_indentLevel;

		CLoggerIndentation::CLoggerIndentation()
		{
			++s_indentLevel;
		}

		CLoggerIndentation::~CLoggerIndentation()
		{
			CRY_ASSERT(s_indentLevel > 0);
			--s_indentLevel;
		}

		int CLoggerIndentation::GetCurrentIndentLevel()
		{
			return s_indentLevel;
		}

		//===================================================================================
		//
		// CLogger
		//
		//===================================================================================

		void CLogger::Printf(const char* szFormat, ...)
		{
			va_list args;
			char text[1024];

			va_start(args, szFormat);
			cry_vsprintf(text, szFormat, args);
			va_end(args);

			CryLog("%*s%s", CLoggerIndentation::GetCurrentIndentLevel() * 4, "", text);
		}

	}
}
