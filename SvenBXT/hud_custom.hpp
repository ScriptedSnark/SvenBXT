#pragma once

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
	void Draw();
	void V_CalcRefdef(struct ref_params_s* pparams);
	const SCREENINFO& GetScreenInfo();
}