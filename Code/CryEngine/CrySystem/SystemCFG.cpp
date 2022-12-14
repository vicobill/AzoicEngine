// Copyright 2004-2022 Crytek GmbH / Crytek Group. All rights reserved.

#include "StdAfx.h"
#include "System.h"
#include <time.h>
#include "XConsole.h"
#include <CrySystem/File/CryFile.h>

#include <CryScriptSystem/IScriptSystem.h>
#include "SystemCFG.h"
#if CRY_PLATFORM_LINUX || CRY_PLATFORM_ANDROID || CRY_PLATFORM_APPLE
	#include <CryCore/Project/Version.h>
	#include <CrySystem/ILog.h>

#endif
#include "CmdLine.h"

#ifndef EXE_VERSION_INFO_0
	#define EXE_VERSION_INFO_0 1
#endif

#ifndef EXE_VERSION_INFO_1
	#define EXE_VERSION_INFO_1 0
#endif

#ifndef EXE_VERSION_INFO_2
	#define EXE_VERSION_INFO_2 0
#endif

#ifndef EXE_VERSION_INFO_3
	#define EXE_VERSION_INFO_3 1
#endif

//////////////////////////////////////////////////////////////////////////
const SFileVersion& CSystem::GetFileVersion()
{
	return m_fileVersion;
}

//////////////////////////////////////////////////////////////////////////
const SFileVersion& CSystem::GetProductVersion()
{
	return m_productVersion;
}

//////////////////////////////////////////////////////////////////////////
const SFileVersion& CSystem::GetBuildVersion()
{
	return m_buildVersion;
}

//////////////////////////////////////////////////////////////////////////
void CSystem::SystemVersionChanged(ICVar* pCVar)
{
	if (CSystem* pThis = static_cast<CSystem*>(gEnv->pSystem))
	{
		pThis->SetVersionInfo(pCVar->GetString());
	}
}

void CSystem::SetVersionInfo(const char* const szVersion)
{
#ifndef _RELEASE
	m_fileVersion.Set(szVersion);
	m_productVersion.Set(szVersion);
	m_buildVersion.Set(szVersion);
	CryLog("SetVersionInfo '%s'", szVersion);
	CryLog("FileVersion: %s", m_fileVersion.ToString().c_str());
	CryLog("ProductVersion: %s", m_productVersion.ToString().c_str());
	CryLog("BuildVersion: %s", m_buildVersion.ToString().c_str());
#endif
}

//////////////////////////////////////////////////////////////////////////
void CSystem::QueryVersionInfo()
{
#if !CRY_PLATFORM_WINDOWS
	//do we need some other values here?
	m_fileVersion[0] = m_productVersion[0] = EXE_VERSION_INFO_3;
	m_fileVersion[1] = m_productVersion[1] = EXE_VERSION_INFO_2;
	m_fileVersion[2] = m_productVersion[2] = EXE_VERSION_INFO_1;
	m_fileVersion[3] = m_productVersion[3] = EXE_VERSION_INFO_0;
	m_buildVersion = m_fileVersion;
#else
	char moduleName[_MAX_PATH];
	DWORD dwHandle;
	UINT len;

	char ver[1024 * 8];

	GetModuleFileName(NULL, moduleName, _MAX_PATH);  //retrieves the PATH for the current module

	#ifdef _LIB
	GetModuleFileName(NULL, moduleName, _MAX_PATH);  //retrieves the PATH for the current module
	#else //_LIB
	cry_strcpy(moduleName, "CrySystem.dll"); // we want to version from the system dll
	#endif //_LIB

	int verSize = GetFileVersionInfoSize(moduleName, &dwHandle);
	if (verSize > 0)
	{
		GetFileVersionInfo(moduleName, dwHandle, 1024 * 8, ver);
		VS_FIXEDFILEINFO* vinfo;
		VerQueryValue(ver, "\\", (void**)&vinfo, &len);

		m_fileVersion[0] = m_productVersion[0] = vinfo->dwFileVersionLS & 0xFFFF;
		m_fileVersion[1] = m_productVersion[1] = vinfo->dwFileVersionLS >> 16;
		m_fileVersion[2] = m_productVersion[2] = vinfo->dwFileVersionMS & 0xFFFF;
		m_fileVersion[3] = m_productVersion[3] = vinfo->dwFileVersionMS >> 16;
		m_buildVersion = m_fileVersion;

		struct LANGANDCODEPAGE
		{
			WORD wLanguage;
			WORD wCodePage;
		}* lpTranslate;

		UINT count = 0;
		char path[256];
		char* version = NULL;

		VerQueryValue(ver, "\\VarFileInfo\\Translation", (LPVOID*)&lpTranslate, &count);
		if (lpTranslate != NULL)
		{
			cry_sprintf(path, "\\StringFileInfo\\%04x%04x\\InternalName", lpTranslate[0].wLanguage, lpTranslate[0].wCodePage);
			VerQueryValue(ver, path, (LPVOID*)&version, &count);
			if (version)
			{
				m_buildVersion.Set(version);
			}
		}
	}
#endif // !CRY_PLATFORM_WINDOWS
}

//////////////////////////////////////////////////////////////////////////
void CSystem::LogVersion()
{
	// Get time.
	time_t ltime;
	time(&ltime);
	tm* today = localtime(&ltime);

	char s[1024];

	strftime(s, 128, "%d %b %y (%H %M %S)", today);

#if !defined(EXCLUDE_NORMAL_LOG)
	const SFileVersion& ver = GetFileVersion();
	CryLogAlways("BackupNameAttachment=\" Build(%d) %s\"  -- used by backup system\n", ver[0], s);      // read by CreateBackupFile()
#endif

	// Use strftime to build a customized time string.
	strftime(s, 128, "Log Started at %c", today);
	CryLogAlways("%s", s);

	CryLogAlways("Built on " __DATE__ " " __TIME__);
	CryLogAlways("Built for: %s Version: 0x%08zx", GetPlatformDescription(), GetPlatformVersion());
	CryLogAlways("Command Line: %s", m_pCmdLine->GetCommandLine());
#if !CRY_PLATFORM_LINUX && !CRY_PLATFORM_ANDROID && !CRY_PLATFORM_APPLE && !CRY_PLATFORM_DURANGO && !CRY_PLATFORM_ORBIS
	GetModuleFileName(NULL, s, sizeof(s));
	CryLogAlways("Executable: %s", s);
#endif

	CryLogAlways("FileVersion: %d.%d.%d.%d", m_fileVersion[3], m_fileVersion[2], m_fileVersion[1], m_fileVersion[0]);
	CryLogAlways("ProductVersion: %d.%d.%d.%d", m_productVersion[3], m_productVersion[2], m_productVersion[1], m_productVersion[0]);
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
class CCVarSaveDump : public ICVarDumpSink
{
public:

	CCVarSaveDump(FILE* pFile)
	{
		m_pFile = pFile;
	}

	virtual void OnElementFound(ICVar* pCVar)
	{
		if (!pCVar)
			return;
		int nFlags = pCVar->GetFlags();
		if (((nFlags & VF_DUMPTODISK) && (nFlags & VF_MODIFIED)) || (nFlags & VF_WASINCONFIG))
		{
			string szValue = pCVar->GetString();

			size_t pos = 1;
			for (;; )
			{
				pos = szValue.find_first_of("\\", pos);

				if (pos == string::npos)
				{
					break;
				}

				szValue.replace(pos, 1, "\\\\", 2);
				pos += 2;
			}

			// replace " with \"
			pos = 1;
			for (;; )
			{
				pos = szValue.find_first_of("\"", pos);

				if (pos == string::npos)
				{
					break;
				}

				szValue.replace(pos, 1, "\\\"", 2);
				pos += 2;
			}

			string szLine = pCVar->GetName();

			if (pCVar->GetType() == ECVarType::String)
				szLine += " = \"" + szValue + "\"\r\n";
			else
				szLine += " = " + szValue + "\r\n";

			if (pCVar->GetFlags() & VF_WARNING_NOTUSED)
				fputs("-- REMARK: the following was not assigned to a console variable\r\n", m_pFile);

			fputs(szLine.c_str(), m_pFile);
		}
	}

private: // --------------------------------------------------------

	FILE* m_pFile;                  //
};

//////////////////////////////////////////////////////////////////////////
void CSystem::SaveConfiguration()
{
}

//////////////////////////////////////////////////////////////////////////
// system cfg
//////////////////////////////////////////////////////////////////////////
CSystemConfiguration::CSystemConfiguration(const string& strSysConfigFilePath, CSystem* pSystem, ILoadConfigurationEntrySink* pSink, ELoadConfigurationType configType, ELoadConfigurationFlags flags)
	: m_strSysConfigFilePath(strSysConfigFilePath), m_bError(false), m_pSink(pSink), m_configType(configType), m_flags(flags)
{
	CRY_ASSERT(pSink);
	m_pSystem = pSystem;
	m_bError = !ParseSystemConfig();
}

//////////////////////////////////////////////////////////////////////////
CSystemConfiguration::~CSystemConfiguration()
{
}

//////////////////////////////////////////////////////////////////////////
bool CSystemConfiguration::OpenFile(const string& filename, CCryFile& file, int flags)
{
	flags |= ICryPak::FOPEN_HINT_QUIET;

	// Absolute paths first
	if (gEnv->pCryPak->IsAbsPath(filename) || filename[0] == '%')
	{
		if (file.Open(filename, "rb", flags | ICryPak::FLAGS_PATH_REAL))
		{
			return true;
		}
		else
		{
			// the file is absolute and was not found, it is not useful to search further
			return false;
		}
	}

	// If the path is relative, search relevant folders in given order:
	// First search in current folder.
	if (file.Open(string("./") + filename, "rb", flags | ICryPak::FLAGS_PATH_REAL))
	{
		return true;
	}

	string gameFolder = PathUtil::RemoveSlash(gEnv->pCryPak->GetGameFolder());
	if (!filename.empty() && filename[0] == '%')
	{
		// When file name start with alias, not check game folder.
		gameFolder = "";
	}

	// Next search inside game folder (ex, game/game.cfg)
	if (!gameFolder.empty() && file.Open(gameFolder + "/" + filename, "rb", flags | ICryPak::FLAGS_PATH_REAL))
	{
		return true;
	}

	// Next Search in registered mod folders.
	if (file.Open(filename, "rb", flags))
	{
		return true;
	}

	// Next Search in game/config subfolder.
	if (!gameFolder.empty() && file.Open(gameFolder + "/config/" + filename, "rb", flags | ICryPak::FLAGS_PATH_REAL))
	{
		return true;
	}

	// Next Search in config subfolder.
	if (file.Open(string("config/") + filename, "rb", flags | ICryPak::FLAGS_PATH_REAL))
	{
		return true;
	}

	// Next Search in engine root.
	if (file.Open(string("%ENGINEROOT%/") + filename, "rb", flags))
	{
		return true;
	}

	// Next Search in engine folder.
	if (file.Open(string("%ENGINEROOT%/Engine/") + filename, "rb", flags))
	{
		return true;
	}

	// Next Search in engine config subfolder, in case loosely stored on drive
	if (file.Open(string("%ENGINEROOT%/Engine/config/") + filename, "rb", flags))
	{
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
bool CSystemConfiguration::ParseSystemConfig()
{
	string filename = m_strSysConfigFilePath;
	if (strlen(PathUtil::GetExt(filename)) == 0)
	{
		filename = PathUtil::ReplaceExtension(filename, "cfg");
	}

	CCryFile file;

	{
		string filenameLog;

		int flags = ICryPak::FOPEN_HINT_QUIET | ICryPak::FOPEN_ONDISK;

		if (!OpenFile(filename, file, flags))
		{
			if (ELoadConfigurationFlags::None == (m_flags & ELoadConfigurationFlags::SuppressConfigNotFoundWarning))
			{
				CryWarning(VALIDATOR_MODULE_SYSTEM, VALIDATOR_WARNING, "Config file %s not found!", filename.c_str());
			}
			return false;
		}
		filenameLog = file.GetAdjustedFilename();

		CryLog("Loading Config file %s (%s)", filename.c_str(), filenameLog.c_str());
	}

	INDENT_LOG_DURING_SCOPE();

	int nLen = file.GetLength();
	if (nLen == 0)
	{
		CryWarning(VALIDATOR_MODULE_SYSTEM, VALIDATOR_WARNING, "Couldn't get length for Config file %s", filename.c_str());
		return false;
	}
	char* szFullText = new char[nLen + 16];
	if (file.ReadRaw(szFullText, nLen) < (size_t)nLen)
	{
		CryWarning(VALIDATOR_MODULE_SYSTEM, VALIDATOR_WARNING, "Couldn't read Config file %s", filename.c_str());
		return false;
	}
	szFullText[nLen] = '\0';
	szFullText[nLen + 1] = '\0';

	string strGroup; // current group e.g. "[General]"

	char* strLast = szFullText + nLen;
	char* str = szFullText;
	while (str < strLast)
	{
		char* szLineStart = str;
		// find the first line break
		while (str < strLast && *str != '\n' && *str != '\r')
			str++;
		// clear it to handle the line as one string
		*str = '\0';
		str++;
		// and skip any remaining line break chars
		while (str < strLast && (*str == '\n' || *str == '\r'))
			str++;

		//trim all whitespace characters at the beginning and the end of the current line
		string strLine = szLineStart;
		strLine.Trim();

		//skip empty lines
		if (strLine.empty())
			continue;

		// detect groups e.g. "[General]" should set strGroup="General"
		const size_t strLineSize = strLine.size();
		if (strLineSize >= 3)
		{
			if (strLine[0] == '[' && strLine[strLineSize - 1] == ']') // currently no comments are allowed to be behind groups
			{
				strGroup = &strLine[1]; // removes '['
				strGroup.pop_back();    // removes ']'
				continue;               // next line
			}
		}

		//skip comments, comments start with ";" or "--" (any preceding whitespaces have already been trimmed)
		if (strLine[0] == ';' || strLine.find("--") == 0)
			continue;
		
		//if line contains a '=' try to read and assign console variable
		const string::size_type posEq(strLine.find("=", 0));
		if (string::npos != posEq)
		{
			string strKey(strLine, 0, posEq);
			strKey.Trim();

			// extract value and remove surrounding quotes, if present
			string strValue(strLine, posEq + 1, strLine.size() - (posEq + 1));
			strValue.Trim();
			if (strValue.front() == '"' && strValue.back() == '"')
			{
				strValue.pop_back();
				strValue.erase(0, 1);
			}
			
			strValue.replace("\\\\", "\\");
			strValue.replace("\\\"", "\"");

			m_pSink->OnLoadConfigurationEntry(strKey, strValue, strGroup);
		}
		else
		{
			gEnv->pLog->LogWithType(ILog::eWarning, "%s -> invalid configuration line: %s", filename.c_str(), strLine.c_str());
		}
	}

	m_pSink->OnLoadConfigurationEntry_End();
	delete[] szFullText;
	return true;
}

//////////////////////////////////////////////////////////////////////////
void CSystem::OnLoadConfigurationEntry(const char* szKey, const char* szValue, const char* szGroup)
{
	if (!gEnv->pConsole)
		return;

	if (*szKey != 0)
		gEnv->pConsole->LoadConfigVar(szKey, szValue);
}

//////////////////////////////////////////////////////////////////////////
void CSystem::LoadConfiguration(const char* sFilename, ILoadConfigurationEntrySink* pSink, ELoadConfigurationType configType, ELoadConfigurationFlags flags)
{
	ELoadConfigurationType lastType = m_env.pConsole->SetCurrentConfigType(configType);

	if (sFilename && strlen(sFilename) > 0)
	{
		if (!pSink)
			pSink = this;

		CSystemConfiguration tempConfig(sFilename, this, pSink, configType, flags);
	}
	m_env.pConsole->SetCurrentConfigType(lastType);
}