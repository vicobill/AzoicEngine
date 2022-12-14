// Copyright 2010-2021 Crytek GmbH / Crytek Group. All rights reserved.

#include "stdafx.h"
#include "EntityLayer.h"
#include "Entity.h"
#include <CryNetwork/ISerialize.h>
#include <CryPhysics/physinterface.h>
#include <Cry3DEngine/I3DEngine.h>

CEntityLayer::CEntityLayer(const char* name, uint16 id, bool havePhysics, int specs, bool defaultLoaded, TGarbageHeaps& garbageHeaps)
	: m_name(name)
	, m_id(id)
	, m_isEnabled(true)
	, m_isEnabledBrush(false)
	, m_isSerialized(true)
	, m_wasReEnabled(false)
	, m_havePhysics(havePhysics)
	, m_specs(specs)
	, m_defaultLoaded(defaultLoaded)
	, m_listeners(4)
	, m_pGarbageHeaps(&garbageHeaps)
	, m_pHeap(NULL)
{
}

CEntityLayer::~CEntityLayer()
{
	for (TEntityProps::iterator it = m_entities.begin(); it != m_entities.end(); ++it)
	{
		CEntity* pEntity = g_pIEntitySystem->GetEntityFromID(it->first);
		if (!pEntity)
			continue;
		if (it->second.m_bIsNoAwake && pEntity->GetPhysicalProxy() && pEntity->GetPhysicalProxy()->GetPhysicalEntity())
		{
			pe_action_awake aa;
			aa.bAwake = false;
			pEntity->GetPhysicalProxy()->GetPhysicalEntity()->Action(&aa);
		}
		it->second.m_bIsNoAwake = false;
	}

	if (m_pHeap)
		m_pGarbageHeaps->push_back(SEntityLayerGarbage(m_pHeap, m_name));
}

void CEntityLayer::AddObject(EntityId id)
{
	CEntity* pEntity = g_pIEntitySystem->GetEntityFromID(id);
	if (pEntity)
	{
		bool bEnableScriptUpdate = false;
		if (auto* pComponent = static_cast<CEntityComponentLuaScript*>(pEntity->GetComponent<IEntityScriptComponent>()))
		{
			bEnableScriptUpdate = pComponent->IsUpdateEnabled();
		}

		EntityProp& property = m_entities.emplace(id, EntityProp(id, false, pEntity->IsHidden(), bEnableScriptUpdate)).first->second;

		if (!m_isEnabled)
		{
			// Entity layer was hidden, make sure the entity is hidden and the ENTITY_EVENT_LAYER_HIDE is sent correctly
			EnableEntity(*pEntity, property, false);
		}
	}
	m_wasReEnabled = false;
}

void CEntityLayer::RemoveObject(EntityId id)
{
	TEntityProps::iterator it = m_entities.find(id);
	if (it != m_entities.end())
		m_entities.erase(it);
}

bool CEntityLayer::IsSkippedBySpec() const
{
	if (m_specs == eSpecType_All)
		return false;

	auto rt = gEnv->pRenderer ? gEnv->pRenderer->GetRenderType() : ERenderType::Undefined;
	switch (rt)
	{
	case ERenderType::GNM:
		if (m_specs & eSpecType_PS4)
		{
			return false;
		}
		break;

#if CRY_PLATFORM_DURANGO
	case ERenderType::Direct3D11:
	case ERenderType::Direct3D12:
		if (m_specs & eSpecType_XBoxOne)
		{
			return false;
		}
		break;
#else
	case ERenderType::Direct3D11:
	case ERenderType::Direct3D12:
#endif
	case ERenderType::Vulkan:
	default:
		if (m_specs & eSpecType_PC)
		{
			return false;
		}
		break;
	}

	return true;
}

void CEntityLayer::Enable(bool bEnable, bool bSerialize /*=true*/, bool bAllowRecursive /*=true*/)
{
	// Wait for the physics thread to avoid massive amounts of ChangeRequest queuing
	gEnv->pSystem->SetThreadState(ESubsys_Physics, false);

#ifdef ENABLE_PROFILING_CODE
	bool bChanged = (m_isEnabledBrush != bEnable) || (m_isEnabled != bEnable);
	float fStartTime = gEnv->pTimer->GetAsyncCurTime();
#endif //ENABLE_PROFILING_CODE

	if (bEnable && IsSkippedBySpec())
		return;

	MEMSTAT_LABEL_FMT("Layer '%s' %s", m_name.c_str(), (bEnable ? "Activating" : "Deactivating"));
	MEMSTAT_CONTEXT_FMT(EMemStatContextType::Entity, "Layer '%s' %s", m_name.c_str(), (bEnable ? "Activating" : "Deactivating"));

	if (bEnable)
	{
		if (m_pHeap)
			m_pGarbageHeaps->push_back(SEntityLayerGarbage(m_pHeap, m_name));
	}
	else // make sure that children are hidden before parents and unhidden after them
	{
		if (bAllowRecursive && !gEnv->pSystem->IsSerializingFile()) // this check should not be needed now, because Enable() is not used in serialization anymore, but im keeping it for extra sanity check
		{
			for (std::vector<CEntityLayer*>::iterator it = m_childs.begin(); it != m_childs.end(); ++it)
				(*it)->Enable(bEnable);
		}
	}

	EnableBrushes(bEnable);
	EnableEntities(bEnable);

	if (bEnable) // make sure that children are hidden before parents and unhidden after them
	{
		if (bAllowRecursive && !gEnv->pSystem->IsSerializingFile()) // this check should not be needed now, because Enable() is not used in serialization anymore, but im keeping it for extra sanity check
		{
			for (std::vector<CEntityLayer*>::iterator it = m_childs.begin(); it != m_childs.end(); ++it)
				(*it)->Enable(bEnable);
		}
	}

	m_isSerialized = bSerialize;

#ifdef ENABLE_PROFILING_CODE
	if (CVar::es_LayerDebugInfo == 5 && bChanged)
	{
		float fTimeMS = (gEnv->pTimer->GetAsyncCurTime() - fStartTime) * 1000.0f;
		CEntitySystem::SLayerProfile layerProfile;
		layerProfile.pLayer = this;
		layerProfile.fTimeOn = gEnv->pTimer->GetCurrTime();
		layerProfile.isEnable = bEnable;
		layerProfile.fTimeMS = fTimeMS;
		g_pIEntitySystem->m_layerProfiles.insert(g_pIEntitySystem->m_layerProfiles.begin(), layerProfile);
	}
#endif //ENABLE_PROFILING_CODE

	if (m_pHeap && m_pHeap->Cleanup())
	{
		m_pHeap->Release();
		m_pHeap = NULL;
	}

	MEMSTAT_LABEL_FMT("Layer '%s' %s", m_name.c_str(), (bEnable ? "Activated" : "Deactivated"));
}

void CEntityLayer::EnableBrushes(bool isEnable)
{
	if (m_isEnabledBrush != isEnable)
	{
		if (m_id)
		{
			// activate brushes but not static lights
			gEnv->p3DEngine->ActivateObjectsLayer(m_id, isEnable, m_havePhysics, true, false, m_name.c_str(), m_pHeap);
		}
		m_isEnabledBrush = isEnable;
	}

	ReEvalNeedForHeap();
}

void CEntityLayer::EnableEntities(bool isEnable)
{
	if (m_isEnabled != isEnable)
	{
		m_isEnabled = isEnable;

		if (isEnable)
		{
			if (gEnv->pRenderer)
			{
				gEnv->pRenderer->ActivateLayer(m_name.c_str(), isEnable); // Want flash instances on materials activated before entities
			}
		}

		// activate static lights but not brushes
		gEnv->p3DEngine->ActivateObjectsLayer(m_id, isEnable, m_havePhysics, false, true, m_name.c_str(), m_pHeap);

		for (TEntityProps::iterator it = m_entities.begin(); it != m_entities.end(); ++it)
		{
			EntityProp& prop = it->second;

			CEntity* pEntity = g_pIEntitySystem->GetEntityFromID(prop.m_id);

			if (pEntity == nullptr)
				continue;

			EnableEntity(*pEntity, prop, isEnable);
		}

		if (!isEnable)
		{
			if (gEnv->pRenderer)
			{
				gEnv->pRenderer->ActivateLayer(m_name.c_str(), isEnable); // Want flash instances on materials deactivated after entities
			}
		}
		m_wasReEnabled = isEnable;

		SEntityEvent evt(isEnable ? ENTITY_EVENT_LAYER_UNHIDE : ENTITY_EVENT_LAYER_HIDE);
		std::for_each(m_entities.begin(), m_entities.end(), [&evt](const std::pair<EntityId, EntityProp> &pair) 
		{ 
			if (CEntity* pEntity = g_pIEntitySystem->GetEntityFromID(pair.first)) 
				pEntity->SendEvent(evt); 
		});

		NotifyActivationToListeners(isEnable);
	}

	ReEvalNeedForHeap();
}

void CEntityLayer::EnableEntity(CEntity& entity, EntityProp& property, bool isEnable)
{
	// when is serializing (reading, as we never call this on writing), we don't want to change those values. we just use the values that come directly from serialization.
	if (!isEnable && !gEnv->pSystem->IsSerializingFile())
	{
		property.m_bIsHidden = entity.IsHidden();

		if (CEntityComponentLuaScript* pScriptComponent = static_cast<CEntityComponentLuaScript*>(entity.GetComponent<IEntityScriptComponent>()))
		{
			property.m_bEnableScriptUpdate = pScriptComponent->IsUpdateEnabled();
		}
		else
		{
			property.m_bEnableScriptUpdate = false;
		}
	}

	if (property.m_bIsHidden)
		return;

	if (isEnable)
	{
		entity.Hide(!isEnable);

		if (CEntityComponentLuaScript* pScriptComponent = static_cast<CEntityComponentLuaScript*>(entity.GetComponent<IEntityScriptComponent>()))
		{
			pScriptComponent->EnableScriptUpdate(property.m_bEnableScriptUpdate);
		}

		if (property.m_bIsNoAwake && entity.GetPhysicalProxy() && entity.GetPhysicalProxy()->GetPhysicalEntity())
		{
			pe_action_awake noAwake;
			noAwake.bAwake = false;
			entity.GetPhysicalProxy()->GetPhysicalEntity()->Action(&noAwake);
		}

		property.m_bIsNoAwake = false;

		SEntityEvent event;
		event.nParam[0] = 1;
		static IEntityClass* pConstraintClass = g_pIEntitySystem->GetClassRegistry()->FindClass("Constraint");
		if (pConstraintClass && entity.GetClass() == pConstraintClass)
		{
			event.event = ENTITY_EVENT_RESET;
			entity.SendEvent(event);
			event.event = ENTITY_EVENT_LEVEL_LOADED;
			entity.SendEvent(event);
		}
	}
	else
	{
		property.m_bIsNoAwake = false;
		CEntityPhysics* pPhProxy = entity.GetPhysicalProxy();
		if (pPhProxy)
		{
			pe_status_awake isawake;
			IPhysicalEntity* pPhEnt = pPhProxy->GetPhysicalEntity();
			if (pPhEnt && pPhEnt->GetStatus(&isawake) == 0)
				property.m_bIsNoAwake = true;
		}
		entity.Hide(!isEnable);
		if (CEntityComponentLuaScript* pScriptComponent = static_cast<CEntityComponentLuaScript*>(entity.GetComponent<IEntityScriptComponent>()))
		{
			pScriptComponent->EnableScriptUpdate(isEnable);
		}
		if (property.m_bIsNoAwake && entity.GetPhysicalProxy() && entity.GetPhysicalProxy()->GetPhysicalEntity())
		{
			pe_action_awake noAwake;
			noAwake.bAwake = false;
			entity.GetPhysicalProxy()->GetPhysicalEntity()->Action(&noAwake);
		}
	}
}

void CEntityLayer::ReEvalNeedForHeap()
{
	if (!IsEnabled() && m_pHeap)
	{
		m_pGarbageHeaps->push_back(SEntityLayerGarbage(m_pHeap, m_name));
		m_pHeap = NULL;
	}
}

void CEntityLayer::NotifyActivationToListeners(bool bActivated)
{
	for (TListenerSet::Notifier notifier(m_listeners); notifier.IsValid(); notifier.Next())
	{
		notifier->LayerEnabled(bActivated);
	}
}

void CEntityLayer::GetMemoryUsage(ICrySizer* pSizer, int* pOutNumEntities)
{
	if (pOutNumEntities)
		*pOutNumEntities = 0;
	for (TEntityProps::iterator it = m_entities.begin(); it != m_entities.end(); ++it)
	{
		EntityProp& prop = it->second;
		CEntity* pEntity = g_pIEntitySystem->GetEntityFromID(prop.m_id);
		if (pEntity)
		{
			pEntity->GetMemoryUsage(pSizer);
			if (pOutNumEntities)
				(*pOutNumEntities)++;
		}
	}
}

void CEntityLayer::Serialize(TSerialize ser, TLayerActivationOpVec& deferredOps)
{
	bool temp_isEnabledBrush = m_isEnabledBrush;
	bool temp_isEnabled = m_isEnabled;
	ser.BeginGroup("Layer");
	ser.Value("name", m_name);
	ser.Value("enabled", temp_isEnabled);
	ser.Value("enabledBrush", temp_isEnabledBrush);
	ser.Value("m_isSerialized", m_isSerialized);

	if (ser.IsReading())
	{
		int count = 0;
		ser.Value("count", count);

		m_entities.clear();

		for (int i = 0; i < count; ++i)
		{
			ser.BeginGroup("LayerEntities");

			EntityId id = 0;
			bool hidden = false;
			bool noAwake = false;
			bool enableScriptUpdates = false;
			ser.Value("entityId", id);
			ser.Value("hidden", hidden);
			ser.Value("noAwake", noAwake);
			ser.Value("active", enableScriptUpdates);

			EntityProp& prop = m_entities[i];
			prop.m_id = id;
			prop.m_bIsHidden = hidden;
			prop.m_bIsNoAwake = noAwake;
			prop.m_bEnableScriptUpdate = enableScriptUpdates;

			ser.EndGroup();
		}

		SPostSerializeLayerActivation brushActivation;
		brushActivation.m_layer = this;
		brushActivation.m_func = &CEntityLayer::EnableBrushes;
		brushActivation.enable = temp_isEnabledBrush;
		deferredOps.push_back(brushActivation);

		SPostSerializeLayerActivation entityActivation;
		entityActivation.m_layer = this;
		entityActivation.m_func = &CEntityLayer::EnableEntities;
		entityActivation.enable = temp_isEnabled;
		deferredOps.push_back(entityActivation);
	}
	else
	{
		int count = (int)m_entities.size();
		ser.Value("count", count);
		for (TEntityProps::const_iterator it = m_entities.begin(); it != m_entities.end(); ++it)
		{
			ser.BeginGroup("LayerEntities");
			const EntityProp& prop = it->second;
			EntityId id = prop.m_id;
			bool hidden = prop.m_bIsHidden;
			bool noAwake = prop.m_bIsNoAwake;
			bool enableScriptUpdates = prop.m_bEnableScriptUpdate;
			ser.Value("entityId", id);
			ser.Value("hidden", hidden);
			ser.Value("noAwake", noAwake);
			ser.Value("active", enableScriptUpdates);
			ser.EndGroup();
		}
	}

	ser.EndGroup();
}
