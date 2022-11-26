#include "ClientDLL.hpp"
#include "HwDLL.hpp"

void Main() {
	AllocConsole();
	FILE* in, *out;
	freopen_s(&in, "conin$", "r", stdin);
	freopen_s(&out, "conout$", "w+", stdout);
	MH_STATUS status = MH_Initialize();
	if (status != MH_OK) {
		printf("Couldn't initialize MinHook: %s\n", MH_StatusToString(status));
	}
	CClientHooks::Initialize();
	CEngineHooks::Initialize(); 
}

DWORD WINAPI DllMain(_In_ void* _DllHandle, _In_ unsigned long _Reason, _In_opt_ void* _Reserved) {
	if (_Reason == DLL_PROCESS_ATTACH) {
		CreateThread(NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(Main), NULL, NULL, NULL);

		return 1;
	}

	return 0;
}