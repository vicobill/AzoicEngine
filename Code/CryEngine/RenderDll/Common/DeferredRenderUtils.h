// Copyright 2009-2021 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#define SDeferMeshVert SVF_P3F_C4B_T2F

typedef stl::aligned_vector<SVF_P3F_C4B_T2F, CRY_PLATFORM_ALIGNMENT> t_arrDeferredMeshVertBuff;
typedef stl::aligned_vector<uint16         , CRY_PLATFORM_ALIGNMENT> t_arrDeferredMeshIndBuff;

class CDeferredRenderUtils
{
public:
	static void CreateUnitFrustumMesh(int tessx, int tessy, t_arrDeferredMeshIndBuff& indBuff, t_arrDeferredMeshVertBuff& vertBuff);
	static void CreateUnitFrustumMeshTransformed(SRenderLight* pLight, ShadowMapFrustum* pFrustum, int nAxis, int tessx, int tessy, t_arrDeferredMeshIndBuff& indBuff, t_arrDeferredMeshVertBuff& vertBuff);
	static void CreateUnitSphere(int rec, /*SRenderLight* pLight, int depth, */ t_arrDeferredMeshIndBuff& indBuff, t_arrDeferredMeshVertBuff& vertBuff);

	static void CreateSimpleLightFrustumMesh(t_arrDeferredMeshIndBuff& indBuff, t_arrDeferredMeshVertBuff& vertBuff);
	static void CreateSimpleLightFrustumMeshTransformed(ShadowMapFrustum* pFrustum, int nFrustNum, t_arrDeferredMeshIndBuff& indBuff, t_arrDeferredMeshVertBuff& vertBuff);
	static void CreateUnitBox(t_arrDeferredMeshIndBuff& indBuff, t_arrDeferredMeshVertBuff& vertBuff);

	static void CreateQuad(t_arrDeferredMeshIndBuff& indBuff, t_arrDeferredMeshVertBuff& vertBuff);

	CDeferredRenderUtils();
	~CDeferredRenderUtils();
private:
	static void SphereTess(Vec3& v0, Vec3& v1, Vec3& v2, t_arrDeferredMeshIndBuff& indBuff, t_arrDeferredMeshVertBuff& vertBuff);
	static void SphereTessR(Vec3& v0, Vec3& v1, Vec3& v2, int depth, t_arrDeferredMeshIndBuff& indBuff, t_arrDeferredMeshVertBuff& vertBuff);

};
