#include "SvenBXT.hpp"
#include "conutils.hpp"

static FILE* logfile = nullptr;

/* Dev Messages */
static void Log(const char* prefix, const char* msg)
{
    if (logfile)
    {
        auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        auto ltime = std::localtime(&time);
        fprintf(logfile, "[%02d:%02d:%02d] [%s] %s", ltime->tm_hour, ltime->tm_min, ltime->tm_sec, prefix, msg);
        fflush(logfile);
    }
}

void PrintMessage(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    char temp[1024];
    vsprintf_s(temp, format, args);

    va_end(args);

    ConUtils::Log(temp);
    Log("Msg", temp);
}

void PrintDevMessage(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    char temp[1024];
    vsprintf_s(temp, format, args);

    va_end(args);

    ConUtils::Log(temp, FOREGROUND_RED | FOREGROUND_GREEN);
    Log("DevMsg", temp);
}

void PrintWarning(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    char temp[1024];
    vsprintf_s(temp, format, args);

    va_end(args);

    ConUtils::Log(temp, FOREGROUND_RED | FOREGROUND_INTENSITY);
    Log("Warning", temp);
}

void PrintDevWarning(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    char temp[1024];
    vsprintf_s(temp, format, args);

    va_end(args);

    ConUtils::Log(temp, FOREGROUND_RED);
    Log("DevWarning", temp);
}
/* END */

typedef void(__cdecl* _Con_Printf) (const char* fmt, ...);
_Con_Printf ORIG_Con_Printf;

typedef void(_cdecl* _Cbuf_AddText)(const char* text);
_Cbuf_AddText ORIG_Cbuf_AddText;

void Cbuf_AddText(const char* text) {
    ORIG_Cbuf_AddText(text);
}

typedef void(__cdecl* _Cmd_AddMallocCommand) (const char* name, void(*func)(void), int flags);
_Cmd_AddMallocCommand ORIG_Cmd_AddMallocCommand;

void __cdecl Cmd_AddMallocCommand(const char* name, void(*func)(void), int flags) {
    ORIG_Cmd_AddMallocCommand(name, func, flags);
}

template<typename Callable> union storage {
	storage() {}
	std::decay_t<Callable> callable;
};

template<int, typename Callable, typename Ret, typename... Args> auto funptr_(Callable&& c, Ret(*)(Args...)) {
	static bool used = false;
	static storage<Callable> s;
	using type = decltype(s.callable);

	if (used)
		s.callable.~type();
	new (&s.callable) type(std::forward<Callable>(c));
	used = true;

	return [](Args... args) -> Ret {
		return Ret(s.callable(std::forward<Args>(args)...));
	};
}

template<typename Fun, int N = 0, typename Callable> Fun* funptr(Callable&& c) {
	return funptr_<N>(std::forward<Callable>(c), (Fun*)nullptr);
}

void SvenBXT::Main() {
    ConUtils::Init();
    svenbxt->AddBXTStuff();
}

void SvenBXT::AddBXTStuff() {
    void* handle;
    void* base;
    size_t size;

    if (MemUtils::GetModuleInfo(L"hw.dll", &handle, &base, &size)) {
        ORIG_Cbuf_AddText = reinterpret_cast<_Cbuf_AddText>(MemUtils::GetSymbolAddress(handle, "Cbuf_AddText"));
        ORIG_Cmd_AddMallocCommand = reinterpret_cast<_Cmd_AddMallocCommand>(MemUtils::GetSymbolAddress(handle, "Cmd_AddMallocCommand"));
        ORIG_Con_Printf = reinterpret_cast<_Con_Printf>(MemUtils::GetSymbolAddress(handle, "ORIG_Con_Printf"));
        auto utils = Utils::Utils(handle, base, size);
        auto fCbuf_AddText = utils.FindAsync(ORIG_Cbuf_AddText, patterns::engine::Cbuf_AddText);
        auto fCmd_AddMallocCommand = utils.FindAsync(ORIG_Cmd_AddMallocCommand, patterns::engine::Cmd_AddMallocCommand);
        auto pattern = fCbuf_AddText.get();
        auto pattern2 = fCmd_AddMallocCommand.get();
        if (ORIG_Cbuf_AddText) {
            PrintDevMessage("[hw dll] Found Cbuf_AddText at %p.\n", ORIG_Cbuf_AddText);
        }
        if (ORIG_Cmd_AddMallocCommand) {
            PrintDevMessage("[hw dll] Found Cmd_AddMallocCommand at %p.\n", ORIG_Cmd_AddMallocCommand);
        }
        void* Host_AutoSave_f;
        auto fHost_AutoSave_f = utils.FindAsync(
            Host_AutoSave_f,
            patterns::engine::Host_AutoSave_f,
            [&](auto pattern) {
                auto f = reinterpret_cast<uintptr_t>(Host_AutoSave_f);
                switch (pattern - patterns::engine::Host_AutoSave_f.cbegin())
                {
                case 0:
                    ORIG_Con_Printf = reinterpret_cast<_Con_Printf>(
                        *reinterpret_cast<ptrdiff_t*>(f + 33)
                        + (f + 37)
                    );
                    break;
                case 1:
                    ORIG_Con_Printf = reinterpret_cast<_Con_Printf>(
                        *reinterpret_cast<ptrdiff_t*>(f + 28)
                        + (f + 32)
                    );
                    break;
                }
            });

        /* COMMANDS START */

        Cmd_AddMallocCommand("bxt_version", funptr<void()>([&]() {
            ORIG_Con_Printf(const_cast<char*>("Build time: %s - %s\n"), __DATE__, __TIME__);
        }), 2);

        /* COMMANDS END */
    } else {
        PrintWarning("[hw dll] Could not get module info of hw.dll.\n");
    }
}

DWORD WINAPI DllMain(_In_ void* _DllHandle, _In_ unsigned long _Reason, _In_opt_ void** unused) {
    if (_Reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(reinterpret_cast<HMODULE>(_DllHandle));
        SvenBXT::Main();

        return 1;
    }

    return 0;
}