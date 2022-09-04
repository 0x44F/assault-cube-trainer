/*
    Assault cube trainer (simple cheat, external)
    Written by 0x44F.
*/

#include <iostream>
#include <Windows.h>
#include <Tlhelp32.h>
#include <vector>
#include <string>
#include <wchar.h>

#include "structs.h"

template <typename T>
void WPM(HANDLE handleProcess, LPVOID address, const T& value)
{
	WriteProcessMemory(handleProcess, address, &value, sizeof(value), NULL);
}

void print_ctrl()
{
	std::cout << "[Numpad 0]	Enable god mode\n";
	std::cout << "[Numpad 1]	Enable infinite ammo\n";
	std::cout << "[Numpad 2]	Enable infinite armor\n";
	std::cout << "[Numpad 3]	Teleport using X, Y and Z co-ordinates\r\n";
}

void god_mode(uintptr_t health_address, HANDLE handleProcess)
{
	int newHealth = 999999;
	WPM(handleProcess, health_address, newHealth);

	std::cout << "\nEnabled god mode.\r\n";
	Sleep(3000);
	std::cout << "\033[2J\033[1;1H";
	print_ctrl();
}

void infinite_ammo(uintptr_t pweap_address, uintptr_t pReserve_address, uintptr_t sweap_address, uintptr_t sReserve_address, HANDLE handleProcess)
{
	int newAmmo = 99999;
	WPM(handleProcess, pweap_address, newAmmo);
	WPM(handleProcess, pReserve_address, newAmmo);

	WPM(handleProcess, sweap_address, newAmmo);
	WPM(handleProcess, sReserve_address, newAmmo);

	std::cout << "\nInfinite ammo enabled\r\n";
	Sleep(3000);
	std::cout << "\033[2J\033[1;1H";
	print_ctrl();
}

void infinite_armor(uintptr_t armor_address, HANDLE handleProcess)
{
	int newarmor = 99999;
	WPM(handleProcess, armor_address, newarmor);
	std::cout << "\nEnabled infinite armor." << std::endl;
	Sleep(3000);
	std::cout << "\033[2J\033[1;1H";
	print_ctrl();
}

void teleport(uintptr_t position_address, HANDLE handleProcess)
{
	std::cout << "\033[2J\033[1;1H";
	std::cout << "Enter the co-ordinates separated by spaces you'd like to teleport to." << std::endl;
	Position pos;
	cin >> pos.x >> pos.y >> pos.z;

	WPM(handleProcess, position_address, pos);
	std::cout << "\nTeleported to location." << std::endl;
	Sleep(3000);
	std::cout << "\033[2J\033[1;1H";
	print_ctrl();
}

DWORD GetProcId(const char* processName)
{
	DWORD procid = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap == NULL)
    {
		exit(-1);
    }
	else
	{
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(pe32); 

		if (Process32First(hSnap, &pe32) == FALSE)
		{
            exit(-1);
		}
		else
		{
			do
			{
				if (_strcmpi(pe32.szExeFile, processName) == 0)
				{
					procid = pe32.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnap, &pe32));
		}
	}
	CloseHandle(hSnap);
	return procid;
}

uintptr_t GetModuleBase_addressess(DWORD processId, const char *modName)
{
	uintptr_t moduleBase_addressess = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
	if (hSnap == NULL)
	{
		exit(-1);
	}
	else
	{
		MODULEENTRY32 me;
		me.dwSize = sizeof(MODULEENTRY32);

		if (Module32First(hSnap, &me))
		{
			do
			{
				if (_strcmpi(me.szModule, modName) == 0)
				{
					moduleBase_addressess = (uintptr_t)me.modBase_address;
					break;
				}
			} while (Module32Next(hSnap, &me));
		}
	}
	CloseHandle(hSnap);
	return moduleBase_addressess;
}

uintptr_t find_addressess(HANDLE hProc, uintptr_t ptr, vector<unsigned int> offsets)
{
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); ++i)
	{
		ReadProcessMemory(hProc, (BYTE *)addr, &addr, sizeof(addr), 0);
		addr += offsets[i];
	}
	return addr;
}

int main()
{
	DWORD process = GetProcId("ac_client.exe");
	std::cout << "pid - " << (unsigned int)process << std::endl;
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, process);

	if (hProcess == NULL)
	{
        exit(0);		
	}
	else
	{
		uintptr_t moduleBase = GetModuleBase_addressess(process, "ac_client.exe");
		uintptr_t playerObjectBase_address = moduleBase + 0x00109B74;

		if (ReadProcessMemory(hProcess, (BYTE *)playerObjectBase_address, &playerObjectBase_address, sizeof(playerObjectBase_address), NULL) == 0)
		{
            exit(0);
		}
		else
		{
			uintptr_t pos_address = playerObjectBase_address + 0x34;

			uintptr_t health_address = playerObjectBase_address + 0xF8;
			uintptr_t armor_address = playerObjectBase_address + 0xFC;

			uintptr_t secondaryweap_sec_address = playerObjectBase_address + 0x114;
			uintptr_t secondaryweap_address = playerObjectBase_address + 0x13C;
			uintptr_t primaryweap_sec_address = playerObjectBase_address + 0x128;
			uintptr_t primaryweap_address = playerObjectBase_address + 0x150;

			std::cout << "\033[2J\033[1;1H";
			print_ctrl();

			while (true)
			{
				if(GetAsyncKeyState(VK_NUMPAD0) & 0x8000) {
					god_mode(health_address, hProcess);
				}
				else if(GetAsyncKeyState(VK_NUMPAD1) & 0x8000) {
					infinite_ammo(primaryweap_address, primaryweap_sec_address, secondaryweap_address, secondaryweap_sec_address, hProcess);
				}
				else if(GetAsyncKeyState(VK_NUMPAD2) & 0x8000) {
					infinite_armor(armor_address, hProcess);
				}
				else if(GetAsyncKeyState(VK_NUMPAD3) & 0x8000) {
					teleport(pos_address, hProcess);
				}
			}
		}
	}

	CloseHandle(hProcess);
	return 0;
}
