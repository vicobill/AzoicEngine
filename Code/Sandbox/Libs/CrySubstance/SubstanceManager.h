// Copyright 2017-2021 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include "CrySubstanceAPI.h"
#include "ISubstanceManager.h"
#include "SubstanceCommon.h"

#include "CrySerialization/Serializer.h"
#include "CrySerialization/ClassFactory.h"

#include "substance/framework/callbacks.h"
#include <typeindex>

struct ISubstanceInstanceRenderer;

class CSubstancePreset;

namespace SubstanceAir
{
class PackageDesc;
class GraphInstance;
class Renderer;
class OutputInstance;
}

typedef  std::unordered_map<uint32 /*crc*/, SubstanceAir::PackageDesc*> PackageMap;

class CSubstanceManager : public ISubstanceManager
{
public:
	static CSubstanceManager*    Instance();

	void                         CreateInstance(const string& archiveName, const string& instanceName, const string& instanceGraph, const std::vector<SSubstanceOutput>& outputs, const Vec2i& resolution) override;
	bool                         GetArchiveContents(const string& archiveName, std::map<string, std::vector<string>>& contents) override;
	bool                         IsRenderPending(const SubstanceAir::UInt id) const override;
	void                         RegisterInstanceRenderer(ISubstanceInstanceRenderer* renderer) override;
	void                         GenerateOutputs(ISubstancePreset* preset, ISubstanceInstanceRenderer* renderer) override;
	SubstanceAir::UInt           GenerateOutputsAsync(ISubstancePreset* preset, ISubstanceInstanceRenderer* renderer) override;
	SubstanceAir::UInt           GenerateOutputsAsync(const std::vector<ISubstancePreset*>& preset, ISubstanceInstanceRenderer* renderer) override;

	SubstanceAir::GraphInstance* InstantiateGraph(const string& archiveName, const string& graphName);
	void                         RemovePresetInstance(CSubstancePreset* preset);

protected:
	enum GenerateType
	{
		Sync,
		Async
	};

	SubstanceAir::PackageDesc* GetPackage(const string& archiveName);
	SubstanceAir::UInt         GenerateOutputs(const std::vector<ISubstancePreset*>& preset, ISubstanceInstanceRenderer* renderer, GenerateType jobType);

	struct CrySubstanceCallbacks : public SubstanceAir::RenderCallbacks
	{
		virtual void outputComputed(SubstanceAir::UInt runUid, size_t userData, const SubstanceAir::GraphInstance* graphInstance, SubstanceAir::OutputInstance* outputInstance) override;
		void outputComputed(SubstanceAir::UInt runUid, const SubstanceAir::GraphInstance* graphInstance, SubstanceAir::OutputInstance* outputInstance);
	};

	CSubstanceManager();
	SubstanceAir::PackageDesc* LoadPackage(const string& archiveName);
	void                       PushPreset(CSubstancePreset* preset, ISubstanceInstanceRenderer* renderer);

private:
	static CSubstanceManager*                              _instance;
	PackageMap                                             m_loadedPackages;
	std::unordered_map<uint32 /*crc*/, int>                m_refCount;
	SubstanceAir::Renderer*                                m_pRenderer;
	CrySubstanceCallbacks*                                 m_pCallbacks;
	std::map<std::type_index, ISubstanceInstanceRenderer*> m_renderers;
	friend void CRY_SUBSTANCE_API InitCrySubstanceLib(IFileManipulator* fileManipulator);
};
