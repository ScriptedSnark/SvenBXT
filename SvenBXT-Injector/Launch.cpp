#define WIN32_LEAN_AND_MEAN
#include <algorithm>
#include <cstdio>
#include <cwchar>
#include <cwctype>
#include <iostream>
#include <string>
#include <tuple>

#include <fcntl.h>
#include <io.h>
#include <windows.h>
#include <TlHelp32.h> // Needs to be after windows.h.

const wchar_t EVENT_NAME[] = L"BunnymodXT-Injector";

struct HandleHolder
{
	HandleHolder(HANDLE handle, HANDLE invalidValue) : _handle(handle), _invalidValue(invalidValue) {};
	HandleHolder() = delete;
	HandleHolder(HandleHolder const&) = delete;
	HandleHolder(HandleHolder&&) = delete;
	void operator=(HandleHolder const&) = delete;
	~HandleHolder()
	{
		if (_handle != _invalidValue)
			CloseHandle(_handle);
	}

	auto get()
	{
		return _handle;
	}
private:
	HANDLE _handle;
	HANDLE _invalidValue;
};

struct VirtualAddressHolder
{
	VirtualAddressHolder(HANDLE process, LPVOID address) : _process(process), _address(address) {};
	VirtualAddressHolder() = delete;
	VirtualAddressHolder(VirtualAddressHolder const&) = delete;
	VirtualAddressHolder(VirtualAddressHolder&&) = delete;
	void operator=(VirtualAddressHolder const&) = delete;
	~VirtualAddressHolder()
	{
		if (_address)
			VirtualFreeEx(_process, _address, 0, MEM_RELEASE);
	}

	auto get()
	{
		return _address;
	}
private:
	HANDLE _process;
	LPVOID _address;
};

auto ContainsSpace(wchar_t* str)
{
	if (!str)
		return false;

	for (; *str; ++str)
		if (std::iswspace(*str))
			return true;

	return false;
}

auto LastSlashPos(std::wstring const& fileName)
{
	auto slash = fileName.rfind('\\');
	if (slash == std::wstring::npos)
		slash = fileName.rfind('/');
	return slash;
}

auto GetErrorMessage()
{
	auto messageBuffer = LPWSTR{ nullptr };
	auto size = FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPWSTR>(&messageBuffer),
		0,
		NULL);
	if (!size)
		return std::wstring{};

	auto message = std::wstring{ messageBuffer, size };
	LocalFree(messageBuffer);

	return message;
}

auto ParseArgs(int argc, wchar_t* argv[])
{
	// No arguments:
	// assume we want to inject into a running svencoop.exe.
	if (argc == 1)
		return std::make_tuple(std::wstring{ L"svencoop.exe" }, std::wstring{}, std::wstring{}, false);

	// Two arguments and the first one is -processname:
	// assume we want to inject into a running process with the name specified in the second argument.
	if (argc == 3 && !std::wcsncmp(argv[1], L"-processname", 13))
		return std::make_tuple(std::wstring{ argv[2] }, std::wstring{}, std::wstring{}, false);

	// None of the above conditions were met:
	// assume we want to launch a process.
	auto cmd_line = std::wstring{};
	for (int i = 1; i < argc; ++i) {
		if (i != 1)
			cmd_line += L' ';
		auto space = ContainsSpace(argv[i]);
		if (space)
			cmd_line += L'"';
		cmd_line += argv[i];
		if (space)
			cmd_line += L'"';
	}
	if (cmd_line.size() > 32767)
		cmd_line.resize(32767); // Max length of lpCommandLine - one character for \0.
	//std::wcout << L"cmd_line: " << cmd_line << L'\n';

	auto have_work_dir = true;
	auto work_dir = std::wstring{ argv[1] };
	auto slash = LastSlashPos(work_dir);
	if (slash != std::wstring::npos)
		work_dir = work_dir.substr(0, slash);
	else
		have_work_dir = false;
	//if (have_work_dir)
	//	std::wcout << L"work_dir: " << work_dir;
	//else
	//	std::wcout << L"No work_dir.";
	//std::wcout << L'\n';

	return std::make_tuple(std::wstring{}, cmd_line, work_dir, have_work_dir);
}

DWORD GetProcessID(std::wstring const& processName)
{
	auto entry = PROCESSENTRY32W{};
	entry.dwSize = sizeof(entry);

	HandleHolder snapshot(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0), INVALID_HANDLE_VALUE);
	if (snapshot.get() == INVALID_HANDLE_VALUE) {
		std::wcerr << L"CreateToolhelp32Snapshot error: " << GetErrorMessage();
		return 0;
	}

	if (Process32FirstW(snapshot.get(), &entry) == TRUE)
		do
			if (processName.compare(entry.szExeFile) == 0)
				return entry.th32ProcessID;
	while (Process32NextW(snapshot.get(), &entry) == TRUE);
			else
				std::wcerr << L"Process32First error: " << GetErrorMessage();

	return 0;
}

auto GetSvenBXTFileName()
{
	wchar_t exe_path[MAX_PATH];
	if (GetModuleFileNameW(NULL, exe_path, MAX_PATH) != 0) {
		// Assume that the path did fit.
		auto fileName = std::wstring{ exe_path };
		auto slash = LastSlashPos(fileName);
		if (slash != std::wstring::npos)
			fileName = fileName.substr(0, slash);
		fileName += L"\\SvenBXT-Rebased.dll";
		return fileName;
	}
	else
		std::wcerr << L"Error getting the injector file name: " << GetErrorMessage();

	return std::wstring{};
}

auto DoInjection(HANDLE targetProcess)
{
	auto dll_file_name = GetSvenBXTFileName();
	if (dll_file_name.empty()) {
		std::getchar();
		return false; // The function outputs an error message by itself.
	}

	VirtualAddressHolder target_addr(targetProcess, VirtualAllocEx(targetProcess, NULL, (dll_file_name.size() + 1) * sizeof(wchar_t), (MEM_COMMIT | MEM_RESERVE), PAGE_READWRITE));
	if (target_addr.get() == NULL) {
		std::wcerr << L"Error allocating memory in the target process: " << GetErrorMessage();
		std::getchar();
		return false;
	}

	if (WriteProcessMemory(targetProcess, target_addr.get(), dll_file_name.c_str(), (dll_file_name.size() + 1) * sizeof(wchar_t), NULL) == 0) {
		std::wcerr << L"Error writing the DLL filename to the target process: " << GetErrorMessage();
		std::getchar();
		return false;
	}

	HandleHolder load_thread(CreateRemoteThread(targetProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryW, target_addr.get(), 0, NULL), NULL);
	if (load_thread.get() == NULL) {
		std::wcerr << L"Error spawning a LoadLibrary thread in the target process: " << GetErrorMessage();
		std::getchar();
		return false;
	}

	WaitForSingleObject(load_thread.get(), INFINITE);
	DWORD exit_code;
	if (GetExitCodeThread(load_thread.get(), &exit_code) == 0) {
		std::wcerr << L"Error getting the LoadLibrary return value: " << GetErrorMessage();
		std::getchar();
		return false;
	}

	if (exit_code == NULL) {
		std::wcerr << L"LoadLibrary failed.\n";
		std::getchar();
		return false;
	}

	std::wcout << "Success! DLL module address: " << std::hex << exit_code << L'\n';
	return true;
}

int main(int argc, wchar_t* argv[])
{
	_setmode(_fileno(stdout), _O_U16TEXT); // To make the Unicode characters print correctly.

	auto process = HANDLE{}; // Our target process.
	auto main_thread = HANDLE{}; // If we are creating our own process, we need to resume this thread.
	auto need_to_resume = false;

	auto parsed_args = ParseArgs(argc, argv);
	auto process_name = std::get<0>(parsed_args);
	auto cmd_line = std::get<1>(parsed_args);
	auto work_dir = std::get<2>(parsed_args);
	auto have_work_dir = std::get<3>(parsed_args);

	std::wcout << L"Injecting SvenBXT-Rebased...\n";

	if (process_name.empty()) {
		// Make a process ourselves.
		wchar_t cmd_line_lpwstr[32768] = {};
		std::copy(cmd_line.begin(), cmd_line.end(), cmd_line_lpwstr);

		auto si = STARTUPINFOW{};
		si.cb = sizeof(si);
		auto pi = PROCESS_INFORMATION{};
		if (!CreateProcessW(NULL, cmd_line_lpwstr, NULL, NULL, FALSE, CREATE_SUSPENDED | DETACHED_PROCESS, NULL, (have_work_dir) ? work_dir.c_str() : NULL, &si, &pi)) {
			std::wcerr << L"Error creating process: " << GetErrorMessage();
			std::getchar();
			return 1;
		}

		std::wcout << L"Successfully created a process. Process ID: " << pi.dwProcessId << L"; thread ID: " << pi.dwThreadId << L'\n';
		process = pi.hProcess;
		main_thread = pi.hThread;
		need_to_resume = true;
	}
	else {
		// Inject into a running process.
		auto process_id = GetProcessID(process_name);
		if (!process_id) {
			std::wcerr << L"The process " << process_name << L" is not running or an error has occurred.\n";
			std::getchar();
			return 1;
		}

		process = OpenProcess((PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ), FALSE, process_id);
		if (process == NULL) {
			std::wcerr << L"Error opening process: " << GetErrorMessage();
			std::getchar();
			return 1;
		}

		std::wcout << L"Successfully opened the process.\n";
	}

	// Resume the main thread when this fires.
	auto resume_event = HANDLE{ NULL };
	if (need_to_resume) {
		resume_event = CreateEventW(NULL, FALSE, FALSE, EVENT_NAME);
		if (resume_event == NULL)
			std::wcerr << L"Error creating an event: " << GetErrorMessage();
	}

	if (DoInjection(process)) {
		if (resume_event != NULL)
			std::wcout << L"Waiting for the DLL to finish loading to resume the process.\n";
	}
	else
		resume_event = NULL;

	if (need_to_resume) {
		if (resume_event != NULL)
			if (WaitForSingleObject(resume_event, INFINITE) == WAIT_FAILED)
				std::wcerr << L"Error waiting for the event: " << GetErrorMessage();

		ResumeThread(main_thread);
	}

	return 0;
}