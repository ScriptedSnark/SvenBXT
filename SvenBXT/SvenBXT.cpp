#include "SvenBXT.hpp"
#include "cmd_wrapper.hpp"
#include "cvars.hpp"
#include "pengfuncs.h"
#include "hud_custom.hpp"
#include "MinHook/MinHook.h"
#include <sstream>
#include "sven_sdk.h"
#include "udis86/udis86.h"

cvar_t** cvar_vars;
ptrdiff_t offEdict;
cl_clientfunc_t* g_pClientFuncs = NULL;
edict_t** sv_player = nullptr;
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

typedef void(_cdecl* _Cbuf_InsertText)(const char* text);
_Cbuf_InsertText ORIG_Cbuf_InsertText;

void Cbuf_AddText(const char* text) {
    ORIG_Cbuf_AddText(text);
}

void Cbuf_InsertText(const char* text) {
    ORIG_Cbuf_InsertText(text);
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

typedef void (*HUD_InitFn)(void);
typedef int (*HUD_VidInitFn)(void);
typedef int (*HUD_RedrawFn)(float, int);
typedef void (*V_CalcRefdefFn)(struct ref_params_s*);

HUD_VidInitFn HUD_VidInit_Original = NULL;
HUD_InitFn HUD_Init_Original = NULL;
HUD_RedrawFn HUD_Redraw_Original = NULL;
V_CalcRefdefFn V_CalcRefdef_Original = NULL;

void HUD_Init_Hooked(void)
{
    CustomHud::Init();
    HUD_Init_Original();
}

int HUD_VidInit_Hooked(void)
{
    CustomHud::VidInit();
    return HUD_VidInit_Original();
}

int HUD_Redraw_Hooked(float time, int intermission)
{
    CustomHud::Draw(time);
    return HUD_Redraw_Original(time, intermission);
}

void V_CalcRefdef_Hooked(struct ref_params_s* pparams)
{
    CustomHud::V_CalcRefdef(pparams);
    return V_CalcRefdef_Original(pparams);
}

/* CLIENT - END */

void SvenBXT::Main() {
    ConUtils::Init();
    svenbxt->AddHWStuff();
    svenbxt->AddCLStuff();
}

void Cmd_Multiwait_f()
{
    if (ORIG_Cmd_Argc() == 1) {
        ORIG_Cbuf_InsertText("wait\n");
        return;
    }

    std::ostringstream ss;
    int num = std::atoi(ORIG_Cmd_Argv(1));
    if (num > 1)
        ss << "wait\nw " << num - 1 << '\n';
    else if (num == 1)
        ss << "wait\n";
    else
        return;

    ORIG_Cbuf_InsertText(ss.str().c_str());
}

void SvenBXT::AddHWStuff() {
    void* handle;
    void* base;
    size_t size;

    if (MemUtils::GetModuleInfo(L"hw.dll", &handle, &base, &size)) {
        ORIG_Cbuf_AddText = reinterpret_cast<_Cbuf_AddText>(MemUtils::GetSymbolAddress(handle, "Cbuf_AddText"));
        ORIG_Cbuf_InsertText = reinterpret_cast<_Cbuf_InsertText>(MemUtils::GetSymbolAddress(handle, "Cbuf_InsertText"));
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
        auto fCbuf_InsertText = utils.FindAsync(ORIG_Cbuf_InsertText, patterns::engine::Cbuf_InsertText);

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
        auto pattern5 = fCbuf_InsertText.get();

        if (ORIG_Cbuf_AddText) {
            PrintDevMessage("[hw dll] Found Cbuf_AddText at %p.\n", ORIG_Cbuf_AddText);
        }
        if (ORIG_Cbuf_InsertText) {
            PrintDevMessage("[hw dll] Found Cbuf_InsertText at %p.\n", ORIG_Cbuf_InsertText);
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

        static constexpr auto p = PATTERN("FF E0 68 ?? ?? ?? ?? FF 35 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 08 A3");
        void* pClientFuncs = (void*)MemUtils::find_pattern(base, size, p);
        g_pClientFuncs = *reinterpret_cast<cl_clientfunc_t**>((BYTE*)pClientFuncs + 0x16);

        // g_pClientFuncs
        ud_t instruction;

        ud_init(&instruction);
        ud_set_mode(&instruction, 32);
        ud_set_input_buffer(&instruction, (const uint8_t*)((BYTE*)pClientFuncs + 0x15), 15); // 15 - longest x86 instruction

        ud_disassemble(&instruction);

        if (instruction.mnemonic == UD_Imov && instruction.operand[0].type == UD_OP_MEM && instruction.operand[1].type == UD_OP_REG && instruction.operand[1].base == UD_R_EAX)
        {
            g_pClientFuncs = reinterpret_cast<cl_clientfunc_t*>(instruction.operand[0].lval.udword);
        }

        if (g_pClientFuncs)
        {
            PrintDevMessage("[hw dll] Found ClientFuncs at %p.\n", g_pClientFuncs);

            CVars::con_color.Assign(ORIG_Cvar_FindVar("con_color"));

            RegisterCVar(CVars::bxt_cross);
            RegisterCVar(CVars::bxt_cross_color);
            RegisterCVar(CVars::bxt_cross_alpha);
            RegisterCVar(CVars::bxt_cross_thickness);
            RegisterCVar(CVars::bxt_cross_size);
            RegisterCVar(CVars::bxt_cross_gap);
            RegisterCVar(CVars::bxt_cross_outline);
            RegisterCVar(CVars::bxt_cross_circle_radius);
            RegisterCVar(CVars::bxt_cross_dot_color);
            RegisterCVar(CVars::bxt_cross_dot_size);
            RegisterCVar(CVars::bxt_cross_top_line);
            RegisterCVar(CVars::bxt_cross_bottom_line);
            RegisterCVar(CVars::bxt_cross_left_line);
            RegisterCVar(CVars::bxt_cross_right_line);

            RegisterCVar(CVars::bxt_hud);
            RegisterCVar(CVars::bxt_hud_color);
            RegisterCVar(CVars::bxt_hud_precision);
            RegisterCVar(CVars::bxt_hud_speedometer);
            RegisterCVar(CVars::bxt_hud_speedometer_offset);
            RegisterCVar(CVars::bxt_hud_speedometer_anchor);
            RegisterCVar(CVars::bxt_hud_jumpspeed);
            RegisterCVar(CVars::bxt_hud_jumpspeed_anchor);
            RegisterCVar(CVars::bxt_hud_jumpspeed_offset);
            RegisterCVar(CVars::bxt_hud_viewangles);
            RegisterCVar(CVars::bxt_hud_viewangles_offset);
            RegisterCVar(CVars::bxt_hud_viewangles_anchor);
            RegisterCVar(CVars::bxt_hud_origin);
            RegisterCVar(CVars::bxt_hud_origin_offset);
            RegisterCVar(CVars::bxt_hud_origin_anchor);

            HUD_Init_Original = g_pClientFuncs->HUD_Init;
            g_pClientFuncs->HUD_Init = HUD_Init_Hooked;

            HUD_VidInit_Original = g_pClientFuncs->HUD_VidInit;
            g_pClientFuncs->HUD_VidInit = HUD_VidInit_Hooked;

            HUD_Redraw_Original = g_pClientFuncs->HUD_Redraw;
            g_pClientFuncs->HUD_Redraw = HUD_Redraw_Hooked;

            V_CalcRefdef_Original = g_pClientFuncs->V_CalcRefdef;
            g_pClientFuncs->V_CalcRefdef = V_CalcRefdef_Hooked;
        }

        /* COMMANDS START - STRUCTS */

        struct Cmd_BXT_Append
        {
            USAGE("Usage: bxt_append <command>\n Appends command to the end of the command buffer, similar to how special appends _special.\n");

            static void handler(const char* command)
            {
                /*
                ORIG_Cbuf_AddText(command);
                ORIG_Cbuf_AddText("\n");
                */
            }
        };

        struct Cmd_BXT_Version
        {
            USAGE("Compilation time: " __TIMESTAMP__);

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
        //Cmd_AddMallocCommand("w", Cmd_Multiwait_f, 2);

        /* COMMANDS END */
    } else {
        PrintWarning("[hw dll] Could not get module info of hw.dll.\n");
    }
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
    }
    else {
        printf("[client dll] Could not get module info of client.dll.\n");
    }
}
DWORD WINAPI DllMain(_In_ void* _DllHandle, _In_ unsigned long _Reason, _In_opt_ void** unused) {
    if (_Reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(reinterpret_cast<HMODULE>(_DllHandle));
        MH_Initialize();
        SvenBXT::Main();

        return 1;
    }

    return 0;
}