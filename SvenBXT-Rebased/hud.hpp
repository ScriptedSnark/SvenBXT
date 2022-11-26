/**
 * Copyright - xWhitey ~ ScriptedSnark, 2022.
 * CCustomHUD.hpp - Client-side custom HUD (speedometer &c)
 */

#ifdef CCUSTOMHUD_HPP_RECURSE_GUARD
#error Recursive header files inclusion detected in CCustomHUD.hpp
#else //CCUSTOMHUD_HPP_RECURSE_GUARD

#define CCUSTOMHUD_HPP_RECURSE_GUARD

#ifndef CCUSTOMHUD_HPP_GUARD
#define CCUSTOMHUD_HPP_GUARD
#pragma once

#include "includes.hpp"

namespace CustomHud
{
	typedef struct playerinfo {
		float origin[3];
		float velocity[3];
		float viewangles[3];
		int flags;
		int movetype;
		int waterlevel;
		float health;
	} playerinfo;
	
	void Init();
	void InitIfNecessary();
	void VidInit();
	void Draw(float flTime);
	void V_CalcRefdef(struct ref_params_s* pparams);
	const SCREENINFO& GetScreenInfo();
}

#endif //CCUSTOMHUD_HPP_GUARD

#undef CCUSTOMHUD_HPP_RECURSE_GUARD
#endif //CCUSTOMHUD_HPP_RECURSE_GUARD
