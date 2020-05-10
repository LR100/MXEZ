#pragma once

#include "MXEZ/Memory/MXEZDObject.h"

// STD
#include <iostream> // TMP

namespace MXEZ
{
	template <class T>
	class MXEZDLinkedList
	{
	public:
		MXEZDLinkedList()
		{
			// std::cout << "MXEZDList() Created" << std::endl; // TMP
			_front = NULL;
			_back = NULL;
			_size = 0;
		};
		virtual ~MXEZDLinkedList() {};

		class Node : public MXEZDObject<Node>
		{
		public:
			Node(MXEZDLinkedList* list) : _list(list)
			{
				// std::cout << "MXEZDList::Node() Created" << std::endl; // TMP
			};

			T		data;
			Node*	next;
			Node*	prev;

			// Remove From List And Release
			void	Remove()
			{
				//std::cout << "Remove Node" << std::endl;
				if (prev)
					prev->next = next;
				else // Is Front
					_list->_front = next;
				if (next)
					next->prev = prev;
				else // Is Back
					_list->_back = prev;
				_list->_size -= 1;
				// next = NULL; // Seems not useful ?? to be verified
				// prev = NULL;
				// And Release
				delete(this);
			}
		private:
			MXEZDLinkedList* _list;
		};


		inline void		Clear()
		{
			MXEZDLinkedList<T>::Node* cursor = _front;

			while (cursor != NULL)
			{
				cursor->Release();
				cursor = cursor->next;
			}
			_front = NULL;
			_size = 0;
		};


		Node* GetFront() const { return (_front); };
		Node* GetBack() const { return (_back); };
		const size_t& GetSize() const { return (_size); };

		bool  IsEmpty() const { return (_front == NULL); };


		void PopBack()
		{
			if (_back) {
				_back->Remove();
			}
		}

		void PopFront()
		{
			if (_front) {
				_front->Remove();
			}
		}

		Node* PushBack(T data)
		{
			//std::cout << "Push Back Node" << std::endl;
			if (!_front)
			{
				_front = new Node(this);
				_front->prev = NULL;
				_front->next = NULL;
				_front->data = data;
				_back = _front;
			}
			else
			{
				_back->next = new Node(this);
				_back->next->prev = _back;
				_back->next->next = NULL;
				_back = _back->next;
				_back->data = data;
			}
			_size += 1;
			return (_back);
		};

		Node* PushFront(T data)
		{
			//std::cout << "Push Front Node" << std::endl;
			if (!_front)
			{
				_front = new Node(this);
				_front->prev = NULL;
				_front->next = NULL;
				_front->data = data;
				_back = _front;
			}
			else
			{
				_front->prev = new Node(this);
				_front->prev->next = _front;
				_front->prev->prev = NULL;
				_front = _front->prev;
				_front->data = data;
			}
			_size += 1;
			return (_front);
		};

	private:

		size_t		_size;

		Node*		_cursor;
		Node*		_front; // Front Is FIRST
		Node*		_back; // Back Is LAST
	};

}