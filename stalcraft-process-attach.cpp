#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <sstream>
#include "Utils.h"
#include "modules.h"
uintptr_t GetModBase(DWORD ProcID, const wchar_t* ModuleName)
{
    uintptr_t modbaseAddr = 0;
    HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, ProcID);
    if (hsnap != INVALID_HANDLE_VALUE)
    {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32First(hsnap, &modEntry))
        {
            do
            {
                if (!_wcsicmp(modEntry.szModule, ModuleName))
                {
                    modbaseAddr = (uintptr_t)modEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(hsnap, &modEntry));
        }
    }
    CloseHandle(hsnap);
    return modbaseAddr;
}

DWORD GetProc(const wchar_t* ProcName)
{
    DWORD ProcID = 0;
    HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hsnap != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 ProcEntry;
        ProcEntry.dwSize = sizeof(ProcEntry);
        if (Process32First(hsnap, &ProcEntry))
        {
            do
            {
                if (!_wcsicmp(ProcEntry.szExeFile, ProcName))
                {
                    ProcID = ProcEntry.th32ProcessID;
                    break;
                }
            } while (Process32Next(hsnap, &ProcEntry));
        }
    }
    CloseHandle(hsnap);
    return ProcID;
}

struct GameHandling
{
    // Necessary variables
    DWORD process;
    uintptr_t gamemodule;
    HANDLE readproc;
}handles;

struct ESP
{
    // Variables For ESP
    HBRUSH Brush;
    HFONT Font;
    HDC game_window;
    HWND game;
    COLORREF TextColor;
    HPEN SnaplineColor;

}esp;


// Structs to store Coords
struct Vec4
{
    float x, y, z, w;
};

// Vec4 stores our clipcoords

struct Vec3
{
    float x, y, z;
};

// Vec3 stores our position

struct Vec2
{
    float x, y;
};

float ScreenX = GetSystemMetrics(SM_CXSCREEN); // Get Width of screen
float ScreenY = GetSystemMetrics(SM_CYSCREEN); // Get Height of screen


void DrawFilledRect(int x, int y, int w, int h)
{
    RECT rect = { x, y, x + w, y + h };
    FillRect(esp.game_window, &rect, esp.Brush);
}

void DrawBorderBox(int x, int y, int w, int h, int thickness)
{

    DrawFilledRect(x, y, w, thickness);

    DrawFilledRect(x, y, thickness, h);

    DrawFilledRect((x + w), y, thickness, h);

    DrawFilledRect(x, y + h, w + thickness, thickness);
}

void DrawLine(int x, int y, HPEN LineColor)
{
    MoveToEx(esp.game_window, (ScreenX / 2), (ScreenY / 2), NULL);
    LineTo(esp.game_window, x, y);
    esp.SnaplineColor = CreatePen(PS_SOLID, 1, RGB(0, 255, 255));
    SelectObject(esp.game_window, esp.SnaplineColor);
}

void DrawString(int x, int y, COLORREF color, const char* text)
{
    SetTextAlign(esp.game_window, TA_CENTER | TA_NOUPDATECP);

    SetBkColor(esp.game_window, RGB(0, 0, 0));

    SetBkMode(esp.game_window, TRANSPARENT);

    SetTextColor(esp.game_window, color);

    SelectObject(esp.game_window, esp.Font);

    TextOutA(esp.game_window, x, y, text, strlen(text));

    DeleteObject(esp.Font);
}

void Logo()
{
    DrawString(150, 60, RGB(255, 255, 255), "STALCRAFT EXTERNAL TOOL");
/*
    DrawString(750, 210, RGB(255, 255, 255), "HP: 99");
    DrawString(1150, 210, RGB(255, 255, 255), "Milky_WWW");
    DrawBorderBox((700), (190), 500, 750, 5);

*/
}

void MegaJump()
{
}

int main()
{
    DWORD aProcesses[1024];
    DWORD cbNeeded;
    DWORD cProcesses;
    unsigned int i;
    SetConsoleTitleA("STALCRAFT External Tool");
    handles.process = GetProc(L"java.exe");
    std::stringstream ss;
    ss << handles.process;
    std::string processid = ss.str();
    if (handles.process == NULL)
    {
        std::cout << "Failed to Get Process";
        getchar();
        exit(0);
    }
    handles.gamemodule = GetModBase(handles.process, L"jvm.dll");
    if (handles.gamemodule == NULL)
    {
        std::cout << "Failed to Get Module" << std::endl;
        getchar();
        exit(0);
    }
    handles.readproc = OpenProcess(PROCESS_ALL_ACCESS, false, handles.process);
    esp.game = FindWindowA(0, "STALCRAFT");
    //esp.game = FindWindowA(0, "Minecraft 1.7.10");
    esp.game_window = GetDC(esp.game);
    std::cout << "CONNECTED!" << "\n" << std::endl;
    std::cout << "PID: " << processid.c_str() << std::endl;
    //printf(handles.gamemodule);
    std::cout << "\n" << "JVM Module: " << std::endl;
    std::cout << "\n" << "Read Process Memory: " << handles.readproc << "\n" << std::endl;
    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
        return 1;
    cProcesses = cbNeeded / sizeof(DWORD);
    PrintModules(handles.process);

    float GOVNO = 1.f;
    WriteProcessMemory(handles.readproc, (PFLOAT)(handles.gamemodule + 0xD64B00),&GOVNO, sizeof(GOVNO), 0);
    while (handles.process != NULL)
    {
        Logo();
        //MegaJump();
    }
    CloseHandle(handles.readproc);
}
