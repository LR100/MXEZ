#pragma once

// STD
#include <deque>
#include <vector>
#include <sstream> // for ToString methods

// MXEZ
#include "MXEZ/AABB/AABB.h"
#include "MXEZ/Memory/MXEZDLinkedList.h"

/// //// ///
////TREE////
/// //// ///

namespace MXEZ
{

	// QuadTree AABB to fit and subdivide all AABB
	// to the best precise and smallest location (sorted by size)
	template <class T, class O>
	class QuadTreeAABB2T
	{
	public:
		QuadTreeAABB2T(const AABB2T<T>& _boundary, unsigned short maxDepth)
		{
			_partCount = 1;
			_depthMax = maxDepth;
			_depthSizes = new Vec2[_depthMax];
			_depthSizes[0] = Vec2((float)_boundary.halfSize.x, (float)_boundary.halfSize.y);
			for (unsigned short i = 1; i < _depthMax; i += 1) // Pre-Init Size Per Depth
				_depthSizes[i] = (_depthSizes[i - 1] * 0.5f);

			_root = new QuadTreeAABB2T::Part(_boundary, NULL, this, 0);
		}
		virtual ~QuadTreeAABB2T() {};

		//////////////////////////////////////////
		//// NODE  ///////////////////////////////
		//////////////

		typedef class Part
		{
		public:

			friend class QuadTreeAABB2T;

			Part(Part* parent, QuadTreeAABB2T<T, O>* tree, const unsigned short& depth)
			{
				_parent = parent;
				_tree = tree;
				_depth = depth;
				_childrenIndexesCount = 0;
				_children = new Part * [4];
				_children[0] = NULL; // NW
				_children[1] = NULL; // NE
				_children[2] = NULL; // SW
				_children[3] = NULL; // SE

				// Others
				//_indexes.reserve(10); // Reserve At Least 10 Indexes
			}
			Part(const AABB2T<T>& boundary, Part* parent, QuadTreeAABB2T<T, O>* tree, const unsigned short& depth)
			{
				_boundary = boundary;
				_parent = parent;
				_tree = tree;
				_depth = depth;
				_childrenIndexesCount = 0;
				_children = new Part * [4];
				_children[0] = NULL; // NW
				_children[1] = NULL; // NE
				_children[2] = NULL; // SW
				_children[3] = NULL; // SE

				// Others
				_boundary.ComputeMinMax();
				//_indexes.reserve(10); // Reserve At Least 10 Indexes
			}
			virtual ~Part() {};

			class Index;


			//////////////////////////////////////////
			// INDEX /////////////////////////////////
			////////////
			// Index is linked to an extern AABB
			// If AABB is modified ask for a Relocation
			class Index
			{
			public:
				// Index List
				

				friend class Part;

				Index(const AABB2T<T>& aabb, const size_t& id, O data) : _aabb(aabb), _id(id), _data(data)
				{
					_part = NULL;
					_indexNode = NULL;
					//std::cout << "New Index AABB C( " << aabb.center.ToString() << " )";
					//std::cout << " S( " << aabb.halfSize.ToString() << " )" << std::endl;
				}
				virtual ~Index() {};

				//// METHODS /////

				/// Getters ///

				const O				GetData() const { return (_data); };
				const size_t&		GetID() const { return (_id); };
				const AABB2T<T>&	GetAABB() const { return (_aabb); };

				/// Setters ///

				// Remove Index from his Part
				void				Remove() { _part->RemoveIndex(this); };
				// Relocate Index at Good Postion from Top of Tree
				bool				RelocateFromTop() { return (_part->RelocateIndexFromTop(this)); };
				// Relocate Index at Good Postion from Current Location
				bool				RelocateFromBot() { return (_part->RelocateIndexFromBot(this)); };


			private:

				typename MXEZDLinkedList<Index*>::Node*		_indexNode;
				const AABB2T<T>&				_aabb;
				const size_t					_id;
				Part*							_part;
				O 								_data;

			};
			//////////////////////////////////////////////////////////////////////////////////////


			//// METHODS /////

			/// Getters ///

			const MXEZDLinkedList<Index*>&			GetIndexes() const { return (_indexes); };
			const std::vector<Index*>&	GetIndexesParents() const { return (_indexesParents); };
			const AABB2T<T>&			GetBoundary() const { return (_boundary); };
			const Part**				GetChildren() const { return ((const Part**)_children); };
			const unsigned int&			GetChildrenIndexesCount() const { return (_childrenIndexesCount); };
			const Part*					GetParent() const { return (_parent); };
			const QuadTreeAABB2T*		GetTree() const { return (_tree); };
			const unsigned short&		GetDepth() const { return (_depth); };

			bool						IsEmpty() const { return (_childrenIndexesCount == 0); };
			inline bool					IsLeaf() const { return (_children[0] == NULL); };

			const Part*					GetDeepestPartAtPosition(const Vec2T<T>& position) const
			{
				if (_boundary.ContainsPoint(position))
				{
					if (_children[0] == NULL)
					{
						return (this); // Last Part Containing Point
					}
					else
					{
						const Part* goodPart;
						if ((goodPart = _children[0]->GetDeepestPartAtPosition(position))) { return (goodPart); }
						if ((goodPart = _children[1]->GetDeepestPartAtPosition(position))) { return (goodPart); }
						if ((goodPart = _children[2]->GetDeepestPartAtPosition(position))) { return (goodPart); }
						if ((goodPart = _children[3]->GetDeepestPartAtPosition(position))) { return (goodPart); }
					}
				}
				return (NULL);
			}

			void						GetIndexesAtPosition(const Vec2T<T>& position, std::vector<const Index*>& indexes) const
			{
				if (_boundary.ContainsPoint(position))
				{
					if (_indexes.GetSize())
					{
						typename MXEZDLinkedList<Index*>::Node* cursor = _indexes.GetFront();
						for (; cursor; cursor = cursor->next)
						{
							//for (size_t i = 0; i < _indexes.size(); i += 1)
							//{

							if (cursor->data->GetAABB().ContainsPoint(position))
								indexes.push_back(cursor->data);
							//}
						}
					}

					if (_children[0] != NULL)
					{
						_children[0]->GetIndexesAtPosition(position, indexes);
						_children[1]->GetIndexesAtPosition(position, indexes);
						_children[2]->GetIndexesAtPosition(position, indexes);
						_children[3]->GetIndexesAtPosition(position, indexes);
					}
				}
			}

			/// Setters ///

			bool						Subdivide()
			{
				if (_depth > _tree->GetMaxDepth())
					return (false);
				//std::cout << "Subdivide" << std::endl;

				// Compute Size of Children Boundaries
				_tree->_pM.vec.x = (T)(_tree->_depthSizes[_depth].x);
				_tree->_pM.vec.y = (T)(_tree->_depthSizes[_depth].y);

				// northWest
				_children[0] = _tree->GetReleasedPart(this, (_depth + 1));
				_children[0]->_boundary.halfSize.x = _tree->_pM.vec.x;
				_children[0]->_boundary.halfSize.y = _tree->_pM.vec.y;
				_children[0]->_boundary.center.x = (_boundary.center.x - _tree->_pM.vec.x);
				_children[0]->_boundary.center.y = (_boundary.center.y - _tree->_pM.vec.y);
				_children[0]->_boundary.ComputeMinMax();

				// northEast
				_children[1] = _tree->GetReleasedPart(this, (_depth + 1));
				_children[1]->_boundary.halfSize.x = _tree->_pM.vec.x;
				_children[1]->_boundary.halfSize.y = _tree->_pM.vec.y;
				_children[1]->_boundary.center.x = (_boundary.center.x + _tree->_pM.vec.x);
				_children[1]->_boundary.center.y = (_boundary.center.y - _tree->_pM.vec.y);
				_children[1]->_boundary.ComputeMinMax();

				// southWest
				_children[2] = _tree->GetReleasedPart(this, (_depth + 1));
				_children[2]->_boundary.halfSize.x = _tree->_pM.vec.x;
				_children[2]->_boundary.halfSize.y = _tree->_pM.vec.y;
				_children[2]->_boundary.center.x = (_boundary.center.x - _tree->_pM.vec.x);
				_children[2]->_boundary.center.y = (_boundary.center.y + _tree->_pM.vec.y);
				_children[2]->_boundary.ComputeMinMax();

				// southEast
				_children[3] = _tree->GetReleasedPart(this, (_depth + 1));
				_children[3]->_boundary.halfSize.x = _tree->_pM.vec.x;
				_children[3]->_boundary.halfSize.y = _tree->_pM.vec.y;
				_children[3]->_boundary.center.x = (_boundary.center.x + _tree->_pM.vec.x);
				_children[3]->_boundary.center.y = (_boundary.center.y + _tree->_pM.vec.y);
				_children[3]->_boundary.ComputeMinMax();

				return (true);
			}

			std::string	ToString() const
			{
				std::stringstream ss;
				ss << "Addr (" << (size_t)this << ") Depth (" << _depth << ") Boundary (" << _boundary.ToString() << ") Indexes (" << _indexes.GetSize() << ")";
				return (ss.str());
			}

		protected:

			void			SetIndexesParents(const std::vector<Index*>& indexes) { _indexesParents = indexes; }

			//private:

			void						ReleaseChildren()
			{
				_children[0] = _tree->SetReleasedPart(_children[0]); // Set Children Indicator to NULL
				_children[1] = _tree->SetReleasedPart(_children[1]); // And also preload Children 
				_tree->SetReleasedPart(_children[2]);
				_tree->SetReleasedPart(_children[3]);
			}

			void						ReleaseChildrenPrepared()
			{
				_children[1] = _tree->SetReleasedPart(_children[1]); // And also preload Children 
				_tree->SetReleasedPart(_children[2]);
				_tree->SetReleasedPart(_children[3]);
			}

			void						ReleaseChildrenAll()
			{
				if (!IsLeaf())
				{
					_children[0]->ReleaseChildrenAll();
					_children[1]->ReleaseChildrenAll();
					_children[2]->ReleaseChildrenAll();
					_children[3]->ReleaseChildrenAll();

					ReleaseChildren();
				}
				else if (_children[1] != NULL)
				{
					ReleaseChildrenPrepared();
				}
			}

			// Index Functions
			bool						InsertIndex(Index* index)
			{
				//std::cout << "Insert Index" << std::endl;

				if (_boundary.ContainsAABB(index->GetAABB()))
				{
					if (_children[0] == NULL)
					{
						if (_children[1] == NULL) // Children Are Not Prepared
						{
							if (!Subdivide())
								return (EmplaceIndex(index));
						}
						else // Children Are Prepared
						{
							// Reset NorthWest
							_children[0] = _tree->GetReleasedPart(this, (_depth + 1));
							_children[0]->_boundary.halfSize.x = _children[1]->_boundary.halfSize.x;
							_children[0]->_boundary.halfSize.y = _children[1]->_boundary.halfSize.y;
							_children[0]->_boundary.center.x = (_boundary.center.x - _children[1]->_boundary.halfSize.x);
							_children[0]->_boundary.center.y = (_boundary.center.y - _children[1]->_boundary.halfSize.y);
							_children[0]->_boundary.ComputeMinMax();
						}
						// Check Can probably be improved
						if (_children[0]->_boundary.ContainsAABB(index->GetAABB()))
							_tree->_pM.pos = 0;
						else if (_children[3]->_boundary.ContainsAABB(index->GetAABB()))
							_tree->_pM.pos = 3;
						else if (_children[1]->_boundary.ContainsAABB(index->GetAABB()))
							_tree->_pM.pos = 1;
						else if (_children[2]->_boundary.ContainsAABB(index->GetAABB()))
							_tree->_pM.pos = 2;
						else
						{
							_children[0] = _tree->SetReleasedPart(_children[0]);
							return (EmplaceIndex(index));
						}
						return (_children[_tree->_pM.pos]->InsertIndex(index));
					}
					else // NO Subdivide - Children Exists
					{
						// Check Can probably be improved
						if (_children[0]->_boundary.ContainsAABB(index->GetAABB()))
							return (_children[0]->InsertIndex(index));
						else if (_children[3]->_boundary.ContainsAABB(index->GetAABB()))
							return (_children[3]->InsertIndex(index));
						else if (_children[1]->_boundary.ContainsAABB(index->GetAABB()))
							return (_children[1]->InsertIndex(index));
						else if (_children[2]->_boundary.ContainsAABB(index->GetAABB()))
							return (_children[2]->InsertIndex(index));
						else
						{
							return (EmplaceIndex(index));
						}
					}
				}
				return (false);
			}

			bool						EmplaceIndex(Index* index)
			{
				if (index->_part == this)
				{

					//std::cout << "SAME Emplace" << std::endl;
					// Already Inside (Not Inserted -> false)
					// Or Part is FULL
					return (false);
				}

				//std::cout << "Emplacing GO at (" << (size_t)this << ")" << std::endl;
				// If (Was Inside a indexes part) -> Remove It


				//index->_pos = _indexes.size();

				/// MUST Be Befor Removing Part in case have common parts and so deleting himself before insertion
				// _indexes.PushBack(index);
				typename MXEZDLinkedList<Index*>::Node* nnode = _indexes.PushBack(index);
				//std::cout << "Indexes Size (" << _indexes.GetSize() << ")" << std::endl;
				this->ModifyParentIndexesCount(1);
				//_indexes.push_back(index);

				if (index->_part)
					index->Remove();

				index->_part = this;
				index->_indexNode = nnode;
				// At Every New Insert Notify this Node is FRESH

				//std::cout << "Emplacing At (" << ToString() << ")" << std::endl;
				//std::cout << "OK Emplace" << std::endl;
				//std::cout << "New Objects Size (" << objects.size() << ")" << std::endl;
				//std::cout << "Is Inserted In AABB " << _boundary.ToString() << "" << std::endl;
				return (true);
			}


			bool						RelocateIndexFromTop(Index* index)
			{
				//std::cout << " GO Relocate From Top " << std::endl;

				//_tree->_pM.posOld = index->_pos; // Old Position in Objects
				_tree->_pM.cursor = _tree->GetRoot(); // Top Of Tree

				if (_tree->_pM.cursor->InsertIndex(index)) // IF New Insertion -> Notify Changed and removed from this part
				{
					/// OBJECT IS RELOCATED -> -1 To ChildrenObjectCount
					//std::cout << " RELOCATED " << std::endl;

					//std::cout << "Relocated Part at Depth (" << _depth << ")" << std::endl;
					//std::cout << "Old Objects Size (" << objects.size() << ")" << std::endl;
					//_indexes.erase(_indexes.begin() + _tree->_pM.posOld); // Remove Object from his oldPos

					////std::cout << "New Objects Size (" << objects.size() << ")" << std::endl;
					//for (_tree->_pM.i = _tree->_pM.posOld; _tree->_pM.i < _indexes.size(); _tree->_pM.i += 1)
					//{
					//	_indexes.at(_tree->_pM.i)->_pos -= 1; // ReOffset all others objects
					//}
					//this->ModifyParentIndexesCount(-1);
				}
				return (true);
			}

			bool						RelocateIndexFromBot(Index* index)
			{
				//std::cout << "\nRelocate Object From Bot" << std::endl;
				//_tree->_pM.posOld = index->_pos; // Old Position in Objects
				_tree->_pM.cursor = index->_part;

				while (_tree->_pM.cursor != NULL && !_tree->_pM.cursor->_boundary.ContainsAABB(index->_aabb))
				{
					//std::cout << "Getting Location From Bot" << std::endl;
					_tree->_pM.cursor = _tree->_pM.cursor->_parent;
				}
				if (_tree->_pM.cursor != NULL)
				{
					if (_tree->_pM.cursor->InsertIndex(index)) // IF New Insertion -> Notify Changed and removed from this part
					{
						//std::cout << "Relocated Part at Depth (" << index->_part->ToString() << ")" << std::endl;
						//std::cout << "Old Objects Size (" << objects.size() << ")" << std::endl;


						//_indexes.erase(_indexes.begin() + _tree->_pM.posOld); // Remove Object from his oldPos

						////std::cout << "New Objects Size (" << objects.size() << ")" << std::endl;
						//for (_tree->_pM.i = _tree->_pM.posOld; _tree->_pM.i < _indexes.size(); _tree->_pM.i += 1)
						//{
						//	_indexes.at(_tree->_pM.i)->_pos -= 1; // ReOffset all others objects
						//}
						//this->ModifyParentIndexesCount(-1);
					}
					else
					{
						//std::cout << "Relocated At Same Part " << _depth << ")" << std::endl;
					}
					//_tree->NotifyChangedPart(index->part);
					return (true);
				}
				//std::cout << "END Relocate No Insert" << std::endl;
				return (false);
			}

			void						RemoveIndex(Index* index)
			{
				//std::cout << "Remove Index From His Part" << std::endl;
				//_indexes.erase(_indexes.begin() + index->_pos); // Remove Object from his Position
				if (index->_indexNode)
				{
					index->_indexNode->Remove();
					index->_indexNode = NULL;
					this->ModifyParentIndexesCount(-1);
				}
			}

			void						ModifyParentIndexesCount(const int& modificator)
			{
				//std::cout << "BF Children Indexes Count : (" << _childrenIndexesCount << ") Modificator (" << modificator << ")" << std::endl;
				_childrenIndexesCount += modificator;
				//std::cout << "Children Indexes Count : (" << _childrenIndexesCount << ")" << std::endl;
				if (_childrenIndexesCount == 0)
				{
					//std::cout << "Remove All Children From This Part" << std::endl;
					ReleaseChildrenAll();
				}
				if (_parent)
					_parent->ModifyParentIndexesCount(modificator);
			}

			MXEZDLinkedList<Index*>				_indexes;
			std::vector<Index*>		_indexesParents;
			AABB2T<T>				_boundary;
			Part** _children;
			unsigned int			_childrenIndexesCount;
			Part* _parent;
			QuadTreeAABB2T* _tree;
			unsigned short			_depth;
		};

		/// //// ///////////////////////////////////////////////////////////////////////////
		////TREE////
		/// //// ///

		//// METHODS /////

		/// Getters ///

		Part* GetRoot() const { return (_root); };
		const unsigned short& GetMaxDepth() const { return (_depthMax); };
		const unsigned int& GetIndexesCount() const { return (_root->GetChildrenIndexesCount()); };

		/*std::vector<const Part::Index*>		GetIndexesAtAABB(const AABB& aabb) const
		{
			std::vector<const typename QuadTreeAABB2T<T, O>::Part::Index*> indexes;

			return (indexes);
		}*/


		const Part* GetDeepestPartAtPosition(const Vec2T<T>& position) const
		{
			return (_root->GetDeepestPartAtPosition(position));
		}

		std::vector<const typename QuadTreeAABB2T<T, O>::Part::Index*>		GetIndexesAtPosition(const Vec2T<T>& position) const
		{
			// std::cout << "std::vector<const typename QuadTreeAABB2T<T, O>::Part::Index*>		GetIndexesAtPosition(const Vec2T<T>& position) const" << std::endl;
			std::vector<const typename QuadTreeAABB2T<T, O>::Part::Index*> indexes;

			_root->GetIndexesAtPosition(position, indexes);

			return (indexes);
		}


		/// Setters ///

		typename Part::Index* InsertData(const AABB2T<T>& aabb, O data)
		{
			typename QuadTreeAABB2T<T, O>::Part::Index* index;
			index = new typename QuadTreeAABB2T<T, O>::Part::Index(aabb, (size_t)&data, data);

			_root->InsertIndex(index);

			return (index);
		}

		// ACCESS  For Inherited Class
	protected:

		
		/// (To Parts)

		void										PartSetIndexesParents(Part* part, const std::vector<typename Part::Index*>& indexes) { part->SetIndexesParents(indexes); };
		std::vector<typename Part::Index*>&			PartGetIndexesParents(Part* part) { return (part->_indexesParents); };
		MXEZDLinkedList<typename Part::Index*>&		PartGetIndexes(Part* part) { return (part->_indexes); };
		Part**										PartGetChildren(Part* part) { return (part->_children); };
		void										PartReleaseChildrenAll(Part* part) { part->ReleaseChildrenAll(); };


		// END ACCESS For Inherited Class
	private:

		// Released Part
		Part* SetReleasedPart(Part* part)
		{
			part->_children[0] = NULL;
			part->_children[1] = NULL;
			part->_parent = NULL;
			part->_childrenIndexesCount = 0;
			_partCount -= 1;
			_releasedParts.push_front(part);
			//std::cout << "Set Released Part: (" << _releasedParts.size() << ")" << std::endl;
			return (NULL);
		}

		Part* UseReleasedPart(const AABB2T<T>& aabb, Part* parent, const unsigned short& depth)
		{
			_partCount += 1;

			//std::cout << "Use Released Part: (" << _releasedParts.size() << ")" << std::endl;
			if (_releasedParts.size())
			{
				Part* part = _releasedParts.back();

				_releasedParts.pop_back();

				part->_boundary = aabb;
				part->_boundary.ComputeMinMax();
				part->_depth = depth;
				part->_parent = parent;

				return (part);
			}
			return (new Part(aabb, parent, this, depth));
		}

		Part* GetReleasedPart(Part* parent, const unsigned short& depth)
		{
			_partCount += 1;

			//std::cout << "Get Released Part: (" << _releasedParts.size() << ")" << std::endl;
			if (_releasedParts.size())
			{
				Part* part = _releasedParts.back();
				_releasedParts.pop_back();
				part->_depth = depth;
				part->_parent = parent;
				return (part);
			}
			return (new Part(parent, this, depth));;
		}


		Part* _root;
		unsigned short			_depthMax;
		Vec2* _depthSizes;

		// Managed Memory QuadParts
		std::deque<Part*>		_releasedParts;
		unsigned int			_partCount;

		// Optimization Struct
		struct PartManipulation
		{
			Vec2T<T>	vec;
			size_t		pos = 0;
			size_t		posOld = 0;
			size_t		i = 0;
			Part*		cursor = NULL;
		};

		PartManipulation	_pM;
	};

	// Float 
	template class QuadTreeAABB2T<float, AABB2>;
	typedef QuadTreeAABB2T<float, AABB2> QuadTreeAABB2;

	template class QuadTreeAABB2T<int, AABB2i>;
	typedef QuadTreeAABB2T<int, AABB2i> QuadTreeAABB2i;

}