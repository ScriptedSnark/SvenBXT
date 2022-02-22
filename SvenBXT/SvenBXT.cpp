#include "SvenBXT.hpp"
#include "cmd_wrapper.hpp"
#include "cvars.hpp"
#include "pengfuncs.h"
#include "hud_custom.hpp"

cvar_t** cvar_vars;
ptrdiff_t offEdict;
struct client_t;
struct svs_t
{
    char unk[4];
    client_t* clients;
    int num_clients;
};
svs_t* svs;

/* END */
typedef void(__cdecl* _Con_Printf) (const char* fmt, ...);
_Con_Printf ORIG_Con_Printf;

typedef void(_cdecl* _Cbuf_AddText)(const char* text);
_Cbuf_AddText ORIG_Cbuf_AddText;

typedef void(__cdecl* _Cvar_RegisterVariable) (cvar_t* cvar);
_Cvar_RegisterVariable ORIG_Cvar_RegisterVariable;

typedef cvar_t* (__cdecl* _Cvar_FindVar) (const char* name);
_Cvar_FindVar ORIG_Cvar_FindVar;


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

/* CLIENT - START */

void RegisterCVar(CVarWrapper& cvar)
{
    if (!ORIG_Cvar_FindVar || !ORIG_Cvar_RegisterVariable)
        return;

    if (ORIG_Cvar_FindVar(cvar.GetPointer()->name))
        return;

    ORIG_Cvar_RegisterVariable(cvar.GetPointer());
    cvar.MarkAsStale();
}


typedef void(__cdecl* _HUD_Init)();
_HUD_Init ORIG_HUD_Init;

typedef void(__cdecl* _HUD_VidInit)();
_HUD_VidInit ORIG_HUD_VidInit;

typedef void(__cdecl* _HUD_Reset)();
_HUD_Reset ORIG_HUD_Reset;

typedef void(__cdecl* _HUD_Redraw)(float time, int intermission);
_HUD_Redraw ORIG_HUD_Redraw;

void __cdecl HOOKED_HUD_Init_Func() {
    CustomHud::Init();
}

void __cdecl HOOKED_HUD_Init() {
    HOOKED_HUD_Init_Func();
}

void __cdecl HOOKED_HUD_VidInit_Func() {
    CustomHud::VidInit();
}

void __cdecl HOOKED_HUD_VidInit() {
    HOOKED_HUD_VidInit_Func();
}

void __cdecl HOOKED_HUD_Reset_Func() {
    //ORIG_HUD_Reset();
    CustomHud::InitIfNecessary();
    CustomHud::VidInit();
}

void __cdecl HOOKED_HUD_Reset() {
    HOOKED_HUD_Reset_Func();
}

void __cdecl HOOKED_HUD_Redraw_Func(float time, int intermission) {
    //ORIG_HUD_Redraw(NULL, NULL);
    CustomHud::Draw();
}

void __cdecl HOOKED_HUD_Redraw(float time, int intermission) {
    HOOKED_HUD_Redraw_Func(time, intermission);
}

/* CLIENT - END */

void SvenBXT::Main() {
    ConUtils::Init();
    svenbxt->AddHWStuff();
    svenbxt->AddCLStuff();
}

void SvenBXT::AddHWStuff() {
    void* handle;
    void* base;
    size_t size;

    if (MemUtils::GetModuleInfo(L"hw.dll", &handle, &base, &size)) {
        ORIG_Cbuf_AddText = reinterpret_cast<_Cbuf_AddText>(MemUtils::GetSymbolAddress(handle, "Cbuf_AddText"));
        ORIG_Cmd_AddMallocCommand = reinterpret_cast<_Cmd_AddMallocCommand>(MemUtils::GetSymbolAddress(handle, "Cmd_AddMallocCommand"));
        ORIG_Con_Printf = reinterpret_cast<_Con_Printf>(MemUtils::GetSymbolAddress(handle, "ORIG_Con_Printf"));
        ORIG_Cvar_FindVar = reinterpret_cast<_Cvar_FindVar>(MemUtils::GetSymbolAddress(handle, "ORIG_Cvar_FindVar"));
        ORIG_Cvar_RegisterVariable = reinterpret_cast<_Cvar_RegisterVariable>(MemUtils::GetSymbolAddress(handle, "Cvar_RegisterVariable"));
        cvar_vars = reinterpret_cast<cvar_t**>(MemUtils::GetSymbolAddress(handle, "cvar_vars"));
        svs = reinterpret_cast<svs_t*>(MemUtils::GetSymbolAddress(handle, "svs"));

        auto utils = Utils::Utils(handle, base, size);
        auto fCbuf_AddText = utils.FindAsync(ORIG_Cbuf_AddText, patterns::engine::Cbuf_AddText);
        auto fCmd_AddMallocCommand = utils.FindAsync(ORIG_Cmd_AddMallocCommand, patterns::engine::Cmd_AddMallocCommand);
        auto fCvar_FindVar = utils.FindAsync(ORIG_Cvar_FindVar, patterns::engine::Cvar_FindVar);
        auto pattern = fCbuf_AddText.get();
        auto pattern2 = fCmd_AddMallocCommand.get();

        auto fCvar_RegisterVariable = utils.FindAsync(
            ORIG_Cvar_RegisterVariable,
            patterns::engine::Cvar_RegisterVariable,
            [&](auto pattern) {
                switch (pattern - patterns::engine::Cvar_RegisterVariable.cbegin())
                {
                case 0: // HL-SteamPipe
                    cvar_vars = reinterpret_cast<cvar_t**>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Cvar_RegisterVariable) + 124));
                    break;
                case 1: // HL-NGHL
                    cvar_vars = reinterpret_cast<cvar_t**>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Cvar_RegisterVariable) + 122));
                    break;
                case 2: // Sven-8832
                    cvar_vars = reinterpret_cast<cvar_t**>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Cvar_RegisterVariable) + 23));
                    break;
                }
            });

        auto pattern3 = fCvar_RegisterVariable.get();
        auto pattern4 = fCvar_FindVar.get();

        if (ORIG_Cbuf_AddText) {
            PrintDevMessage("[hw dll] Found Cbuf_AddText at %p.\n", ORIG_Cbuf_AddText);
        }
        if (ORIG_Cmd_AddMallocCommand) {
            PrintDevMessage("[hw dll] Found Cmd_AddMallocCommand at %p.\n", ORIG_Cmd_AddMallocCommand);
        }
        if (ORIG_Cvar_RegisterVariable) {
            PrintDevMessage("[hw dll] Found Cvar_RegisterVariable at %p (using the %s pattern).\n", ORIG_Cvar_RegisterVariable, pattern->name());
            PrintDevMessage("[hw dll] Found cvar_vars at %p.\n", cvar_vars);
        }
        else {
            PrintDevMessage("[hw dll] Could not find Cvar_RegisterVariable.\n");
        }

        if (ORIG_Cvar_FindVar) {
            PrintDevMessage("[hw dll] Found Cvar_FindVar at %p.\n", ORIG_Cvar_FindVar);
        }
        else
        {
            PrintDevWarning("[hw dll] Could not find Cvar_FindVar.\n");
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
                    svs = reinterpret_cast<svs_t*>(*reinterpret_cast<uintptr_t*>(f + 45) - 8);
                    offEdict = *reinterpret_cast<ptrdiff_t*>(f + 122);
                    break;
                case 1:
                    ORIG_Con_Printf = reinterpret_cast<_Con_Printf>(
                        *reinterpret_cast<ptrdiff_t*>(f + 28)
                        + (f + 32)
                    );
                    svs = reinterpret_cast<svs_t*>(*reinterpret_cast<uintptr_t*>(f + 40) - 8);
                    offEdict = *reinterpret_cast<ptrdiff_t*>(f + 118);
                    break;
                }
            });

        if (svs) {
            PrintDevMessage("[hw dll] Found svs at %p.\n", svs);
            offEdict = 0x4a84;
        }
        else
            PrintDevWarning("[hw dll] Could not find svs.\n");

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
            USAGE("Compilation time: " __TIMESTAMP__);

            static void handler(const char* command)
            {
                ORIG_Con_Printf("Compilation time: %s\n", __TIMESTAMP__);
                CustomHud::Draw();
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

bool TryGettingAccurateInfo(float origin[3], float velocity[3])
{
    if (!svs || svs->num_clients < 1)
        return false;

    edict_t* pl = *reinterpret_cast<edict_t**>(reinterpret_cast<uintptr_t>(svs->clients) + offEdict);
    origin[0] = pl->v.origin[0];
    origin[1] = pl->v.origin[1];
    origin[2] = pl->v.origin[2];
    velocity[0] = pl->v.velocity[0];
    velocity[1] = pl->v.velocity[1];
    velocity[2] = pl->v.velocity[2];
    return true;
}



void SvenBXT::AddCLStuff() {
    void* handle;
    void* base;
    size_t size;
    if (MemUtils::GetModuleInfo(L"client.dll", &handle, &base, &size)) {
        const std::wstring& moduleName = L"client.dll";
        auto utils = Utils::Utils(handle, base, size);
        /* gEngfuncs hook - START */
        pEngfuncs = reinterpret_cast<cl_enginefunc_t*>(MemUtils::GetSymbolAddress(handle, "gEngfuncs"));
        if (pEngfuncs) {
            PrintDevMessage("[client dll] pEngfuncs is %p.\n", pEngfuncs);
        }
        else {
            // In AG, this thing is the main function, so check that first.
            auto pInitialize = MemUtils::GetSymbolAddress(handle, "?Initialize_Body@@YAHPAUcl_enginefuncs_s@@H@Z");
            if (!pInitialize)
                pInitialize = MemUtils::GetSymbolAddress(handle, "Initialize");
            if (pInitialize)
            {
                PrintDevMessage("[client dll] Found Initialize at %p.\n", pInitialize);

                // In some cases Initialize contains just a jump to the real function (Residual Life).
                if (*reinterpret_cast<byte*>(pInitialize) == 0xE9) {
                    pInitialize = reinterpret_cast<void*>(
                        *reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(pInitialize) + 1)
                        + reinterpret_cast<uintptr_t>(pInitialize) + 5);
                    PrintDevMessage("Jump detected, found the real Initialize at %p.\n", pInitialize);
                }

                // Find "mov edi, offset dword; rep movsd" inside Initialize. The pointer to gEngfuncs is that dword.
                static constexpr auto p = PATTERN("BF ?? ?? ?? ?? F3 A5");
                auto addr = MemUtils::find_pattern(pInitialize, 40, p);
                if (!addr) {
                    static constexpr auto p = PATTERN("B9 ?? ?? ?? ?? 8B 54 24 10");
                    addr = MemUtils::find_pattern(pInitialize, 40, p);
                }
                if (!addr) {
                    // client.dll that comes with TWHL Tower 2
                    static constexpr auto p = PATTERN("B8 ?? ?? ?? ?? 8B F8 F3 A5");
                    addr = MemUtils::find_pattern(pInitialize, 40, p);
                }

                if (addr)
                {
                    pEngfuncs = *reinterpret_cast<cl_enginefunc_t**>(addr + 1);
                    PrintDevMessage("[client dll] pEngfuncs is %p.\n", pEngfuncs);
                }
                else
                {
                    PrintDevWarning("[client dll] Couldn't find the pattern in Initialize.\n");
                    PrintDevWarning("Custom HUD is not available.\n");
                    PrintDevWarning("Clientside logging is not available.\n");
                }
            }
            else
            {
                PrintDevWarning("[client dll] Couldn't get the address of Initialize.\n");
                PrintDevWarning("Custom HUD is not available.\n");
                PrintDevWarning("Clientside logging is not available.\n");
            }

            if (pEngfuncs)
            {
                const char* gamedir = pEngfuncs->pfnGetGameDirectory();
                PrintDevMessage("[client dll] Game directory is %s.\n", gamedir);
            }
        }
        /* gEngfuncs hook - END */

        /* HUD Functions hook - START */

        ORIG_HUD_Init = reinterpret_cast<_HUD_Init>(MemUtils::GetSymbolAddress(handle, "HUD_Init"));
        ORIG_HUD_VidInit = reinterpret_cast<_HUD_VidInit>(MemUtils::GetSymbolAddress(handle, "HUD_VidInit"));
        ORIG_HUD_Reset = reinterpret_cast<_HUD_Reset>(MemUtils::GetSymbolAddress(handle, "HUD_Reset"));
        ORIG_HUD_Redraw = (decltype(ORIG_HUD_Redraw))(MemUtils::GetSymbolAddress(handle, "HUD_Redraw"));

        MemUtils::AddSymbolLookupHook(handle, reinterpret_cast<void*>(ORIG_HUD_Init), reinterpret_cast<void*>(HOOKED_HUD_Init));
        MemUtils::AddSymbolLookupHook(handle, reinterpret_cast<void*>(ORIG_HUD_VidInit), reinterpret_cast<void*>(HOOKED_HUD_VidInit));
        MemUtils::AddSymbolLookupHook(handle, reinterpret_cast<void*>(ORIG_HUD_Reset), reinterpret_cast<void*>(HOOKED_HUD_Reset));
        MemUtils::AddSymbolLookupHook(handle, reinterpret_cast<void*>(ORIG_HUD_Redraw), reinterpret_cast<void*>(HOOKED_HUD_Redraw));
        MemUtils::Intercept(moduleName,
            ORIG_HUD_Init, HOOKED_HUD_Init,
            ORIG_HUD_VidInit, HOOKED_HUD_VidInit,
            ORIG_HUD_Reset, HOOKED_HUD_Reset,
            ORIG_HUD_Redraw, HOOKED_HUD_Redraw);

        if (ORIG_HUD_Init)
        {
            PrintDevMessage("[client dll] Found HUD_Init at %p.\n", ORIG_HUD_Init);
            RegisterCVar(CVars::bxt_hud);
            RegisterCVar(CVars::bxt_hud_speedometer);
            RegisterCVar(CVars::bxt_hud_speedometer_offset);
            RegisterCVar(CVars::bxt_hud_speedometer_anchor);

            HOOKED_HUD_Init_Func();
        }
        else
            PrintDevWarning("[client dll] Could not find HUD_Init.\n");

        if (ORIG_HUD_VidInit)
        {
            PrintDevMessage("[client dll] Found HUD_VidInit at %p.\n", ORIG_HUD_VidInit);
            HOOKED_HUD_VidInit_Func();
        }
        else
            PrintDevWarning("[client dll] Could not find HUD_VidInit.\n");

        if (ORIG_HUD_Reset)
        {
            PrintDevMessage("[client dll] Found HUD_Reset at %p.\n", ORIG_HUD_Reset);
            HOOKED_HUD_Reset_Func();
        }
        else
            PrintDevWarning("[client dll] Could not find HUD_Reset.\n");

        if (ORIG_HUD_Redraw)
        {
            HOOKED_HUD_Redraw_Func(NULL, NULL);
            CustomHud::Draw();
            PrintDevMessage("[client dll] Found HUD_Redraw at %p.\n", ORIG_HUD_Redraw);
        }
        else
            PrintDevWarning("[client dll] Could not find HUD_Redraw.\n");

        /* HUD Functions hook - END */
    }
    else {
        printf("[client dll] Could not get module info of client.dll.\n");
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