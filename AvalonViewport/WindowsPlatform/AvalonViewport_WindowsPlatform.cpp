/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/
#include "AvalonViewport_WindowsPlatform.h"

//#include "Windows.h"
#include "WinCon.h"

#include "..\..\WindowsPlatformSupport\WindowsPlatformErrorHandling.h"


void AvalonViewport_WindowsPlatform::InitializeViewport()
{
	//CreateGameConsole();
	SetFullscreen();
}

void AvalonViewport_WindowsPlatform::TeardownViewport()
{
	CloseHandle(mGameProcessInformation.hProcess);
	CloseHandle(mGameProcessInformation.hThread);
}

bool AvalonViewport_WindowsPlatform::ResizeViewport(short SizeX, short SizeY)
{
	const HANDLE OutputConsole = GetStdHandle(STD_OUTPUT_HANDLE); //hStdOutWrite;//

	// Shrink the Viewport, so we can set the buffer
	SMALL_RECT MinViewportSize = { 0, 0, 1, 1 };
	if (!SetConsoleWindowInfo(OutputConsole, TRUE, &MinViewportSize))
	{
		AvalonWindowsError::HandleWindowsError("SetConsoleWindowInfo");
	}

	// Set the Buffer Size
	COORD BufferSize = { SizeX, SizeY };
	if (!SetConsoleScreenBufferSize(OutputConsole, BufferSize))
	{
		AvalonWindowsError::HandleWindowsError("SetConsoleScreenBufferSize");
	}

	// Set the actual Viewport Size
	SMALL_RECT ViewportSize = { 0, 0, SizeX - 1, SizeY - 1 };
	if (!SetConsoleWindowInfo(OutputConsole, TRUE, &ViewportSize))
	{
		AvalonWindowsError::HandleWindowsError("SetConsoleWindowInfo");
	}

	// Resize the window, we want to do this AFTER the viewport is resized!!
	MONITORINFO MonitorInfo;
	MonitorInfo.cbSize = sizeof(MonitorInfo);

	HWND ConsoleWindow = GetConsoleWindow();
	HMONITOR Monitor = MonitorFromWindow(ConsoleWindow, MONITOR_DEFAULTTOPRIMARY);

	// Get the monitor info
	if (!GetMonitorInfo(Monitor, &MonitorInfo))
	{
		AvalonWindowsError::HandleWindowsError("GetMonitorInfo");
	}

	int PosX = 0; //MonitorInfo.rcMonitor.left;
	int PosY = 0; //MonitorInfo.rcMonitor.top;
	int Width = MonitorInfo.rcMonitor.right;
	int Height = MonitorInfo.rcMonitor.bottom;

	UINT Flags = 0; //SWP_NOZORDER, SWP_NOACTIVATE, SWP_FRAMECHANGED
	if (!SetWindowPos(ConsoleWindow, NULL, PosX, PosY, Width, Height, Flags))
	{
		AvalonWindowsError::HandleWindowsError("SetWindowPos");
	}

	return true;
}

bool AvalonViewport_WindowsPlatform::SetFullscreen()
{
	SetConsoleTitle(TEXT("The Path"));

	HWND ConsoleWindow = GetConsoleWindow();

	//////////////////////////////////////////////////////////////////////
	// Set new window style and size, makes it borderless!
	LONG NewStyle = (GetWindowLong(ConsoleWindow, GWL_STYLE) & ~(WS_CAPTION | WS_THICKFRAME)); //WS_CAPTION
	if (!SetWindowLongPtr(ConsoleWindow, GWL_STYLE, NewStyle | WS_MAXIMIZE))
	{
		AvalonWindowsError::HandleWindowsError("SetWindowLongPtr");
	}

	LONG Style_EX = GetWindowLong(ConsoleWindow, GWL_EXSTYLE);
	LONG NewStyle_EX = Style_EX & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
	if (!SetWindowLongPtr(ConsoleWindow, GWL_EXSTYLE, NewStyle_EX))
	{
		AvalonWindowsError::HandleWindowsError("SetWindowLongPtr");
	}
	//////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////
	// Set the viewport to the max size!
	const HANDLE OutputConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD NewSize = GetLargestConsoleWindowSize(OutputConsole);
	if (NewSize.X == 0 && NewSize.Y == 0)
	{
		AvalonWindowsError::HandleWindowsError("GetLargestConsoleWindowSize");
	}

	ResizeViewport(NewSize.X, NewSize.Y);
	//////////////////////////////////////////////////////////////////////

	SetCursorVisible(false);


	// Hide the Scrollbar
	if (!ShowScrollBar(ConsoleWindow, SB_BOTH, FALSE))
	{
		AvalonWindowsError::HandleWindowsError("ShowScrollBar");
	}

	return true;
}

void AvalonViewport_WindowsPlatform::WriteArea(short PosX, short PosY, short SizeX, short SizeY)
{
	const HANDLE OutputConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	const int BufferSize = SizeX * SizeY;
	CHAR_INFO* CharacterInfo = new CHAR_INFO[BufferSize]();
	for (int i = 0; i < BufferSize; i++)
	{
		CharacterInfo[i].Char.AsciiChar = 'A';
		CharacterInfo[i].Char.UnicodeChar = 'A';
		CharacterInfo[i].Attributes = FOREGROUND_BLUE;
	}

	COORD Buffer = { SizeX, SizeY };
	COORD Position = { PosX, PosY };
	SMALL_RECT WriteArea = { PosX, PosY, SizeX, SizeY };

	if (!WriteConsoleOutput(OutputConsole, CharacterInfo, Buffer, Position, &WriteArea))
	{
		AvalonWindowsError::HandleWindowsError("WriteConsoleOutput");
	}

	delete[] CharacterInfo;
}

bool AvalonViewport_WindowsPlatform::SetCursorVisible(bool Visible)
{
	const HANDLE OutputConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO CursorInfo = { 1, Visible };
	if (!SetConsoleCursorInfo(OutputConsole, &CursorInfo))
	{
		AvalonWindowsError::HandleWindowsError("SetConsoleCursorInfo");
	}

	return true;
}

bool AvalonViewport_WindowsPlatform::FillScreenTest()
{
	const HANDLE OutputConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_SCREEN_BUFFER_INFO CurrentBufferInfo;
	if (!GetConsoleScreenBufferInfo(OutputConsole, &CurrentBufferInfo))
	{
		AvalonWindowsError::HandleWindowsError("GetConsoleScreenBufferInfo");
	}

	WriteArea(0, 0, CurrentBufferInfo.dwSize.X, CurrentBufferInfo.dwSize.Y);

	return true;
}

COORD FViewportCoordToCOORD(const FCoord& InCoord)
{
	COORD OutCoord = COORD();
	OutCoord.X = InCoord.X;
	OutCoord.Y = InCoord.Y;

	return OutCoord;
}

FCoord COORDToFViewportCoord(const COORD& InCoord)
{
	FCoord OutCoord = FCoord();
	OutCoord.X = InCoord.X;
	OutCoord.Y = InCoord.Y;

	return OutCoord;
}

bool AvalonViewport_WindowsPlatform::DrawArea(const FFrameBuffer& FrameBuffer)
{
	const HANDLE OutputConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	COORD Buffer = FViewportCoordToCOORD(FrameBuffer.GetSize());

	int BufferLength = Buffer.X * Buffer.Y;
	CHAR_INFO* CharacterBuffer = new CHAR_INFO[BufferLength]();
	for (int Index = 0; Index < BufferLength; ++Index)
	{
		const FCellData& CellData = FrameBuffer[Index];
		CHAR_INFO& CharInfo = CharacterBuffer[Index];
		
		CharInfo.Char.AsciiChar		 = static_cast<CHAR>(CellData.mChar);
		CharInfo.Char.UnicodeChar	 = CellData.mChar;
		CharInfo.Attributes			 = CellData.mAttributes;
	}

	COORD Position = FViewportCoordToCOORD(FrameBuffer.GetPos());
	SMALL_RECT WriteArea = { Position.X, Position.Y, Buffer.X, Buffer.Y };

	if (!WriteConsoleOutput(OutputConsole, CharacterBuffer, Buffer, Position, &WriteArea))
	{
		AvalonWindowsError::HandleWindowsError("WriteConsoleOutput");
	}

	delete[] CharacterBuffer;

	return true;
}


FCoord AvalonViewport_WindowsPlatform::GetViewportSize() const
{
	const HANDLE OutputConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_SCREEN_BUFFER_INFO BufferInfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &BufferInfo);

	return COORDToFViewportCoord(BufferInfo.dwSize);
}

void AvalonViewport_WindowsPlatform::CreateLogWindow()
{
	STARTUPINFOA si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);

	ZeroMemory(&mGameProcessInformation, sizeof(mGameProcessInformation));

	si.lpTitle = (LPSTR)"The Path Log";

	if (CreateProcessA(
		NULL,            // No module name (use command line)
		(LPSTR)"cmd.exe", // Command line
		NULL,            // Process handle not inheritable
		NULL,            // Thread handle not inheritable
		FALSE,           // Set handle inheritance to FALSE
		CREATE_NEW_CONSOLE, // Opens a new console window
		NULL,            // Use parent's environment block
		NULL,            // Use parent's starting directory 
		&si,             // Pointer to STARTUPINFO structure
		&mGameProcessInformation)             // Pointer to PROCESS_INFORMATION structure
		) {
		WaitForSingleObject(mGameProcessInformation.hProcess, INFINITE);
	}
}

void AvalonViewport_WindowsPlatform::CreateGameConsole()
{
	STARTUPINFOA ConsoleStartupInfo;
	ZeroMemory(&ConsoleStartupInfo, sizeof(ConsoleStartupInfo));
	ConsoleStartupInfo.cb = sizeof(ConsoleStartupInfo);

	// Name the process/window
	ConsoleStartupInfo.lpTitle = (LPSTR)"The Path";

	// Setup the console to handle input
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	ConsoleStartupInfo.dwFlags |= STARTF_USESTDHANDLES;


	// Create pipes for STDIN, STDOUT, and STDERR
	CreatePipe(&hStdInRead, &hStdInWrite, &sa, 0);
	CreatePipe(&hStdOutRead, &hStdOutWrite, &sa, 0);
	CreatePipe(&hStdErrRead, &hStdErrWrite, &sa, 0);

	// Set the handles in STARTUPINFO
	ConsoleStartupInfo.hStdInput = hStdInRead;
	ConsoleStartupInfo.hStdOutput = hStdOutWrite;
	ConsoleStartupInfo.hStdError = hStdErrWrite;

	ZeroMemory(&mGameProcessInformation, sizeof(mGameProcessInformation));

	if (CreateProcessA(
		NULL,            // No module name (use command line)
		(LPSTR)"cmd.exe", // Command line
		NULL,            // Process handle not inheritable
		NULL,            // Thread handle not inheritable
		TRUE,           // Set handle inheritance to FALSE
		CREATE_NEW_CONSOLE, // Opens a new console window
		NULL,            // Use parent's environment block
		NULL,            // Use parent's starting directory 
		&ConsoleStartupInfo,             // Pointer to STARTUPINFO structure
		&mGameProcessInformation)             // Pointer to PROCESS_INFORMATION structure
		) {
		//WaitForSingleObject(mGameProcessInformation.hProcess, INFINITE);
	}

	// Close unused handles in the parent process
	CloseHandle(hStdInRead);
	CloseHandle(hStdOutWrite);
	CloseHandle(hStdErrWrite);

	HWND ConsoleWindow = FindWindowA(NULL, ConsoleStartupInfo.lpTitle);

	//////////////////////////////////////////////////////////////////////
	// Set new window style and size, makes it borderless!
	LONG NewStyle = (GetWindowLong(ConsoleWindow, GWL_STYLE) & ~(WS_CAPTION | WS_THICKFRAME)); //WS_CAPTION
	if (!SetWindowLongPtr(ConsoleWindow, GWL_STYLE, NewStyle | WS_MAXIMIZE))
	{
		AvalonWindowsError::HandleWindowsError("SetWindowLongPtr");
	}

	LONG Style_EX = GetWindowLong(ConsoleWindow, GWL_EXSTYLE);
	LONG NewStyle_EX = Style_EX & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
	if (!SetWindowLongPtr(ConsoleWindow, GWL_EXSTYLE, NewStyle_EX))
	{
		AvalonWindowsError::HandleWindowsError("SetWindowLongPtr");
	}
	//////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////
	// Set the viewport to the max size!
	const HANDLE OutputConsole = hStdOutWrite;
	COORD NewSize = GetLargestConsoleWindowSize(OutputConsole);
	if (NewSize.X == 0 && NewSize.Y == 0)
	{
		AvalonWindowsError::HandleWindowsError("GetLargestConsoleWindowSize");
	}

	ResizeViewport(NewSize.X, NewSize.Y);
	//////////////////////////////////////////////////////////////////////

	SetCursorVisible(false);


	// Hide the Scrollbar
	if (!ShowScrollBar(ConsoleWindow, SB_BOTH, FALSE))
	{
		AvalonWindowsError::HandleWindowsError("ShowScrollBar");
	}

}

void* AvalonViewport_WindowsPlatform::GetInputHandle()
{
	return hStdInRead;
}