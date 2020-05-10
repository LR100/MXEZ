#include "MXEZ/Memory/MXEZDMemoryManager.h"

// STD
#include <iostream> // TMP

namespace MXEZ
{

#define MXEZ_DMEMORYMANAGER_BLOCK_MAX_SIZE 1024000

	MXEZDMemoryManager::MXEZDMemoryManager(size_t objectSize) : _objectSize(objectSize)
	{
		std::cout << "MXEZDMemoryManager Created" << std::endl; // TMP

		_blocks.clear();
		// After many test thoses values seems to be a good average of uses
		Reset(16, 16);
	}

	void* MXEZDMemoryManager::Get()
	{
		// std::cout << "MXEZDMemoryManager Get" << std::endl; // TMP
		void* ptr = NULL;

		// Until memory is found or created
		while (1)
		{
			// Use last block first
			for (_blockCursor = _blocks.size(); _blockCursor-- != 0;)
			{
				ptr = _blocks.at(_blockCursor)->Get();
				// Memory is found
				if (ptr) {
					return (ptr);
				}
			}
			// No more memory found in all existing blocks, create a new one
			if (ExpandBlocks())
			{
				// Get Memory in the last block (the one created)
				ptr = _blocks.back()->Get();
				// Memory is found
				if (ptr) {
					return (ptr);
				}
			}
		}
	}

	void MXEZDMemoryManager::Release(void* object)
	{
		// Get block Index of object
		MXEZ_DMEMORYMANAGER_BLOCK_INDEX_TYPE	index;
		MXEZ_DMEMORYMANAGER_BLOCK_INDEX_TYPE*	indexPos = (MXEZ_DMEMORYMANAGER_BLOCK_INDEX_TYPE*)object;
		// Index is located just before the object itself
		--indexPos;
		index = *indexPos;

		// std::cout << "Release Object of addr (" << object << ") With index addr (" << (void*)indexPos << ") At Block index (" << index << ")" << std::endl;

		// Release Object in this block
		_blocks.at(index)->Release(object);

		// Then Check if block is not first && the last && empty and remove it 
		while (_blocks.at(index)->IsEmpty() && index != 0 && index == (_blocks.size() - 1)) {
			// std::cout << "DELETE BLOCK (" << index << ") !!!" << std::endl;
			delete (_blocks.at(index));
			_blocks.pop_back();
			index--;
		}

	}

	void MXEZDMemoryManager::Reset(size_t blockObjectCount, size_t blockObjectCountFactor)
	{
		_blockObjectCountFactor = blockObjectCountFactor;
		if (_blocks.size())
		{
			// Delete all existing blocks;
			for (int i = 0; i < _blocks.size(); i += 1) {
				delete (_blocks.at(i));
			}
			_blocks.clear();
		}
		_blockObjectCount = blockObjectCount;

		_blockCursor = 0;
		// Set First Block Object Count
		_blockObjectCount = 256;
		// Create First Block
		_blocks.push_back(new MXEZDMemoryManager::Block(_objectSize, _blockObjectCount, _blocks.size()));
	}

	bool MXEZDMemoryManager::ExpandBlocks()
	{
		if (_blocks.back()->GetBlockSize() < MXEZ_DMEMORYMANAGER_BLOCK_MAX_SIZE)
		{
			_blockObjectCount = (_blocks.back()->GetObjectCount() * _blockObjectCountFactor);
		}
		else
		{
			// std::cout << "MAX BLOCK SIZE REACHED" << std::endl;
			_blockObjectCount = _blocks.back()->GetObjectCount();
		}

		// Create A new Block
		_blocks.push_back(new MXEZDMemoryManager::Block(_objectSize, _blockObjectCount, _blocks.size()));
		return (true);
	}

	MXEZDMemoryManager::Block::Block(size_t objectSize, size_t objectCount, MXEZ_DMEMORYMANAGER_BLOCK_INDEX_TYPE index) : _index(index), _objectCount(objectCount), _objectSize(objectSize)
	{
		//								   // Size of Index Before memory of the object
		size_t memPartSize = (objectSize + sizeof(MXEZ_DMEMORYMANAGER_BLOCK_INDEX_TYPE));

		_blockSize = (memPartSize * objectCount);
		_memory = malloc(_blockSize);
		if (_memory == NULL) {
			throw "MXEZDMemoryManager::Block Cannot allocate more memory";
		}
		memset(_memory, 0, _blockSize);
		// std::cout << "MXEZDMemoryManager::Block() Start at (" << _memory << ") End stop at (" << (void*)&((char*)_memory)[_blockSize] << ")" << std::endl;
		// std::cout << "MXEZDMemoryManager::Block() Constructor memPartSize (" << memPartSize << ")" << std::endl; // TMP

		char* ptrMemoryChar = (char*)_memory;
		MXEZ_DMEMORYMANAGER_BLOCK_INDEX_TYPE* ptr;

		// Init Memory with object preceed by the block index (0, 1, 2, ...)
		for (size_t i = 0; i < objectCount; i += 1)
		{
			ptr = (MXEZ_DMEMORYMANAGER_BLOCK_INDEX_TYPE*)(&ptrMemoryChar[memPartSize * i]);
			*ptr = _index;
			// std::cout << "Add Object of addr (" << (void*)&ptr[1] << ") With index addr (" << (void*)ptr << ") End Object" << std::endl;
			_releasedObjects.push_back((void*)&ptr[1]);
		}
	}

	MXEZDMemoryManager::Block::~Block()
	{
		if (_memory) {
			free(_memory);
		}
	}

	void* MXEZDMemoryManager::Block::Get()
	{
		if (_releasedObjects.size())
		{
			void* obj;
			obj = _releasedObjects.back();
			_releasedObjects.pop_back();
			return (obj);
		}
		return (NULL);
	}

	void MXEZDMemoryManager::Block::Release(void* object)
	{
		_releasedObjects.push_back(object);
	}

	const size_t& MXEZDMemoryManager::Block::GetObjectCount() const
	{
		return (_objectCount);
	}

	const size_t& MXEZDMemoryManager::Block::GetBlockSize() const
	{
		return (_blockSize);
	}

	bool MXEZDMemoryManager::Block::IsEmpty() const
	{
		return (_objectCount == _releasedObjects.size());
	}

}