// Copyright 2004-2021 Crytek GmbH / Crytek Group. All rights reserved.

#include "stdafx.h"
#include "EntityAudioProxy.h"
#include "Entity.h"
#include <CryAudio/IAudioSystem.h>
#include <CryAudio/IListener.h>

CRYREGISTER_CLASS(CEntityComponentAudio);

CEntityComponentAudio::AuxObjectPair CEntityComponentAudio::s_nullAuxObjectPair(CryAudio::InvalidAuxObjectId, SAuxObjectWrapper(nullptr));

//////////////////////////////////////////////////////////////////////////
CEntityComponentAudio::CEntityComponentAudio()
	: m_auxObjectIdCounter(CryAudio::InvalidAuxObjectId)
	, m_environmentId(CryAudio::InvalidEnvironmentId)
	, m_flags(eEntityAudioProxyFlags_CanMoveWithEntity)
	, m_fadeDistance(0.0f)
	, m_environmentFadeDistance(0.0f)
{
	m_componentFlags.Add(EEntityComponentFlags::NoSave);
}

//////////////////////////////////////////////////////////////////////////
CEntityComponentAudio::~CEntityComponentAudio()
{
	std::for_each(m_mapAuxObjects.begin(), m_mapAuxObjects.end(), SReleaseAudioProxy());
	m_mapAuxObjects.clear();
}

//////////////////////////////////////////////////////////////////////////
void CEntityComponentAudio::Initialize()
{
	CRY_ASSERT(m_mapAuxObjects.empty());

	// Creating the default AudioProxy.
	CreateAudioAuxObject();
	OnMove();
}

//////////////////////////////////////////////////////////////////////////
void CEntityComponentAudio::OnMove()
{
	Matrix34 const tm = GetWorldTransformMatrix();
	CRY_ASSERT(tm.IsValid(), "Invalid Matrix34 during CEntityComponentAudio::OnMove");

	if ((m_flags & eEntityAudioProxyFlags_CanMoveWithEntity) != 0)
	{
		std::for_each(m_mapAuxObjects.begin(), m_mapAuxObjects.end(), SRepositionAudioProxy(tm, CryAudio::SRequestUserData::GetEmptyObject()));
	}
}

//////////////////////////////////////////////////////////////////////////
void CEntityComponentAudio::OnTransformChanged()
{
	OnMove();
}

//////////////////////////////////////////////////////////////////////////
void CEntityComponentAudio::OnListenerMoveInside(Vec3 const& listenerPos)
{
	m_pEntity->SetPos(listenerPos);
}

//////////////////////////////////////////////////////////////////////////
void CEntityComponentAudio::OnListenerExclusiveMoveInside(CEntity const* const __restrict pEntity, CEntity const* const __restrict pAreaHigh, CEntity const* const __restrict pAreaLow, float const fade)
{
	IEntityAreaComponent const* const __restrict pAreaProxyLow = static_cast<IEntityAreaComponent const* const __restrict>(pAreaLow->GetProxy(ENTITY_PROXY_AREA));
	IEntityAreaComponent* const __restrict pAreaProxyHigh = static_cast<IEntityAreaComponent* const __restrict>(pAreaHigh->GetProxy(ENTITY_PROXY_AREA));

	if (pAreaProxyLow != nullptr && pAreaProxyHigh != nullptr)
	{
		Vec3 onHighHull3d(ZERO);
		Vec3 const pos(pEntity->GetWorldPos());
		EntityId const entityId = pEntity->GetId();
		bool const bInsideLow = pAreaProxyLow->CalcPointWithin(entityId, pos);

		if (bInsideLow)
		{
			pAreaProxyHigh->ClosestPointOnHullDistSq(entityId, pos, onHighHull3d);
			m_pEntity->SetPos(onHighHull3d);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void CEntityComponentAudio::OnListenerEnter(CEntity const* const pEntity)
{
	m_pEntity->SetPos(pEntity->GetWorldPos());
}

//////////////////////////////////////////////////////////////////////////
void CEntityComponentAudio::OnListenerMoveNear(Vec3 const& closestPointToArea)
{
	m_pEntity->SetPos(closestPointToArea);
}

//////////////////////////////////////////////////////////////////////////
Cry::Entity::EventFlags CEntityComponentAudio::GetEventMask() const
{
	return ENTITY_EVENT_XFORM | ENTITY_EVENT_ENTERAREA | ENTITY_EVENT_MOVENEARAREA | ENTITY_EVENT_ENTERNEARAREA | ENTITY_EVENT_MOVEINSIDEAREA | ENTITY_EVENT_SET_NAME;
}

//////////////////////////////////////////////////////////////////////////
void CEntityComponentAudio::ProcessEvent(const SEntityEvent& event)
{
	if (m_pEntity != nullptr)
	{
		switch (event.event)
		{
		case ENTITY_EVENT_XFORM:
			{
				int const flags = static_cast<int>(event.nParam[0]);

				if ((flags & (ENTITY_XFORM_POS | ENTITY_XFORM_ROT)) > 0)
				{
					OnMove();
				}

				break;
			}
		case ENTITY_EVENT_ENTERAREA:
			{
				if ((m_pEntity->GetFlags() & ENTITY_FLAG_VOLUME_SOUND) > 0)
				{
					EntityId const entityId = static_cast<EntityId>(event.nParam[0]); // Entering entity!
					CEntity* const pIEntity = g_pIEntitySystem->GetEntityFromID(entityId);

					if ((pIEntity != nullptr) && (pIEntity->GetFlagsExtended() & ENTITY_FLAG_EXTENDED_AUDIO_LISTENER) > 0)
					{
						OnListenerEnter(pIEntity);
					}
				}

				break;
			}
		case ENTITY_EVENT_MOVENEARAREA:
		case ENTITY_EVENT_ENTERNEARAREA:
			{
				if ((m_pEntity->GetFlags() & ENTITY_FLAG_VOLUME_SOUND) > 0)
				{
					EntityId const entityId = static_cast<EntityId>(event.nParam[0]); // Near entering/moving entity!
					CEntity* const pIEntity = g_pIEntitySystem->GetEntityFromID(entityId);

					if (pIEntity != nullptr)
					{
						if ((pIEntity->GetFlagsExtended() & ENTITY_FLAG_EXTENDED_AUDIO_LISTENER) > 0)
						{
							OnListenerMoveNear(event.vec);
						}
					}
				}

				break;
			}
		case ENTITY_EVENT_MOVEINSIDEAREA:
			{
				if ((m_pEntity->GetFlags() & ENTITY_FLAG_VOLUME_SOUND) > 0)
				{
					EntityId const entityId = static_cast<EntityId>(event.nParam[0]); // Inside moving entity!
					CEntity* const __restrict pIEntity = g_pIEntitySystem->GetEntityFromID(entityId);

					if (pIEntity != nullptr)
					{
						EntityId const area1Id = static_cast<EntityId>(event.nParam[2]); // AreaEntityID (low)
						EntityId const area2Id = static_cast<EntityId>(event.nParam[3]); // AreaEntityID (high)

						CEntity* const __restrict pArea1 = g_pIEntitySystem->GetEntityFromID(area1Id);
						CEntity* const __restrict pArea2 = g_pIEntitySystem->GetEntityFromID(area2Id);

						if (pArea1 != nullptr)
						{
							if (pArea2 != nullptr)
							{
								if ((pIEntity->GetFlagsExtended() & ENTITY_FLAG_EXTENDED_AUDIO_LISTENER) > 0)
								{
									OnListenerExclusiveMoveInside(pIEntity, pArea2, pArea1, event.fParam[0]);
								}
							}
							else
							{
								if ((pIEntity->GetFlagsExtended() & ENTITY_FLAG_EXTENDED_AUDIO_LISTENER) > 0)
								{
									OnListenerMoveInside(event.vec);
								}
							}
						}
					}
				}

				break;
			}
#if defined(INCLUDE_ENTITYSYSTEM_PRODUCTION_CODE)
		case ENTITY_EVENT_SET_NAME:
			{
				CryFixedStringT<CryAudio::MaxObjectNameLength> name(m_pEntity->GetName());
				size_t numAuxObjects = 0;

				for (auto const& objectPair : m_mapAuxObjects)
				{
					if (numAuxObjects > 0)
					{
						// First AuxAudioObject is not explicitly identified, it keeps the entity's name.
						// All additional objects however are being explicitly identified.
						name.Format("%s_aux_object_#%" PRISIZE_T, m_pEntity->GetName(), numAuxObjects + 1);
					}

					objectPair.second.pIObject->SetName(name.c_str());
					++numAuxObjects;
				}

				break;
			}
#endif // INCLUDE_ENTITYSYSTEM_PRODUCTION_CODE
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void CEntityComponentAudio::GameSerialize(TSerialize ser)
{
}

//////////////////////////////////////////////////////////////////////////
bool CEntityComponentAudio::ExecuteTrigger(
	CryAudio::ControlId const audioTriggerId,
	CryAudio::AuxObjectId const audioAuxObjectId /* = DefaultAuxObjectId */,
	EntityId const entityId /*= INVALID_ENTITYID*/,
	CryAudio::SRequestUserData const& userData /* = SAudioRequestUserData::GetEmptyObject() */)
{
	if (m_pEntity != nullptr)
	{
		if (audioAuxObjectId != CryAudio::InvalidAuxObjectId)
		{
			AuxObjectPair const& audioObjectPair = GetAudioAuxObjectPair(audioAuxObjectId);

			if (audioObjectPair.first != CryAudio::InvalidAuxObjectId)
			{
				audioObjectPair.second.pIObject->ExecuteTrigger(audioTriggerId, entityId, userData);
				return true;
			}
#if defined(INCLUDE_ENTITYSYSTEM_PRODUCTION_CODE)
			else
			{
				gEnv->pSystem->Warning(VALIDATOR_MODULE_ENTITYSYSTEM, VALIDATOR_WARNING, VALIDATOR_FLAG_AUDIO, nullptr, "<Audio> Could not find AuxAudioProxy with id '%u' on entity '%s' to ExecuteTrigger '%u'", audioAuxObjectId, m_pEntity->GetEntityTextDescription().c_str(), audioTriggerId);
			}
#endif  // INCLUDE_ENTITYSYSTEM_PRODUCTION_CODE
		}
		else
		{
			for (auto const& auxObjectPair : m_mapAuxObjects)
			{
				auxObjectPair.second.pIObject->ExecuteTrigger(audioTriggerId, entityId, userData);
			}
			return !m_mapAuxObjects.empty();
		}
	}
	else
	{
		gEnv->pSystem->Warning(VALIDATOR_MODULE_ENTITYSYSTEM, VALIDATOR_WARNING, VALIDATOR_FLAG_AUDIO, nullptr, "<Audio> Trying to execute an audio trigger on an EntityAudioProxy without a valid entity!");
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
bool CEntityComponentAudio::ExecuteTriggerWithCallbacks(
	CryAudio::STriggerCallbackData const& callbackData,
	CryAudio::AuxObjectId const audioAuxObjectId /* = DefaultAuxObjectId */,
	EntityId const entityId /*= INVALID_ENTITYID*/,
	CryAudio::SRequestUserData const& userData /* = SAudioRequestUserData::GetEmptyObject() */)
{
	if (m_pEntity != nullptr)
	{
		if (audioAuxObjectId != CryAudio::InvalidAuxObjectId)
		{
			AuxObjectPair const& audioObjectPair = GetAudioAuxObjectPair(audioAuxObjectId);

			if (audioObjectPair.first != CryAudio::InvalidAuxObjectId)
			{
				audioObjectPair.second.pIObject->ExecuteTriggerWithCallbacks(callbackData, entityId, userData);
				return true;
			}
#if defined(INCLUDE_ENTITYSYSTEM_PRODUCTION_CODE)
			else
			{
				gEnv->pSystem->Warning(VALIDATOR_MODULE_ENTITYSYSTEM, VALIDATOR_WARNING, VALIDATOR_FLAG_AUDIO, nullptr, "<Audio> Could not find AuxAudioProxy with id '%u' on entity '%s' to execute trigger with id '%u'", audioAuxObjectId, m_pEntity->GetEntityTextDescription().c_str(), callbackData.triggerId);
			}
#endif  // INCLUDE_ENTITYSYSTEM_PRODUCTION_CODE
		}
		else
		{
			for (auto const& auxObjectPair : m_mapAuxObjects)
			{
				auxObjectPair.second.pIObject->ExecuteTriggerWithCallbacks(callbackData, entityId, userData);
			}
			return !m_mapAuxObjects.empty();
		}
	}
	else
	{
		gEnv->pSystem->Warning(VALIDATOR_MODULE_ENTITYSYSTEM, VALIDATOR_WARNING, VALIDATOR_FLAG_AUDIO, nullptr, "<Audio> Trying to execute an audio trigger on an EntityAudioProxy without a valid entity!");
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
void CEntityComponentAudio::StopTrigger(
	CryAudio::ControlId const audioTriggerId,
	CryAudio::AuxObjectId const audioAuxObjectId /* = DefaultAuxObjectId */,
	CryAudio::SRequestUserData const& userData /* = SAudioRequestUserData::GetEmptyObject() */)
{
	if (audioAuxObjectId != CryAudio::InvalidAuxObjectId)
	{
		AuxObjectPair const& audioObjectPair = GetAudioAuxObjectPair(audioAuxObjectId);

		if (audioObjectPair.first != CryAudio::InvalidAuxObjectId)
		{
			(SStopTrigger(audioTriggerId, userData))(audioObjectPair);
		}
	}
	else
	{
		std::for_each(m_mapAuxObjects.begin(), m_mapAuxObjects.end(), SStopTrigger(audioTriggerId, userData));
	}
}

//////////////////////////////////////////////////////////////////////////
void CEntityComponentAudio::SetSwitchState(CryAudio::ControlId const audioSwitchId, CryAudio::SwitchStateId const audioStateId, CryAudio::AuxObjectId const audioAuxObjectId /*= DEFAULT_AUDIO_PROXY_ID*/)
{
	if (audioAuxObjectId != CryAudio::InvalidAuxObjectId)
	{
		AuxObjectPair const& audioObjectPair = GetAudioAuxObjectPair(audioAuxObjectId);

		if (audioObjectPair.first != CryAudio::InvalidAuxObjectId)
		{
			(SSetSwitchState(audioSwitchId, audioStateId))(audioObjectPair);
		}
	}
	else
	{
		std::for_each(m_mapAuxObjects.begin(), m_mapAuxObjects.end(), SSetSwitchState(audioSwitchId, audioStateId));
	}
}

//////////////////////////////////////////////////////////////////////////
void CEntityComponentAudio::SetParameter(CryAudio::ControlId const parameterId, float const value, CryAudio::AuxObjectId const audioAuxObjectId /*= DefaultAuxObjectId*/)
{
	if (audioAuxObjectId != CryAudio::InvalidAuxObjectId)
	{
		AuxObjectPair const& audioObjectPair = GetAudioAuxObjectPair(audioAuxObjectId);

		if (audioObjectPair.first != CryAudio::InvalidAuxObjectId)
		{
			(SSetParameter(parameterId, value))(audioObjectPair);
		}
	}
	else
	{
		std::for_each(m_mapAuxObjects.begin(), m_mapAuxObjects.end(), SSetParameter(parameterId, value));
	}
}

//////////////////////////////////////////////////////////////////////////
void CEntityComponentAudio::SetObstructionCalcType(CryAudio::EOcclusionType const occlusionType, CryAudio::AuxObjectId const audioAuxObjectId /*= DEFAULT_AUDIO_PROXY_ID*/)
{
	if (audioAuxObjectId != CryAudio::InvalidAuxObjectId)
	{
		AuxObjectPair const& audioObjectPair = GetAudioAuxObjectPair(audioAuxObjectId);

		if (audioObjectPair.first != CryAudio::InvalidAuxObjectId)
		{
			(SSetOcclusionType(occlusionType))(audioObjectPair);
		}
	}
	else
	{
		std::for_each(m_mapAuxObjects.begin(), m_mapAuxObjects.end(), SSetOcclusionType(occlusionType));
	}
}

//////////////////////////////////////////////////////////////////////////
void CEntityComponentAudio::SetOcclusionRayOffset(float const offset, CryAudio::AuxObjectId const audioAuxObjectId /*= CryAudio::DefaultAuxObjectId*/)
{
	if (audioAuxObjectId != CryAudio::InvalidAuxObjectId)
	{
		AuxObjectPair const& audioObjectPair = GetAudioAuxObjectPair(audioAuxObjectId);

		if (audioObjectPair.first != CryAudio::InvalidAuxObjectId)
		{
			(SSetOcclusionRayOffset(offset))(audioObjectPair);
		}
	}
	else
	{
		std::for_each(m_mapAuxObjects.begin(), m_mapAuxObjects.end(), SSetOcclusionRayOffset(offset));
	}
}

//////////////////////////////////////////////////////////////////////////
void CEntityComponentAudio::SetEnvironmentAmount(CryAudio::EnvironmentId const audioEnvironmentId, float const amount, CryAudio::AuxObjectId const audioAuxObjectId /*= DEFAULT_AUDIO_PROXY_ID*/)
{
	if (audioAuxObjectId != CryAudio::InvalidAuxObjectId)
	{
		AuxObjectPair const& audioObjectPair = GetAudioAuxObjectPair(audioAuxObjectId);

		if (audioObjectPair.first != CryAudio::InvalidAuxObjectId)
		{
			SSetEnvironmentAmount(audioEnvironmentId, amount)(audioObjectPair);
		}
	}
	else
	{
		std::for_each(m_mapAuxObjects.begin(), m_mapAuxObjects.end(), SSetEnvironmentAmount(audioEnvironmentId, amount));
	}
}

//////////////////////////////////////////////////////////////////////////
void CEntityComponentAudio::SetCurrentEnvironments(CryAudio::AuxObjectId const audioAuxObjectId /*= DEFAULT_AUDIO_PROXY_ID*/)
{
	if (audioAuxObjectId != CryAudio::InvalidAuxObjectId)
	{
		AuxObjectPair const& audioObjectPair = GetAudioAuxObjectPair(audioAuxObjectId);

		if (audioObjectPair.first != CryAudio::InvalidAuxObjectId)
		{
			SSetCurrentEnvironments(m_pEntity->GetId())(audioObjectPair);
		}
	}
	else
	{
		std::for_each(m_mapAuxObjects.begin(), m_mapAuxObjects.end(), SSetCurrentEnvironments(m_pEntity->GetId()));
	}
}

//////////////////////////////////////////////////////////////////////////
void CEntityComponentAudio::AudioAuxObjectsMoveWithEntity(bool const bCanMoveWithEntity)
{
	if (bCanMoveWithEntity)
	{
		m_flags |= eEntityAudioProxyFlags_CanMoveWithEntity;
	}
	else
	{
		m_flags &= ~eEntityAudioProxyFlags_CanMoveWithEntity;
	}
}

//////////////////////////////////////////////////////////////////////////
void CEntityComponentAudio::AddAsListenerToAudioAuxObject(CryAudio::AuxObjectId const audioAuxObjectId, void (*func)(CryAudio::SRequestInfo const* const), CryAudio::ESystemEvents const eventMask)
{
	AuxObjects::const_iterator const iter(m_mapAuxObjects.find(audioAuxObjectId));

	if (iter != m_mapAuxObjects.end())
	{
		gEnv->pAudioSystem->AddRequestListener(func, iter->second.pIObject, eventMask);
	}
}

//////////////////////////////////////////////////////////////////////////
void CEntityComponentAudio::RemoveAsListenerFromAudioAuxObject(CryAudio::AuxObjectId const audioAuxObjectId, void (*func)(CryAudio::SRequestInfo const* const))
{
	AuxObjects::const_iterator const iter(m_mapAuxObjects.find(audioAuxObjectId));

	if (iter != m_mapAuxObjects.end())
	{
		gEnv->pAudioSystem->RemoveRequestListener(func, iter->second.pIObject);
	}
}

//////////////////////////////////////////////////////////////////////////
void CEntityComponentAudio::SetAudioAuxObjectOffset(Matrix34 const& offset, CryAudio::AuxObjectId const audioAuxObjectId /*= DEFAULT_AUDIO_PROXY_ID*/)
{
	if (audioAuxObjectId != CryAudio::InvalidAuxObjectId)
	{
		AuxObjectPair& audioObjectPair = GetAudioAuxObjectPair(audioAuxObjectId);

		if (audioObjectPair.first != CryAudio::InvalidAuxObjectId)
		{
			SSetAuxAudioProxyOffset(offset, m_pEntity->GetWorldTM())(audioObjectPair);
		}
	}
	else
	{
		std::for_each(m_mapAuxObjects.begin(), m_mapAuxObjects.end(), SSetAuxAudioProxyOffset(offset, m_pEntity->GetWorldTM()));
	}
}

//////////////////////////////////////////////////////////////////////////
Matrix34 const& CEntityComponentAudio::GetAudioAuxObjectOffset(CryAudio::AuxObjectId const audioAuxObjectId /*= DEFAULT_AUDIO_PROXY_ID*/)
{
	AuxObjects::const_iterator const iter(m_mapAuxObjects.find(audioAuxObjectId));

	if (iter != m_mapAuxObjects.end())
	{
		return iter->second.offset;
	}

	static const Matrix34 identityMatrix(IDENTITY);
	return identityMatrix;
}

//////////////////////////////////////////////////////////////////////////
float CEntityComponentAudio::GetGreatestFadeDistance() const
{
	return std::max<float>(m_fadeDistance, m_environmentFadeDistance);
}

//////////////////////////////////////////////////////////////////////////
CryAudio::AuxObjectId CEntityComponentAudio::CreateAudioAuxObject()
{
	char const* szName = nullptr;

#if defined(INCLUDE_ENTITYSYSTEM_PRODUCTION_CODE)
	if (m_auxObjectIdCounter == std::numeric_limits<CryAudio::AuxObjectId>::max())
	{
		CryFatalError("<Audio> Exceeded numerical limits during CEntityAudioProxy::CreateAudioProxy!");
	}
	else if (m_pEntity == nullptr)
	{
		CryFatalError("<Audio> nullptr entity pointer during CEntityAudioProxy::CreateAudioProxy!");
	}

	CryFixedStringT<CryAudio::MaxObjectNameLength> name(m_pEntity->GetName());
	size_t const numAuxObjects = m_mapAuxObjects.size();

	if (numAuxObjects > 0)
	{
		// First AuxAudioObject is not explicitly identified, it keeps the entity's name.
		// All additional objects however are being explicitly identified.
		name.Format("%s_aux_object_#%" PRISIZE_T, m_pEntity->GetName(), numAuxObjects + 1);
	}

	szName = name.c_str();
#endif // INCLUDE_ENTITYSYSTEM_PRODUCTION_CODE

	CryAudio::SCreateObjectData const objectData(szName, CryAudio::EOcclusionType::Ignore, m_pEntity->GetWorldTM(), true);
	CryAudio::IObject* const pIObject = gEnv->pAudioSystem->CreateObject(objectData);
	m_mapAuxObjects.insert(AuxObjectPair(++m_auxObjectIdCounter, SAuxObjectWrapper(pIObject)));
	return m_auxObjectIdCounter;
}

//////////////////////////////////////////////////////////////////////////
bool CEntityComponentAudio::RemoveAudioAuxObject(CryAudio::AuxObjectId const audioAuxObjectId)
{
	bool bSuccess = false;

	if (audioAuxObjectId != CryAudio::DefaultAuxObjectId)
	{
		AuxObjects::iterator iter(m_mapAuxObjects.find(audioAuxObjectId));

		if (iter != m_mapAuxObjects.end())
		{
			gEnv->pAudioSystem->ReleaseObject(iter->second.pIObject);
			m_mapAuxObjects.erase(iter);
			bSuccess = true;
		}
		else
		{
			gEnv->pSystem->Warning(VALIDATOR_MODULE_ENTITYSYSTEM, VALIDATOR_WARNING, VALIDATOR_FLAG_AUDIO, nullptr, "<Audio> AuxAudioProxy with ID '%u' not found during CEntityAudioProxy::RemoveAuxAudioProxy (%s)!", audioAuxObjectId, m_pEntity->GetEntityTextDescription().c_str());
			CRY_ASSERT(false);
		}
	}
	else
	{
		gEnv->pSystem->Warning(VALIDATOR_MODULE_ENTITYSYSTEM, VALIDATOR_ERROR, VALIDATOR_FLAG_AUDIO, nullptr, "<Audio> Trying to remove the default AudioProxy during CEntityAudioProxy::RemoveAuxAudioProxy (%s)!", m_pEntity->GetEntityTextDescription().c_str());
		CRY_ASSERT(false);
	}

	return bSuccess;
}

//////////////////////////////////////////////////////////////////////////
CEntityComponentAudio::AuxObjectPair& CEntityComponentAudio::GetAudioAuxObjectPair(CryAudio::AuxObjectId const audioAuxObjectId)
{
	AuxObjects::iterator const iter(m_mapAuxObjects.find(audioAuxObjectId));

	if (iter != m_mapAuxObjects.end())
	{
		return *iter;
	}

	return s_nullAuxObjectPair;
}

//////////////////////////////////////////////////////////////////////////
void CEntityComponentAudio::SetEnvironmentAmountInternal(CEntity const* const pIEntity, float const amount) const
{
	// If the passed-in entity is our parent we skip it.
	// Meaning we do not apply our own environment to ourselves.
	if (pIEntity != nullptr && m_pEntity != nullptr && pIEntity != m_pEntity)
	{
		auto pIEntityAudioComponent = pIEntity->GetComponent<IEntityAudioComponent>();

		if ((pIEntityAudioComponent != nullptr) && (m_environmentId != CryAudio::InvalidEnvironmentId))
		{
			// Only set the audio-environment-amount on the entities that already have an AudioProxy.
			// Passing INVALID_AUDIO_PROXY_ID to address all auxiliary AudioProxies on pEntityAudioProxy.
			CRY_ASSERT(amount >= 0.0f && amount <= 1.0f);
			pIEntityAudioComponent->SetEnvironmentAmount(m_environmentId, amount, CryAudio::InvalidAuxObjectId);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void CEntityComponentAudio::ToggleAbsoluteVelocityTracking(bool const enable, CryAudio::AuxObjectId const audioAuxObjectId /* = CryAudio::DefaultAuxObjectId */, CryAudio::SRequestUserData const& userData /* = CryAudio::SRequestUserData::GetEmptyObject() */)
{
	if (audioAuxObjectId != CryAudio::InvalidAuxObjectId)
	{
		AuxObjectPair const& audioObjectPair = GetAudioAuxObjectPair(audioAuxObjectId);

		if (audioObjectPair.first != CryAudio::InvalidAuxObjectId)
		{
			(SToggleAbsoluteVelocityTracking(enable))(audioObjectPair);
		}
	}
	else
	{
		std::for_each(m_mapAuxObjects.begin(), m_mapAuxObjects.end(), SToggleAbsoluteVelocityTracking(enable));
	}
}

//////////////////////////////////////////////////////////////////////////
void CEntityComponentAudio::ToggleRelativeVelocityTracking(bool const enable, CryAudio::AuxObjectId const audioAuxObjectId /* = CryAudio::DefaultAuxObjectId */, CryAudio::SRequestUserData const& userData /* = CryAudio::SRequestUserData::GetEmptyObject() */)
{
	if (audioAuxObjectId != CryAudio::InvalidAuxObjectId)
	{
		AuxObjectPair const& audioObjectPair = GetAudioAuxObjectPair(audioAuxObjectId);

		if (audioObjectPair.first != CryAudio::InvalidAuxObjectId)
		{
			(SToggleRelativeVelocityTracking(enable))(audioObjectPair);
		}
	}
	else
	{
		std::for_each(m_mapAuxObjects.begin(), m_mapAuxObjects.end(), SToggleRelativeVelocityTracking(enable));
	}
}

//////////////////////////////////////////////////////////////////////////
void CEntityComponentAudio::AddListener(CryAudio::ListenerId const listenerId)
{
	for (auto const& objectPair : m_mapAuxObjects)
	{
		objectPair.second.pIObject->AddListener(listenerId);
	}
}

//////////////////////////////////////////////////////////////////////////
void CEntityComponentAudio::RemoveListener(CryAudio::ListenerId const listenerId)
{
	for (auto const& objectPair : m_mapAuxObjects)
	{
		objectPair.second.pIObject->RemoveListener(listenerId);
	}
}
