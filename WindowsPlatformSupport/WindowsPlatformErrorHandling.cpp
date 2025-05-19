#include "WindowsPlatformErrorHandling.h"

#include "Windows.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <codecvt>

/* std::wstring stringToWstring(const char* utf8Bytes)
{
    //setup converter
    using convert_type = std::codecvt_utf8<typename std::wstring::value_type>;
    std::wstring_convert<convert_type, typename std::wstring::value_type> converter;

    //use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
    return converter.from_bytes(utf8Bytes);
}*/

void AvalonWindowsError::HandleWindowsError(const char* InErrorMessage, bool ForceExit)
{
    /*const DWORD ERROR_ID = GetLastError();

    void* MsgBuffer = nullptr;
    LCID LocalLanguageInfo;
    GetLocaleInfoEx(L"en-US", LOCALE_RETURN_NUMBER | LOCALE_ILANGUAGE, (wchar_t*)&LocalLanguageInfo, sizeof(LocalLanguageInfo));

    //get error message and attach it to Msgbuffer
    FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, ERROR_ID, LocalLanguageInfo, (wchar_t*)&MsgBuffer, 0, NULL);

    //concat string to DisplayBuffer
    const std::wstring DisplayBuffer = stringToWstring(InErrorMessage) + L", failed with error " + std::to_wstring(ERROR_ID) + L": " + static_cast<wchar_t*>(MsgBuffer);

    // Display the error message and exit the process
    MessageBoxExW(NULL, DisplayBuffer.c_str(), L"Error", MB_ICONERROR | MB_OK, static_cast<WORD>(LocalLanguageInfo));

    ExitProcess(ERROR_ID);*/
}
