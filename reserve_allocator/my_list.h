#pragma once
#include <stdexcept>


namespace my
{
	template<typename T, typename Alloc>
	class List
	{
	public:
		struct Node;
		class Iterator;

		using nodeAllocator = typename std::allocator_traits<Alloc>::template rebind_alloc<Node>;

		using value_type = typename std::allocator_traits<Alloc>::template value_type;
		using reference = value_type&;
		using const_reference = const value_type&;
		using pointer = typename std::allocator_traits<Alloc>::template pointer;
		using const_pointer = typename std::allocator_traits<Alloc>::template const_pointer;
		using size_type = typename std::allocator_traits<Alloc>::template size_type;


		struct Node
		{
			Node(Node* prev_, Node* next_, const_reference value_) : prev(prev_), next(next_), value(value_) {}
			
			Node* prev;
			Node* next;
			value_type value;
		};

		class Iterator
		{
		public:
			Iterator(Node* ptrToNode = nullptr) : mPtr(ptrToNode) {}

			Iterator& operator++()
			{
				mPtr = mPtr->next;
				return *this;
			}

			Iterator& operator--()
			{
				mPtr = mPtr->prev;
				return *this;
			}

			reference operator*()
			{
				return mPtr->value;
			}

			bool operator==(const Iterator& other)
			{
				return mPtr == other.mPtr;
			}

			bool operator!=(const Iterator& other)
			{
				return mPtr != other.mPtr;
			}

			Node* mPtr;
		};

		List() :mHeadNode(nullptr), mSize(0)
		{
			mHeadNode = reinterpret_cast<Node*>(std::allocator_traits<nodeAllocator>::allocate(mAllocator, 1));

			mHeadNode->next = mHeadNode;
			mHeadNode->prev = mHeadNode;
		}

		//TODO:
		List(List&& other) noexcept : mAllocator(std::move(other.mAllocator))
		{
			mSize = other.mSize;
			other.mSize = 0;

			mHeadNode = other.mHeadNode;
			other.mHeadNode = nullptr;
		}

		//TODO:
		List(const List& other) noexcept : mHeadNode(other.mHeadNode),mSize(other.mSize)
		{

		}

		~List()
		{
			if (mHeadNode != nullptr)
			{
				Node* removableNode = mHeadNode->next;

				while (removableNode != mHeadNode)
				{
					auto nextNode = removableNode->next;

					std::allocator_traits<nodeAllocator>::destroy(mAllocator, removableNode);
					std::allocator_traits<nodeAllocator>::deallocate(mAllocator, removableNode, sizeof(Node));

					removableNode = nextNode;
				}

				std::allocator_traits<nodeAllocator>::deallocate(mAllocator, removableNode, sizeof(Node));
			}
		}

		size_type max_size() const
		{
			return std::allocator_traits<nodeAllocator>::max_size(mAllocator)- 1;
		}
		size_type size() const
		{
			return mSize;
		}
		bool empty() const
		{
			return !mSize;
		}

		Iterator insert(Iterator where, const_reference value)
		{
			if(mSize + 1 > this->max_size())
			{
				throw std::exception("Maximum number of values exceeded");
			}

			Node* thisNode = where.mPtr;

			Node* newNode = reinterpret_cast<Node*>(std::allocator_traits<nodeAllocator>::allocate(mAllocator, 1));

			std::allocator_traits<nodeAllocator>::construct(mAllocator, newNode, thisNode->prev, thisNode, value);

			thisNode->prev->next = newNode;
			thisNode->prev = newNode;

			++mSize;

			return Iterator(newNode);
		}
		Iterator insert(Iterator where, size_type count, const_reference value)
		{
			while(count > 0)
			{
				where = this->insert(where, value);
				--count;
			}
			return where;
		}

		void push_back(const_reference value)
		{
			this->insert(this->end(), value);
		}
		void push_front(const_reference value)
		{
			this->insert(this->begin(), value);
		}

		Iterator erase(Iterator where)
		{
			Node* thisNode = where.mPtr;

			if (thisNode == mHeadNode)
			{
				throw std::exception("you cannot delete a head node");
			}

			Node* afterThisNode = thisNode->next;
			Node* beforeThisNode = thisNode->prev;

			afterThisNode->prev = beforeThisNode;
			beforeThisNode->next = afterThisNode;

			std::allocator_traits<nodeAllocator>::destroy(mAllocator, thisNode);
			std::allocator_traits<nodeAllocator>::deallocate(mAllocator, thisNode, sizeof(Node));

			--mSize;

			return Iterator(afterThisNode);
		}
		Iterator erase(Iterator begin, Iterator end)
		{
			while (begin != end)
			{
				begin = this->erase(begin);
			}
			
			return begin;
		}
		void pop_back()
		{
			Iterator preLastNodeIter = --this->end();
			this->erase(preLastNodeIter);
		}
		void pop_front()
		{
			this->erase(this->begin());
		}
		void clear()
		{
			this->erase(this->begin(), this->end());
		}

		void remove(const_reference value)
		{
			for (auto beg = this->begin(); beg != this->end(); ++beg)
			{
				if (*beg == value)
				{
					this->erase(beg);
				}
			}
		}

		Iterator begin()
		{
			return Iterator(mHeadNode->next);
		}
		Iterator end()
		{
			return Iterator(mHeadNode);
		}

		reference back()
		{
			return *--this->end();
		}

		reference front()
		{
			return *this->begin();
		}

		//TODO:
		//void sort();

	private:
		nodeAllocator mAllocator;
		Node* mHeadNode;
		size_type mSize;
	};

}