#include "pch.h"
#include <Windows.h>
#include <string>
#include "ini.h"
// test

void Main();
DWORD WINAPI HotKey(LPVOID);

#define TheGameFlowManager 0xA99BBC
#define GRaceStatus_fObj 0xA98284

void (*Music_ForceKillMusic)() = (void(*)())0x64C800;
void (*Music_ForceLicensedMusic)() = (void(*)())0x64C7D0;
void (*Music_ForceCanyonMusic)() = (void(*)())0x64C770;
void (*Music_ForcePursuitMusic)() = (void(*)())0x64C7A0;
void (*Music_ForceCrewMusic)() = (void(*)())0x64C740;
void(*Hud_ShowMessage)(const char* MessageToShow) = (void(*)(const char*))0x65B1B0;
int(__cdecl* Music_SetWait)(int theWait) = (int(__cdecl*)(int))0x64C700;
char* (__thiscall* GRaceStatus_IsCanyonRace)(void* GRaceStatus) = (char* (__thiscall*)(void*))0x404340;
char* (__thiscall* GRaceStatus_IsChallengeRace)(void* GRaceStatus) = (char* (__thiscall*)(void*))0x4042B0;
char* (__thiscall* GRaceStatus_IsSpeedtrapRace)(void* GRaceStatus) = (char* (__thiscall*)(void*))0x404310;
char* (__thiscall* GRaceStatus_IsTutorialRace)(void* GRaceStatus) = (char* (__thiscall*)(void*))0x404400;
char* (__thiscall* GRaceStatus_IsCheckpointRace)(void* GRaceStatus) = (char* (__thiscall*)(void*))0x61AAF0;
char* (__thiscall* GRaceStatus_IsPursuitRace)(void* GRaceStatus) = (char* (__thiscall*)(void*))0x4042E0;

DWORD GameState;
int keyValue, textValue;
bool canSkip = false;
bool IsOnFocus;

void Main()
{
    mINI::INIFile file("MusicSkipper.ini");
    mINI::INIStructure ini;
    file.read(ini);

    std::string& getKey = ini["Main"]["SkipKey"];
    std::string& getText = ini["Main"]["ShowText"];
    keyValue = std::stoi(getKey);
    textValue = std::stoi(getText);

    CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&HotKey, NULL, 0, NULL);
}

DWORD WINAPI HotKey(LPVOID)
{
    while (true)
    {
        Sleep(100);
        GameState = *(int*)TheGameFlowManager;
        IsOnFocus = !(*(bool*)0xAB0B3C);
        DWORD* GRaceStatus = (DWORD*)*(DWORD*)GRaceStatus_fObj;
        const char* IsCanyon = GRaceStatus_IsCanyonRace(GRaceStatus);
        const char* IsSpeedtrap = GRaceStatus_IsSpeedtrapRace(GRaceStatus);
        const char* IsChallenge = GRaceStatus_IsChallengeRace(GRaceStatus);
        const char* IsCheckpoint = GRaceStatus_IsCheckpointRace(GRaceStatus);
        const char* IsPursuitRace = GRaceStatus_IsPursuitRace(GRaceStatus);
        const char* IsTutorial = GRaceStatus_IsTutorialRace(GRaceStatus);
        if (IsCanyon)
        {
            canSkip = false;
        }
        else
        {
            canSkip = true;
        }
        if ((GetAsyncKeyState(keyValue) & 1) && (GameState == 6) && IsOnFocus && canSkip == true)
        {
            if (IsSpeedtrap || IsChallenge || IsCheckpoint || IsTutorial)
            {
                Music_ForceKillMusic();
                Music_SetWait(1);
                Sleep(100);
                Music_ForceLicensedMusic();
            }
            else if (IsPursuitRace)
            {
                Music_ForceKillMusic();
                Music_SetWait(1);
                Sleep(100);
                Music_ForcePursuitMusic();
            }
            else
            {
                Music_ForceKillMusic();
                Music_SetWait(1);
                Sleep(100);
                Music_ForceLicensedMusic();
            }
            if (textValue == 1)
            {
                Hud_ShowMessage("^Music Skipped");
            }

            while ((GetAsyncKeyState(84) & 0x8000) > 0) { Sleep(100); }
        }
    }
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        Main();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
