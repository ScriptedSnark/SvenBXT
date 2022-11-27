/**
 * Copyright - xWhitey ~ ScriptedSnark, 2022.
 * HwDLL.hpp - Engine hooks & different engine mods
 */

#ifdef HWDLL_HPP_RECURSE_GUARD
#error Recursive header files inclusion detected in HwDLL.hpp
#else //HWDLL_HPP_RECURSE_GUARD

#define HWDLL_HPP_RECURSE_GUARD

#ifndef HWDLL_HPP_GUARD
#define HWDLL_HPP_GUARD
#pragma once

#include "includes.hpp"

#define Find(future_name) \
            { \
                FindAsync(hwDll, patterns::engine::future_name, ORIG_##future_name, [&](std::string ver) { \
                    pEngfuncs->Con_Printf("[hw dll] Found " #future_name " at %p (using the %s pattern).\n", ORIG_##future_name, ver.c_str()); \
                }, [&]() { pEngfuncs->Con_Printf("[hw dll] Couldn't find " #future_name ".\n"); }); \
            }

#define Hook(future_name) \
            { \
                FindAsync(hwDll, patterns::engine::future_name, ORIG_##future_name, [&](std::string ver) { \
                    pEngfuncs->Con_Printf("[hw dll] Found " #future_name " at %p (using the %s pattern).\n", ORIG_##future_name, ver.c_str()); \
                    void* p##future_name = (void*)ORIG_##future_name; \
                    MH_CreateHook(p##future_name, HOOKED_##future_name, reinterpret_cast<void**>(&ORIG_##future_name));\
                }, [&]() { pEngfuncs->Con_Printf("[hw dll] Couldn't find " #future_name ".\n"); }); \
            }

typedef void(__cdecl* _Cbuf_AddText)(const char*);
extern _Cbuf_AddText ORIG_Cbuf_AddText;
typedef void(__cdecl* _Cbuf_InsertText)(const char*);
extern _Cbuf_InsertText ORIG_Cbuf_InsertText;
typedef void(__cdecl* _SPR_Set)(HSPRITE_HL, int, int, int);
extern _SPR_Set ORIG_SPR_Set;
typedef void(__cdecl* _Draw_FillRGBA)(int, int, int, int, int, int, int, int);
extern _Draw_FillRGBA ORIG_Draw_FillRGBA;

typedef struct CEngineHooks {
    static void Initialize();
} CEngineHooks;

#endif //HWDLL_HPP_GUARD

#undef HWDLL_HPP_RECURSE_GUARD
#endif //HWDLL_HPP_RECURSE_GUARD
