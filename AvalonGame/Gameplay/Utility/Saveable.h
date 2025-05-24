/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include <vector>
#include <string>

namespace tinyxml2 { class XMLDocument; class XMLElement; }

struct FSaveContext
{
    // float
    void Load(const char* ID, float& OutValue) const;
    void Save(const char* ID, float Value);

    // string
    void Load(const char* ID, std::string& OutValue) const;
    void Save(const char* ID, std::string Value);

    void LoadTextString(std::string& OutValue) const;
    void SaveTextString(std::string Value);

    // int
    void Load(const char* ID, int& OutValue) const;
    void Save(const char* ID, int Value);

    // bool
    void Load(const char* ID, bool& OutValue) const;
    void Save(const char* ID, bool Value);

    // general
    std::string GetSaveID() const;

    template <class T, typename Functor>
    T* AllocateChildWithFactory(const char* ID, Functor& Factory)
    {
        FSaveContext Context;
        GetChild(Context, ID);

        T* NewObject = Factory(Context);
        if (ISaveable* Saveable = dynamic_cast<ISaveable*>(NewObject))
        {
            ISaveable::Load(Saveable, Context);

        }
        
        return NewObject;
    }

    /*template <class T>
    T* AllocateChildObject(const char* ID)
    {
        AllocateChildWithFactory(ID
            , [](FSaveContext& InContext) {return new T(); });
    }*/

    template <class T, typename Functor>
    void AllocateChildrenWithFactory(std::vector<T>& OutChildObjects, Functor& Factory)
    {
        std::vector<FSaveContext> Children;
        GetChildren(Children);

        for (FSaveContext& Context : Children)
        {
            T NewObject = nullptr;
            Factory(NewObject,Context);
            if (ISaveable* Saveable = dynamic_cast<ISaveable*>(NewObject.get()))
            {
                ISaveable::Load(Saveable, Context);
            }

            OutChildObjects.push_back(NewObject);
        }
    }

    template <class T>
    void AllocateChildren(std::vector<T*>& OutChildObjects)
    {
        AllocateChildrenWithFactory(OutChildObjects
            , [](FSaveContext& InContext) {return new T(); });
    }

    template <typename Functor>
    void LoadFromWrapper(const char* WrapperID, Functor& ForEach)
    {
        FSaveContext Wrapper;
        GetChild(Wrapper, WrapperID);

        if (Wrapper.mElement == nullptr)
        {
            return;
        }

        std::vector<FSaveContext> WrapperContents;
        Wrapper.GetChildren(WrapperContents);

        for (FSaveContext& Context : WrapperContents)
        {
            ForEach(Context);
        }
    }

    template <class T, typename Functor>
    void AllocateFromWrapper( const char* WrapperID
                            , std::vector<T*>& OutChildObjects
                            , Functor& Factory)
    {
        FSaveContext Wrapper;
        GetChild(Wrapper, WrapperID);

        if (Wrapper.mElement == nullptr)
        {
            return;
        }

        std::vector<FSaveContext> WrapperContents;
        Wrapper.GetChildren(WrapperContents);

        for (FSaveContext& Context : WrapperContents)
        {
            T* NewObject = Factory(Context);
            if (ISaveable* Saveable = dynamic_cast<ISaveable*>(NewObject))
            {
                ISaveable::Load(Saveable, Context);

            }

            OutChildObjects.push_back(NewObject);
        }
    }

    template <class T, typename Functor>
    void AllocateFromWrapper( const char* WrapperID
                            , std::vector<T*>& OutChildObjects)
    {
        AllocateFromWrapper( WrapperID
                           , OutChildObjects
                           , [](FSaveContext& InContext) {return new T(); });
    }

    template <class T>
    void SaveUnderWrapper(const char* ID, std::vector<T*>& SaveableList)
    {
        FSaveContext Wrapper;
        CreateChild(Wrapper, ID);

        for (T* Saveable : SaveableList)
        {
            ISaveable::Save(Saveable, Wrapper);
        }
    }

    template <typename Functor>
    void LoadChildren(Functor& ForEach)
    {
        std::vector<FSaveContext> Children;
        GetChildren(Children);

        for (FSaveContext& Context : Children)
        {
            ForEach(Context);
        }
    }

    void GetChild(FSaveContext& OutChild, const char* ID = nullptr);

    bool IsValid() const { return mElement != nullptr; }

private:
    void GetChildren(std::vector<FSaveContext>& OutChildren, const char* ID = nullptr);

    void CreateChild(FSaveContext& OutChild, const char* ID);

    tinyxml2::XMLElement* mElement = nullptr;
    bool mSavingToFile = false;

    friend class ISaveable;
};

class ISaveable
{
public:
    static void Load(ISaveable* Saveable, FSaveContext& Context);
    static void Save(ISaveable* Saveable, FSaveContext& ParentContext);

    static void LoadFromFile(ISaveable* Saveable, std::string Filename);
    static void SaveToFile(ISaveable* Saveable, std::string Filename);

    // We save our Journal Entries in a special way
    static bool LoadJournalEntry(ISaveable& JournalEntry, int Index);
    static void SaveJournalEntry(ISaveable& JournalEntry);
    static void MakeNewJournalEntry(ISaveable& JournalEntry);

    virtual std::string GetSaveID() { return mElementID; }

    void SetSavesToFile() { mSavesToFile = true; }
    void SetConstData() { mConstData = true; }

    virtual bool CanSave(FSaveContext& Context);

private:
    virtual void Load(FSaveContext& Context) = 0;
    virtual void Save(FSaveContext& Context) = 0;

    std::string mElementID;
    bool mSavesToFile = false; // TODO : I think we need to specify WHICH file eh? How is this working?
    bool mConstData = false;
};
