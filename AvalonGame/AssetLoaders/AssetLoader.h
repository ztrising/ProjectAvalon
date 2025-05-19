/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

namespace tinyxml2 { class XMLDocument; }

namespace AssetLoaders
{
    /***************************************************************************************
    *  Save/Load
    ****************************************************************************************/
    void LoadGame();
    void CreateNewSave();
    void GetLoadedGameSave(char* OutSaveFile);
    void OpenJournalFile(tinyxml2::XMLDocument& OutFile);
    void SaveJournal(tinyxml2::XMLDocument& InFile);
    /****************************************************************************************/

    /***************************************************************************************
    *  Utility
    ****************************************************************************************/
    void OpenAvalonFile(const char* RelativePath, tinyxml2::XMLDocument& OutFile);
    void OpenGameDataFile(const char* Asset, tinyxml2::XMLDocument& OutFile);
    void SaveGameDataFile(const char* Asset, tinyxml2::XMLDocument& InFile);

    // For visuals we access the static files
    void OpenConstFile(const char* RelativePath, tinyxml2::XMLDocument& OutFile);
    void OpenWidgetFile(const char* WidgetAsset, tinyxml2::XMLDocument& OutFile);
    /****************************************************************************************/
}