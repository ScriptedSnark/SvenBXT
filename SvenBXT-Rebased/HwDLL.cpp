#include "HwDLL.hpp"

#pragma warning(disable: 4996)

_Cbuf_AddText ORIG_Cbuf_AddText;
_Cbuf_InsertText ORIG_Cbuf_InsertText;
_SPR_Set ORIG_SPR_Set;
_Draw_FillRGBA ORIG_Draw_FillRGBA;

//-----------------------------------------------------------------------------
// Purpose: set custom HUD color for sprites
//-----------------------------------------------------------------------------
void __cdecl HOOKED_SPR_Set(HSPRITE_HL sprite, int r, int g, int b) {
	int bxt_r = 0, bxt_g = 0, bxt_b = 0;
	sscanf(pEngfuncs->pfnGetCvarString("bxt_hud_color"), "%d %d %d", &bxt_r, &bxt_g, &bxt_b);

	ORIG_SPR_Set(sprite, bxt_r, bxt_g, bxt_b);
}

//-----------------------------------------------------------------------------
// Purpose: set custom HUD color for Draw_FillRGBA
//-----------------------------------------------------------------------------
void __cdecl HOOKED_Draw_FillRGBA(int x, int y, int w, int h, int r, int g, int b, int a) {
	int bxt_r = 0, bxt_g = 0, bxt_b = 0;
	sscanf(pEngfuncs->pfnGetCvarString("bxt_hud_color"), "%d %d %d", &bxt_r, &bxt_g, &bxt_b);

	ORIG_Draw_FillRGBA(x, y, w, h, bxt_r, bxt_g, bxt_b, a);
}

void Cmd_BXT_Append() {
	if (pEngfuncs->Cmd_Argc() > 1) {
		ORIG_Cbuf_AddText(pEngfuncs->Cmd_Argv(1));
		ORIG_Cbuf_AddText("\n");
	} else {
		pEngfuncs->Con_Printf("Usage: bxt_append <command>\n Appends command to the end of the command buffer, similar to how special appends _special.\n");
	}
}

void Cmd_Special() {
	ORIG_Cbuf_AddText("_special\n");
}

void Cmd_Multiwait() {
	if (pEngfuncs->Cmd_Argc() > 1) {
		int num = atoi(pEngfuncs->Cmd_Argv(1));
		std::ostringstream ss;
		if (num > 1)
			ss << "wait\nw " << num - 1 << '\n';
		else if (num == 1)
			ss << "wait\n";
		else
			return;

		ORIG_Cbuf_InsertText(ss.str().c_str());
	} else {
		ORIG_Cbuf_InsertText("wait\n");
	}
}

void CEngineHooks::Initialize() {
	void* hwDll = GetModuleHandleA("hw");
	if (hwDll) {
		Find(Cbuf_AddText);
		Find(Cbuf_InsertText);
		Hook(SPR_Set);
		Hook(Draw_FillRGBA);
		MH_EnableHook(MH_ALL_HOOKS);
		pEngfuncs->pfnAddCommand("bxt_append", Cmd_BXT_Append);
		pEngfuncs->pfnAddCommand("w", Cmd_Multiwait);
		pEngfuncs->pfnAddCommand("special", Cmd_Special);
	} else {
		printf("Couldn't find hw.dll module!\n");
	}
}