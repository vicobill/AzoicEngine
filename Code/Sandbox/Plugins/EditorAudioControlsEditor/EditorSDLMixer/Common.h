// Copyright 2018-2021 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include "Item.h"

#include <CryIcon.h>
#include <CryAudioImplSDLMixer/GlobalData.h>
#include <FileDialogs/ExtensionFilter.h>

namespace ACE
{
namespace Impl
{
namespace SDLMixer
{
class CDataPanel;

extern CDataPanel* g_pDataPanel;

using ConnectionsByContext = std::map<CryAudio::ContextId, CryAudio::Impl::SDL_mixer::SPoolSizes>;
extern ConnectionsByContext g_connections;

static CryIcon s_errorIcon;
static CryIcon s_eventIcon;
static CryIcon s_folderIcon;

static QString const s_emptyTypeName("");
static QString const s_eventTypeName("Audio File");
static QString const s_folderTypeName("Folder");

//////////////////////////////////////////////////////////////////////////
inline void InitIcons()
{
	s_errorIcon = CryIcon("icons:Dialogs/dialog-error.ico");
	s_eventIcon = CryIcon("icons:audio/impl/sdlmixer/event.ico");
	s_folderIcon = CryIcon("icons:General/Folder.ico");
}

//////////////////////////////////////////////////////////////////////////
inline CryIcon const& GetTypeIcon(EItemType const type)
{
	switch (type)
	{
	case EItemType::Event:
		{
			return s_eventIcon;
		}
	case EItemType::Folder:
		{
			return s_folderIcon;
		}
	default:
		{
			return s_errorIcon;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
inline QString const& TypeToString(EItemType const type)
{
	switch (type)
	{
	case EItemType::Event:
		{
			return s_eventTypeName;
		}
	case EItemType::Folder:
		{
			return s_folderTypeName;
		}
	default:
		{
			return s_emptyTypeName;
		}
	}
}
} // namespace SDLMixer
} // namespace Impl
} // namespace ACE
