// Copyright 2010-2021 Crytek GmbH / Crytek Group. All rights reserved.

#include "StdAfx.h"
#include "AIFlyingVehicle.h"

CAIFlyingVehicle::CAIFlyingVehicle()
	: m_combatModeEnabled(false)
	, m_firingAllowed(true)
{
	_fastcast_CAIFlyingVehicle = true;
}

CAIFlyingVehicle::~CAIFlyingVehicle()
{
	SetObservable(false);
	SetObserver(false);
}

void CAIFlyingVehicle::SetObserver(bool observer)
{
	if (m_observer != observer)
	{
		if (observer)
		{
			uint8 faction = GetFactionID();
			ObserverParams observerParams;
			observerParams.entityId = GetEntityID();
			observerParams.factionsToObserveMask = GetFactionVisionMask(faction);
			observerParams.faction = faction;
			observerParams.typesToObserveMask = GetObserverTypeMask();
			observerParams.typeMask = GetObservableTypeMask();
			observerParams.eyePosition = GetPos();
			observerParams.eyeDirection = GetViewDir();
			observerParams.sightRange = m_Parameters.m_PerceptionParams.sightRange;
			float secondaryFov;
			GetSightFOVCos(observerParams.fovCos, secondaryFov);
			observerParams.callback = functor(CAIFlyingVehicle::OnVisionChanged);

			PhysSkipList skipList;
			GetPhysicalSkipEntities(skipList);

			observerParams.skipListSize = std::min<size_t>(skipList.size(), ObserverParams::MaxSkipListSize);
			for (size_t i = 0; i < static_cast<size_t>(observerParams.skipListSize); ++i)
				observerParams.skipList[i] = skipList[i];

			VisionID visionID = GetVisionID();
			if (!visionID)
			{
				visionID = gAIEnv.pVisionMap->CreateVisionID(GetName());

				SetVisionID(visionID);
			}

			gAIEnv.pVisionMap->RegisterObserver(visionID, observerParams);
		}
		else
		{
			if (VisionID visionID = GetVisionID())
				gAIEnv.pVisionMap->UnregisterObserver(visionID);
		}

		m_observer = observer;
	}
}

// Sandy - Dec 2011: Unfortunately this code will cause Lua scripts to be reloaded
// while they are running, causing all sorts of undefined behavior.
// Not sure what this 'hot-fix' was supposed to do though, so I am
// going to leave it here now as a warning!
//
////This is a "hot-fix". Need to dig out the real issue
//void  CAIFlyingVehicle::Reset(EObjectResetType type)
//{
//	if (AIOBJRESET_INIT == type)
//	{
//		if (IEntity *entity = GetEntity())
//		{
//			if (IEntityComponent *proxy = entity->GetProxy(ENTITY_PROXY_SCRIPT))
//			{
//				SEntitySpawnParams params;
//				proxy->Reload(entity, params);
//			}
//		}
//	}
//
//	Base::Reset(type);
//}

void CAIFlyingVehicle::OnVisionChanged(const VisionID& observerID, const ObserverParams& observerParams, const VisionID& observableID, const ObservableParams& observableParams, bool visible)
{
	const ObserverParams* params = gAIEnv.pVisionMap->GetObserverParams(observerID);

	if (params)
	{
		IEntity* ent = gEnv->pEntitySystem->GetEntity(params->entityId);
		IAIObject* obj = ent->GetAI();

		if (obj)
		{
			if (CPuppet* puppet = obj->CastToCPuppet())
			{
				CAIFlyingVehicle* flyingAI = static_cast<CAIFlyingVehicle*>(puppet);

				const EntityId targetID = observableParams.entityId;
				if (targetID != flyingAI->GetEntityID())
				{
					if (visible)
					{
						if (targetID)
						{
							if (IEntity* entity = gEnv->pEntitySystem->GetEntity(observableParams.entityId))
							{
								if (IAIObject* aiObject = entity->GetAI())
								{
									uint16 aiObjectType = aiObject->GetAIType();
									if ((aiObjectType == AIOBJECT_GRENADE) || (aiObjectType == AIOBJECT_RPG))
									{
										const Vec3& pos = entity->GetPos();
										const Vec3& ownPos = flyingAI->GetPos();
										Vec3 diff = ownPos - pos;
										diff.Normalize();

										pe_status_dynamics dynamics;
										if (entity->GetPhysics())
										{
											entity->GetPhysics()->GetStatus(&dynamics);

											if (dynamics.v.dot(diff) > 0.9f)
											{
												GoalParams param;
												param.SetName("params");

												GoalParams paramChild;
												paramChild.SetName("impulse");
												paramChild.SetValue(pos);

												GoalParams paramChild2;
												paramChild2.SetName("dir");
												paramChild2.SetValue(dynamics.v);
												paramChild.AddChild(paramChild2);

												GoalParams paramChild3;
												paramChild3.SetName("id");
												paramChild3.SetValue(observableParams.entityId);
												paramChild.AddChild(paramChild3);

												param.AddChild(paramChild);

												CGoalPipe* pipe = flyingAI->GetCurrentGoalPipe();
												if (pipe)
												{
													pipe->ParseParams(param);
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void CAIFlyingVehicle::Serialize(TSerialize ser)
{
	Base::Serialize(ser);

	ser.Value("m_combatModeEnabled", m_combatModeEnabled);
	ser.Value("m_firingAllowed", m_firingAllowed);
}

namespace
{
void AISendSignal(CAIFlyingVehicle* const pFlyingVehicle, const AISignals::ISignalDescription& signalDescription)
{
	CRY_ASSERT(pFlyingVehicle);

	const AISignals::SignalSharedPtr pSignal = gEnv->pAISystem->GetSignalManager()->CreateSignal(AISIGNAL_DEFAULT, signalDescription, pFlyingVehicle->GetEntityID());

	gEnv->pAISystem->SendSignal(AISignals::ESignalFilter::SIGNALFILTER_SENDER, pSignal);
}
}

void CAIFlyingVehicle::PostSerialize()
{
	Base::PostSerialize();

	if (m_combatModeEnabled)
	{
		AISendSignal(this, gEnv->pAISystem->GetSignalManager()->GetBuiltInSignalDescriptions().GetOnCombatTargetEnabled_DEPRECATED());
	}
	else
	{
		AISendSignal(this, gEnv->pAISystem->GetSignalManager()->GetBuiltInSignalDescriptions().GetOnCombatTargetDisabled_DEPRECATED());
	}

	if (m_firingAllowed)
	{
		AISendSignal(this, gEnv->pAISystem->GetSignalManager()->GetBuiltInSignalDescriptions().GetOnFiringAllowed_DEPRECATED());
	}
	else
	{
		AISendSignal(this, gEnv->pAISystem->GetSignalManager()->GetBuiltInSignalDescriptions().GetOnFiringNotAllowed_DEPRECATED());
	}
}

void CAIFlyingVehicle::SetSignal(const AISignals::SignalSharedPtr& pSignal)
{
	if (pSignal->GetSignalDescription() == gEnv->pAISystem->GetSignalManager()->GetBuiltInSignalDescriptions().GetOnCombatTargetEnabled_DEPRECATED())
	{
		m_combatModeEnabled = true;
	}
	else if (pSignal->GetSignalDescription() == gEnv->pAISystem->GetSignalManager()->GetBuiltInSignalDescriptions().GetOnCombatTargetDisabled_DEPRECATED())
	{
		m_combatModeEnabled = false;
	}
	else if (pSignal->GetSignalDescription() == gEnv->pAISystem->GetSignalManager()->GetBuiltInSignalDescriptions().GetOnFiringAllowed_DEPRECATED())
	{
		m_firingAllowed = true;
	}
	else if (pSignal->GetSignalDescription() == gEnv->pAISystem->GetSignalManager()->GetBuiltInSignalDescriptions().GetOnFiringNotAllowed_DEPRECATED())
	{
		m_firingAllowed = false;
	}

	Base::SetSignal(pSignal);
}
