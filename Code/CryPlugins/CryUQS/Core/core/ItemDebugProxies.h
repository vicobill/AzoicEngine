// Copyright 2016-2021 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include <CryRenderer/IRenderAuxGeom.h>

// *INDENT-OFF* - <hard to read code and declarations due to inconsistent indentation>

namespace UQS
{
	namespace Core
	{

		//===================================================================================
		//
		// CItemDebugProxyBase
		//
		//===================================================================================

		class CItemDebugProxyBase
		{
		public:
			virtual               ~CItemDebugProxyBase() {}

			// this is the position where we will add more details about the item in the 3D render world
			virtual Vec3          GetPivot() const = 0;

			// - used for finding the "closest" item to the camera for which we will draw more debugging details
			// - returns true if the underlying item is within given camera view (and should be considered for getting focused), false if it is beyond a reasonable distance and should there not be considered
			virtual bool          GetDistanceToCameraView(const SDebugCameraView& cameraView, float& dist) const = 0;

			virtual void          Serialize(Serialization::IArchive& ar) = 0;
		};

		//===================================================================================
		//
		// CItemDebugProxy_Sphere
		//
		//===================================================================================

		class CItemDebugProxy_Sphere : public CItemDebugProxyBase, public IItemDebugProxy_Sphere
		{
		public:
			explicit              CItemDebugProxy_Sphere();

			// IItemDebugProxy_Sphere
			virtual void          SetPosAndRadius(const Vec3& pos, float radius) override;
			// ~IItemDebugProxy_Sphere

			// CItemDebugProxyBase
			virtual Vec3          GetPivot() const override;
			virtual bool          GetDistanceToCameraView(const SDebugCameraView& cameraView, float& dist) const override;
			virtual void          Serialize(Serialization::IArchive& ar) override;
			// ~CItemDebugProxyBase

		private:
			Vec3                  m_pos;
			float                 m_radius;
		};
		
		//===================================================================================
		//
		// CItemDebugProxy_Path
		//
		//===================================================================================

		class CItemDebugProxy_Path : public CItemDebugProxyBase, public IItemDebugProxy_Path
		{
		public:
			explicit              CItemDebugProxy_Path();

			// IItemDebugProxy_Path
			virtual void          AddPoint(const Vec3& point) override;
			virtual void          SetClosed(bool bClosed) override;
			// ~IItemDebugProxy_Path

			// CItemDebugProxyBase
			virtual Vec3          GetPivot() const override;
			virtual bool          GetDistanceToCameraView(const SDebugCameraView& cameraView, float& dist) const override;
			virtual void          Serialize(Serialization::IArchive& ar) override;
			// ~CItemDebugProxyBase

		private:
			std::vector<Vec3>     m_pathPoints;
			bool                  m_bClosed;
		};

		//===================================================================================
		//
		// CItemDebugProxy_AABB
		//
		//===================================================================================

		class CItemDebugProxy_AABB : public CItemDebugProxyBase, public IItemDebugProxy_AABB
		{
		public:
			explicit              CItemDebugProxy_AABB();

			// IItemDebugProxy_AABB
			virtual void          SetAABB(const AABB& aabb) override;
			// ~IItemDebugProxy_AABB

			// CItemDebugProxyBase
			virtual Vec3          GetPivot() const override;
			virtual bool          GetDistanceToCameraView(const SDebugCameraView& cameraView, float& dist) const override;
			virtual void          Serialize(Serialization::IArchive& ar) override;
			// ~CItemDebugProxyBase

		private:
			AABB                  m_aabb;
		};
	}
}
