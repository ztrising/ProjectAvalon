/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "AvalonTags.h"

#include <sstream>
#include <algorithm>

void FAvalonTag::Load(const char* ID, FSaveContext& Context)
{
    /*
    * <SomeElement Tag="Red">
    */

    Context.Load(ID, mTagString);
}

void FAvalonTag::Save(const char* ID, FSaveContext& Context)
{
    Context.Save(ID, mTagString);
}

void FAvalonTagContainer::Load(const char* ID, FSaveContext& Context)
{
    /*
    * <SomeElement Tags="Red,Green,Blue">
    */

    std::string ContainerString;
    Context.Load(ID, ContainerString);

    std::vector<std::string> TagsInString;
    {
        std::stringstream stream(ContainerString);
        std::string TagString;
        while (std::getline(stream, TagString, ','))
        {
            FAvalonTag NewTag(TagString);
            mTags.push_back(NewTag);
        }
    }

    std::sort(mTags.begin(), mTags.end());
}

void FAvalonTagContainer::Save(const char* ID, FSaveContext& Context)
{
    std::string TagContainerString = "";
    for (const FAvalonTag& Tag : mTags)
    {
        TagContainerString += Tag.mTagString;
        TagContainerString += ',';
    }

    Context.Save(ID, TagContainerString);
}

void FAvalonTagContainer::AddTag(const FAvalonTag& Tag)
{
    auto it = std::upper_bound(mTags.cbegin(), mTags.cend(), Tag);
    mTags.insert(it, Tag);
}

void FAvalonTagContainer::Merge(const FAvalonTagContainer& Other)
{
    std::vector<FAvalonTag> NewTags;

    std::set_union( mTags.begin(), mTags.end(),
                    Other.mTags.begin(), Other.mTags.end(),
                    std::back_inserter(NewTags));

    mTags = NewTags;
}

bool FAvalonTagContainer::AllMatch(const FAvalonTagContainer& Other) const
{
    std::vector<FAvalonTag> diff;
    std::set_difference(mTags.begin(), mTags.end(), Other.mTags.begin(), Other.mTags.end(),
        std::inserter(diff, diff.begin()));

    return diff.size() == 0;
}

bool FAvalonTagContainer::AnyMatch(const FAvalonTagContainer& Other) const
{
    for (const FAvalonTag OtherTag : Other.mTags)
    {
        if (ContainsTag(OtherTag))
        {
            return true;
        }
    }

    return false;
}

bool FAvalonTagContainer::NoMatch(const FAvalonTagContainer& Other) const
{
    for (const FAvalonTag OtherTag : Other.mTags)
    {
        if (ContainsTag(OtherTag))
        {
            return false;
        }
    }

    return true;
}

bool FAvalonTagContainer::ContainsTag(const FAvalonTag& Tag) const
{
    return std::find(mTags.begin(), mTags.end(), Tag) != mTags.end();
}

/*static*/ IFilterTest* IFilterTest::FilterFactory(FSaveContext& Context)
{
    std::string ID = Context.GetSaveID();
    if (ID == "TagExpression")
    {
        FTagExpression* Expression = new FTagExpression();
        ISaveable::Save(Expression, Context);
        return Expression;
    }
    else if (ID == "TagOperation")
    {
        FTagOperation* Operation = new FTagOperation();
        ISaveable::Save(Operation, Context);
        return Operation;
    }

    return nullptr;
}

void IFilterTest::Load(FSaveContext& Context)
{
    /*
    * <FilterTestClass Type="AnyMatch" />
    */

    std::string StringValue;
    Context.Load("Type", StringValue);

    if (StringValue == "AnyMatch")
    {
        mFilterType = ETagFilterType::AnyMatch;
    }
    else if (StringValue == "AllMatch")
    {
        mFilterType = ETagFilterType::AllMatch;
    }
    else if (StringValue == "NoMatch")
    {
        mFilterType = ETagFilterType::NoMatch;
    }
}

void IFilterTest::Save(FSaveContext& Context)
{
    std::string FilterString;
    switch (mFilterType)
    {
    case ETagFilterType::AnyMatch:  FilterString = "AnyMatch"; break;
    case ETagFilterType::AllMatch:  FilterString = "AllMatch"; break;
    case ETagFilterType::NoMatch:   FilterString = "NoMatch"; break;
    }

    Context.Save("Type", FilterString);
}

void FTagExpression::Load(FSaveContext& Context)
{
    // Expected Format:
    /*
    *   <TagExpression Type="AnyMatch">
    *       <TagExpression Type="AnyMatch">
    *           <TagOperation Type="AnyMatch" Tags="Blue,Green,Purple"/>
    *           <TagOperation Type="NoMatch" Tags="Red,Blurp"/>
    *       </TagExpression>
    *       <TagOperation Type="AnyMatch" Tags="Blue,Green,Purple"/>
    *   </TagExpression>
    */

    // Load the "Type"
    IFilterTest::Load(Context);

    // Load the Filters
    std::vector<IFilterTest*> TempArray;
    Context.AllocateChildrenWithFactory(mFilters, IFilterTest::FilterFactory);
}

void FTagExpression::Save(FSaveContext& Context)
{
    IFilterTest::Save(Context);

    for (IFilterTest* Filter : mFilters)
    {
        ISaveable::Save(Filter, Context);
    }
}

bool FTagExpression::PassesFilter(const FAvalonTagContainer& Container) const
{
    if (mFilters.size() == 0)
    {
        return true;
    }

    bool AnyPassed = false;
    for (IFilterTest* Test : mFilters)
    {
        if (Test->PassesFilter(Container))
        {
            if (mFilterType == ETagFilterType::NoMatch)
            {
                return false;
            }

            AnyPassed = true;
        }
        else
        {
            if (mFilterType == ETagFilterType::AllMatch)
            {
                return false;
            }
        }
    }

    if (mFilterType == ETagFilterType::AnyMatch && !AnyPassed)
    {
        return false;
    }

    return true;
}

void FTagOperation::Load(FSaveContext& Context)
{
    // Expected Format:
    // <TagOperation Type="AnyMatch" Tags ="Blue,Green,Purple" />

    IFilterTest::Load(Context);
    mFilterContainer.Load("Tags", Context);
}

void FTagOperation::Save(FSaveContext& Context)
{
    IFilterTest::Save(Context);
    mFilterContainer.Save("Tags", Context);
}

bool FTagOperation::PassesFilter(const FAvalonTagContainer& Container) const
{
    switch (mFilterType)
    {
    case ETagFilterType::AllMatch:  return mFilterContainer.AllMatch(Container);
    case ETagFilterType::AnyMatch:  return mFilterContainer.AnyMatch(Container);
    case ETagFilterType::NoMatch:   return mFilterContainer.NoMatch(Container);
    }

    return false;
}

void FAvalonTagQuery::Load(FSaveContext& Context)
{
    // Expected Format:
    /*
    *   <TagQuery>
    *       <TagExpression Type="AnyMatch">
    *           <TagOperation Type="AnyMatch" Tags="Blue,Green,Purple"/>
    *           <TagOperation Type="NoMatch" Tags="Red,Blurp"/>
    *       </TagExpression>
    *   </TagQuery>
    */
    std::vector<IFilterTest*> TempArray;
    Context.AllocateChildrenWithFactory(TempArray, IFilterTest::FilterFactory);

    if (TempArray.size() > 0)
    {
        mRootTest = TempArray[0];
    }
}

void FAvalonTagQuery::Save(FSaveContext& Context)
{
    if (mRootTest != nullptr)
    {
        ISaveable::Save(mRootTest, Context);
    }
}

bool FAvalonTagQuery::PassesQuery(const FAvalonTagContainer& TagContainer) const
{
    if (mRootTest)
    {
        return mRootTest->PassesFilter(TagContainer);
    }

    return true;
}
