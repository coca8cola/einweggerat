// MyFirstWTLWindow.cpp : Defines the entry point for the application.
//
#include "../stdafx.h"
#include "resource.h"
#include "MyWindow.h"
#include "../mudlib.h"
#include "cmdline.h"
#include <iostream>
#include <string>
#include <sstream>
using namespace std;
CAppModule _Module;

int Run(LPTSTR cmdline = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
    CEmuMessageLoop theLoop;
    _Module.AddMessageLoop(&theLoop);
    CMyWindow dlgMain;
    // Create the main window
    if (NULL == dlgMain.CreateEx())
        return 1;       // uh oh, window creation failed
    // Show the window
    LONG style = dlgMain.GetWindowLong(GWL_STYLE);
    style &= ~WS_POPUP;
    dlgMain.SetWindowLong(GWL_STYLE, style);
    dlgMain.SetWindowPos(0, 100, 100, 640, 480, SWP_NOZORDER);
    dlgMain.CenterWindow();
    dlgMain.SetIcon(LoadIcon(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_ICON1)));
#ifdef _WIN64
    dlgMain.SetWindowText(L"einweggerät - 64bit");
#else
    dlgMain.SetWindowText(L"einweggerät - 32bit");
#endif
    CMenu menu;
    menu.Attach(LoadMenu(_Module.GetResourceInstance(), MAKEINTRESOURCE(MENU_MAINFRAME)));
    dlgMain.SetMenu(menu);
    menu.DestroyMenu();

    int argc = 1;
    char** cmdargptr = Mud_Misc::cmdlinetoargANSI(GetCommandLineA(), &argc);

    if (argc < 2)
    {
        if (!AttachConsole(ATTACH_PARENT_PROCESS))
        {
            AllocConsole();
            AttachConsole(GetCurrentProcessId());
            Mud_Misc::redirectiotoconsole();
            dlgMain.ShowWindow(nCmdShow);
            int nRet = theLoop.Run(dlgMain);
            _Module.RemoveMessageLoop();
            LocalFree(cmdargptr);
            ExitProcess(0);
            return nRet;
        }
        else
        {
            Mud_Misc::redirectiotoconsole();
            cmdline::parser a;
            a.add<string>("core_name", 'c', "core filename", true, "");
            a.add<string>("rom_name", 'r', "rom filename", true, "");
            a.add("pergame", 'g', "per-game configuration");
            a.add("threads", 't', "use multithreaded core execution");
            a.parse_check(argc, cmdargptr);
            printf("\nPress any key to continue....\n");
            _Module.RemoveMessageLoop();
            LocalFree(cmdargptr);
            ExitProcess(0);
            return 0;
        }

    }

    if (!AttachConsole(ATTACH_PARENT_PROCESS))
    {
        AllocConsole();
        AttachConsole(GetCurrentProcessId());
        Mud_Misc::redirectiotoconsole();
    }
    else
    {
        Mud_Misc::redirectiotoconsole();
    }
    cmdline::parser a;
    a.add<string>("core_name", 'c', "core filename", true, "");
    a.add<string>("rom_name", 'r', "rom filename", true, "");
    a.add("pergame", 'g', "per-game configuration");
    a.add("threads", 't', "use multithreaded core execution");
    a.parse_check(argc, cmdargptr);

    wstring rom = s2ws(a.get<string>("rom_name"));
    wstring core = s2ws(a.get<string>("core_name"));
    bool percore = a.exist("pergame");
    bool thread = a.exist("threads");
    dlgMain.ShowWindow(nCmdShow);
    dlgMain.start((TCHAR*)rom.c_str(), (TCHAR*)core.c_str(), percore, thread);
    int nRet = theLoop.Run(dlgMain);
    _Module.RemoveMessageLoop();
    LocalFree(cmdargptr);
    ExitProcess(0);
    return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
    HRESULT hRes = ::CoInitialize(NULL);
    // If you are running on NT 4.0 or higher you can use the following call instead to 
    // make the EXE free threaded. This means that calls come in on a random RPC thread.
    //	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
    ATLASSERT(SUCCEEDED(hRes));
    // this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
    ::DefWindowProc(NULL, 0, 0, 0L);
    AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls
    hRes = _Module.Init(NULL, hInstance);
    ATLASSERT(SUCCEEDED(hRes));
    int nRet = Run(lpstrCmdLine, nCmdShow);
    _Module.Term();
    ::CoUninitialize();
    return nRet;
}