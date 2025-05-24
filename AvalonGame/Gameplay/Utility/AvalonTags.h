/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "Saveable.h"
#include "../../AvalonUnit/AvalonUnit.h"

#include <vector>
#include <string>

struct FAvalonTag
{
    FAvalonTag() {}
    FAvalonTag(std::string String) : mTagString(String) {}

    void Load(const char* ID, FSaveContext& Context);
    void Save(const char* ID, FSaveContext& Context);

    bool operator==(const FAvalonTag& rhs) const
    {
        return (mTagString == rhs.mTagString);
    }

    bool operator<(const FAvalonTag& rhs) const
    {
        return (mTagString < rhs.mTagString);
    }

    // TODO: make this... not a string?
    std::string mTagString;
};

struct FAvalonTagContainer
{
    void Load(const char* ID, FSaveContext& Context);
    void Save(const char* ID, FSaveContext& Context);

    void AddTag(const FAvalonTag& Tag);
    void Merge(const FAvalonTagContainer& Other);

    bool AllMatch(const FAvalonTagContainer& Other) const;
    bool AnyMatch(const FAvalonTagContainer& Other) const;
    bool NoMatch(const FAvalonTagContainer& Other) const;

    bool ContainsTag(const FAvalonTag& Tag) const;

    unsigned int NumTags() const { return mTags.size(); }

    std::vector<FAvalonTag> mTags;
};

enum class ETagFilterType
{
    AnyMatch,
    AllMatch,
    NoMatch,
};

struct IFilterTest : public ISaveable, public IAvalonUnit
{
    static void FilterFactory(HardUnitRef& OutNewFilter, FSaveContext& Context);

    virtual void Load(FSaveContext& Context) override;
    virtual void Save(FSaveContext& Context) override;

    virtual bool PassesFilter(const FAvalonTagContainer& Container) const = 0;

protected:
    ETagFilterType mFilterType = ETagFilterType::AnyMatch;
};

struct FTagExpression : public IFilterTest
{
    void Load(FSaveContext& Context) override;
    void Save(FSaveContext& Context) override;

    bool PassesFilter(const FAvalonTagContainer& Container) const override;

private:
    //std::vector<IFilterTest*> mFilters;
    HardRefList mFilters;
};

struct FTagOperation : public IFilterTest
{
    void Load(FSaveContext& Context) override;
    void Save(FSaveContext& Context) override;

    bool PassesFilter(const FAvalonTagContainer& Container) const override;

private:
    FAvalonTagContainer mFilterContainer;
};

struct FAvalonTagQuery : public ISaveable
{
    void Load(FSaveContext& Context) override;
    void Save(FSaveContext& Context) override;

    bool PassesQuery(const FAvalonTagContainer& TagContainer) const;

private:
    //IFilterTest* mRootTest = nullptr;
    HardUnitRef mRootTest;
};
