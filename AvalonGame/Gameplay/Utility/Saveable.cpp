/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "Saveable.h"
#include "../../../ExternalTools/TinyXML/tinyxml2.h"
#include "../../AssetLoaders/AssetLoader.h"
/***************************************************************************************
*  ISaveable
****************************************************************************************/
/*static*/ void ISaveable::Load(ISaveable* Saveable, FSaveContext& Context)
{
	Saveable->mElementID = Context.mElement->Value();
	Saveable->Load(Context);
}

/*static*/ void ISaveable::Save(ISaveable* Saveable, FSaveContext& ParentContext)
{
	if (Saveable->CanSave(ParentContext) == false)
	{
		return;
	}

	tinyxml2::XMLElement* Outer = ParentContext.mElement;

	const char* ElementID = Saveable->mElementID.c_str();
	tinyxml2::XMLElement* Element = Outer->InsertNewChildElement(ElementID);

	FSaveContext Context;
	Context.mElement = Element;
	Context.mSavingToFile = ParentContext.mSavingToFile;

	Saveable->Save(Context);
}

/*static*/ void ISaveable::LoadFromFile(ISaveable* Saveable, std::string Filename)
{
	tinyxml2::XMLDocument doc;

	if (Saveable->mConstData)
	{
		AssetLoaders::OpenConstFile(Filename.c_str(), doc);
	}
	else
	{
		AssetLoaders::OpenGameDataFile(Filename.c_str(), doc);
	}

	tinyxml2::XMLElement* Root = doc.FirstChildElement("AvalonData");

	FSaveContext Context;
	Context.mElement = Root->FirstChildElement();

	ISaveable::Load(Saveable, Context);
}

/*static*/ void ISaveable::SaveToFile(ISaveable* Saveable, std::string Filename)
{
	tinyxml2::XMLDocument doc;

	if (Saveable->mConstData)
	{
		return; // What are you doing?
	}

	AssetLoaders::OpenGameDataFile(Filename.c_str(), doc);

	FSaveContext Context;
	Context.mElement = doc.FirstChildElement("AvalonData");
	Context.mElement->DeleteChildren();
	Context.mSavingToFile = true;

	ISaveable::Save(Saveable, Context);

	AssetLoaders::SaveGameDataFile(Filename.c_str(), doc);
}

/*static*/ bool ISaveable::LoadJournalEntry(ISaveable& JournalEntry, int Index)
{
	bool FoundEntry = true;

	tinyxml2::XMLDocument doc;
	AssetLoaders::OpenJournalFile(doc);

	tinyxml2::XMLElement* AvalonData = doc.FirstChildElement("AvalonData");
	tinyxml2::XMLElement* JournalRoot = AvalonData->FirstChildElement("Journal");

	tinyxml2::XMLElement* LastEntry = JournalRoot->LastChildElement("JournalEntry");
	tinyxml2::XMLElement* Element = LastEntry;

	int EntryCount = Index;
	while (Element && EntryCount > 0)
	{
		Element = Element->PreviousSiblingElement();
		EntryCount--;
	}

	if (Element == nullptr)
	{
		Element = LastEntry;
		FoundEntry = false;
	}

	FSaveContext Context;
	Context.mElement = Element;

	JournalEntry.Load(Context);

	return FoundEntry;
}

/*static*/ void ISaveable::SaveJournalEntry(ISaveable& JournalEntry)
{
	tinyxml2::XMLDocument doc;
	AssetLoaders::OpenJournalFile(doc);

	tinyxml2::XMLElement* AvalonData = doc.FirstChildElement("AvalonData");
	tinyxml2::XMLElement* JournalRoot = AvalonData->FirstChildElement("Journal");

	tinyxml2::XMLElement* Element = JournalRoot->LastChildElement("JournalEntry");

	FSaveContext Context;
	Context.mElement = Element;

	JournalEntry.Save(Context);
	AssetLoaders::SaveJournal(doc);
}

/*static*/ void ISaveable::MakeNewJournalEntry(ISaveable& JournalEntry)
{
	tinyxml2::XMLDocument doc;
	AssetLoaders::OpenJournalFile(doc);

	tinyxml2::XMLElement* AvalonData = doc.FirstChildElement("AvalonData");
	tinyxml2::XMLElement* JournalRoot = AvalonData->FirstChildElement("Journal");

	FSaveContext Context;
	Context.mElement = JournalRoot->InsertNewChildElement("JournalEntry");

	JournalEntry.Save(Context);
	AssetLoaders::SaveJournal(doc);
}

bool ISaveable::CanSave(FSaveContext& Context)
{
	if (mSavesToFile && !Context.mSavingToFile)
	{
		return false;
	}

	return true;
}

/***************************************************************************************
*  FSaveContext
****************************************************************************************/

// float
void FSaveContext::Load(const char* ID, float& OutValue) const
{
	mElement->QueryFloatAttribute(ID, &OutValue);
}

void FSaveContext::Save(const char* ID, float Value)
{
	mElement->SetAttribute(ID, Value);
}

// string
void FSaveContext::Load(const char* ID, std::string& OutValue) const
{
	const char* Value = "";
	mElement->QueryStringAttribute(ID, &Value);
	OutValue = Value;
}

void FSaveContext::Save(const char* ID, std::string Value)
{
	mElement->SetAttribute(ID, Value.c_str());
}

void FSaveContext::LoadTextString(std::string& OutValue) const
{
	OutValue = mElement->GetText();
}

void FSaveContext::SaveTextString(std::string Value)
{
	mElement->SetText(Value.c_str());
}

// int
void FSaveContext::Load(const char* ID, int& OutValue) const
{
	mElement->QueryIntAttribute(ID, &OutValue);
}

void FSaveContext::Save(const char* ID, int Value)
{
	mElement->SetAttribute(ID, Value);
}

// bool
void FSaveContext::Load(const char* ID, bool& OutValue) const
{
	mElement->QueryBoolAttribute(ID, &OutValue);
}

void FSaveContext::Save(const char* ID, bool Value)
{
	mElement->SetAttribute(ID, Value);
}

// general
std::string FSaveContext::GetSaveID() const
{
	std::string ID = mElement->Value();
	return ID;
}

void FSaveContext::GetChild(FSaveContext& OutChild, const char* ID)
{
	OutChild.mElement = mElement->FirstChildElement(ID);
}

void FSaveContext::GetChildren(std::vector<FSaveContext>& OutChildren, const char* ID)
{
	tinyxml2::XMLElement* ChildElement = mElement->FirstChildElement(ID);
	while (ChildElement != nullptr)
	{
		FSaveContext Context;
		Context.mElement = ChildElement;
		Context.mSavingToFile = mSavingToFile;
		OutChildren.push_back(Context);

		ChildElement = ChildElement->NextSiblingElement();
	}
}

void FSaveContext::CreateChild(FSaveContext& OutChild, const char* ID)
{
	OutChild.mElement = mElement->InsertNewChildElement(ID);
	OutChild.mSavingToFile = mSavingToFile;
}