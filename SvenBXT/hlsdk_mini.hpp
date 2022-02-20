#pragma once

/* CVAR FLAGS - START */
#define FCVAR_ARCHIVE        (1<<0)    // set to cause it to be saved to vars.rc
#define FCVAR_USERINFO        (1<<1)    // changes the client's info string
#define FCVAR_SERVER        (1<<2)    // notifies players when changed
#define	FCVAR_EXTDLL        (1<<3)    // defined by external DLL
#define FCVAR_CLIENTDLL     (1<<4)  // defined by the client dll
#define FCVAR_PROTECTED     (1<<5)  // It's a server cvar, but we don't send the data since it's a password, etc.  Sends 1 if it's not bland/zero, 0 otherwise as value
#define FCVAR_SPONLY        (1<<6)  // This cvar cannot be changed by clients connected to a multiplayer server.
#define FCVAR_PRINTABLEONLY (1<<7)  // This cvar's string cannot contain unprintable characters ( e.g., used for player name etc ).
#define FCVAR_UNLOGGED        (1<<8)  // If this is a FCVAR_SERVER, don't log changes to the log file / console if we are creating a log
#define FCVAR_NOEXTRAWHITEPACE    (1<<9)  // strip trailing/leading white space from this cvar
/* CVAR FLAGS - END */
#define vec3_t Vector
typedef float vec_t;				// needed before including progdefs.h
//=========================================================
// 2DVector - used for many pathfinding and many other 
// operations that are treated as planar rather than 3d.
//=========================================================
class Vector2D
{
public:
	inline Vector2D(void) { }
	inline Vector2D(float X, float Y) { x = X; y = Y; }
	inline Vector2D operator+(const Vector2D& v)	const { return Vector2D(x + v.x, y + v.y); }
	inline Vector2D operator-(const Vector2D& v)	const { return Vector2D(x - v.x, y - v.y); }
	inline Vector2D operator*(float fl)				const { return Vector2D(x * fl, y * fl); }
	inline Vector2D operator/(float fl)				const { return Vector2D(x / fl, y / fl); }

	inline float Length(void)						const { return sqrt(x * x + y * y); }

	inline Vector2D Normalize(void) const
	{
		Vector2D vec2;

		float flLen = Length();
		if (flLen == 0)
		{
			return Vector2D(0, 0);
		}
		else
		{
			flLen = 1 / flLen;
			return Vector2D(x * flLen, y * flLen);
		}
	}

	vec_t	x = 0, y = 0;
};

inline float DotProduct(const Vector2D& a, const Vector2D& b) { return(a.x * b.x + a.y * b.y); }
inline Vector2D operator*(float fl, const Vector2D& v) { return v * fl; }
//=========================================================
// 3D Vector
//=========================================================
class Vector						// same data-layout as engine's vec3_t,
{								//		which is a vec_t[3]
public:
	// Construction/destruction
	inline Vector(void) { }
	inline Vector(float X, float Y, float Z) { x = X; y = Y; z = Z; }
	//inline Vector(double X, double Y, double Z)		{ x = (float)X; y = (float)Y; z = (float)Z;	}
	//inline Vector(int X, int Y, int Z)				{ x = (float)X; y = (float)Y; z = (float)Z;	}
	inline Vector(const Vector& v) { x = v.x; y = v.y; z = v.z; }
	inline Vector(const float rgfl[3]) { x = rgfl[0]; y = rgfl[1]; z = rgfl[2]; }
	inline Vector& operator=(const Vector& v) { x = v.x; y = v.y; z = v.z; return *this; }

	// Operators
	inline Vector operator-(void) const { return Vector(-x, -y, -z); }
	inline int operator==(const Vector& v) const { return x == v.x && y == v.y && z == v.z; }
	inline int operator!=(const Vector& v) const { return !(*this == v); }
	inline Vector operator+(const Vector& v) const { return Vector(x + v.x, y + v.y, z + v.z); }
	inline Vector operator-(const Vector& v) const { return Vector(x - v.x, y - v.y, z - v.z); }
	inline Vector operator*(float fl) const { return Vector(x * fl, y * fl, z * fl); }
	inline Vector operator/(float fl) const { return Vector(x / fl, y / fl, z / fl); }
	inline Vector& operator+=(const Vector& v) { x += v.x; y += v.y; z += v.z; return *this; }
	inline Vector& operator*=(float fl) { x *= fl; y *= fl; z *= fl; return *this; }

	// Methods
	inline void CopyToArray(float* rgfl) const { rgfl[0] = x, rgfl[1] = y, rgfl[2] = z; }
	inline float Length(void) const { return sqrt(x * x + y * y + z * z); }
	operator float* () { return &x; } // Vectors will now automatically convert to float * when needed
	operator const float* () const { return &x; } // Vectors will now automatically convert to float * when needed
	inline Vector Normalize(void) const
	{
		float flLen = Length();
		if (flLen == 0) return Vector(0, 0, 1); // ????
		flLen = 1 / flLen;
		return Vector(x * flLen, y * flLen, z * flLen);
	}

	inline Vector2D Make2D(void) const
	{
		Vector2D	Vec2;

		Vec2.x = x;
		Vec2.y = y;

		return Vec2;
	}
	inline float Length2D(void) const { return sqrt(x * x + y * y); }

	// Members
	vec_t x = 0, y = 0, z = 0;
};
inline Vector operator*(float fl, const Vector& v) { return v * fl; }
inline float DotProduct(const Vector& a, const Vector& b) { return(a.x * b.x + a.y * b.y + a.z * b.z); }
inline Vector CrossProduct(const Vector& a, const Vector& b) { return Vector(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); }



#ifndef __cplusplus
typedef enum { false, true }	qboolean;
#else 
typedef int qboolean;
#endif

// FUNC
typedef int(*pfnUserMsgHook)	(const char* pszName, int iSize, void* pbuf);

typedef struct rect_s
{
	int				left, right, top, bottom;
} wrect_t;

typedef struct cvar_s
{
	char* name;
	char* string;
	int        flags;
	float    value;
	struct cvar_s* next;
} cvar_t;

typedef struct SCREENINFO_s
{
	int		iSize;
	int		iWidth;
	int		iHeight;
	int		iFlags;
	int		iCharHeight;
	short	charWidths[256];
} SCREENINFO;

typedef struct client_data_s
{
	// fields that cannot be modified  (ie. have no effect if changed)
	vec3_t origin;

	// fields that can be changed by the cldll
	vec3_t viewangles;
	int		iWeaponBits;
	float	fov;	// field of view
} client_data_t;

typedef int HSPRITE_HL;	// handle to a graphic

typedef struct client_sprite_s
{
	char szName[64];
	char szSprite[64];
	int hspr;
	int iRes;
	wrect_t rc;
} client_sprite_t;

typedef struct client_textmessage_s
{
	int		effect;
	std::byte	r1, g1, b1, a1;		// 2 colors for effects
	std::byte	r2, g2, b2, a2;
	float	x;
	float	y;
	float	fadein;
	float	fadeout;
	float	holdtime;
	float	fxtime;
	const char* pName;
	const char* pMessage;
} client_textmessage_t;

typedef struct hud_player_info_s
{
	char* name;
	short ping;
	std::byte thisplayer;  // TRUE if this is the calling player

  // stuff that's unused at the moment,  but should be done
	std::byte spectator;
	std::byte packetloss;

	char* model;
	short topcolor;
	short bottomcolor;

} hud_player_info_t;

typedef struct cl_enginefuncs_s
{
	// sprite handlers
	HSPRITE_HL					(*pfnSPR_Load)			(const char* szPicName);
	int							(*pfnSPR_Frames)			(HSPRITE_HL hPic);
	int							(*pfnSPR_Height)			(HSPRITE_HL hPic, int frame);
	int							(*pfnSPR_Width)			(HSPRITE_HL hPic, int frame);
	void						(*pfnSPR_Set)				(HSPRITE_HL hPic, int r, int g, int b);
	void						(*pfnSPR_Draw)			(int frame, int x, int y, const wrect_t* prc);
	void						(*pfnSPR_DrawHoles)		(int frame, int x, int y, const wrect_t* prc);
	void						(*pfnSPR_DrawAdditive)	(int frame, int x, int y, const wrect_t* prc);
	void						(*pfnSPR_EnableScissor)	(int x, int y, int width, int height);
	void						(*pfnSPR_DisableScissor)	(void);
	client_sprite_t* (*pfnSPR_GetList)			(char* psz, int* piCount);

	// screen handlers
	void						(*pfnFillRGBA)			(int x, int y, int width, int height, int r, int g, int b, int a);
	int							(*pfnGetScreenInfo) 		(SCREENINFO* pscrinfo);
	void						(*pfnSetCrosshair)		(HSPRITE_HL hspr, wrect_t rc, int r, int g, int b);

	// cvar handlers
	struct cvar_s* (*pfnRegisterVariable)	(char* szName, char* szValue, int flags);
	float						(*pfnGetCvarFloat)		(char* szName);
	char* (*pfnGetCvarString)		(char* szName);

	// command handlers
	int							(*pfnAddCommand)			(char* cmd_name, void (*function)(void));
	int							(*pfnHookUserMsg)			(char* szMsgName, pfnUserMsgHook pfn);
	int							(*pfnServerCmd)			(char* szCmdString);
	int							(*pfnClientCmd)			(char* szCmdString);

	void						(*pfnGetPlayerInfo)		(int ent_num, hud_player_info_t* pinfo);

	// sound handlers
	void						(*pfnPlaySoundByName)		(char* szSound, float volume);
	void						(*pfnPlaySoundByIndex)	(int iSound, float volume);

	// vector helpers
	void						(*pfnAngleVectors)		(const float* vecAngles, float* forward, float* right, float* up);

	// text message system
	client_textmessage_t* (*pfnTextMessageGet)		(const char* pName);
	int							(*pfnDrawCharacter)		(int x, int y, int number, int r, int g, int b);
	int							(*pfnDrawConsoleString)	(int x, int y, char* string);
	void						(*pfnDrawSetTextColor)	(float r, float g, float b);
	void						(*pfnDrawConsoleStringLen)(const char* string, int* length, int* height);

	void						(*pfnConsolePrint)		(const char* string);
	void						(*pfnCenterPrint)			(const char* string);


	// Added for user input processing
	int							(*GetWindowCenterX)		(void);
	int							(*GetWindowCenterY)		(void);
	void						(*GetViewAngles)			(float*);
	void						(*SetViewAngles)			(float*);
	int							(*GetMaxClients)			(void);
	void						(*Cvar_SetValue)			(char* cvar, float value);

	int       					(*Cmd_Argc)					(void);
	char* (*Cmd_Argv)				(int arg);
	void						(*Con_Printf)				(char* fmt, ...);
	void						(*Con_DPrintf)			(char* fmt, ...);
	void						(*Con_NPrintf)			(int pos, char* fmt, ...);
	void						(*Con_NXPrintf)			(struct con_nprint_s* info, char* fmt, ...);

	const char* (*PhysInfo_ValueForKey)	(const char* key);
	const char* (*ServerInfo_ValueForKey)(const char* key);
	float						(*GetClientMaxspeed)		(void);
	int							(*CheckParm)				(char* parm, char** ppnext);
	void						(*Key_Event)				(int key, int down);
	void						(*GetMousePosition)		(int* mx, int* my);
	int							(*IsNoClipping)			(void);

	struct cl_entity_s* (*GetLocalPlayer)		(void);
	struct cl_entity_s* (*GetViewModel)			(void);
	struct cl_entity_s* (*GetEntityByIndex)		(int idx);

	float						(*GetClientTime)			(void);
	void						(*V_CalcShake)			(void);
	void						(*V_ApplyShake)			(float* origin, float* angles, float factor);

	int							(*PM_PointContents)		(float* point, int* truecontents);
	int							(*PM_WaterEntity)			(float* p);
	struct pmtrace_s* (*PM_TraceLine)			(float* start, float* end, int flags, int usehull, int ignore_pe);

	struct model_s* (*CL_LoadModel)			(const char* modelname, int* index);
	int							(*CL_CreateVisibleEntity)	(int type, struct cl_entity_s* ent);

	const struct model_s* (*GetSpritePointer)		(HSPRITE_HL hSprite);
	void						(*pfnPlaySoundByNameAtLocation)	(char* szSound, float volume, float* origin);

	unsigned short				(*pfnPrecacheEvent)		(int type, const char* psz);
	void						(*pfnPlaybackEvent)		(int flags, const struct edict_s* pInvoker, unsigned short eventindex, float delay, float* origin, float* angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2);
	void						(*pfnWeaponAnim)			(int iAnim, int body);
	float						(*pfnRandomFloat)			(float flLow, float flHigh);
	long						(*pfnRandomLong)			(long lLow, long lHigh);
	void						(*pfnHookEvent)			(char* name, void (*pfnEvent)(struct event_args_s* args));
	int							(*Con_IsVisible)			();
	const char* (*pfnGetGameDirectory)	(void);
	struct cvar_s* (*pfnGetCvarPointer)		(const char* szName);
	const char* (*Key_LookupBinding)		(const char* pBinding);
	const char* (*pfnGetLevelName)		(void);
	void						(*pfnGetScreenFade)		(struct screenfade_s* fade);
	void						(*pfnSetScreenFade)		(struct screenfade_s* fade);
	void* (*VGui_GetPanel)         ();
	void                         (*VGui_ViewportPaintBackground) (int extents[4]);

	std::byte* (*COM_LoadFile)				(char* path, int usehunk, int* pLength);
	char* (*COM_ParseFile)			(char* data, char* token);
	void						(*COM_FreeFile)				(void* buffer);

	struct triangleapi_s* pTriAPI;
	struct efx_api_s* pEfxAPI;
	struct event_api_s* pEventAPI;
	struct demo_api_s* pDemoAPI;
	struct net_api_s* pNetAPI;
	struct IVoiceTweak_s* pVoiceTweak;

	// returns 1 if the client is a spectator only (connected to a proxy), 0 otherwise or 2 if in dev_overview mode
	int							(*IsSpectateOnly) (void);
	struct model_s* (*LoadMapSprite)			(const char* filename);

	// file search functions
	void						(*COM_AddAppDirectoryToSearchPath) (const char* pszBaseDir, const char* appName);
	int							(*COM_ExpandFilename)				 (const char* fileName, char* nameOutBuffer, int nameOutBufferSize);

	// User info
	// playerNum is in the range (1, MaxClients)
	// returns NULL if player doesn't exit
	// returns "" if no value is set
	const char* (*PlayerInfo_ValueForKey)(int playerNum, const char* key);
	void						(*PlayerInfo_SetValueForKey)(const char* key, const char* value);

	// Gets a unique ID for the specified player. This is the same even if you see the player on a different server.
	// iPlayer is an entity index, so client 0 would use iPlayer=1.
	// Returns false if there is no player on the server in the specified slot.
	qboolean(*GetPlayerUniqueID)(int iPlayer, char playerID[16]);

	// TrackerID access
	int							(*GetTrackerIDForPlayer)(int playerSlot);
	int							(*GetPlayerForTrackerID)(int trackerID);

	// Same as pfnServerCmd, but the message goes in the unreliable stream so it can't clog the net stream
	// (but it might not get there).
	int							(*pfnServerCmdUnreliable)(char* szCmdString);

	void						(*pfnGetMousePos)(struct tagPOINT* ppt);
	void						(*pfnSetMousePos)(int x, int y);
	void						(*pfnSetMouseEnable)(qboolean fEnable);
} cl_enginefunc_t;
