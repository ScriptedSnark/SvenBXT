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

typedef struct CClientHooks {
	static void Initialize();
} CClientHooks;

#endif //CLIENTDLL_HPP_GUARD

#undef CLIENTDLL_HPP_RECURSE_GUARD
#endif //CLIENTDLL_HPP_RECURSE_GUARD
