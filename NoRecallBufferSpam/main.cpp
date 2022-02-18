#include "main.h"

typedef void(__cdecl* CTimer__Update)();
CTimer__Update O_CTimer__Update;

typedef int(__fastcall* AddTextToRecallBuffer)(void*, void*, char*);
AddTextToRecallBuffer O_AddTextToRecallBuffer;

DWORD sampAddress;


char* getRecallBufferText(int iRecall) {
	uintptr_t StInputInfo = *reinterpret_cast<uintptr_t*>(sampAddress + 0x21A0E8);
	return reinterpret_cast<char*>(StInputInfo + 0x1565 + 129 * iRecall); // 0x1565 char szRecallBufffer[10][129] offset
   // https://github.com/BlastHackNet/mod_s0beit_sa-1/blob/dc9b3b13599a8b6325e566f567b5391b0b2a6dc8/src/samp.h#L857
}

// https://cdn.discordapp.com/attachments/779600054558130209/943958883943190558/unknown.png
int __fastcall AddTextToRecallBufferHook(void* stInputInfo, void* edx, char* newRecalltext) { 
	char* oldRecallText = getRecallBufferText(0); 
	if (!strcmp(newRecalltext, oldRecallText)) { // если прошлое сообщение отправленное в чат совпадает с новым, то не даём новому добавится в буффер
		return 0; 
	}

	return O_AddTextToRecallBuffer(stInputInfo, edx, newRecalltext);
}

void __cdecl CTimer__UpdateHook() {
	static bool initialized = false;
	if (!initialized)
	{
		sampAddress = (DWORD)GetModuleHandle("samp.dll");
		if (sampAddress) {

			DWORD hookAdrress = sampAddress + 0x65930;
			MH_CreateHook(reinterpret_cast<void*>(hookAdrress), &AddTextToRecallBufferHook, reinterpret_cast<void**>(&O_AddTextToRecallBuffer));
			MH_EnableHook(reinterpret_cast<void*>(hookAdrress));
			initialized = true;

		}
	}
	return O_CTimer__Update();

}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReasonForCall, LPVOID lpReserved)
{
	if (dwReasonForCall == DLL_PROCESS_ATTACH) {
		MH_Initialize();
		MH_CreateHook(reinterpret_cast<void*>(0x561B10), &CTimer__UpdateHook, reinterpret_cast<void**>(&O_CTimer__Update));
		MH_EnableHook(reinterpret_cast<void*>(0x561B10));
	}

	if (dwReasonForCall == DLL_PROCESS_DETACH)
		MH_Uninitialize();
	return TRUE;
}
