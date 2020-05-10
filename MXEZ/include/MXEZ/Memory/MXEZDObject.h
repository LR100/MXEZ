#pragma once

#include "MXEZ/Memory/MXEZDMemoryManager.h"

#include <iostream> // TMP

namespace MXEZ
{

	template <class T>
	class MXEZDObject
	{
	public:
		MXEZDObject() { };

		inline void* operator new(size_t size)
		{
			return (_memManager->Get());
		}

		inline void operator delete(void* object)
		{
			_memManager->Release(object);
		}

		static MXEZDMemoryManager* GetMemoryManager() { return (_memManager); };

	private:

		// Be Sure That The Memory Manager is able to allow Block of the same size that the object size !!
		static MXEZDMemoryManager* _memManager;
	};

	template<class T>
	MXEZDMemoryManager* MXEZDObject <T>::_memManager = new MXEZDMemoryManager(sizeof(T));

}