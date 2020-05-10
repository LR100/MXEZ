#pragma once

#include <deque>
#include <vector>

namespace MXEZ
{
	class MXEZDMemoryManager
	{
	public:
		MXEZDMemoryManager(size_t objectSize);
		virtual ~MXEZDMemoryManager() {};

		void*		Get();
		void		Release(void* object);
		void		Reset(size_t blockObjectCount, size_t blockObjectCountFactor);

	private:

#define MXEZ_DMEMORYMANAGER_BLOCK_INDEX_TYPE uint16_t

		class Block
		{
		public:
			Block(size_t objectSize, size_t objectCount, MXEZ_DMEMORYMANAGER_BLOCK_INDEX_TYPE index);
			~Block();

			void*					Get();
			void					Release(void* object);
			const size_t&			GetObjectCount() const;
			const size_t&			GetBlockSize() const;
			// Return True if the block have no space used
			bool					IsEmpty() const;	

		private:

			size_t					_blockSize;
			const size_t			_objectCount;
			const size_t			_objectSize;
			std::deque<void*>		_releasedObjects;
			void*					_memory;
			const MXEZ_DMEMORYMANAGER_BLOCK_INDEX_TYPE		_index;
		};

		bool	ExpandBlocks();

		// Managed Memory (Block of the same size)

		const size_t			_objectSize;
	
		// !!!!! MAX MXEZ_DMEMORYMANAGER_BLOCK_INDEX_SIZE BYTES MAXVALUE BLOCKS !!!!!! 

		std::vector<MXEZDMemoryManager::Block*>		_blocks;
		
		// Could also keep a list of block indexes with available space
		size_t										_blockCursor;
		size_t										_blockObjectCount;
		size_t										_blockObjectCountFactor;
	};


}