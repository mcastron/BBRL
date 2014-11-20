#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include <vector>
#include <ostream>
#include <iostream>

class MEMORY_OBJECT
{
public:

    void SetAllocated() { Allocated = true; }
    void ClearAllocated() { Allocated = false; }
    bool IsAllocated() const { return Allocated; }

private:

    bool Allocated;
};

template <class T>
class MEMORY_POOL
{
public:

    MEMORY_POOL()
    : NumAllocated(0)
    {
    }

    ~MEMORY_POOL()
    {
        DeleteAll();
    }

    T* Construct()
    {
        T* obj = Allocate();
        return new (obj) T;
    }

    void Destroy(T* obj)
    {
        obj.T::~T();
        Free(obj);
    }

    T* Allocate() 
    { 
        if (FreeList.empty())
            NewChunk();
        T* obj = FreeList.back();
        FreeList.pop_back();
        assert(!obj->IsAllocated());
        obj->SetAllocated();
        NumAllocated++;
        return obj;
    }
    
    void Free(T* obj) 
    { 
        assert(obj->IsAllocated());
        obj->ClearAllocated();
        FreeList.push_back(obj);
        NumAllocated--;
    }
    
    void DeleteAll()
    {
        for (ChunkIterator i_chunk = Chunks.begin(); i_chunk != Chunks.end(); ++i_chunk)
            delete *i_chunk;
        Chunks.clear();
        FreeList.clear();
        NumAllocated = 0;
    }
    
    int GetNumAllocated() const { return NumAllocated; }

private:

    struct CHUNK
    {
        static const int Size = 256;
        T Objects[Size];
    };

    void NewChunk()
    {
        CHUNK* chunk = new CHUNK;
        Chunks.push_back(chunk);
        for (int i = CHUNK::Size - 1; i >= 0; --i)
        {
            FreeList.push_back(&chunk->Objects[i]);
            chunk->Objects[i].ClearAllocated();
        }
    }

    std::vector<CHUNK*> Chunks;
    std::vector<T*> FreeList;
    int NumAllocated;
    typedef typename std::vector<CHUNK*>::iterator ChunkIterator;
};

#endif // MEMORY_POOL_H
