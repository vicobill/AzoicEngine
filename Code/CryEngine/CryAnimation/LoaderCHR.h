// Copyright 2006-2021 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include "ModelAnimationSet.h"
#include "ModelMesh.h"
#include <CrySystem/IStreamEngine.h>
#include "ChrParamLoader.h"

class CSkin;
class CDefaultSkeleton;

class CryCHRLoader : public IStreamCallback
{

public:
	CryCHRLoader()
	{
		m_pModelSkel = 0;
		m_pModelSkin = 0;
	}
	~CryCHRLoader()
	{
		CRY_ASSERT(m_pStream == nullptr);
		ClearModel();
	}

	bool        BeginLoadCHRRenderMesh(CDefaultSkeleton* pSkel, const DynArray<CCharInstance*>& pCharInstances, EStreamTaskPriority estp);
	bool        BeginLoadSkinRenderMesh(CSkin* pSkin, int nRenderLod, EStreamTaskPriority estp);
	void        AbortLoad();

	static void ClearCachedLodSkeletonResults();

	// the controller manager for the new model; this remains the same during the whole lifecycle the file without extension
	string m_strGeomFileNameNoExt;

public: // IStreamCallback Members
	virtual void StreamAsyncOnComplete(IReadStream* pStream, unsigned nError);
	virtual void StreamOnComplete(IReadStream* pStream, unsigned nError);

private:
	struct SmartContentCGF
	{
		CContentCGF* pCGF;
		SmartContentCGF(CContentCGF* pCGF) : pCGF(pCGF) {}
		~SmartContentCGF() { if (pCGF) g_pI3DEngine->ReleaseChunkfileContent(pCGF); }
		CContentCGF* operator->() { return pCGF; }
		CContentCGF& operator*()  { return *pCGF; }
		operator CContentCGF*() const { return pCGF; }
		operator bool() const { return pCGF != NULL; }
	};

private:

	void                    EndStreamSkel(IReadStream* pStream);
	void                    EndStreamSkinAsync(IReadStream* pStream);
	void                    EndStreamSkinSync(IReadStream* pStream);

	void                    PrepareMesh(CMesh* pMesh);
	void                    PrepareRenderChunks(DynArray<RChunk>& arrRenderChunks, CMesh* pMesh);

	void                    ClearModel();

private:
	CDefaultSkeleton*        m_pModelSkel;
	CSkin*                   m_pModelSkin;
	IReadStreamPtr           m_pStream;
	DynArray<RChunk>         m_arrNewRenderChunks;
	_smart_ptr<IRenderMesh>  m_pNewRenderMesh;
	DynArray<CCharInstance*> m_RefByInstances;
};
