#include "SvenBXT.hpp"
#include "conutils.hpp"
#include "cmd_wrapper.hpp"

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

typedef int(__cdecl* _Cmd_Argc) ();
_Cmd_Argc ORIG_Cmd_Argc;
typedef char* (__cdecl* _Cmd_Args) ();
_Cmd_Args ORIG_Cmd_Args;
typedef char* (__cdecl* _Cmd_Argv) (unsigned n);
_Cmd_Argv ORIG_Cmd_Argv;


struct CmdFuncs
{
    inline static void AddCommand(const char* name, void (*handler)())
    {
        ORIG_Cmd_AddMallocCommand(name, handler, 2);  // 2 - Cmd_AddGameCommand.
    }

    inline static const char* Argv(int i)
    {
        return ORIG_Cmd_Argv(i);
    }

    inline static int Argc()
    {
        return ORIG_Cmd_Argc();
    }

    inline static void UsagePrint(const char* s)
    {
        return ORIG_Con_Printf("%s\n", s);
    }
};

void SvenBXT::Main() {
    ConUtils::Init();
    svenbxt->AddHWStuff();
}

void SvenBXT::AddHWStuff() {
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

        void* Host_Tell_f;
        auto fHost_Tell_f = utils.FindAsync(
            Host_Tell_f,
            patterns::engine::Host_Tell_f,
            [&](auto pattern) {
                uintptr_t offCmd_Argc, offCmd_Args, offCmd_Argv;
                switch (pattern - patterns::engine::Host_Tell_f.cbegin())
                {
                default:
                case 0: // SteamPipe.
                    offCmd_Argc = 28;
                    offCmd_Args = 42;
                    offCmd_Argv = 145;
                    break;
                case 1: // NGHL.
                    offCmd_Argc = 24;
                    offCmd_Args = 38;
                    offCmd_Argv = 143;
                    break;
                case 2: // 4554.
                    offCmd_Argc = 25;
                    offCmd_Args = 39;
                    offCmd_Argv = 144;
                    break;
                case 4: // Sven-8832
                    offCmd_Argc = 44;
                    offCmd_Args = 59;
                    offCmd_Argv = 163;
                    break;
                }

                auto f = reinterpret_cast<uintptr_t>(Host_Tell_f);
                ORIG_Cmd_Argc = reinterpret_cast<_Cmd_Argc>(
                    *reinterpret_cast<uintptr_t*>(f + offCmd_Argc)
                    + (f + offCmd_Argc + 4)
                    );
                ORIG_Cmd_Args = reinterpret_cast<_Cmd_Args>(
                    *reinterpret_cast<uintptr_t*>(f + offCmd_Args)
                    + (f + offCmd_Args + 4)
                    );
                ORIG_Cmd_Argv = reinterpret_cast<_Cmd_Argv>(
                    *reinterpret_cast<uintptr_t*>(f + offCmd_Argv)
                    + (f + offCmd_Argv + 4)
                    );
            });

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

        /* COMMANDS START - STRUCTS */

        struct Cmd_BXT_Append
        {
            USAGE("Usage: bxt_append <command>\n Appends command to the end of the command buffer, similar to how special appends _special.\n");

            static void handler(const char* command)
            {
                ORIG_Cbuf_AddText(command);
                ORIG_Cbuf_AddText("\n");
            }
        };

        struct Cmd_BXT_Version
        {
            USAGE("BXT version: " __TIMESTAMP__);

            static void handler(const char* command)
            {
                ORIG_Con_Printf("Compilation time: %s\n", __TIMESTAMP__);
            }
        };

        /* COMMANDS START - CMDWRAPPER */

        using CmdWrapper::Handler;
        typedef CmdWrapper::CmdWrapper<CmdFuncs> wrapper;

        wrapper::Add<Cmd_BXT_Append, Handler<const char*>>("bxt_append");
        wrapper::Add<Cmd_BXT_Version, Handler<const char*>>("bxt_version");

        /* COMMANDS END */
    } else {
        PrintWarning("[hw dll] Could not get module info of hw.dll.\n");
    }
}

/*void SvenBXT::AddCLStuff() {
    void* handle;
    void* base;
    size_t size;

    if (MemUtils::GetModuleInfo(L"client.dll", &handle, &base, &size)) {

    }
    else {
        printf("[client dll] Could not get module info of client.dll.\n");
    }
}
*/

DWORD WINAPI DllMain(_In_ void* _DllHandle, _In_ unsigned long _Reason, _In_opt_ void** unused) {
    if (_Reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(reinterpret_cast<HMODULE>(_DllHandle));
        SvenBXT::Main();

        return 1;
    }

    return 0;
}