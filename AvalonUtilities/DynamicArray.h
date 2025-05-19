/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

template <class T>
class DynamicArray
{
private:
    T* mData;
    unsigned int mCount;

public:
    DynamicArray();
    ~DynamicArray();

    void Add(T Element);
    void Insert(T Element, unsigned int Index);
    bool Remove(T Element);
    bool Contains(T Element) const;
    bool IsValidIndex(const unsigned int Index) const;
    int GetIndexOf(T Element) const;
    unsigned int Num() const;
    void Clear();

    T& operator[](const unsigned int Index) const;
};

template <class T>
DynamicArray<T>::DynamicArray()
{
    mCount = 0;
    mData = nullptr;
}

template <class T>
DynamicArray<T>::~DynamicArray()
{
    if (mCount > 0)
    {
        delete[] mData;
    }

}

template <class T>
void DynamicArray<T>::Add(T Element)
{
    if (Contains(Element))
    {
        return;
    }

    T* tmp = new T[mCount + 1];

    for (unsigned int Index = 0; Index < mCount; ++Index)
    {
        tmp[Index] = mData[Index];
    }

    mCount++;

    if (mCount > 1)
    {
        delete[] mData;
    }

    mData = tmp;

    mData[mCount - 1] = Element;
}

template <class T>
void DynamicArray<T>::Insert(T Element, unsigned int InsertIndex)
{
    if (Contains(Element))
    {
        return;
    }

    if (InsertIndex > mCount)
    {
        Add(Element);
        return;
    }

    T* tmp = new T[mCount + 1];

    unsigned int CopyIndex = 0;
    for (unsigned int Index = 0; Index < mCount; ++Index)
    {
        if (Index == InsertIndex)
        {
            tmp[Index] = Element;
        }
        else
        {
            tmp[Index] = mData[CopyIndex];
            CopyIndex++;
        }
    }

    mCount++;

    delete[] mData;
    mData = tmp;

    mData[mCount - 1] = Element;
}

template <class T>
bool DynamicArray<T>::Remove(T Element)
{
    if (!Contains(Element))
    {
        return false;
    }

    if (mCount == 1)
    {
        mCount = 0;
        delete[] mData;
        mData = nullptr;
    }
    else
    {
        T* tmp = new T[mCount - 1];

        unsigned int TmpIndex = 0;
        for (unsigned int Index = 0; Index < mCount; ++Index)
        {
            if (mData[Index] != Element)
            {
                tmp[TmpIndex] = mData[Index];
                TmpIndex++;
            }
        }

        mCount--;

        delete[] mData;
        mData = tmp;
    }

    return true;
}

template <class T>
bool DynamicArray<T>::Contains(T Element) const
{
    for (unsigned int Index = 0; Index < mCount; ++Index)
    {
        if (mData[Index] == Element)
        {
            return true;
        }
    }

    return false;
}

template <class T>
bool DynamicArray<T>::IsValidIndex(const unsigned int Index) const
{
    return (Index >= 0) && (Index < mCount);
}

template <class T>
int DynamicArray<T>::GetIndexOf(T Element) const
{
    for (unsigned int Index = 0; Index < mCount; ++Index)
    {
        if (mData[Index] == Element)
        {
            return Index;
        }
    }

    return -1;
}

template <class T>
unsigned int DynamicArray<T>::Num() const
{
    return mCount;
}

template <class T>
void DynamicArray<T>::Clear()
{
    if (mCount > 0)
    {
        mCount = 0;
        delete[] mData;
        mData = nullptr;
    }
}

template <class T>
T& DynamicArray<T>::operator[](const unsigned int Index) const
{
    return mData[Index];
}
