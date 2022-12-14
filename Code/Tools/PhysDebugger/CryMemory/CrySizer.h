#pragma once

template<class T,bool alloc> struct SizerAllocator {};
template<class T> struct SizerAllocator<T,false> {
	static T *alloc(int count,T* src=nullptr) { return src; }
};
template<class T> struct SizerAllocator<T,true> {
	static T *alloc(int count,T* src=nullptr) { 
		return new typename std::remove_const<T>::type[count];
	}
};

class ICrySizer {
public:
	template<class T> bool AddObject(T* const& ptr, size_t size, int nCount=1) { 
		if (GetMode())
			const_cast<T*&>(ptr) = SizerAllocator<T,!std::is_polymorphic<T>::value>::alloc(size/sizeof(T),ptr);
		return AddObjectRaw((void*)ptr, size, std::is_polymorphic<T>::value);
	}
	bool AddObject(struct geom* const& ptr, size_t size, int nCount=1) { return AddPartsAlloc((struct geom*&)ptr,size); }
	bool AddObject(struct entity_contact* const& ptr, size_t size, int nCount=1) { return AddContactsAlloc((struct entity_contact*&)ptr,size); }
	bool AddObject(int* const& ptr, size_t size, int nCount=1) { return AddIntArrayAlloc((int*&)ptr, size); }

	virtual bool AddPartsAlloc(struct geom* &ptr, size_t size) { return AddObjectRaw(ptr,size); }
	virtual bool AddContactsAlloc(struct entity_contact* &ptr, size_t size) { return AddObjectRaw(ptr,size); }
	virtual bool AddIntArrayAlloc(int* &ptr, size_t size) { if (GetMode()) ptr=new int[size/sizeof(int)]; return AddObjectRaw(ptr,size); }
	virtual bool AddObjectRaw(void* ptr, size_t size, bool hasVMT=false, bool mapPtrs=true) = 0;
	virtual int  GetMode() const = 0;
};

namespace EMemStatContextType { enum Type { Other, Physics }; }
#define SIZER_COMPONENT_NAME
#define MEMSTAT_USAGE
#define MEMSTAT_CONTEXT

