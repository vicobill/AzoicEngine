// Copyright 2017-2021 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include <CryAudio/IAudioInterfacesCommonData.h>
#include <CrySerialization/Forward.h>
#include <CrySchematyc/Reflection/TypeDesc.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CryEntitySystem/IEntityComponent.h>

class CPlugin_CryDefaultEntities;

namespace Cry
{
namespace Audio
{
namespace DefaultComponents
{
struct SPreloadSerializeHelper final
{
	void Serialize(Serialization::IArchive& archive);
	bool operator==(SPreloadSerializeHelper const& other) const { return m_id == other.m_id; }

	CryAudio::PreloadRequestId m_id = CryAudio::InvalidPreloadRequestId;
	string                     m_name;
};

class CPreloadComponent final : public IEntityComponent
{
protected:

	friend CPlugin_CryDefaultEntities;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

	// IEntityComponent
	virtual void                    Initialize() override;
	virtual void                    OnShutDown() override;
	virtual Cry::Entity::EventFlags GetEventMask() const override                    { return Cry::Entity::EventFlags(); }
	virtual void                    ProcessEvent(const SEntityEvent& event) override {}
	// ~IEntityComponent

	// Properties exposed to UI
	SPreloadSerializeHelper m_preload;

public:

	CPreloadComponent() = default;

	static void ReflectType(Schematyc::CTypeDesc<CPreloadComponent>& desc);

	void        Load();
	void        Unload();

private:

	bool m_bLoaded = false;

#if defined(INCLUDE_DEFAULT_PLUGINS_PRODUCTION_CODE)
	Serialization::FunctorActionButton<std::function<void()>> m_loadButton;
	Serialization::FunctorActionButton<std::function<void()>> m_unloadButton;
#endif  // INCLUDE_DEFAULT_PLUGINS_PRODUCTION_CODE
};

//////////////////////////////////////////////////////////////////////////
inline void ReflectType(Schematyc::CTypeDesc<SPreloadSerializeHelper>& desc)
{
	desc.SetGUID("C9353DDE-5F53-482F-AFEF-7D6A500ACDD9"_cry_guid);
}

//////////////////////////////////////////////////////////////////////////
inline void SPreloadSerializeHelper::Serialize(Serialization::IArchive& archive)
{
	archive(Serialization::AudioPreloadRequest<string>(m_name), "preloadName", "^");

	if (archive.isInput())
	{
		m_id = CryAudio::StringToId(m_name.c_str());
	}
}
} // namespace DefaultComponents
} // namespace Audio
} // namespace Cry
