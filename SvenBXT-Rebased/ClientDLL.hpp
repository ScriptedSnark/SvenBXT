/**
 * Copyright - xWhitey ~ ScriptedSnark ~ YaLTeR, 2022.
 * ClientDLL.hpp - Various client.dll mods & hooks
 */

#ifdef CLIENTDLL_HPP_RECURSE_GUARD
#error Recursive header files inclusion detected in ClientDLL.hpp
#else //CLIENTDLL_HPP_RECURSE_GUARD

#define CLIENTDLL_HPP_RECURSE_GUARD

#ifndef CLIENTDLL_HPP_GUARD
#define CLIENTDLL_HPP_GUARD
#pragma once

#include "includes.hpp"

typedef void (*_HUD_Init)(void);
typedef int (*_HUD_VidInit)(void);
typedef int (*_HUD_Redraw)(float, int);
typedef void (*_HUD_Frame)(double);
typedef void (*_CL_CreateMove)(float, struct usercmd_s*, int);
typedef void (*_V_CalcRefdef)(struct ref_params_s*);
typedef void (*_HUD_PlayerMove)(struct playermove_s*, qboolean);
typedef void (*_IN_ActivateMouse)();
typedef void (*_IN_DeactivateMouse)();
typedef void (*_SMR_StudioSetupBones)(void);

#define FindbySymbol(func_name) \
	if ((ORIG_##func_name = reinterpret_cast<_##func_name>(GetProcAddress(reinterpret_cast<HMODULE>(clientDll), "" #func_name "")))) \
		pEngfuncs->Con_Printf("[client dll] Found " #func_name " at %p.\n", ORIG_HUD_VidInit); \
	else \
		pEngfuncs->Con_Printf("[client dll] Could not find " #func_name ".\n"); \

#define CreateHook(func_name) \
	status = MH_CreateHook(ORIG_##func_name, HOOKED_##func_name, reinterpret_cast<void**>(&ORIG_##func_name)); \
	if (status != MH_OK) { \
		pEngfuncs->Con_Printf("[client dll] Couldn't create hook for " #func_name ": %s\n", MH_StatusToString(status)); \
	}

typedef struct CClientHooks {
	static void Initialize();
} CClientHooks;

#endif //CLIENTDLL_HPP_GUARD

#undef CLIENTDLL_HPP_RECURSE_GUARD
#endif //CLIENTDLL_HPP_RECURSE_GUARD
