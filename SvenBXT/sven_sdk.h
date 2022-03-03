// Include SDK

#ifndef SDK_H
#define SDK_H

#ifdef _WIN32
#pragma once
#endif

#include <Windows.h>

#include "hlsdk_mini.hpp"

typedef struct cl_clientfuncs_s
{
	int (*Initialize) (cl_enginefunc_t* pEnginefuncs, int iVersion);
	void (*HUD_Init) (void);
	int (*HUD_VidInit) (void);
	int (*HUD_Redraw) (float time, int intermission);
	int (*HUD_UpdateClientData) (client_data_t* pcldata, float flTime);
	void (*HUD_Reset) (void);
	void (*HUD_PlayerMove) (struct playermove_s* ppmove, int server);
	void (*HUD_PlayerMoveInit) (struct playermove_s* ppmove);
	char (*HUD_PlayerMoveTexture) (const char* name);
	void (*IN_ActivateMouse) (void);
	void (*IN_DeactivateMouse) (void);
	void (*IN_MouseEvent) (int mstate);
	void (*IN_ClearStates) (void);
	void (*IN_Accumulate) (void);
	void (*CL_CreateMove) (float frametime, struct usercmd_s* cmd, int active);
	int (*CL_IsThirdPerson) (void);
	void (*CL_CameraOffset) (float* ofs);
	struct kbutton_s* (*KB_Find) (const char* name);
	void (*CAM_Think) (void);
	void (*V_CalcRefdef) (struct ref_params_s* pparams);
	int (*HUD_AddEntity) (int type, struct cl_entity_s* ent, const char* modelname);
	void (*HUD_CreateEntities) (void);
	void (*HUD_DrawNormalTriangles) (void);
	void (*HUD_DrawTransparentTriangles) (void);
	void (*HUD_StudioEvent) (const struct mstudioevent_s* event, const struct cl_entity_s* entity);
	void (*HUD_PostRunCmd) (struct local_state_s* from, struct local_state_s* to, struct usercmd_s* cmd, int runfuncs, double time, unsigned int random_seed);
	void (*HUD_Shutdown) (void);
	void (*HUD_TxferLocalOverrides) (struct entity_state_s* state, const struct clientdata_s* client);
	void (*HUD_ProcessPlayerState) (struct entity_state_s* dst, const struct entity_state_s* src);
	void (*HUD_TxferPredictionData) (struct entity_state_s* ps, const struct entity_state_s* pps, struct clientdata_s* pcd, const struct clientdata_s* ppcd, struct weapon_data_s* wd, const struct weapon_data_s* pwd);
	void (*Demo_ReadBuffer) (int size, unsigned const char* buffer);
	int (*HUD_ConnectionlessPacket) (struct netadr_s* net_from, const char* args, const char* response_buffer, int* response_buffer_size);
	int (*HUD_GetHullBounds) (int hullnumber, float* mins, float* maxs);
	void (*HUD_Frame) (double time);
	int (*HUD_Key_Event) (int down, int keynum, const char* pszCurrentBinding);
	void (*HUD_TempEntUpdate) (double frametime, double client_time, double cl_gravity, struct tempent_s** ppTempEntFree, struct tempent_s** ppTempEntActive, int (*Callback_AddVisibleEntity)(struct cl_entity_s* pEntity), void (*Callback_TempEntPlaySound)(struct tempent_s* pTemp, float damp));
	struct cl_entity_s* (*HUD_GetUserEntity) (int index);
	void (*HUD_VoiceStatus) (int entindex, qboolean bTalking);
	void (*HUD_DirectorMessage) (unsigned char command, unsigned int firstObject, unsigned int secondObject, unsigned int flags);
	int (*HUD_GetStudioModelInterface) (int version, struct r_studio_interface_s** ppinterface, struct engine_studio_api_s* pstudio);
	void (*HUD_ChatInputPosition) (int* x, int* y);
	int (*HUD_GetPlayerTeam) (int iplayer);
	void (*ClientFactory) (void);
} cl_clientfunc_t;

#endif