// Copyright 2015-2021 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include <xg.h>
#include <numeric>

#if BUFFER_ENABLE_DIRECT_ACCESS
extern const GUID BufferPointerGuid;
#endif

class SGPUMemHdl
{
public:
	SGPUMemHdl()
		: m_handleAndFlags(0)
	{
	}

	explicit SGPUMemHdl(IDefragAllocator::Hdl hdl)
		: m_handleAndFlags((UINT_PTR)hdl << FlagsShift)
	{
	}

	explicit SGPUMemHdl(void* pFixed)
		: m_handleAndFlags((UINT_PTR)pFixed | IsFixedFlag)
	{
		CRY_ASSERT(pFixed != nullptr);
	}

	ILINE bool IsValid() const
	{
		return m_handleAndFlags > IsFixedFlag;
	}

	ILINE int IsFixed() const
	{
		return m_handleAndFlags & IsFixedFlag;
	}

	void* GetFixedAddress() const
	{
		CRY_ASSERT(IsFixed());
		return (void*)(m_handleAndFlags & ~FlagsMask);
	}

	IDefragAllocator::Hdl GetHandle() const
	{
		CRY_ASSERT(!IsFixed());
		return (IDefragAllocator::Hdl)(m_handleAndFlags >> FlagsShift);
	}

	friend bool operator == (const SGPUMemHdl& a, const SGPUMemHdl& b)
	{
		return a.m_handleAndFlags == b.m_handleAndFlags;
	}

	friend bool operator < (const SGPUMemHdl& a, const SGPUMemHdl& b)
	{
		return a.m_handleAndFlags < b.m_handleAndFlags;
	}

private:
	enum
	{
		IsFixedFlag = 0x1,
		FlagsMask = 0x1,
		FlagsShift = 1,
	};

private:
	UINT_PTR m_handleAndFlags;
};

class CDurangoGPUMemoryManager : public IDefragAllocatorPolicy
{
public:
	struct AllocateResult
	{
		AllocateResult()
			: baseAddress()
		{
		}

		SGPUMemHdl hdl;
		void* baseAddress;
	};

	enum
	{
		AllocAlign = 32 * 1024,
	};

public:
	CDurangoGPUMemoryManager();
	~CDurangoGPUMemoryManager();

	IDefragAllocatorStats GetStats();

	bool Init(size_t size, size_t bankSize, size_t reserveSize, uint32 xgMemType, bool allowAdditionalBanks);
	void DeInit();

	size_t GetPoolSize() const;
	//! Not including overflow allocations
	size_t GetPoolAllocated() const;
	//! Only overflow allocations
	size_t GetPoolOverflowAllocated() const;
	//! The number of overflow allocations
	size_t GetPoolOverflowAllocationCount() const;
	//! Including overflow allocations
	size_t GetTotalAllocated() const;
	//! Pool size minus all allocations including overflow
	size_t GetTotalRemainingPoolSize() const;

	void RT_Tick();

	AllocateResult AllocatePinned(size_t amount, size_t align);
	void Free(SGPUMemHdl hdl, UINT64 fence = ~0ull);
	void FreeUnused(SGPUMemHdl hdl);

	void BindContext(SGPUMemHdl hdl, CDeviceTexture* pDevTex);
	void* WeakPin(SGPUMemHdl hdl);
	void* Pin(SGPUMemHdl hdl);
	void Unpin(SGPUMemHdl hdl);

	void GpuPin(SGPUMemHdl hdl);
	void GpuUnpin(SGPUMemHdl hdl, ID3DXboxPerformanceContext* pCtx);
	void GpuUnpin(SGPUMemHdl hdl, ID3D11DmaEngineContextX* pCtx);

public: // IDefragAllocatorPolicy Members
	virtual uint32 BeginCopy(void* pContext, UINT_PTR dstOffset, UINT_PTR srcOffset, UINT_PTR size, IDefragAllocatorCopyNotification* pCompleteNotification);
	virtual void Relocate(uint32 userMoveId, void* pContext, UINT_PTR newOffset, UINT_PTR oldOffset, UINT_PTR size);
	virtual void CancelCopy(uint32 userMoveId, void* pContext, bool bSync);
	virtual void SyncCopy(void* pContext, UINT_PTR dstOffset, UINT_PTR srcOffset, UINT_PTR size);

	virtual uint32 Hash(UINT_PTR offset, UINT_PTR size);

private:
	enum
	{
		BankAlign = 64 * 1024,
		MaxCopies = 64,

		// Minimum alignment that can be requested of D3DAllocateGraphicsMemory
		MinD3DAlignment = 4096,
	};

	struct PendingFree
	{
		UINT64     fence;
		SGPUMemHdl hdl;

		friend bool operator < (const PendingFree& a, const PendingFree& b)
		{
			return a.hdl < b.hdl;
		}
	};

	struct Bank
	{
		Bank()
			: pBase(NULL)
			, pBuffer(NULL)
		{
		}

		void*  pBase;
		ID3D11Buffer* pBuffer;
	};

	struct CopyDesc
	{
		UINT_PTR src;
		UINT_PTR dst;
		UINT_PTR size;
		uint32 idx;
		uint64* fence;
		bool copied;

		CopyDesc()
			: src()
			, dst()
			, size()
			, idx(~0u)
			, fence()
			, copied()
		{}
	};

	struct InflightCopy
	{
		InflightCopy()
		{
			memset(this, 0, sizeof(*this));
		}

		InflightCopy(UINT_PTR srcOffset, UINT_PTR dstOffset, UINT_PTR size, IDefragAllocatorCopyNotification* pNotification)
			: srcOffset(srcOffset)
			, dstOffset(dstOffset)
			, size(size)
			, copying(false)
			, cooling(false)
			, cancelled(false)
			, copyFence(0)
			, inUseFence(0)
			, pNotification(pNotification)
		{
		}

		UINT_PTR srcOffset;
		UINT_PTR dstOffset;
		UINT_PTR size;
		bool copying;
		bool cooling;
		bool cancelled;
		uint64 copyFence;
		uint64 inUseFence;
		IDefragAllocatorCopyNotification* pNotification;
	};

	typedef std::vector<PendingFree> PendingFreeVec;
	typedef std::vector<CopyDesc, stl::STLGlobalAllocator<CopyDesc> > QueuedCopyVec;

private:
	CDurangoGPUMemoryManager(const CDurangoGPUMemoryManager&);
	CDurangoGPUMemoryManager& operator=(const CDurangoGPUMemoryManager&);

private:
	void TickFrees_Locked();
	void TickUnpins_Locked();
	void RemovePendingUnpin_Locked(IDefragAllocator::Hdl hdl);
	void PushPendingUnpin(SGPUMemHdl hdl, UINT64 fence);

	void CollectGarbage(size_t maxMoves, size_t maxAmount);
	void CompleteMoves();
	void PerformCopies();
	void QueueCopy(const CopyDesc& copy);	
	void ScheduleCopies(CopyDesc* descriptions, size_t ncopies);
	void Relocate_Int(CDeviceTexture* pDevTex, char* pOldTexBase, char* pTexBase, UINT_PTR size);
	char* GetPhysicalAddress(UINT_PTR offs);

private:
	CryCriticalSectionNonRecursive m_lock;

	IDefragAllocator* m_pAllocator;
	void* m_pCPUAddr;
	UINT_PTR m_bankShift;
	UINT m_memType;
	size_t m_maximumAllocationSize;
	bool m_allowAdditionalBanks;
	size_t m_overflowAllocationSize;
	std::unordered_map<void*, size_t> m_overflowAllocationMap;

	PendingFreeVec m_pendingFrees;
	std::vector<Bank> m_banks;

	PendingFreeVec m_pendingUnpins;

	uint64 m_tickFence;
	QueuedCopyVec m_queuedCopies;
	InflightCopy m_copies[MaxCopies];
	uint32 m_nLastCopyIdx;
};

#if DURANGO_USE_ESRAM
// An allocation in ESRAM
struct SESRAMAllocation
{
private:
	static const uint32 INVALID_PTR = ~0;
	static const uint16 INVALID_BLOCK = ~0;

public:
	SESRAMAllocation() : m_esramPtr(INVALID_PTR), m_beginBlock(INVALID_BLOCK), m_endBlock(INVALID_BLOCK) {}

	BOOL        IsValid() const { return m_beginBlock != INVALID_BLOCK; }
	void        Invalidate() { m_esramPtr = INVALID_PTR; m_beginBlock = m_endBlock = INVALID_BLOCK; }

	uint32      m_esramPtr;     // The start location of this allocation in ESRAM
	uint32      m_esramSize;    // The usable size of this allocation in bytes for resource creation. 
								// The actual allocation size is equal or larger than this size.
	uint16      m_beginBlock;   // The first block of this allocation
	uint16      m_endBlock;     // One past the end, like STL
};

// An area of free space in ESRAM
class SESRAMFreeSpace
{
public:
	SESRAMFreeSpace(uint16 beginBlock, uint16 endBlock) : m_beginBlock(beginBlock), m_endBlock(endBlock) {}

	uint16 m_beginBlock;
	uint16 m_endBlock;   // One past the end, like STL
};

class CDurangoESRAMManager
{
private:
	static const uint32 ESRAM_SIZE = 32 * 1024 * 1024;
	static const uint32 BLOCK_SIZE = 64 * 1024;
	static const uint32 BLOCK_COUNT = ESRAM_SIZE / BLOCK_SIZE;
	std::vector<SESRAMFreeSpace> m_freeSpaces;
public:
	void Allocate(uint32 numBytes, uint32 alignment, SESRAMAllocation& alloc);
	void Free(SESRAMAllocation& alloc);

	CDurangoESRAMManager()
	{
		m_freeSpaces.emplace_back(0, BLOCK_COUNT);
	}
};
#endif

interface ID3D11DmaEngineContextX;

class CDurangoGPURingMemAllocator
{
public:
	typedef uint16 TAllocateContext;

public:
	CDurangoGPURingMemAllocator();
	~CDurangoGPURingMemAllocator();

	bool  Init(ID3D11DmaEngineContextX* pContext, uint32 size);

	void* BeginAllocate(uint32 size, uint32 align, TAllocateContext& contextOut);
	void  EndAllocate(TAllocateContext context, uint64 freeFence);

private:
	typedef uint16 TBlockId;

	struct Block
	{
		uint64   fence;
		uint32   offset;
		TBlockId next;
	};

	enum
	{
		InvalidBlockId = 0xffff,
		MaxBlocks = 32768,
		BaseAlignment = 64 * 1024,
	};

private:
	TBlockId AllocateBlockId()
	{
		if (m_freeBlocks.empty())
		{
			return InvalidBlockId;
		}
		TBlockId ret = m_freeBlocks.back();
		m_freeBlocks.pop_back();
		return ret;
	}

	void ReleaseBlockId(TBlockId id)
	{
		m_freeBlocks.push_back(id);
	}

private:
	ID3D11DmaEngineContextX* m_pContext;
	char*                    m_pCPUAddr;
	uint32                   m_capacity;

	// First block in a series of allocates
	TBlockId              m_allocateHead;
	size_t                m_allocateDepth;

	std::vector<Block>    m_blocks;
	std::vector<TBlockId> m_freeBlocks;
	TBlockId              m_freeHead;
	TBlockId              m_startSentinalId;
	TBlockId              m_endSentinalId;
};

union SMinimisedBufferDesc
{
	uint64 key;

	struct
	{
		uint32 size;
	};

	friend bool operator<(SMinimisedBufferDesc a, SMinimisedBufferDesc b)
	{
		return a.key < b.key;
	}
};

union SMinimisedTexture2DDesc
{
	uint64 key;

	struct
	{
		uint16 width;
		uint16 height;

		uint16 mips : 4;
		uint16 arraySize : 12;
		uint8  format;
		uint8  isCube : 1;
		uint8  tileMode : 7;
	};

	friend bool operator<(SMinimisedTexture2DDesc a, SMinimisedTexture2DDesc b)
	{
		return a.key < b.key;
	}
};

struct SDeviceBufferDesc
{
	SMinimisedBufferDesc    desc;
	XG_RESOURCE_LAYOUT      layout;
};

struct SDeviceTextureDesc
{
	XG_RESOURCE_LAYOUT layout;
	XG_TILE_MODE xgTileMode;
	D3D11_TEXTURE2D_DESC d3dDesc;
};
