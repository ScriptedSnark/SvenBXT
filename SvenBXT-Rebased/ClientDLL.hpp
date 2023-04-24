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
typedef void (*_HUD_DrawTransparentTriangles)(void);
typedef void (*_SMR_StudioSetupBones)(void);

// HUD things
extern cvar_t* con_color;

extern cvar_t* bxt_hud;
extern cvar_t* bxt_hud_color;
extern cvar_t* bxt_hud_precision;
extern cvar_t* bxt_hud_speedometer;
extern cvar_t* bxt_hud_speedometer_offset;
extern cvar_t* bxt_hud_speedometer_anchor;
extern cvar_t* bxt_hud_jumpspeed;
extern cvar_t* bxt_hud_jumpspeed_anchor;
extern cvar_t* bxt_hud_jumpspeed_offset;
extern cvar_t* bxt_hud_viewangles;
extern cvar_t* bxt_hud_viewangles_offset;
extern cvar_t* bxt_hud_viewangles_anchor;
extern cvar_t* bxt_hud_origin;
extern cvar_t* bxt_hud_origin_offset;
extern cvar_t* bxt_hud_origin_anchor;
extern cvar_t* bxt_hud_timer;
extern cvar_t* bxt_hud_timer_offset;
extern cvar_t* bxt_hud_timer_anchor;

// OpenGL crosshair cvars
extern cvar_t* bxt_cross;
extern cvar_t* bxt_cross_color;
extern cvar_t* bxt_cross_alpha;
extern cvar_t* bxt_cross_thickness;
extern cvar_t* bxt_cross_size;
extern cvar_t* bxt_cross_gap;
extern cvar_t* bxt_cross_outline;
extern cvar_t* bxt_cross_circle_radius;
extern cvar_t* bxt_cross_dot_color;
extern cvar_t* bxt_cross_dot_size;
extern cvar_t* bxt_cross_top_line;
extern cvar_t* bxt_cross_bottom_line;
extern cvar_t* bxt_cross_left_line;
extern cvar_t* bxt_cross_right_line;

// Viewmodel cvars
extern cvar_t* bxt_viewmodel_bob_angled;

// Tri cvars
extern cvar_t* bxt_show_triggers;

#define FindbySymbol(func_name) \
	if ((ORIG_##func_name = reinterpret_cast<_##func_name>(GetProcAddress(reinterpret_cast<HMODULE>(clientDll), "" #func_name "")))) \
		pEngfuncs->Con_Printf("[client dll] Found " #func_name " at %p.\n", ORIG_##func_name); \
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
