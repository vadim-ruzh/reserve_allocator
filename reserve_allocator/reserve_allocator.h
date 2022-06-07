#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <new>
#include <iostream>


static size_t AlignUp(size_t n,size_t Alignment) noexcept
{
    if (n % Alignment)
    {
        n = (n / Alignment + 1) * Alignment;
    }

    return n;
}

template<std::size_t ReserveSize, std::size_t Alignment = alignof(std::max_align_t)>
class MemoryPool
{
public:
    uint8_t* Allocate(std::size_t requiredMemorySize)
    {
	    if (IsNotReserved())
	    {
	        ReserveMemory();
	    }

        const size_t alignMemorySize = AlignUp(requiredMemorySize,Alignment);

        if(IsPointerInMemoryPool(mCurrentPlaceInMemory + alignMemorySize))
        {
            uint8_t* ptrToAllocatedMemory = mCurrentPlaceInMemory;
            mCurrentPlaceInMemory += alignMemorySize;
            mAllocationCounter++;

            return ptrToAllocatedMemory;
        }

        throw std::runtime_error("Out of bounds of the reserved memory");
    }

    void Deallocate()
    {
    	mAllocationCounter--;

        if(mAllocationCounter == 0)
        {
            UnReserveMemory();
        }
    }

    bool IsPointerInMemoryPool(uint8_t* ptr) const
    {
    	return mBeginOfMemory <= ptr && ptr <= mBeginOfMemory + ReserveSize;
    }

private:

    bool IsNotReserved() const
    {
        return mBeginOfMemory == nullptr ? true : false;
    }

    void ReserveMemory()
    {
        if (IsNotReserved())
        {
            mBeginOfMemory = static_cast<uint8_t*>(_aligned_malloc(ReserveSize,Alignment));
            mCurrentPlaceInMemory = mBeginOfMemory;
        }
    }

    void UnReserveMemory()
    {
        if(mBeginOfMemory != nullptr)
        {
            _aligned_free(mBeginOfMemory);
        }
    }

    uint8_t* mBeginOfMemory = nullptr;
    uint8_t* mCurrentPlaceInMemory = nullptr;
    std::size_t mAllocationCounter = 0;
};

template<typename T, std::size_t ReservedMemorySize,std::size_t Alignment = alignof(std::max_align_t)>
class LinearAllocator
{
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;

    LinearAllocator() : mMemoryPool(std::make_unique<MemoryPool<ReservedMemorySize,Alignment>>()){}

    template<typename U, size_t UReservedMemorySize, std::size_t UAlignment>
    LinearAllocator(LinearAllocator<U, UReservedMemorySize, UAlignment> const&) : LinearAllocator(){}

    template<typename U>
    struct rebind
    {
        using other = LinearAllocator<U, ReservedMemorySize,Alignment>;
    };

    static size_type max_size()
    {
        return ReservedMemorySize/AlignUp(sizeof(value_type), Alignment) - 1;
    }

    pointer allocate(std::size_t size)
    {
        const size_t bytesToAllocate = size * sizeof(value_type);

        return  reinterpret_cast<pointer>(mMemoryPool->Allocate(bytesToAllocate));
    }

    void deallocate(pointer p, std::size_t n)
    {
	    if (mMemoryPool->IsPointerInMemoryPool(reinterpret_cast<uint8_t*>(p)))
	    {
            mMemoryPool->Deallocate();
	    }
	    else
	    {
	        _aligned_free(p);
	    }
    }

    template<typename U, typename ...Args>
    void construct(U* p, Args &&...args)
    {
        new(p) U(std::forward<Args>(args)...);
    }

    void destroy(pointer p)
    {
        p->~T();
    }

private:
    std::unique_ptr<MemoryPool<ReservedMemorySize, Alignment>> mMemoryPool;
};