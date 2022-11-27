#include "ClientDLL.hpp"
#include "hud.hpp"

#pragma warning(disable: 26812)

cl_enginefunc_t* pEngfuncs;

_HUD_Init ORIG_HUD_Init = nullptr;
_HUD_VidInit ORIG_HUD_VidInit = nullptr;
_HUD_Redraw ORIG_HUD_Redraw = nullptr;
_HUD_Frame ORIG_HUD_Frame = nullptr;
_CL_CreateMove ORIG_CL_CreateMove = nullptr;
_V_CalcRefdef ORIG_V_CalcRefdef = nullptr;
_HUD_PlayerMove ORIG_HUD_PlayerMove = nullptr;
_IN_ActivateMouse ORIG_IN_ActivateMouse = nullptr;
_IN_DeactivateMouse ORIG_IN_DeactivateMouse = nullptr;
_HUD_DrawTransparentTriangles ORIG_HUD_DrawTransparentTriangles = nullptr;

#ifdef _DEBUG
cvar_t* bxt_autojump;
#endif

bool ducktap_down = false;

void IN_BXT_TAS_Ducktap_Down() {
	ducktap_down = true;
}

void IN_BXT_TAS_Ducktap_Up() {
	ducktap_down = false;
}

ref_params_s* params;
playermove_s* pmove;

void HOOKED_HUD_Init()
{
	CustomHud::Init();

	ORIG_HUD_Init();
}

int HOOKED_HUD_VidInit()
{
	CustomHud::VidInit();
	return ORIG_HUD_VidInit();
}

int HOOKED_HUD_Redraw(float time, int intermission)
{
	CustomHud::Draw(time);
	return ORIG_HUD_Redraw(time, intermission);
}

void HOOKED_V_CalcRefdef(struct ref_params_s* pparams)
{
	CustomHud::V_CalcRefdef(pparams);
	ORIG_V_CalcRefdef(pparams);
	params = pparams;
}

void HOOKED_HUD_PlayerMove(struct playermove_s* ppmove, qboolean server) {
	ORIG_HUD_PlayerMove(ppmove, server);
	pmove = ppmove;
}

void HOOKED_CL_CreateMove(float frametime, usercmd_s* cmd, int active) {
	ORIG_CL_CreateMove(frametime, cmd, active);
	
#ifdef _DEBUG
	if (!pmove) return;

	if (ducktap_down) {
		cmd->buttons |= IN_DUCK;

		static bool s_duck_was_down_last_frame = false;
		static bool should_release_duck;

		should_release_duck = (!(pmove->flags & FL_ONGROUND) && pmove->waterlevel < 1);

		if (s_duck_was_down_last_frame && (pmove->flags & FL_ONGROUND) && pmove->waterlevel < 1)
			should_release_duck = true;

		if (should_release_duck) {
			cmd->buttons &= ~IN_DUCK;
		}

		s_duck_was_down_last_frame = ((cmd->buttons & IN_DUCK) != 0);
	}
	else {
		if (bxt_autojump->value != 0.0 && (cmd->buttons & IN_JUMP) && !(pmove->flags & FL_ONGROUND)) {
			cmd->buttons &= ~IN_JUMP;
		}
	}
#endif
}

void HOOKED_HUD_DrawTransparentTriangles(void)
{
	ORIG_HUD_DrawTransparentTriangles();
}

extern float m_flTurnoff, hudTime;
extern bool DrawTimer;

void CustomTimer()
{
	if (!hudTime)
		return;

	if (2 == pEngfuncs->Cmd_Argc())
	{
		if (atof(pEngfuncs->Cmd_Argv(1)) > 300)
			return;

		m_flTurnoff = hudTime + atof(pEngfuncs->Cmd_Argv(1));
		DrawTimer = true;
	}
}

void CL_RegisterCmds() {
#ifdef _DEBUG
	pEngfuncs->pfnAddCommand("+bxt_tas_ducktap", IN_BXT_TAS_Ducktap_Down);
	pEngfuncs->pfnAddCommand("-bxt_tas_ducktap", IN_BXT_TAS_Ducktap_Up);
#endif
	pEngfuncs->pfnAddCommand("bxt_customtimer", CustomTimer);
}

void CL_RegisterCVars()
{
	if (!ORIG_HUD_Init)
		return;

#ifdef _DEBUG
	bxt_autojump = CVAR_CREATE("bxt_autojump", "1", 0);
#endif

	// HUD things
	CVAR_CREATE("bxt_hud", "1", 0);
	CVAR_CREATE("bxt_hud_color", "100 130 200", 0);
	CVAR_CREATE("bxt_hud_precision", "6", 0);
	CVAR_CREATE("bxt_hud_speedometer", "1", 0);
	CVAR_CREATE("bxt_hud_speedometer_offset", "", 0);
	CVAR_CREATE("bxt_hud_speedometer_anchor", "0.5 1", 0);
	CVAR_CREATE("bxt_hud_jumpspeed", "0", 0);
	CVAR_CREATE("bxt_hud_jumpspeed_anchor", "0.5 1", 0);
	CVAR_CREATE("bxt_hud_jumpspeed_offset", "", 0);
	CVAR_CREATE("bxt_hud_viewangles", "0", 0);
	CVAR_CREATE("bxt_hud_viewangles_offset", "", 0);
	CVAR_CREATE("bxt_hud_viewangles_anchor", "1 0", 0);
	CVAR_CREATE("bxt_hud_origin", "0", 0);
	CVAR_CREATE("bxt_hud_origin_offset", "", 0);
	CVAR_CREATE("bxt_hud_origin_anchor", "1 0", 0);

	// OpenGL crosshair cvars
	CVAR_CREATE("bxt_cross", "0", 0);
	CVAR_CREATE("bxt_cross_color", "", 0);
	CVAR_CREATE("bxt_cross_alpha", "255", 0);
	CVAR_CREATE("bxt_cross_thickness", "2", 0);
	CVAR_CREATE("bxt_cross_size", "10", 0);
	CVAR_CREATE("bxt_cross_gap", "3", 0);
	CVAR_CREATE("bxt_cross_outline", "0", 0);
	CVAR_CREATE("bxt_cross_circle_radius", "0", 0);
	CVAR_CREATE("bxt_cross_dot_color", "", 0);
	CVAR_CREATE("bxt_cross_dot_size", "0", 0);
	CVAR_CREATE("bxt_cross_top_line", "1", 0);
	CVAR_CREATE("bxt_cross_bottom_line", "1", 0);
	CVAR_CREATE("bxt_cross_left_line", "1", 0);
	CVAR_CREATE("bxt_cross_right_line", "1", 0);

	// Tri cvars
	CVAR_CREATE("bxt_show_triggers", "0", 0);
}

void CClientHooks::Initialize() {
	void* clientDll = GetModuleHandleA("client");
	if (clientDll) {
		pEngfuncs = reinterpret_cast<cl_enginefunc_t*>(GetProcAddress(reinterpret_cast<HMODULE>(clientDll), "gEngfuncs"));

		if (pEngfuncs)
			pEngfuncs->Con_Printf("[client dll] pEngfuncs is %p.\n", pEngfuncs);
		else {
			FARPROC pInitialize = GetProcAddress(reinterpret_cast<HMODULE>(clientDll), "Initialize");

			if (pInitialize) {
				printf("[client dll] Found Initialize at %p.\n", pInitialize);
				
				char* addr;
				FindAsync(clientDll, new CIntelligentPattern("BF ?? ?? ?? ?? F3 A5", "Unspecified"), addr, [&](std::string ver) {
						pEngfuncs = *reinterpret_cast<cl_enginefunc_t**>(addr + 1);
						printf("[client dll] pEngfuncs is %p.\n", pEngfuncs);
					}, [&]() {
						FindAsync(clientDll, new CIntelligentPattern("B9 ?? ?? ?? ?? 8B 54 24 10", "Unspecified"), addr,
							[&](std::string ver) {
								pEngfuncs = *reinterpret_cast<cl_enginefunc_t**>(addr + 1);
								printf("[client dll] pEngfuncs is %p.\n", pEngfuncs);
							}, [&]() { printf("[client dll] Couldn't get the address of Initialize.\nCustom HUD is unavailable.\nClient-side logging is unavailable.\n"); });
				});
				if (addr && pEngfuncs) {
					if ((ORIG_HUD_Init = reinterpret_cast<_HUD_Init>(GetProcAddress(reinterpret_cast<HMODULE>(clientDll), "HUD_Init"))))
						pEngfuncs->Con_Printf("[client dll] Found HUD_Init at %p.\n", ORIG_HUD_Init), CL_RegisterCVars(), CL_RegisterCmds();
					else
						pEngfuncs->Con_Printf("[client dll] Could not find HUD_Init.\n");

					FindbySymbol(HUD_VidInit);
					FindbySymbol(HUD_Redraw);
					FindbySymbol(HUD_Frame);
					FindbySymbol(HUD_PlayerMove);
					FindbySymbol(V_CalcRefdef);
					FindbySymbol(CL_CreateMove);
					FindbySymbol(IN_ActivateMouse);
					FindbySymbol(IN_DeactivateMouse);
					FindbySymbol(HUD_DrawTransparentTriangles);

					MH_STATUS status;
					CreateHook(HUD_Init);
					CreateHook(HUD_VidInit);
					CreateHook(HUD_Redraw);
					CreateHook(V_CalcRefdef);
					CreateHook(CL_CreateMove);
					CreateHook(HUD_PlayerMove);
					CreateHook(HUD_DrawTransparentTriangles);

					status = MH_EnableHook(MH_ALL_HOOKS);
					if (status != MH_OK) {
						pEngfuncs->Con_Printf("[client dll] Couldn't enable hooks: %s\n", MH_StatusToString(status));
					}

					if (status == MH_OK) {
						pEngfuncs->Con_Printf("[client dll] Hooked!\nSvenBXT build date: " __TIMESTAMP__ "\n");
					}
				}
			} else {
				printf("[client dll] Couldn't get the address of Initialize.\nCustom HUD is unavailable.\nClient-side logging is unavailable.\n");
			}
		}
	} else {
		printf("Couldn't find client.dll module!\n");
	}
}