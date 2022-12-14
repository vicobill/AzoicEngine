// Copyright 2006-2021 Crytek GmbH / Crytek Group. All rights reserved.

#ifndef __CONTEXTESTABLISHER_H__
#define __CONTEXTESTABLISHER_H__

#pragma once

#include <CryNetwork/INetwork.h>
#include "Config.h"
#include "Protocol/NetChannel.h"

class CContextEstablisher : public IContextEstablisher, public _reference_target_t
{
public:
	CContextEstablisher();
	~CContextEstablisher();

	void OnFailDisconnect(CNetChannel* pChannel) { m_disconnectOnFail = pChannel; }

	void AddTask(EContextViewState state, IContextEstablishTask* pTask);
	void Start();
	bool StepTo(SContextEstablishState& state);
	void Fail(EDisconnectionCause cause, const string& reason);
	bool IsDone() { return m_done || m_offset < 0 || m_offset == m_tasks.size(); }
	void Done();

	void GetMemoryStatistics(ICrySizer* pSizer)
	{
		SIZER_COMPONENT_NAME(pSizer, "CContextEstablisher");

		pSizer->Add(*this);
		pSizer->AddContainer(m_tasks);
	}

#if LOG_CONTEXT_ESTABLISHMENT
	void OutputTiming(void);
#endif
#if ENABLE_DEBUG_KIT
	void DebugDraw();
#endif

	const string& GetFailureReason() { return m_failureReason; }

private:
	EContextEstablishTaskResult PerformSteps(int& offset, SContextEstablishState& state);

	struct STask
	{
		STask() : state(eCVS_Initial), pTask(0)
#if LOG_CONTEXT_ESTABLISHMENT
			, numRuns(0)
			, done(0.0f)
#endif
		{
		}
		STask(EContextViewState s, IContextEstablishTask* p) : state(s), pTask(p)
#if LOG_CONTEXT_ESTABLISHMENT
			, numRuns(0)
			, done(0.0f)
#endif
		{
		}

		EContextViewState      state;
		IContextEstablishTask* pTask;

#if LOG_CONTEXT_ESTABLISHMENT
		int        numRuns;
		CTimeValue done;
#endif

		bool operator<(const STask& rhs) const
		{
			return state < rhs.state;
		}
	};
	// -1 if not started, otherwise how far down are we
	int                     m_offset;
	std::vector<STask>      m_tasks;
	bool                    m_done;

	string                  m_failureReason;
	int                     m_debuggedFrame;

	_smart_ptr<CNetChannel> m_disconnectOnFail;

	CTimeValue              m_begin;
};

typedef _smart_ptr<CContextEstablisher> CContextEstablisherPtr;

#if LOG_CONTEXT_ESTABLISHMENT
void NetWarnEstablishment(const char* szFormat, ...) PRINTF_PARAMS(1, 2);
void NetLogEstablishment(int level, const char* szFormat, ...) PRINTF_PARAMS(2, 3);
#else
inline void NetWarnEstablishment(const char* szFormat, ...) PRINTF_PARAMS(1, 2);
inline void NetWarnEstablishment(const char* szFormat, ...) {}
inline void NetLogEstablishment(int level, const char* szFormat, ...) PRINTF_PARAMS(2, 3);
inline void NetLogEstablishment(int level, const char* szFormat, ...) {}
#endif


#endif
