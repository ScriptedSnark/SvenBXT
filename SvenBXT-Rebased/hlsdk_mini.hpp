#pragma once

#ifndef M_PI
const double M_PI = 3.14159265358979323846;
#endif

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

// edict->flags
#define FL_FLY           (1 << 0) // Changes the SV_Movestep() behavior to not need to be on ground
#define FL_SWIM          (1 << 1) // Changes the SV_Movestep() behavior to not need to be on ground (but stay in water)
#define FL_CONVEYOR      (1 << 2)
#define FL_CLIENT        (1 << 3)
#define FL_INWATER       (1 << 4)
#define FL_MONSTER       (1 << 5)
#define FL_GODMODE       (1 << 6)
#define FL_NOTARGET      (1 << 7)
#define FL_SKIPLOCALHOST (1 << 8) // Don't send entity to local host, it's predicting this entity itself
#define FL_ONGROUND      (1 << 9) // At rest / on the ground
#define FL_PARTIALGROUND (1 << 10) // not all corners are valid
#define FL_WATERJUMP     (1 << 11) // player jumping out of water
#define FL_FROZEN        (1 << 12) // Player is frozen for 3rd person camera
#define FL_FAKECLIENT    (1 << 13) // JAC: fake client, simulated server side; don't send network messages to them
#define FL_DUCKING       (1 << 14) // Player flag -- Player is fully crouched
#define FL_FLOAT         (1 << 15) // Apply floating force to this entity when in water
#define FL_GRAPHED       (1 << 16) // worldgraph has this ent listed as something that blocks a connection

// UNDONE: Do we need these?
#define FL_IMMUNE_WATER (1 << 17)
#define FL_IMMUNE_SLIME (1 << 18)
#define FL_IMMUNE_LAVA  (1 << 19)

#define FL_PROXY        (1 << 20) // This is a spectator proxy
#define FL_ALWAYSTHINK  (1 << 21) // Brush model flag -- call think every frame regardless of nextthink - ltime (for constantly changing velocity/path)
#define FL_BASEVELOCITY (1 << 22) // Base velocity has been applied this frame (used to convert base velocity into momentum)
#define FL_MONSTERCLIP  (1 << 23) // Only collide in with monsters who have FL_MONSTERCLIP set
#define FL_ONTRAIN      (1 << 24) // Player is _controlling_ a train, so movement commands should be ignored on client during prediction.
#define FL_WORLDBRUSH   (1 << 25) // Not moveable/removeable brush entity (really part of the world, but represented as an entity for transparency or something)
#define FL_SPECTATOR    (1 << 26) // This client is a spectator, don't run touch functions, etc.
#define FL_CUSTOMENTITY (1 << 29) // This is a custom entity
#define FL_KILLME       (1 << 30) // This entity is marked for death -- This allows the engine to kill ents at the appropriate time
#define FL_DORMANT      (1 << 31) // Entity is dormant, no updates to client

// Goes into globalvars_t.trace_flags
#define FTRACE_SIMPLEBOX (1 << 0) // Traceline with a simple box

// walkmove modes
#define WALKMOVE_NORMAL    0 // normal walkmove
#define WALKMOVE_WORLDONLY 1 // doesn't hit ANY entities, no matter what the solid type
#define WALKMOVE_CHECKONLY 2 // move, but don't touch triggers

// edict->movetype values
#define MOVETYPE_NONE 0 // never moves
//#define	MOVETYPE_ANGLENOCLIP	1
//#define	MOVETYPE_ANGLECLIP		2
#define MOVETYPE_WALK          3 // Player only - moving on the ground
#define MOVETYPE_STEP          4 // gravity, special edge handling -- monsters use this
#define MOVETYPE_FLY           5 // No gravity, but still collides with stuff
#define MOVETYPE_TOSS          6 // gravity/collisions
#define MOVETYPE_PUSH          7 // no clip to world, push and crush
#define MOVETYPE_NOCLIP        8 // No gravity, no collisions, still do velocity/avelocity
#define MOVETYPE_FLYMISSILE    9 // extra size to monsters
#define MOVETYPE_BOUNCE        10 // Just like Toss, but reflect velocity when contacting surfaces
#define MOVETYPE_BOUNCEMISSILE 11 // bounce w/o gravity
#define MOVETYPE_FOLLOW        12 // track movement of aiment
#define MOVETYPE_PUSHSTEP      13 // BSP model that needs physics/world collisions (uses nearest hull for world collision)

// edict->solid values
// NOTE: Some movetypes will cause collisions independent of SOLID_NOT/SOLID_TRIGGER when the entity moves
// SOLID only effects OTHER entities colliding with this one when they move - UGH!
#define SOLID_NOT      0 // no interaction with other objects
#define SOLID_TRIGGER  1 // touch on edge, but not blocking
#define SOLID_BBOX     2 // touch on edge, block
#define SOLID_SLIDEBOX 3 // touch on edge, but not an onground
#define SOLID_BSP      4 // bsp clip, touch on edge, block

// edict->deadflag values
#define DEAD_NO          0 // alive
#define DEAD_DYING       1 // playing death animation or still falling off of a ledge waiting to hit ground
#define DEAD_DEAD        2 // dead. lying still.
#define DEAD_RESPAWNABLE 3
#define DEAD_DISCARDBODY 4

#define DAMAGE_NO  0
#define DAMAGE_YES 1
#define DAMAGE_AIM 2

// entity effects
#define EF_BRIGHTFIELD 1 // swirling cloud of particles
#define EF_MUZZLEFLASH 2 // single frame ELIGHT on entity attachment 0
#define EF_BRIGHTLIGHT 4 // DLIGHT centered at entity origin
#define EF_DIMLIGHT    8 // player flashlight
#define EF_INVLIGHT    16 // get lighting from ceiling
#define EF_NOINTERP    32 // don't interpolate the next frame
#define EF_LIGHT       64 // rocket flare glow sprite
#define EF_NODRAW      128 // don't draw entity
#define EF_NIGHTVISION 256 // player nightvision
#define EF_SNIPERLASER 512 // sniper laser effect
#define EF_FIBERCAMERA 1024 // fiber camera

// entity flags
#define EFLAG_SLERP 1 // do studio interpolation of this entity

//
// temp entity events
//
#define TE_BEAMPOINTS 0 // beam effect between two points
// coord coord coord (start position)
// coord coord coord (end position)
// short (sprite index)
// byte (starting frame)
// byte (frame rate in 0.1's)
// byte (life in 0.1's)
// byte (line width in 0.1's)
// byte (noise amplitude in 0.01's)
// byte,byte,byte (color)
// byte (brightness)
// byte (scroll speed in 0.1's)

#define TE_BEAMENTPOINT 1 // beam effect between point and entity
// short (start entity)
// coord coord coord (end position)
// short (sprite index)
// byte (starting frame)
// byte (frame rate in 0.1's)
// byte (life in 0.1's)
// byte (line width in 0.1's)
// byte (noise amplitude in 0.01's)
// byte,byte,byte (color)
// byte (brightness)
// byte (scroll speed in 0.1's)

#define TE_GUNSHOT 2 // particle effect plus ricochet sound
// coord coord coord (position)

#define TE_EXPLOSION 3 // additive sprite, 2 dynamic lights, flickering particles, explosion sound, move vertically 8 pps
// coord coord coord (position)
// short (sprite index)
// byte (scale in 0.1's)
// byte (framerate)
// byte (flags)
//
// The Explosion effect has some flags to control performance/aesthetic features:
#define TE_EXPLFLAG_NONE        0 // all flags clear makes default Half-Life explosion
#define TE_EXPLFLAG_NOADDITIVE  1 // sprite will be drawn opaque (ensure that the sprite you send is a non-additive sprite)
#define TE_EXPLFLAG_NODLIGHTS   2 // do not render dynamic lights
#define TE_EXPLFLAG_NOSOUND     4 // do not play client explosion sound
#define TE_EXPLFLAG_NOPARTICLES 8 // do not draw particles

#define TE_TAREXPLOSION 4 // Quake1 "tarbaby" explosion with sound
// coord coord coord (position)

#define TE_SMOKE 5 // alphablend sprite, move vertically 30 pps
// coord coord coord (position)
// short (sprite index)
// byte (scale in 0.1's)
// byte (framerate)

#define TE_TRACER 6 // tracer effect from point to point
// coord, coord, coord (start)
// coord, coord, coord (end)

#define TE_LIGHTNING 7 // TE_BEAMPOINTS with simplified parameters
// coord, coord, coord (start)
// coord, coord, coord (end)
// byte (life in 0.1's)
// byte (width in 0.1's)
// byte (amplitude in 0.01's)
// short (sprite model index)

#define TE_BEAMENTS 8
// short (start entity)
// short (end entity)
// short (sprite index)
// byte (starting frame)
// byte (frame rate in 0.1's)
// byte (life in 0.1's)
// byte (line width in 0.1's)
// byte (noise amplitude in 0.01's)
// byte,byte,byte (color)
// byte (brightness)
// byte (scroll speed in 0.1's)

#define TE_SPARKS 9 // 8 random tracers with gravity, ricochet sprite
// coord coord coord (position)

#define TE_LAVASPLASH 10 // Quake1 lava splash
// coord coord coord (position)

#define TE_TELEPORT 11 // Quake1 teleport splash
// coord coord coord (position)

#define TE_EXPLOSION2 12 // Quake1 colormaped (base palette) particle explosion with sound
// coord coord coord (position)
// byte (starting color)
// byte (num colors)

#define TE_BSPDECAL 13 // Decal from the .BSP file
// coord, coord, coord (x,y,z), decal position (center of texture in world)
// short (texture index of precached decal texture name)
// short (entity index)
// [optional - only included if previous short is non-zero (not the world)] short (index of model of above entity)

#define TE_IMPLOSION 14 // tracers moving toward a point
// coord, coord, coord (position)
// byte (radius)
// byte (count)
// byte (life in 0.1's)

#define TE_SPRITETRAIL 15 // line of moving glow sprites with gravity, fadeout, and collisions
// coord, coord, coord (start)
// coord, coord, coord (end)
// short (sprite index)
// byte (count)
// byte (life in 0.1's)
// byte (scale in 0.1's)
// byte (velocity along vector in 10's)
// byte (randomness of velocity in 10's)

#define TE_BEAM 16 // obsolete

#define TE_SPRITE 17 // additive sprite, plays 1 cycle
// coord, coord, coord (position)
// short (sprite index)
// byte (scale in 0.1's)
// byte (brightness)

#define TE_BEAMSPRITE 18 // A beam with a sprite at the end
// coord, coord, coord (start position)
// coord, coord, coord (end position)
// short (beam sprite index)
// short (end sprite index)

#define TE_BEAMTORUS 19 // screen aligned beam ring, expands to max radius over lifetime
// coord coord coord (center position)
// coord coord coord (axis and radius)
// short (sprite index)
// byte (starting frame)
// byte (frame rate in 0.1's)
// byte (life in 0.1's)
// byte (line width in 0.1's)
// byte (noise amplitude in 0.01's)
// byte,byte,byte (color)
// byte (brightness)
// byte (scroll speed in 0.1's)

#define TE_BEAMDISK 20 // disk that expands to max radius over lifetime
// coord coord coord (center position)
// coord coord coord (axis and radius)
// short (sprite index)
// byte (starting frame)
// byte (frame rate in 0.1's)
// byte (life in 0.1's)
// byte (line width in 0.1's)
// byte (noise amplitude in 0.01's)
// byte,byte,byte (color)
// byte (brightness)
// byte (scroll speed in 0.1's)

#define TE_BEAMCYLINDER 21 // cylinder that expands to max radius over lifetime
// coord coord coord (center position)
// coord coord coord (axis and radius)
// short (sprite index)
// byte (starting frame)
// byte (frame rate in 0.1's)
// byte (life in 0.1's)
// byte (line width in 0.1's)
// byte (noise amplitude in 0.01's)
// byte,byte,byte (color)
// byte (brightness)
// byte (scroll speed in 0.1's)

#define TE_BEAMFOLLOW 22 // create a line of decaying beam segments until entity stops moving
// short (entity:attachment to follow)
// short (sprite index)
// byte (life in 0.1's)
// byte (line width in 0.1's)
// byte,byte,byte (color)
// byte (brightness)

#define TE_GLOWSPRITE 23
// coord, coord, coord (pos) short (model index) byte (scale / 10)

#define TE_BEAMRING 24 // connect a beam ring to two entities
// short (start entity)
// short (end entity)
// short (sprite index)
// byte (starting frame)
// byte (frame rate in 0.1's)
// byte (life in 0.1's)
// byte (line width in 0.1's)
// byte (noise amplitude in 0.01's)
// byte,byte,byte (color)
// byte (brightness)
// byte (scroll speed in 0.1's)

#define TE_STREAK_SPLASH 25 // oriented shower of tracers
// coord coord coord (start position)
// coord coord coord (direction vector)
// byte (color)
// short (count)
// short (base speed)
// short (ramdon velocity)

#define TE_BEAMHOSE 26 // obsolete

#define TE_DLIGHT 27 // dynamic light, effect world, minor entity effect
// coord, coord, coord (pos)
// byte (radius in 10's)
// byte byte byte (color)
// byte (brightness)
// byte (life in 10's)
// byte (decay rate in 10's)

#define TE_ELIGHT 28 // point entity light, no world effect
// short (entity:attachment to follow)
// coord coord coord (initial position)
// coord (radius)
// byte byte byte (color)
// byte (life in 0.1's)
// coord (decay rate)

#define TE_TEXTMESSAGE 29
// short 1.2.13 x (-1 = center)
// short 1.2.13 y (-1 = center)
// byte Effect 0 = fade in/fade out
// 1 is flickery credits
// 2 is write out (training room)

// 4 bytes r,g,b,a color1	(text color)
// 4 bytes r,g,b,a color2	(effect color)
// ushort 8.8 fadein time
// ushort 8.8  fadeout time
// ushort 8.8 hold time
// optional ushort 8.8 fxtime	(time the highlight lags behing the leading text in effect 2)
// string text message		(512 chars max sz string)
#define TE_LINE 30
// coord, coord, coord		startpos
// coord, coord, coord		endpos
// short life in 0.1 s
// 3 bytes r, g, b

#define TE_BOX 31
// coord, coord, coord		boxmins
// coord, coord, coord		boxmaxs
// short life in 0.1 s
// 3 bytes r, g, b

#define TE_KILLBEAM 99 // kill all beams attached to entity
// short (entity)

#define TE_LARGEFUNNEL 100
// coord coord coord (funnel position)
// short (sprite index)
// short (flags)

#define TE_BLOODSTREAM 101 // particle spray
// coord coord coord (start position)
// coord coord coord (spray vector)
// byte (color)
// byte (speed)

#define TE_SHOWLINE 102 // line of particles every 5 units, dies in 30 seconds
// coord coord coord (start position)
// coord coord coord (end position)

#define TE_BLOOD 103 // particle spray
// coord coord coord (start position)
// coord coord coord (spray vector)
// byte (color)
// byte (speed)

#define TE_DECAL 104 // Decal applied to a brush entity (not the world)
// coord, coord, coord (x,y,z), decal position (center of texture in world)
// byte (texture index of precached decal texture name)
// short (entity index)

#define TE_FIZZ 105 // create alpha sprites inside of entity, float upwards
// short (entity)
// short (sprite index)
// byte (density)

#define TE_MODEL 106 // create a moving model that bounces and makes a sound when it hits
// coord, coord, coord (position)
// coord, coord, coord (velocity)
// angle (initial yaw)
// short (model index)
// byte (bounce sound type)
// byte (life in 0.1's)

#define TE_EXPLODEMODEL 107 // spherical shower of models, picks from set
// coord, coord, coord (origin)
// coord (velocity)
// short (model index)
// short (count)
// byte (life in 0.1's)

#define TE_BREAKMODEL 108 // box of models or sprites
// coord, coord, coord (position)
// coord, coord, coord (size)
// coord, coord, coord (velocity)
// byte (random velocity in 10's)
// short (sprite or model index)
// byte (count)
// byte (life in 0.1 secs)
// byte (flags)

#define TE_GUNSHOTDECAL 109 // decal and ricochet sound
// coord, coord, coord (position)
// short (entity index???)
// byte (decal???)

#define TE_SPRITE_SPRAY 110 // spay of alpha sprites
// coord, coord, coord (position)
// coord, coord, coord (velocity)
// short (sprite index)
// byte (count)
// byte (speed)
// byte (noise)

#define TE_ARMOR_RICOCHET 111 // quick spark sprite, client ricochet sound.
// coord, coord, coord (position)
// byte (scale in 0.1's)

#define TE_PLAYERDECAL 112 // ???
// byte (playerindex)
// coord, coord, coord (position)
// short (entity???)
// byte (decal number???)
// [optional] short (model index???)

#define TE_BUBBLES 113 // create alpha sprites inside of box, float upwards
// coord, coord, coord (min start position)
// coord, coord, coord (max start position)
// coord (float height)
// short (model index)
// byte (count)
// coord (speed)

#define TE_BUBBLETRAIL 114 // create alpha sprites along a line, float upwards
// coord, coord, coord (min start position)
// coord, coord, coord (max start position)
// coord (float height)
// short (model index)
// byte (count)
// coord (speed)

#define TE_BLOODSPRITE 115 // spray of opaque sprite1's that fall, single sprite2 for 1..2 secs (this is a high-priority tent)
// coord, coord, coord (position)
// short (sprite1 index)
// short (sprite2 index)
// byte (color)
// byte (scale)

#define TE_WORLDDECAL 116 // Decal applied to the world brush
// coord, coord, coord (x,y,z), decal position (center of texture in world)
// byte (texture index of precached decal texture name)

#define TE_WORLDDECALHIGH 117 // Decal (with texture index > 256) applied to world brush
// coord, coord, coord (x,y,z), decal position (center of texture in world)
// byte (texture index of precached decal texture name - 256)

#define TE_DECALHIGH 118 // Same as TE_DECAL, but the texture index was greater than 256
// coord, coord, coord (x,y,z), decal position (center of texture in world)
// byte (texture index of precached decal texture name - 256)
// short (entity index)

#define TE_PROJECTILE 119 // Makes a projectile (like a nail) (this is a high-priority tent)
// coord, coord, coord (position)
// coord, coord, coord (velocity)
// short (modelindex)
// byte (life)
// byte (owner)  projectile won't collide with owner (if owner == 0, projectile will hit any client).

#define TE_SPRAY 120 // Throws a shower of sprites or models
// coord, coord, coord (position)
// coord, coord, coord (direction)
// short (modelindex)
// byte (count)
// byte (speed)
// byte (noise)
// byte (rendermode)

#define TE_PLAYERSPRITES 121 // sprites emit from a player's bounding box (ONLY use for players!)
// byte (playernum)
// short (sprite modelindex)
// byte (count)
// byte (variance) (0 = no variance in size) (10 = 10% variance in size)

#define TE_PARTICLEBURST 122 // very similar to lavasplash.
// coord (origin)
// short (radius)
// byte (particle color)
// byte (duration * 10) (will be randomized a bit)

#define TE_FIREFIELD 123 // makes a field of fire.
// coord (origin)
// short (radius) (fire is made in a square around origin. -radius, -radius to radius, radius)
// short (modelindex)
// byte (count)
// byte (flags)
// byte (duration (in seconds) * 10) (will be randomized a bit)
//
// to keep network traffic low, this message has associated flags that fit into a byte:
#define TEFIRE_FLAG_ALLFLOAT  1 // all sprites will drift upwards as they animate
#define TEFIRE_FLAG_SOMEFLOAT 2 // some of the sprites will drift upwards. (50% chance)
#define TEFIRE_FLAG_LOOP      4 // if set, sprite plays at 15 fps, otherwise plays at whatever rate stretches the animation over the sprite's duration.
#define TEFIRE_FLAG_ALPHA     8 // if set, sprite is rendered alpha blended at 50% else, opaque
#define TEFIRE_FLAG_PLANAR    16 // if set, all fire sprites have same initial Z instead of randomly filling a cube.
#define TEFIRE_FLAG_ADDITIVE  32 // if set, sprite is rendered non-opaque with additive

#define TE_PLAYERATTACHMENT 124 // attaches a TENT to a player (this is a high-priority tent)
// byte (entity index of player)
// coord (vertical offset) ( attachment origin.z = player origin.z + vertical offset )
// short (model index)
// short (life * 10 );

#define TE_KILLPLAYERATTACHMENTS 125 // will expire all TENTS attached to a player.
// byte (entity index of player)

#define TE_MULTIGUNSHOT 126 // much more compact shotgun message
// This message is used to make a client approximate a 'spray' of gunfire.
// Any weapon that fires more than one bullet per frame and fires in a bit of a spread is
// a good candidate for MULTIGUNSHOT use. (shotguns)
//
// NOTE: This effect makes the client do traces for each bullet, these client traces ignore
//		 entities that have studio models.Traces are 4096 long.
//
// coord (origin)
// coord (origin)
// coord (origin)
// coord (direction)
// coord (direction)
// coord (direction)
// coord (x noise * 100)
// coord (y noise * 100)
// byte (count)
// byte (bullethole decal texture index)

#define TE_USERTRACER 127 // larger message than the standard tracer, but allows some customization.
// coord (origin)
// coord (origin)
// coord (origin)
// coord (velocity)
// coord (velocity)
// coord (velocity)
// byte ( life * 10 )
// byte ( color ) this is an index into an array of color vectors in the engine. (0 - )
// byte ( length * 10 )

#define MSG_BROADCAST      0 // unreliable to all
#define MSG_ONE            1 // reliable to one (msg_entity)
#define MSG_ALL            2 // reliable to all
#define MSG_INIT           3 // write to the init string
#define MSG_PVS            4 // Ents in PVS of org
#define MSG_PAS            5 // Ents in PAS of org
#define MSG_PVS_R          6 // Reliable to PVS
#define MSG_PAS_R          7 // Reliable to PAS
#define MSG_ONE_UNRELIABLE 8 // Send to one client, but don't put in reliable stream, put in unreliable datagram ( could be dropped )
#define MSG_SPEC           9 // Sends to all spectator proxies

// contents of a spot in the world
#define CONTENTS_EMPTY -1
#define CONTENTS_SOLID -2
#define CONTENTS_WATER -3
#define CONTENTS_SLIME -4
#define CONTENTS_LAVA  -5
#define CONTENTS_SKY   -6
/* These additional contents constants are defined in bspfile.h
#define	CONTENTS_ORIGIN		-7		// removed at csg time
#define	CONTENTS_CLIP		-8		// changed to contents_solid
#define	CONTENTS_CURRENT_0		-9
#define	CONTENTS_CURRENT_90		-10
#define	CONTENTS_CURRENT_180	-11
#define	CONTENTS_CURRENT_270	-12
#define	CONTENTS_CURRENT_UP		-13
#define	CONTENTS_CURRENT_DOWN	-14
#define CONTENTS_TRANSLUCENT	-15
*/
#define CONTENTS_LADDER -16

#define CONTENT_FLYFIELD         -17
#define CONTENT_GRAVITY_FLYFIELD -18
#define CONTENT_FOG              -19

#define CONTENT_EMPTY -1
#define CONTENT_SOLID -2
#define CONTENT_WATER -3
#define CONTENT_SLIME -4
#define CONTENT_LAVA  -5
#define CONTENT_SKY   -6

// channels
#define CHAN_AUTO              0
#define CHAN_WEAPON            1
#define CHAN_VOICE             2
#define CHAN_ITEM              3
#define CHAN_BODY              4
#define CHAN_STREAM            5 // allocate stream channel from the static or dynamic area
#define CHAN_STATIC            6 // allocate channel from the static area
#define CHAN_NETWORKVOICE_BASE 7 // voice data coming across the network
#define CHAN_NETWORKVOICE_END  500 // network voice data reserves slots (CHAN_NETWORKVOICE_BASE through CHAN_NETWORKVOICE_END).
#define CHAN_BOT               501 // channel used for bot chatter.

// attenuation values
#define ATTN_NONE   0
#define ATTN_NORM   (float)0.8
#define ATTN_IDLE   (float)2
#define ATTN_STATIC (float)1.25

// pitch values
#define PITCH_NORM 100 // non-pitch shifted
#define PITCH_LOW  95 // other values are possible - 0-255, where 255 is very high
#define PITCH_HIGH 120

// volume values
#define VOL_NORM 1.0

// plats
#define PLAT_LOW_TRIGGER 1

// Trains
#define SF_TRAIN_WAIT_RETRIGGER 1
#define SF_TRAIN_START_ON       4 // Train is initially moving
#define SF_TRAIN_PASSABLE       8 // Train is not solid -- used to make water trains

// Break Model Defines

#define BREAK_TYPEMASK 0x4F
#define BREAK_GLASS    0x01
#define BREAK_METAL    0x02
#define BREAK_FLESH    0x04
#define BREAK_WOOD     0x08

#define BREAK_SMOKE    0x10
#define BREAK_TRANS    0x20
#define BREAK_CONCRETE 0x40
#define BREAK_2        0x80

// Colliding temp entity sounds

#define BOUNCE_GLASS     BREAK_GLASS
#define BOUNCE_METAL     BREAK_METAL
#define BOUNCE_FLESH     BREAK_FLESH
#define BOUNCE_WOOD      BREAK_WOOD
#define BOUNCE_SHRAP     0x10
#define BOUNCE_SHELL     0x20
#define BOUNCE_CONCRETE  BREAK_CONCRETE
#define BOUNCE_SHOTSHELL 0x80

// Temp entity bounce sound types
#define TE_BOUNCE_NULL      0
#define TE_BOUNCE_SHELL     1
#define TE_BOUNCE_SHOTSHELL 2

// Rendering constants
enum
{
	kRenderNormal, // src
	kRenderTransColor, // c*a+dest*(1-a)
	kRenderTransTexture, // src*a+dest*(1-a)
	kRenderGlow, // src*a+dest -- No Z buffer checks
	kRenderTransAlpha, // src*srca+dest*(1-srca)
	kRenderTransAdd, // src*a+dest
};

enum
{
	kRenderFxNone = 0,
	kRenderFxPulseSlow,
	kRenderFxPulseFast,
	kRenderFxPulseSlowWide,
	kRenderFxPulseFastWide,
	kRenderFxFadeSlow,
	kRenderFxFadeFast,
	kRenderFxSolidSlow,
	kRenderFxSolidFast,
	kRenderFxStrobeSlow,
	kRenderFxStrobeFast,
	kRenderFxStrobeFaster,
	kRenderFxFlickerSlow,
	kRenderFxFlickerFast,
	kRenderFxNoDissipation,
	kRenderFxDistort, // Distort/scale/translate flicker
	kRenderFxHologram, // kRenderFxDistort + distance fade
	kRenderFxDeadPlayer, // kRenderAmt is the player index
	kRenderFxExplode, // Scale up really big!
	kRenderFxGlowShell, // Glowing Shell
	kRenderFxClampMinScale, // Keep this sprite from getting very small (SPRITES only!)
	kRenderFxLightMultiplier, //CTM !!!CZERO added to tell the studiorender that the value in iuser2 is a lightmultiplier
};

typedef unsigned char byte;
typedef unsigned short word;
#define _DEF_BYTE_

#ifndef __cplusplus
typedef enum
{
	false,
	true
} qboolean;
#else
typedef int qboolean;
#endif

typedef struct
{
	Vector normal;
	float dist;
} plane_t;

typedef struct usercmd_s
{
	short	lerp_msec;      // Interpolation time on client
	byte	msec;           // Duration in ms of command
	vec3_t	viewangles;     // Command view angles.

// intended velocities
	float	forwardmove;    // Forward velocity.
	float	sidemove;       // Sideways velocity.
	float	upmove;         // Upward velocity.
	byte	lightlevel;     // Light level at spot where we are standing.
	unsigned short  buttons;  // Attack buttons
	byte    impulse;          // Impulse command issued.
	byte	weaponselect;	// Current weapon id

// Experimental player impact stuff.
	int		impact_index;
	vec3_t	impact_position;
} usercmd_t;

// physent_t
typedef struct physent_s
{
	char			name[32];             // Name of model, or "player" or "world".
	int				player;
	vec3_t			origin;               // Model's origin in world coordinates.
	struct model_s* model;		          // only for bsp models
	struct model_s* studiomodel;         // SOLID_BBOX, but studio clip intersections.
	vec3_t			mins, maxs;	          // only for non-bsp models
	int				info;		          // For client or server to use to identify (index into edicts or cl_entities)
	vec3_t			angles;               // rotated entities need this info for hull testing to work.

	int				solid;				  // Triggers and func_door type WATER brushes are SOLID_NOT
	int				skin;                 // BSP Contents for such things like fun_door water brushes.
	int				rendermode;			  // So we can ignore glass

	// Complex collision detection.
	float			frame;
	int				sequence;
	byte			controller[4];
	byte			blending[2];

	int				movetype;
	int				takedamage;
	int				blooddecal;
	int				team;
	int				classnumber;

	// For mods
	int				iuser1;
	int				iuser2;
	int				iuser3;
	int				iuser4;
	float			fuser1;
	float			fuser2;
	float			fuser3;
	float			fuser4;
	vec3_t			vuser1;
	vec3_t			vuser2;
	vec3_t			vuser3;
	vec3_t			vuser4;
} physent_t;

#define	MAX_PHYSENTS 600 		  // Must have room for all entities in the world.
#define MAX_MOVEENTS 64
#define	MAX_CLIP_PLANES	5

#define PM_NORMAL			0x00000000
#define PM_STUDIO_IGNORE	0x00000001		// Skip studio models
#define PM_STUDIO_BOX		0x00000002		// Use boxes for non-complex studio models (even in traceline)
#define PM_GLASS_IGNORE		0x00000004		// Ignore entities with non-normal rendermode
#define PM_WORLD_ONLY		0x00000008		// Only trace against the world

// Values for flags parameter of PM_TraceLine
#define PM_TRACELINE_ANYVISIBLE		0
#define PM_TRACELINE_PHYSENTSONLY	1

typedef struct
{
	vec3_t	normal;
	float	dist;
} pmplane_t;

#define IN_ATTACK	(1 << 0)
#define IN_JUMP		(1 << 1)
#define IN_DUCK		(1 << 2)
#define IN_FORWARD	(1 << 3)
#define IN_BACK		(1 << 4)
#define IN_USE		(1 << 5)
#define IN_CANCEL	(1 << 6)
#define IN_LEFT		(1 << 7)
#define IN_RIGHT	(1 << 8)
#define IN_MOVELEFT	(1 << 9)
#define IN_MOVERIGHT (1 << 10)
#define IN_ATTACK2	(1 << 11)
#define IN_RUN      (1 << 12)
#define IN_RELOAD	(1 << 13)
#define IN_ALT1		(1 << 14)
#define IN_SCORE	(1 << 15)   // Used by client.dll for when scoreboard is held down

typedef struct pmtrace_s pmtrace_t;

struct pmtrace_s
{
	qboolean	allsolid;	      // if true, plane is not valid
	qboolean	startsolid;	      // if true, the initial point was in a solid area
	qboolean	inopen, inwater;  // End point is in empty space or in water
	float		fraction;		  // time completed, 1.0 = didn't hit anything
	vec3_t		endpos;			  // final position
	pmplane_t	plane;		      // surface normal at impact
	int			ent;			  // entity at impact
	vec3_t      deltavelocity;    // Change in player's velocity caused by impact.  
								  // Only run on server.
	int         hitgroup;
};

#define MAX_PHYSINFO_STRING 256

typedef struct playermove_s playermove_t;

struct playermove_s
{
	int				player_index;  // So we don't try to run the PM_CheckStuck nudging too quickly.
	qboolean		server;        // For debugging, are we running physics code on server side?

	qboolean		multiplayer;   // 1 == multiplayer server
	float			time;          // realtime on host, for reckoning duck timing
	float			frametime;	   // Duration of this frame

	vec3_t			forward, right, up; // Vectors for angles
	// player state
	vec3_t			origin;        // Movement origin.
	vec3_t			angles;        // Movement view angles.
	vec3_t			oldangles;     // Angles before movement view angles were looked at.
	vec3_t			velocity;      // Current movement direction.
	vec3_t			movedir;       // For waterjumping, a forced forward velocity so we can fly over lip of ledge.
	vec3_t			basevelocity;  // Velocity of the conveyor we are standing, e.g.

	// For ducking/dead
	vec3_t			view_ofs;      // Our eye position.
	float			flDuckTime;    // Time we started duck
	qboolean		bInDuck;       // In process of ducking or ducked already?

	// For walking/falling
	int				flTimeStepSound;  // Next time we can play a step sound
	int				iStepLeft;

	float			flFallVelocity;
	vec3_t			punchangle;

	float			flSwimTime;

	float			flNextPrimaryAttack;

	int				effects;		// MUZZLE FLASH, e.g.

	int				flags;         // FL_ONGROUND, FL_DUCKING, etc.
	int				usehull;       // 0 = regular player hull, 1 = ducked player hull, 2 = point hull
	float			gravity;       // Our current gravity and friction.
	float			friction;
	int				oldbuttons;    // Buttons last usercmd
	float			waterjumptime; // Amount of time left in jumping out of water cycle.
	qboolean		dead;          // Are we a dead player?
	int				deadflag;
	int				spectator;     // Should we use spectator physics model?
	int				movetype;      // Our movement type, NOCLIP, WALK, FLY

	int				onground;
	int				waterlevel;
	int				watertype;
	int				oldwaterlevel;

	char			sztexturename[256];
	char			chtexturetype;

	float			maxspeed;
	float			clientmaxspeed; // Player specific maxspeed

	// For mods
	int				iuser1;
	int				iuser2;
	int				iuser3;
	int				iuser4;
	float			fuser1;
	float			fuser2;
	float			fuser3;
	float			fuser4;
	vec3_t			vuser1;
	vec3_t			vuser2;
	vec3_t			vuser3;
	vec3_t			vuser4;
	// world state
	// Number of entities to clip against.
	int				numphysent;
	physent_t		physents[MAX_PHYSENTS];
	// Number of momvement entities (ladders)
	int				nummoveent;
	// just a list of ladders
	physent_t		moveents[MAX_MOVEENTS];

	// All things being rendered, for tracing against things you don't actually collide with
	int				numvisent;
	physent_t		visents[MAX_PHYSENTS];

	// input to run through physics.
	usercmd_t		cmd;

	// Trace results for objects we collided with.
	int				numtouch;
	pmtrace_t		touchindex[MAX_PHYSENTS];

	char			physinfo[MAX_PHYSINFO_STRING]; // Physics info string

	struct movevars_s* movevars;
	vec3_t player_mins[4];
	vec3_t player_maxs[4];

	/*
	const char* (*PM_Info_ValueForKey) (const char* s, const char* key);
	void			(*PM_Particle)(vec3_t origin, int color, float life, int zpos, int zvel);
	int				(*PM_TestPlayerPosition) (vec3_t pos, pmtrace_t* ptrace);
	void			(*Con_NPrintf)(int idx, char* fmt, ...);
	void			(*Con_DPrintf)(char* fmt, ...);
	void			(*Con_Printf)(char* fmt, ...);
	double			(*Sys_FloatTime)(void);
	void			(*PM_StuckTouch)(int hitent, pmtrace_t* ptraceresult);
	int				(*PM_PointContents) (vec3_t p, int* truecontents);
	int				(*PM_TruePointContents) (vec3_t p);
	int				(*PM_HullPointContents) (struct hull_s* hull, int num, vec3_t p);
	pmtrace_t(*PM_PlayerTrace) (vec3_t start, vec3_t end, int traceFlags, int ignore_pe);
	struct pmtrace_s* (*PM_TraceLine)(float* start, float* end, int flags, int usehulll, int ignore_pe);
	int32(*RandomLong)(int32 lLow, int32 lHigh);
	float			(*RandomFloat)(float flLow, float flHigh);
	int				(*PM_GetModelType)(struct model_s* mod);
	void			(*PM_GetModelBounds)(struct model_s* mod, vec3_t mins, vec3_t maxs);
	void* (*PM_HullForBsp)(physent_t* pe, vec3_t offset);
	float			(*PM_TraceModel)(physent_t* pEnt, vec3_t start, vec3_t end, trace_t* trace);
	int				(*COM_FileSize)(char* filename);
	byte* (*COM_LoadFile) (char* path, int usehunk, int* pLength);
	void			(*COM_FreeFile) (void* buffer);
	char* (*memfgets)(byte* pMemFile, int fileSize, int* pFilePos, char* pBuffer, int bufferSize);
	qboolean		runfuncs;
	void			(*PM_PlaySound) (int channel, const char* sample, float volume, float attenuation, int fFlags, int pitch);
	const char* (*PM_TraceTexture) (int ground, vec3_t vstart, vec3_t vend);
	void			(*PM_PlaybackEventFull) (int flags, int clientindex, unsigned short eventindex, float delay, float* origin, float* angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2);
	*/
};

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

typedef struct ref_params_s
{
	// Output
	float	vieworg[3];
	float	viewangles[3];

	float	forward[3];
	float	right[3];
	float   up[3];

	// Client frametime;
	float	frametime;
	// Client time
	float	time;

	// Misc
	int		intermission;
	int		paused;
	int		spectator;
	int		onground;
	int		waterlevel;

	float	simvel[3];
	float	simorg[3];

	float	viewheight[3];
	float	idealpitch;

	float	cl_viewangles[3];

	int		health;
	float	crosshairangle[3];
	float	viewsize;

	float	punchangle[3];
	int		maxclients;
	int		viewentity;
	int		playernum;
	int		max_entities;
	int		demoplayback;
	int		hardware;

	int		smoothing;

	// Last issued usercmd
	struct usercmd_s* cmd;

	// Movevars
	struct movevars_s* movevars;

	int		viewport[4];		// the viewport coordinates x ,y , width, height

	int		nextView;			// the renderer calls ClientDLL_CalcRefdef() and Renderview
								// so long in cycles until this value is 0 (multiple views)
	int		onlyClientDraw;		// if !=0 nothing is drawn by the engine except clientDraw functions
} ref_params_t;

typedef struct link_s
{
	struct link_s* prev, * next;
} link_t;

typedef struct edict_s edict_t;
typedef int	string_t;

typedef struct entvars_s
{
	string_t	classname;
	string_t	globalname;

	vec3_t		origin;
	vec3_t		oldorigin;
	vec3_t		velocity;
	vec3_t		basevelocity;
	vec3_t      clbasevelocity;  // Base velocity that was passed in to server physics so 
								 //  client can predict conveyors correctly.  Server zeroes it, so we need to store here, too.
	vec3_t		movedir;

	vec3_t		angles;			// Model angles
	vec3_t		avelocity;		// angle velocity (degrees per second)
	vec3_t		punchangle;		// auto-decaying view angle adjustment
	vec3_t		v_angle;		// Viewing angle (player only)

	// For parametric entities
	vec3_t		endpos;
	vec3_t		startpos;
	float		impacttime;
	float		starttime;

	int			fixangle;		// 0:nothing, 1:force view angles, 2:add avelocity
	float		idealpitch;
	float		pitch_speed;
	float		ideal_yaw;
	float		yaw_speed;

	int			modelindex;
	string_t	model;

	int			viewmodel;		// player's viewmodel
	int			weaponmodel;	// what other players see

	vec3_t		absmin;		// BB max translated to world coord
	vec3_t		absmax;		// BB max translated to world coord
	vec3_t		mins;		// local BB min
	vec3_t		maxs;		// local BB max
	vec3_t		size;		// maxs - mins

	float		ltime;
	float		nextthink;

	int			movetype;
	int			solid;

	int			skin;
	int			body;			// sub-model selection for studiomodels
	int 		effects;

	float		gravity;		// % of "normal" gravity
	float		friction;		// inverse elasticity of MOVETYPE_BOUNCE

	int			light_level;

	int			sequence;		// animation sequence
	int			gaitsequence;	// movement animation sequence for player (0 for none)
	float		frame;			// % playback position in animation sequences (0..255)
	float		animtime;		// world time when frame was set
	float		framerate;		// animation playback rate (-8x to 8x)
	std::byte		controller[4];	// bone controller setting (0..255)
	std::byte		blending[2];	// blending amount between sub-sequences (0..255)

	float		scale;			// sprite rendering scale (0..255)

	int			rendermode;
	float		renderamt;
	vec3_t		rendercolor;
	int			renderfx;

	float		health;
	float		frags;
	int			weapons;  // bit mask for available weapons
	float		takedamage;

	int			deadflag;
	vec3_t		view_ofs;	// eye position

	int			button;
	int			impulse;

	edict_t* chain;			// Entity pointer when linked into a linked list
	edict_t* dmg_inflictor;
	edict_t* enemy;
	edict_t* aiment;		// entity pointer when MOVETYPE_FOLLOW
	edict_t* owner;
	edict_t* groundentity;

	int			spawnflags;
	int			flags;

	int			colormap;		// lowbyte topcolor, highbyte bottomcolor
	int			team;

	float		max_health;
	float		teleport_time;
	float		armortype;
	float		armorvalue;
	int			waterlevel;
	int			watertype;

	string_t	target;
	string_t	targetname;
	string_t	netname;
	string_t	message;

	float		dmg_take;
	float		dmg_save;
	float		dmg;
	float		dmgtime;

	string_t	noise;
	string_t	noise1;
	string_t	noise2;
	string_t	noise3;

	float		speed;
	float		air_finished;
	float		pain_finished;
	float		radsuit_finished;

	edict_t* pContainingEntity;

	int			playerclass;
	float		maxspeed;

	float		fov;
	int			weaponanim;

	int			pushmsec;

	int			bInDuck;
	int			flTimeStepSound;
	int			flSwimTime;
	int			flDuckTime;
	int			iStepLeft;
	float		flFallVelocity;

	int			gamestate;

	int			oldbuttons;

	int			groupinfo;

	// For mods
	int			iuser1;
	int			iuser2;
	int			iuser3;
	int			iuser4;
	float		fuser1;
	float		fuser2;
	float		fuser3;
	float		fuser4;
	vec3_t		vuser1;
	vec3_t		vuser2;
	vec3_t		vuser3;
	vec3_t		vuser4;
	edict_t* euser1;
	edict_t* euser2;
	edict_t* euser3;
	edict_t* euser4;
} entvars_t;

#define	MAX_ENT_LEAFS	48

struct edict_s
{
	qboolean	free;
	int			serialnumber;
	link_t		area;				// linked to a division node or leaf

	int			headnode;			// -1 to use normal leaf check
	int			num_leafs;
	short		leafnums[MAX_ENT_LEAFS];

	float		freetime;			// sv.time when the object was freed

	void* pvPrivateData;		// Alloced and freed by engine, used by DLLs

	entvars_t	v;					// C exported fields from progs

	// other fields from progs come immediately after
};

typedef struct
{
	qboolean allsolid; // if true, plane is not valid
	qboolean startsolid; // if true, the initial point was in a solid area
	qboolean inopen, inwater;
	float fraction; // time completed, 1.0 = didn't hit anything
	Vector endpos; // final position
	plane_t plane; // surface normal at impact
	edict_t* ent; // entity the surface is on
	int hitgroup; // 0 == generic, non zero is specific body part
} trace_t;


typedef struct
{
	std::byte r, g, b;
} color24;

typedef struct
{
	unsigned r, g, b, a;
} colorVec;

typedef struct entity_state_s entity_state_t;

struct entity_state_s
{
	// Fields which are filled in by routines outside of delta compression
	int			entityType;
	// Index into cl_entities array for this entity.
	int			number;
	float		msg_time;

	// Message number last time the player/entity state was updated.
	int			messagenum;

	// Fields which can be transitted and reconstructed over the network stream
	vec3_t		origin;
	vec3_t		angles;

	int			modelindex;
	int			sequence;
	float		frame;
	int			colormap;
	short		skin;
	short		solid;
	int			effects;
	float		scale;

	std::byte		eflags;

	// Render information
	int			rendermode;
	int			renderamt;
	color24		rendercolor;
	int			renderfx;

	int			movetype;
	float		animtime;
	float		framerate;
	int			body;
	std::byte		controller[4];
	std::byte		blending[4];
	vec3_t		velocity;

	// Send bbox down to client for use during prediction.
	vec3_t		mins;
	vec3_t		maxs;

	int			aiment;
	// If owned by a player, the index of that player ( for projectiles ).
	int			owner;

	// Friction, for prediction.
	float		friction;
	// Gravity multiplier
	float		gravity;

	// PLAYER SPECIFIC
	int			team;
	int			playerclass;
	int			health;
	qboolean	spectator;
	int         weaponmodel;
	int			gaitsequence;
	// If standing on conveyor, e.g.
	vec3_t		basevelocity;
	// Use the crouched hull, or the regular player hull.
	int			usehull;
	// Latched buttons last time state updated.
	int			oldbuttons;
	// -1 = in air, else pmove entity number
	int			onground;
	int			iStepLeft;
	// How fast we are falling
	float		flFallVelocity;

	float		fov;
	int			weaponanim;

	// Parametric movement overrides
	vec3_t				startpos;
	vec3_t				endpos;
	float				impacttime;
	float				starttime;

	// For mods
	int			iuser1;
	int			iuser2;
	int			iuser3;
	int			iuser4;
	float		fuser1;
	float		fuser2;
	float		fuser3;
	float		fuser4;
	vec3_t		vuser1;
	vec3_t		vuser2;
	vec3_t		vuser3;
	vec3_t		vuser4;
};

typedef struct efrag_s
{
	struct mleaf_s* leaf;
	struct efrag_s* leafnext;
	struct cl_entity_s* entity;
	struct efrag_s* entnext;
} efrag_t;

typedef struct
{
	std::byte					mouthopen;		// 0 = mouth closed, 255 = mouth agape
	std::byte					sndcount;		// counter for running average
	int						sndavg;			// running average
} mouth_t;

typedef struct
{
	float					prevanimtime;
	float					sequencetime;
	std::byte					prevseqblending[2];
	vec3_t					prevorigin;
	vec3_t					prevangles;

	int						prevsequence;
	float					prevframe;

	std::byte					prevcontroller[4];
	std::byte					prevblending[2];
} latchedvars_t;

typedef struct
{
	// Time stamp for this movement
	float					animtime;

	vec3_t					origin;
	vec3_t					angles;
} position_history_t;

#define HISTORY_MAX		64  // Must be power of 2

typedef struct cl_entity_s cl_entity_t;

struct cl_entity_s
{
	int						index;      // Index into cl_entities ( should match actual slot, but not necessarily )

	qboolean				player;     // True if this entity is a "player"

	entity_state_t			baseline;   // The original state from which to delta during an uncompressed message
	entity_state_t			prevstate;  // The state information from the penultimate message received from the server
	entity_state_t			curstate;   // The state information from the last message received from server

	int						current_position;  // Last received history update index
	position_history_t		ph[HISTORY_MAX];   // History of position and angle updates for this player

	mouth_t					mouth;			// For synchronizing mouth movements.

	latchedvars_t			latched;		// Variables used by studio model rendering routines

	// Information based on interplocation, extrapolation, prediction, or just copied from last msg received.
	//
	float					lastmove;

	// Actual render position and angles
	vec3_t					origin;
	vec3_t					angles;

	// Attachment points
	vec3_t					attachment[4];

	// Other entity local information
	int						trivial_accept;

	struct model_s* model;			// cl.model_precache[ curstate.modelindes ];  all visible entities have a model
	struct efrag_s* efrag;			// linked list of efrags
	struct mnode_s* topnode;		// for bmodels, first world node that splits bmodel, or NULL if not split

	float					syncbase;		// for client-side animations -- used by obsolete alias animation system, remove?
	int						visframe;		// last frame this entity was found in an active leaf
	colorVec				cvFloorColor;
};

//--------------------------------------------------------------------------
// sequenceDefaultBits_e
//	
// Enumerated list of possible modifiers for a command.  This enumeration
// is used in a bitarray controlling what modifiers are specified for a command.
//---------------------------------------------------------------------------
enum sequenceModifierBits
{
	SEQUENCE_MODIFIER_EFFECT_BIT = (1 << 1),
	SEQUENCE_MODIFIER_POSITION_BIT = (1 << 2),
	SEQUENCE_MODIFIER_COLOR_BIT = (1 << 3),
	SEQUENCE_MODIFIER_COLOR2_BIT = (1 << 4),
	SEQUENCE_MODIFIER_FADEIN_BIT = (1 << 5),
	SEQUENCE_MODIFIER_FADEOUT_BIT = (1 << 6),
	SEQUENCE_MODIFIER_HOLDTIME_BIT = (1 << 7),
	SEQUENCE_MODIFIER_FXTIME_BIT = (1 << 8),
	SEQUENCE_MODIFIER_SPEAKER_BIT = (1 << 9),
	SEQUENCE_MODIFIER_LISTENER_BIT = (1 << 10),
	SEQUENCE_MODIFIER_TEXTCHANNEL_BIT = (1 << 11),
};
typedef enum sequenceModifierBits sequenceModifierBits_e;


//---------------------------------------------------------------------------
// sequenceCommandEnum_e
// 
// Enumerated sequence command types.
//---------------------------------------------------------------------------
enum sequenceCommandEnum_
{
	SEQUENCE_COMMAND_ERROR = -1,
	SEQUENCE_COMMAND_PAUSE = 0,
	SEQUENCE_COMMAND_FIRETARGETS,
	SEQUENCE_COMMAND_KILLTARGETS,
	SEQUENCE_COMMAND_TEXT,
	SEQUENCE_COMMAND_SOUND,
	SEQUENCE_COMMAND_GOSUB,
	SEQUENCE_COMMAND_SENTENCE,
	SEQUENCE_COMMAND_REPEAT,
	SEQUENCE_COMMAND_SETDEFAULTS,
	SEQUENCE_COMMAND_MODIFIER,
	SEQUENCE_COMMAND_POSTMODIFIER,
	SEQUENCE_COMMAND_NOOP,

	SEQUENCE_MODIFIER_EFFECT,
	SEQUENCE_MODIFIER_POSITION,
	SEQUENCE_MODIFIER_COLOR,
	SEQUENCE_MODIFIER_COLOR2,
	SEQUENCE_MODIFIER_FADEIN,
	SEQUENCE_MODIFIER_FADEOUT,
	SEQUENCE_MODIFIER_HOLDTIME,
	SEQUENCE_MODIFIER_FXTIME,
	SEQUENCE_MODIFIER_SPEAKER,
	SEQUENCE_MODIFIER_LISTENER,
	SEQUENCE_MODIFIER_TEXTCHANNEL,
};
typedef enum sequenceCommandEnum_ sequenceCommandEnum_e;


//---------------------------------------------------------------------------
// sequenceCommandType_e
// 
// Typeerated sequence command types.
//---------------------------------------------------------------------------
enum sequenceCommandType_
{
	SEQUENCE_TYPE_COMMAND,
	SEQUENCE_TYPE_MODIFIER,
};
typedef enum sequenceCommandType_ sequenceCommandType_e;


//---------------------------------------------------------------------------
// sequenceCommandMapping_s
// 
// A mapping of a command enumerated-value to its name.
//---------------------------------------------------------------------------
typedef struct sequenceCommandMapping_ sequenceCommandMapping_s;
struct sequenceCommandMapping_
{
	sequenceCommandEnum_e	commandEnum;
	const char* commandName;
	sequenceCommandType_e	commandType;
};


//---------------------------------------------------------------------------
// sequenceCommandLine_s
// 
// Structure representing a single command (usually 1 line) from a
//	.SEQ file entry.
//---------------------------------------------------------------------------
typedef struct sequenceCommandLine_ sequenceCommandLine_s;
struct sequenceCommandLine_
{
	int						commandType;		// Specifies the type of command
	client_textmessage_t	clientMessage;		// Text HUD message struct
	char* speakerName;		// Targetname of speaking entity
	char* listenerName;		// Targetname of entity being spoken to
	char* soundFileName;		// Name of sound file to play
	char* sentenceName;		// Name of sentences.txt to play
	char* fireTargetNames;	// List of targetnames to fire
	char* killTargetNames;	// List of targetnames to remove
	float					delay;				// Seconds 'till next command
	int						repeatCount;		// If nonzero, reset execution pointer to top of block (N times, -1 = infinite)
	int						textChannel;		// Display channel on which text message is sent
	int						modifierBitField;	// Bit field to specify what clientmessage fields are valid
	sequenceCommandLine_s* nextCommandLine;	// Next command (linked list)
};


//---------------------------------------------------------------------------
// sequenceEntry_s
// 
// Structure representing a single command (usually 1 line) from a
//	.SEQ file entry.
//---------------------------------------------------------------------------
typedef struct sequenceEntry_ sequenceEntry_s;
struct sequenceEntry_
{
	char* fileName;		// Name of sequence file without .SEQ extension
	char* entryName;		// Name of entry label in file
	sequenceCommandLine_s* firstCommand;	// Linked list of commands in entry
	sequenceEntry_s* nextEntry;		// Next loaded entry
	qboolean				isGlobal;		// Is entry retained over level transitions?
};



//---------------------------------------------------------------------------
// sentenceEntry_s
// Structure representing a single sentence of a group from a .SEQ
// file entry.  Sentences are identical to entries in sentences.txt, but
// can be unique per level and are loaded/unloaded with the level.
//---------------------------------------------------------------------------
typedef struct sentenceEntry_ sentenceEntry_s;
struct sentenceEntry_
{
	char* data;			// sentence data (ie "We have hostiles" )
	sentenceEntry_s* nextEntry;		// Next loaded entry
	qboolean				isGlobal;		// Is entry retained over level transitions?
	unsigned int			index;			// this entry's position in the file.
};

//--------------------------------------------------------------------------
// sentenceGroupEntry_s
// Structure representing a group of sentences found in a .SEQ file.
// A sentence group is defined by all sentences with the same name, ignoring
// the number at the end of the sentence name.  Groups enable a sentence
// to be picked at random across a group.
//--------------------------------------------------------------------------
typedef struct sentenceGroupEntry_ sentenceGroupEntry_s;
struct sentenceGroupEntry_
{
	char* groupName;		// name of the group (ie CT_ALERT )
	unsigned int			numSentences;	// number of sentences in group
	sentenceEntry_s* firstSentence;	// head of linked list of sentences in group
	sentenceGroupEntry_s* nextEntry;		// next loaded group
};

typedef __int16 int16;
typedef unsigned __int16 uint16;
typedef __int32 int32;
typedef unsigned __int32 uint32;
typedef __int64 int64;
typedef unsigned __int64 uint64;

#define	MAX_ALIAS_NAME	32

typedef struct cmdalias_s
{
	struct cmdalias_s* next;
	char	name[MAX_ALIAS_NAME];
	char* value;
} cmdalias_t;

/*
==============================================================================
STUDIO MODELS
Studio models are position independent, so the cache manager can move them.
==============================================================================
*/


#define MAXSTUDIOTRIANGLES	20000	// TODO: tune this
#define MAXSTUDIOVERTS		2048	// TODO: tune this
#define MAXSTUDIOSEQUENCES	2048	// total animation sequences -- KSH incremented
#define MAXSTUDIOSKINS		100		// total textures
#define MAXSTUDIOSRCBONES	512		// bones allowed at source movement
#define MAXSTUDIOBONES		128		// total bones actually used
#define MAXSTUDIOMODELS		32		// sub-models per model
#define MAXSTUDIOBODYPARTS	32
#define MAXSTUDIOGROUPS		16
#define MAXSTUDIOANIMATIONS	2048		
#define MAXSTUDIOMESHES		256
#define MAXSTUDIOEVENTS		1024
#define MAXSTUDIOPIVOTS		256
#define MAXSTUDIOCONTROLLERS 8

typedef struct
{
	int					id;
	int					version;

	char				name[64];
	int					length;

	vec3_t				eyeposition;	// ideal eye position
	vec3_t				min;			// ideal movement hull size
	vec3_t				max;

	vec3_t				bbmin;			// clipping bounding box
	vec3_t				bbmax;

	int					flags;

	int					numbones;			// bones
	int					boneindex;

	int					numbonecontrollers;		// bone controllers
	int					bonecontrollerindex;

	int					numhitboxes;			// complex bounding boxes
	int					hitboxindex;

	int					numseq;				// animation sequences
	int					seqindex;

	int					numseqgroups;		// demand loaded sequences
	int					seqgroupindex;

	int					numtextures;		// raw textures
	int					textureindex;
	int					texturedataindex;

	int					numskinref;			// replaceable textures
	int					numskinfamilies;
	int					skinindex;

	int					numbodyparts;
	int					bodypartindex;

	int					numattachments;		// queryable attachable points
	int					attachmentindex;

	int					soundtable;
	int					soundindex;
	int					soundgroups;
	int					soundgroupindex;

	int					numtransitions;		// animation node to animation node transition graph
	int					transitionindex;
} studiohdr_t;

// header for demand loaded sequence group data
typedef struct
{
	int					id;
	int					version;

	char				name[64];
	int					length;
} studioseqhdr_t;

// bones
typedef struct
{
	char				name[32];	// bone name for symbolic links
	int		 			parent;		// parent bone
	int					flags;		// ??
	int					bonecontroller[6];	// bone controller index, -1 == none
	float				value[6];	// default DoF values
	float				scale[6];   // scale for delta DoF values
} mstudiobone_t;


// bone controllers
typedef struct
{
	int					bone;	// -1 == 0
	int					type;	// X, Y, Z, XR, YR, ZR, M
	float				start;
	float				end;
	int					rest;	// byte index value at rest
	int					index;	// 0-3 user set controller, 4 mouth
} mstudiobonecontroller_t;

// intersection boxes
typedef struct
{
	int					bone;
	int					group;			// intersection group
	vec3_t				bbmin;		// bounding box
	vec3_t				bbmax;
} mstudiobbox_t;

#if !defined( CACHE_USER ) && !defined( QUAKEDEF_H )
#define CACHE_USER
typedef struct cache_user_s
{
	void* data;
} cache_user_t;
#endif

//
// demand loaded sequence groups
//
typedef struct
{
	char				label[32];	// textual name
	char				name[64];	// file name
	int					unused1;    // was "cache"  - index pointer
	int					unused2;    // was "data" -  hack for group 0
} mstudioseqgroup_t;

// sequence descriptions
typedef struct
{
	char				label[32];	// sequence label

	float				fps;		// frames per second	
	int					flags;		// looping/non-looping flags

	int					activity;
	int					actweight;

	int					numevents;
	int					eventindex;

	int					numframes;	// number of frames per sequence

	int					numpivots;	// number of foot pivots
	int					pivotindex;

	int					motiontype;
	int					motionbone;
	vec3_t				linearmovement;
	int					automoveposindex;
	int					automoveangleindex;

	vec3_t				bbmin;		// per sequence bounding box
	vec3_t				bbmax;

	int					numblends;
	int					animindex;		// mstudioanim_t pointer relative to start of sequence group data
										// [blend][bone][X, Y, Z, XR, YR, ZR]

	int					blendtype[2];	// X, Y, Z, XR, YR, ZR
	float				blendstart[2];	// starting value
	float				blendend[2];	// ending value
	int					blendparent;

	int					seqgroup;		// sequence group for demand loading

	int					entrynode;		// transition node at entry
	int					exitnode;		// transition node at exit
	int					nodeflags;		// transition rules

	int					nextseq;		// auto advancing sequences
} mstudioseqdesc_t;

// events
typedef struct
{
	int 				frame;
	int					event;
	int					type;
	char				options[64];
} mstudioevent_t;

// pivots
typedef struct
{
	vec3_t				org;	// pivot point
	int					start;
	int					end;
} mstudiopivot_t;

// attachment
typedef struct
{
	char				name[32];
	int					type;
	int					bone;
	vec3_t				org;	// attachment point
	vec3_t				vectors[3];
} mstudioattachment_t;

typedef struct
{
	unsigned short	offset[6];
} mstudioanim_t;

// animation frames
typedef union
{
	struct {
		byte	valid;
		byte	total;
	} num;
	short		value;
} mstudioanimvalue_t;



// body part index
typedef struct
{
	char				name[64];
	int					nummodels;
	int					base;
	int					modelindex; // index into models array
} mstudiobodyparts_t;



// skin info
typedef struct
{
	char					name[64];
	int						flags;
	int						width;
	int						height;
	int						index;
} mstudiotexture_t;


// skin families
// short	index[skinfamilies][skinref]

// studio models
typedef struct
{
	char				name[64];

	int					type;

	float				boundingradius;

	int					nummesh;
	int					meshindex;

	int					numverts;		// number of unique vertices
	int					vertinfoindex;	// vertex bone info
	int					vertindex;		// vertex vec3_t
	int					numnorms;		// number of unique surface normals
	int					norminfoindex;	// normal bone info
	int					normindex;		// normal vec3_t

	int					numgroups;		// deformation groups
	int					groupindex;
} mstudiomodel_t;


// vec3_t	boundingbox[model][bone][2];	// complex intersection info


// meshes
typedef struct
{
	int					numtris;
	int					triindex;
	int					skinref;
	int					numnorms;		// per mesh normals
	int					normindex;		// normal vec3_t
} mstudiomesh_t;

// triangles
#if 0
typedef struct
{
	short				vertindex;		// index into vertex array
	short				normindex;		// index into normal array
	short				s, t;			// s,t position on skin
} mstudiotrivert_t;
#endif

// lighting options
#define STUDIO_NF_FLATSHADE		0x0001
#define STUDIO_NF_CHROME		0x0002
#define STUDIO_NF_FULLBRIGHT	0x0004
#define STUDIO_NF_NOMIPS        0x0008
#define STUDIO_NF_ALPHA         0x0010
#define STUDIO_NF_ADDITIVE      0x0020
#define STUDIO_NF_MASKED        0x0040

// motion flags
#define STUDIO_X		0x0001
#define STUDIO_Y		0x0002	
#define STUDIO_Z		0x0004
#define STUDIO_XR		0x0008
#define STUDIO_YR		0x0010
#define STUDIO_ZR		0x0020
#define STUDIO_LX		0x0040
#define STUDIO_LY		0x0080
#define STUDIO_LZ		0x0100
#define STUDIO_AX		0x0200
#define STUDIO_AY		0x0400
#define STUDIO_AZ		0x0800
#define STUDIO_AXR		0x1000
#define STUDIO_AYR		0x2000
#define STUDIO_AZR		0x4000
#define STUDIO_TYPES	0x7FFF
#define STUDIO_RLOOP	0x8000	// controller that wraps shortest distance

// sequence flags
#define STUDIO_LOOPING	0x0001

// bone flags
#define STUDIO_HAS_NORMALS	0x0001
#define STUDIO_HAS_VERTICES 0x0002
#define STUDIO_HAS_BBOX		0x0004
#define STUDIO_HAS_CHROME	0x0008	// if any of the textures have chrome on them

#define RAD_TO_STUDIO		(32768.0/M_PI)
#define STUDIO_TO_RAD		(M_PI/32768.0)

#define STUDIO_INTERFACE_VERSION 1

typedef struct engine_studio_api_s
{
	// Allocate number*size bytes and zero it
	void* (*Mem_Calloc)				(int number, size_t size);
	// Check to see if pointer is in the cache
	void* (*Cache_Check)				(struct cache_user_s* c);
	// Load file into cache ( can be swapped out on demand )
	void			(*LoadCacheFile)				(char* path, struct cache_user_s* cu);
	// Retrieve model pointer for the named model
	struct model_s* (*Mod_ForName)				(const char* name, int crash_if_missing);
	// Retrieve pointer to studio model data block from a model
	void* (*Mod_Extradata)				(struct model_s* mod);
	// Retrieve indexed model from client side model precache list
	struct model_s* (*GetModelByIndex)			(int index);
	// Get entity that is set for rendering
	struct cl_entity_s* (*GetCurrentEntity)		(void);
	// Get referenced player_info_t
	struct player_info_s* (*PlayerInfo)			(int index);
	// Get most recently received player state data from network system
	struct entity_state_s* (*GetPlayerState)		(int index);
	// Get viewentity
	struct cl_entity_s* (*GetViewEntity)			(void);
	// Get current frame count, and last two timestampes on client
	void			(*GetTimes)					(int* framecount, double* current, double* old);
	// Get a pointer to a cvar by name
	struct cvar_s* (*GetCvar)					(const char* name);
	// Get current render origin and view vectors ( up, right and vpn )
	void			(*GetViewInfo)				(float* origin, float* upv, float* rightv, float* vpnv);
	// Get sprite model used for applying chrome effect
	struct model_s* (*GetChromeSprite)			(void);
	// Get model counters so we can incement instrumentation
	void			(*GetModelCounters)			(int** s, int** a);
	// Get software scaling coefficients
	void			(*GetAliasScale)				(float* x, float* y);

	// Get bone, light, alias, and rotation matrices
	float**** (*StudioGetBoneTransform) (void);
	float**** (*StudioGetLightTransform)(void);
	float*** (*StudioGetAliasTransform) (void);
	float*** (*StudioGetRotationMatrix) (void);

	// Set up body part, and get submodel pointers
	void			(*StudioSetupModel)			(int bodypart, void** ppbodypart, void** ppsubmodel);
	// Check if entity's bbox is in the view frustum
	int				(*StudioCheckBBox)			(void);
	// Apply lighting effects to model
	void			(*StudioDynamicLight)			(struct cl_entity_s* ent, struct alight_s* plight);
	void			(*StudioEntityLight)			(struct alight_s* plight);
	void			(*StudioSetupLighting)		(struct alight_s* plighting);

	// Draw mesh vertices
	void			(*StudioDrawPoints)			(void);

	// Draw hulls around bones
	void			(*StudioDrawHulls)			(void);
	// Draw bbox around studio models
	void			(*StudioDrawAbsBBox)			(void);
	// Draws bones
	void			(*StudioDrawBones)			(void);
	// Loads in appropriate texture for model
	void			(*StudioSetupSkin)			(void* ptexturehdr, int index);
	// Sets up for remapped colors
	void			(*StudioSetRemapColors)		(int top, int bottom);
	// Set's player model and returns model pointer
	struct model_s* (*SetupPlayerModel)			(int index);
	// Fires any events embedded in animation
	void			(*StudioClientEvents)			(void);
	// Retrieve/set forced render effects flags
	int				(*GetForceFaceFlags)			(void);
	void			(*SetForceFaceFlags)			(int flags);
	// Tell engine the value of the studio model header
	void			(*StudioSetHeader)			(void* header);
	// Tell engine which model_t * is being renderered
	void			(*SetRenderModel)				(struct model_s* model);

	// Final state setup and restore for rendering
	void			(*SetupRenderer)				(int rendermode);
	void			(*RestoreRenderer)			(void);

	// Set render origin for applying chrome effect
	void			(*SetChromeOrigin)			(void);

	// True if using D3D/OpenGL
	int				(*IsHardware)					(void);

	// Only called by hardware interface
	void			(*GL_StudioDrawShadow)		(void);
	void			(*GL_SetRenderMode)			(int mode);

	void			(*StudioSetRenderamt)			(int iRenderamt); 	//!!!CZERO added for rendering glass on viewmodels
	void			(*StudioSetCullState)			(int iCull);
	void			(*StudioRenderShadow)			(int iSprite, float* p1, float* p2, float* p3, float* p4);
} engine_studio_api_t;

typedef struct server_studio_api_s
{
	// Allocate number*size bytes and zero it
	void* (*Mem_Calloc)				(int number, size_t size);
	// Check to see if pointer is in the cache
	void* (*Cache_Check)				(struct cache_user_s* c);
	// Load file into cache ( can be swapped out on demand )
	void			(*LoadCacheFile)				(char* path, struct cache_user_s* cu);
	// Retrieve pointer to studio model data block from a model
	void* (*Mod_Extradata)				(struct model_s* mod);
} server_studio_api_t;


// client blending
typedef struct r_studio_interface_s
{
	int				version;
	int				(*StudioDrawModel)			(int flags);
	int				(*StudioDrawPlayer)			(int flags, struct entity_state_s* pplayer);
} r_studio_interface_t;

extern r_studio_interface_t* pStudioAPI;

// server blending
#define SV_BLENDING_INTERFACE_VERSION 1

typedef struct sv_blending_interface_s
{
	int				version;

	void			(*SV_StudioSetupBones)		(struct model_s* pModel,
		float				frame,
		int				sequence,
		const vec3_t		angles,
		const	vec3_t		origin,
		const	byte* pcontroller,
		const	byte* pblending,
		int				iBone,
		const edict_t* pEdict);
} sv_blending_interface_t;

typedef enum
{
	TRI_FRONT = 0,
	TRI_NONE = 1,
} TRICULLSTYLE;

#define TRI_API_VERSION		1

#define TRI_TRIANGLES		0
#define TRI_TRIANGLE_FAN	1
#define TRI_QUADS			2
#define TRI_POLYGON			3
#define TRI_LINES			4	
#define TRI_TRIANGLE_STRIP	5
#define TRI_QUAD_STRIP		6

typedef struct triangleapi_s
{
	int			version;

	void		(*RenderMode)(int mode);
	void		(*Begin)(int primitiveCode);
	void		(*End) (void);

	void		(*Color4f) (float r, float g, float b, float a);
	void		(*Color4ub) (unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	void		(*TexCoord2f) (float u, float v);
	void		(*Vertex3fv) (float* worldPnt);
	void		(*Vertex3f) (float x, float y, float z);
	void		(*Brightness) (float brightness);
	void		(*CullFace) (TRICULLSTYLE style);
	int			(*SpriteTexture) (struct model_s* pSpriteModel, int frame);
	int			(*WorldToScreen) (float* world, float* screen);  // Returns 1 if it's z clipped
	void		(*Fog) (float flFogColor[3], float flStart, float flEnd, int bOn); // Works just like GL_FOG, flFogColor is r/g/b.
	void		(*ScreenToWorld) (float* screen, float* world);
	void		(*GetMatrix) (const int pname, float* matrix);
	int			(*BoxInPVS) (float* mins, float* maxs);
	void		(*LightAtPoint) (float* pos, float* value);
	void		(*Color4fRendermode) (float r, float g, float b, float a, int rendermode);
	void		(*FogParams) (float flDensity, int iFogSkybox); // Used with Fog()...sets fog density and whether the fog should be applied to the skybox

} triangleapi_t;
// ********************************************************
// Functions exported by the engine
// ********************************************************

// Function type declarations for engine exports
typedef HSPRITE_HL					(*pfnEngSrc_pfnSPR_Load_t)			(const char* szPicName);
typedef int							(*pfnEngSrc_pfnSPR_Frames_t)			(HSPRITE_HL hPic);
typedef int							(*pfnEngSrc_pfnSPR_Height_t)			(HSPRITE_HL hPic, int frame);
typedef int							(*pfnEngSrc_pfnSPR_Width_t)			(HSPRITE_HL hPic, int frame);
typedef void						(*pfnEngSrc_pfnSPR_Set_t)				(HSPRITE_HL hPic, int r, int g, int b);
typedef void						(*pfnEngSrc_pfnSPR_Draw_t)			(int frame, int x, int y, const struct rect_s* prc);
typedef void						(*pfnEngSrc_pfnSPR_DrawHoles_t)		(int frame, int x, int y, const struct rect_s* prc);
typedef void						(*pfnEngSrc_pfnSPR_DrawAdditive_t)	(int frame, int x, int y, const struct rect_s* prc);
typedef void						(*pfnEngSrc_pfnSPR_EnableScissor_t)	(int x, int y, int width, int height);
typedef void						(*pfnEngSrc_pfnSPR_DisableScissor_t)	(void);
typedef struct client_sprite_s* (*pfnEngSrc_pfnSPR_GetList_t)			(char* psz, int* piCount);
typedef void						(*pfnEngSrc_pfnFillRGBA_t)			(int x, int y, int width, int height, int r, int g, int b, int a);
typedef int							(*pfnEngSrc_pfnGetScreenInfo_t) 		(struct SCREENINFO_s* pscrinfo);
typedef void						(*pfnEngSrc_pfnSetCrosshair_t)		(HSPRITE_HL hspr, wrect_t rc, int r, int g, int b);
typedef struct cvar_s* (*pfnEngSrc_pfnRegisterVariable_t)	(char* szName, char* szValue, int flags);
typedef float						(*pfnEngSrc_pfnGetCvarFloat_t)		(char* szName);
typedef char* (*pfnEngSrc_pfnGetCvarString_t)		(char* szName);
typedef int							(*pfnEngSrc_pfnAddCommand_t)			(char* cmd_name, void (*pfnEngSrc_function)(void));
typedef int							(*pfnEngSrc_pfnHookUserMsg_t)			(char* szMsgName, pfnUserMsgHook pfn);
typedef int							(*pfnEngSrc_pfnServerCmd_t)			(char* szCmdString);
typedef int							(*pfnEngSrc_pfnClientCmd_t)			(char* szCmdString);
typedef void						(*pfnEngSrc_pfnPrimeMusicStream_t)	(char* szFilename, int looping);
typedef void						(*pfnEngSrc_pfnGetPlayerInfo_t)		(int ent_num, struct hud_player_info_s* pinfo);
typedef void						(*pfnEngSrc_pfnPlaySoundByName_t)		(char* szSound, float volume);
typedef void						(*pfnEngSrc_pfnPlaySoundByNameAtPitch_t)	(char* szSound, float volume, int pitch);
typedef void						(*pfnEngSrc_pfnPlaySoundVoiceByName_t)		(char* szSound, float volume, int pitch);
typedef void						(*pfnEngSrc_pfnPlaySoundByIndex_t)	(int iSound, float volume);
typedef void						(*pfnEngSrc_pfnAngleVectors_t)		(const float* vecAngles, float* forward, float* right, float* up);
typedef struct client_textmessage_s* (*pfnEngSrc_pfnTextMessageGet_t)		(const char* pName);
typedef int							(*pfnEngSrc_pfnDrawCharacter_t)		(int x, int y, int number, int r, int g, int b);
typedef int							(*pfnEngSrc_pfnDrawConsoleString_t)	(int x, int y, char* string);
typedef void						(*pfnEngSrc_pfnDrawSetTextColor_t)	(float r, float g, float b);
typedef void						(*pfnEngSrc_pfnDrawConsoleStringLen_t)(const char* string, int* length, int* height);
typedef void						(*pfnEngSrc_pfnConsolePrint_t)		(const char* string);
typedef void						(*pfnEngSrc_pfnCenterPrint_t)			(const char* string);
typedef int							(*pfnEngSrc_GetWindowCenterX_t)		(void);
typedef int							(*pfnEngSrc_GetWindowCenterY_t)		(void);
typedef void						(*pfnEngSrc_GetViewAngles_t)			(float*);
typedef void						(*pfnEngSrc_SetViewAngles_t)			(float*);
typedef int							(*pfnEngSrc_GetMaxClients_t)			(void);
typedef void						(*pfnEngSrc_Cvar_SetValue_t)			(char* cvar, float value);
typedef int       					(*pfnEngSrc_Cmd_Argc_t)					(void);
typedef char* (*pfnEngSrc_Cmd_Argv_t)				(int arg);
typedef void						(*pfnEngSrc_Con_Printf_t)				(char* fmt, ...);
typedef void						(*pfnEngSrc_Con_DPrintf_t)			(char* fmt, ...);
typedef void						(*pfnEngSrc_Con_NPrintf_t)			(int pos, char* fmt, ...);
typedef void						(*pfnEngSrc_Con_NXPrintf_t)			(struct con_nprint_s* info, char* fmt, ...);
typedef const char* (*pfnEngSrc_PhysInfo_ValueForKey_t)	(const char* key);
typedef const char* (*pfnEngSrc_ServerInfo_ValueForKey_t)(const char* key);
typedef float						(*pfnEngSrc_GetClientMaxspeed_t)		(void);
typedef int							(*pfnEngSrc_CheckParm_t)				(char* parm, char** ppnext);
typedef void						(*pfnEngSrc_Key_Event_t)				(int key, int down);
typedef void						(*pfnEngSrc_GetMousePosition_t)		(int* mx, int* my);
typedef int							(*pfnEngSrc_IsNoClipping_t)			(void);
typedef struct cl_entity_s* (*pfnEngSrc_GetLocalPlayer_t)		(void);
typedef struct cl_entity_s* (*pfnEngSrc_GetViewModel_t)			(void);
typedef struct cl_entity_s* (*pfnEngSrc_GetEntityByIndex_t)		(int idx);
typedef float						(*pfnEngSrc_GetClientTime_t)			(void);
typedef void						(*pfnEngSrc_V_CalcShake_t)			(void);
typedef void						(*pfnEngSrc_V_ApplyShake_t)			(float* origin, float* angles, float factor);
typedef int							(*pfnEngSrc_PM_PointContents_t)		(float* point, int* truecontents);
typedef int							(*pfnEngSrc_PM_WaterEntity_t)			(float* p);
typedef struct pmtrace_s* (*pfnEngSrc_PM_TraceLine_t)			(float* start, float* end, int flags, int usehull, int ignore_pe);
typedef struct model_s* (*pfnEngSrc_CL_LoadModel_t)			(const char* modelname, int* index);
typedef int							(*pfnEngSrc_CL_CreateVisibleEntity_t)	(int type, struct cl_entity_s* ent);
typedef const struct model_s* (*pfnEngSrc_GetSpritePointer_t)		(HSPRITE_HL HSPRITE_HL);
typedef void						(*pfnEngSrc_pfnPlaySoundByNameAtLocation_t)	(char* szSound, float volume, float* origin);
typedef unsigned short				(*pfnEngSrc_pfnPrecacheEvent_t)		(int type, const char* psz);
typedef void						(*pfnEngSrc_pfnPlaybackEvent_t)		(int flags, const struct edict_s* pInvoker, unsigned short eventindex, float delay, float* origin, float* angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2);
typedef void						(*pfnEngSrc_pfnWeaponAnim_t)			(int iAnim, int body);
typedef float						(*pfnEngSrc_pfnRandomFloat_t)			(float flLow, float flHigh);
typedef int32(*pfnEngSrc_pfnRandomLong_t)			(int32 lLow, int32 lHigh);
typedef void						(*pfnEngSrc_pfnHookEvent_t)			(char* name, void (*pfnEvent)(struct event_args_s* args));
typedef int							(*pfnEngSrc_Con_IsVisible_t)			();
typedef const char* (*pfnEngSrc_pfnGetGameDirectory_t)	(void);
typedef struct cvar_s* (*pfnEngSrc_pfnGetCvarPointer_t)		(const char* szName);
typedef const char* (*pfnEngSrc_Key_LookupBinding_t)		(const char* pBinding);
typedef const char* (*pfnEngSrc_pfnGetLevelName_t)		(void);
typedef void						(*pfnEngSrc_pfnGetScreenFade_t)		(struct screenfade_s* fade);
typedef void						(*pfnEngSrc_pfnSetScreenFade_t)		(struct screenfade_s* fade);
typedef void* (*pfnEngSrc_VGui_GetPanel_t)         ();
typedef void                        (*pfnEngSrc_VGui_ViewportPaintBackground_t) (int extents[4]);
typedef byte* (*pfnEngSrc_COM_LoadFile_t)				(char* path, int usehunk, int* pLength);
typedef char* (*pfnEngSrc_COM_ParseFile_t)			(char* data, char* token);
typedef void						(*pfnEngSrc_COM_FreeFile_t)				(void* buffer);
typedef struct triangleapi_s* pTriAPI;
typedef struct efx_api_s* pEfxAPI;
typedef struct event_api_s* pEventAPI;
typedef struct demo_api_s* pDemoAPI;
typedef struct net_api_s* pNetAPI;
typedef struct IVoiceTweak_s* pVoiceTweak;
typedef int							(*pfnEngSrc_IsSpectateOnly_t) (void);
typedef struct model_s* (*pfnEngSrc_LoadMapSprite_t)			(const char* filename);
typedef void						(*pfnEngSrc_COM_AddAppDirectoryToSearchPath_t) (const char* pszBaseDir, const char* appName);
typedef int							(*pfnEngSrc_COM_ExpandFilename_t)				 (const char* fileName, char* nameOutBuffer, int nameOutBufferSize);
typedef const char* (*pfnEngSrc_PlayerInfo_ValueForKey_t)(int playerNum, const char* key);
typedef void						(*pfnEngSrc_PlayerInfo_SetValueForKey_t)(const char* key, const char* value);
typedef qboolean(*pfnEngSrc_GetPlayerUniqueID_t)(int iPlayer, char playerID[16]);
typedef int							(*pfnEngSrc_GetTrackerIDForPlayer_t)(int playerSlot);
typedef int							(*pfnEngSrc_GetPlayerForTrackerID_t)(int trackerID);
typedef int							(*pfnEngSrc_pfnServerCmdUnreliable_t)(char* szCmdString);
typedef void						(*pfnEngSrc_GetMousePos_t)(struct tagPOINT* ppt);
typedef void						(*pfnEngSrc_SetMousePos_t)(int x, int y);
typedef void						(*pfnEngSrc_SetMouseEnable_t)(qboolean fEnable);
typedef struct cvar_s* (*pfnEngSrc_GetFirstCVarPtr_t)();
typedef unsigned int				(*pfnEngSrc_GetFirstCmdFunctionHandle_t)();
typedef unsigned int				(*pfnEngSrc_GetNextCmdFunctionHandle_t)(unsigned int cmdhandle);
typedef const char* (*pfnEngSrc_GetCmdFunctionName_t)(unsigned int cmdhandle);
typedef float						(*pfnEngSrc_GetClientOldTime_t)();
typedef float						(*pfnEngSrc_GetServerGravityValue_t)();
typedef struct model_s* (*pfnEngSrc_GetModelByIndex_t)(int index);
typedef void						(*pfnEngSrc_pfnSetFilterMode_t)(int mode);
typedef void						(*pfnEngSrc_pfnSetFilterColor_t)(float r, float g, float b);
typedef void						(*pfnEngSrc_pfnSetFilterBrightness_t)(float brightness);
typedef sequenceEntry_s* (*pfnEngSrc_pfnSequenceGet_t)(const char* fileName, const char* entryName);
typedef void						(*pfnEngSrc_pfnSPR_DrawGeneric_t)(int frame, int x, int y, const struct rect_s* prc, int src, int dest, int w, int h);
typedef sentenceEntry_s* (*pfnEngSrc_pfnSequencePickSentence_t)(const char* sentenceName, int pickMethod, int* entryPicked);
// draw a complete string
typedef int							(*pfnEngSrc_pfnDrawString_t)		(int x, int y, const char* str, int r, int g, int b);
typedef int							(*pfnEngSrc_pfnDrawStringReverse_t)		(int x, int y, const char* str, int r, int g, int b);
typedef const char* (*pfnEngSrc_LocalPlayerInfo_ValueForKey_t)(const char* key);
typedef int							(*pfnEngSrc_pfnVGUI2DrawCharacter_t)		(int x, int y, int ch, unsigned int font);
typedef int							(*pfnEngSrc_pfnVGUI2DrawCharacterAdd_t)	(int x, int y, int ch, int r, int g, int b, unsigned int font);
typedef unsigned int		(*pfnEngSrc_COM_GetApproxWavePlayLength) (const char* filename);
typedef void* (*pfnEngSrc_pfnGetCareerUI_t)();
typedef void						(*pfnEngSrc_Cvar_Set_t)			(char* cvar, char* value);
typedef int							(*pfnEngSrc_pfnIsPlayingCareerMatch_t)();
typedef double						(*pfnEngSrc_GetAbsoluteTime_t) (void);
typedef void						(*pfnEngSrc_pfnProcessTutorMessageDecayBuffer_t)(int* buffer, int bufferLength);
typedef void						(*pfnEngSrc_pfnConstructTutorMessageDecayBuffer_t)(int* buffer, int bufferLength);
typedef void						(*pfnEngSrc_pfnResetTutorMessageDecayData_t)();
typedef void						(*pfnEngSrc_pfnFillRGBABlend_t)			(int x, int y, int width, int height, int r, int g, int b, int a);
typedef int						(*pfnEngSrc_pfnGetAppID_t)			(void);
typedef cmdalias_t* (*pfnEngSrc_pfnGetAliases_t)		(void);
typedef void					(*pfnEngSrc_pfnVguiWrap2_GetMouseDelta_t) (int* x, int* y);

// Pointers to the exported engine functions themselves
typedef struct cl_enginefuncs_s
{
	pfnEngSrc_pfnSPR_Load_t					pfnSPR_Load;
	pfnEngSrc_pfnSPR_Frames_t				pfnSPR_Frames;
	pfnEngSrc_pfnSPR_Height_t				pfnSPR_Height;
	pfnEngSrc_pfnSPR_Width_t				pfnSPR_Width;
	pfnEngSrc_pfnSPR_Set_t					pfnSPR_Set;
	pfnEngSrc_pfnSPR_Draw_t					pfnSPR_Draw;
	pfnEngSrc_pfnSPR_DrawHoles_t			pfnSPR_DrawHoles;
	pfnEngSrc_pfnSPR_DrawAdditive_t			pfnSPR_DrawAdditive;
	pfnEngSrc_pfnSPR_EnableScissor_t		pfnSPR_EnableScissor;
	pfnEngSrc_pfnSPR_DisableScissor_t		pfnSPR_DisableScissor;
	pfnEngSrc_pfnSPR_GetList_t				pfnSPR_GetList;
	pfnEngSrc_pfnFillRGBA_t					pfnFillRGBA;
	pfnEngSrc_pfnGetScreenInfo_t			pfnGetScreenInfo;
	pfnEngSrc_pfnSetCrosshair_t				pfnSetCrosshair;
	pfnEngSrc_pfnRegisterVariable_t			pfnRegisterVariable;
	pfnEngSrc_pfnGetCvarFloat_t				pfnGetCvarFloat;
	pfnEngSrc_pfnGetCvarString_t			pfnGetCvarString;
	pfnEngSrc_pfnAddCommand_t				pfnAddCommand;
	pfnEngSrc_pfnHookUserMsg_t				pfnHookUserMsg;
	pfnEngSrc_pfnServerCmd_t				pfnServerCmd;
	pfnEngSrc_pfnClientCmd_t				pfnClientCmd;
	pfnEngSrc_pfnGetPlayerInfo_t			pfnGetPlayerInfo;
	pfnEngSrc_pfnPlaySoundByName_t			pfnPlaySoundByName;
	pfnEngSrc_pfnPlaySoundByIndex_t			pfnPlaySoundByIndex;
	pfnEngSrc_pfnAngleVectors_t				pfnAngleVectors;
	pfnEngSrc_pfnTextMessageGet_t			pfnTextMessageGet;
	pfnEngSrc_pfnDrawCharacter_t			pfnDrawCharacter;
	pfnEngSrc_pfnDrawConsoleString_t		pfnDrawConsoleString;
	pfnEngSrc_pfnDrawSetTextColor_t			pfnDrawSetTextColor;
	pfnEngSrc_pfnDrawConsoleStringLen_t		pfnDrawConsoleStringLen;
	pfnEngSrc_pfnConsolePrint_t				pfnConsolePrint;
	pfnEngSrc_pfnCenterPrint_t				pfnCenterPrint;
	pfnEngSrc_GetWindowCenterX_t			GetWindowCenterX;
	pfnEngSrc_GetWindowCenterY_t			GetWindowCenterY;
	pfnEngSrc_GetViewAngles_t				GetViewAngles;
	pfnEngSrc_SetViewAngles_t				SetViewAngles;
	pfnEngSrc_GetMaxClients_t				GetMaxClients;
	pfnEngSrc_Cvar_SetValue_t				Cvar_SetValue;
	pfnEngSrc_Cmd_Argc_t					Cmd_Argc;
	pfnEngSrc_Cmd_Argv_t					Cmd_Argv;
	pfnEngSrc_Con_Printf_t					Con_Printf;
	pfnEngSrc_Con_DPrintf_t					Con_DPrintf;
	pfnEngSrc_Con_NPrintf_t					Con_NPrintf;
	pfnEngSrc_Con_NXPrintf_t				Con_NXPrintf;
	pfnEngSrc_PhysInfo_ValueForKey_t		PhysInfo_ValueForKey;
	pfnEngSrc_ServerInfo_ValueForKey_t		ServerInfo_ValueForKey;
	pfnEngSrc_GetClientMaxspeed_t			GetClientMaxspeed;
	pfnEngSrc_CheckParm_t					CheckParm;
	pfnEngSrc_Key_Event_t					Key_Event;
	pfnEngSrc_GetMousePosition_t			GetMousePosition;
	pfnEngSrc_IsNoClipping_t				IsNoClipping;
	pfnEngSrc_GetLocalPlayer_t				GetLocalPlayer;
	pfnEngSrc_GetViewModel_t				GetViewModel;
	pfnEngSrc_GetEntityByIndex_t			GetEntityByIndex;
	pfnEngSrc_GetClientTime_t				GetClientTime;
	pfnEngSrc_V_CalcShake_t					V_CalcShake;
	pfnEngSrc_V_ApplyShake_t				V_ApplyShake;
	pfnEngSrc_PM_PointContents_t			PM_PointContents;
	pfnEngSrc_PM_WaterEntity_t				PM_WaterEntity;
	pfnEngSrc_PM_TraceLine_t				PM_TraceLine;
	pfnEngSrc_CL_LoadModel_t				CL_LoadModel;
	pfnEngSrc_CL_CreateVisibleEntity_t		CL_CreateVisibleEntity;
	pfnEngSrc_GetSpritePointer_t			GetSpritePointer;
	pfnEngSrc_pfnPlaySoundByNameAtLocation_t	pfnPlaySoundByNameAtLocation;
	pfnEngSrc_pfnPrecacheEvent_t			pfnPrecacheEvent;
	pfnEngSrc_pfnPlaybackEvent_t			pfnPlaybackEvent;
	pfnEngSrc_pfnWeaponAnim_t				pfnWeaponAnim;
	pfnEngSrc_pfnRandomFloat_t				pfnRandomFloat;
	pfnEngSrc_pfnRandomLong_t				pfnRandomLong;
	pfnEngSrc_pfnHookEvent_t				pfnHookEvent;
	pfnEngSrc_Con_IsVisible_t				Con_IsVisible;
	pfnEngSrc_pfnGetGameDirectory_t			pfnGetGameDirectory;
	pfnEngSrc_pfnGetCvarPointer_t			pfnGetCvarPointer;
	pfnEngSrc_Key_LookupBinding_t			Key_LookupBinding;
	pfnEngSrc_pfnGetLevelName_t				pfnGetLevelName;
	pfnEngSrc_pfnGetScreenFade_t			pfnGetScreenFade;
	pfnEngSrc_pfnSetScreenFade_t			pfnSetScreenFade;
	pfnEngSrc_VGui_GetPanel_t				VGui_GetPanel;
	pfnEngSrc_VGui_ViewportPaintBackground_t	VGui_ViewportPaintBackground;
	pfnEngSrc_COM_LoadFile_t				COM_LoadFile;
	pfnEngSrc_COM_ParseFile_t				COM_ParseFile;
	pfnEngSrc_COM_FreeFile_t				COM_FreeFile;
	struct triangleapi_s* pTriAPI;
	struct efx_api_s* pEfxAPI;
	struct event_api_s* pEventAPI;
	struct demo_api_s* pDemoAPI;
	struct net_api_s* pNetAPI;
	struct IVoiceTweak_s* pVoiceTweak;
	pfnEngSrc_IsSpectateOnly_t				IsSpectateOnly;
	pfnEngSrc_LoadMapSprite_t				LoadMapSprite;
	pfnEngSrc_COM_AddAppDirectoryToSearchPath_t		COM_AddAppDirectoryToSearchPath;
	pfnEngSrc_COM_ExpandFilename_t			COM_ExpandFilename;
	pfnEngSrc_PlayerInfo_ValueForKey_t		PlayerInfo_ValueForKey;
	pfnEngSrc_PlayerInfo_SetValueForKey_t	PlayerInfo_SetValueForKey;
	pfnEngSrc_GetPlayerUniqueID_t			GetPlayerUniqueID;
	pfnEngSrc_GetTrackerIDForPlayer_t		GetTrackerIDForPlayer;
	pfnEngSrc_GetPlayerForTrackerID_t		GetPlayerForTrackerID;
	pfnEngSrc_pfnServerCmdUnreliable_t		pfnServerCmdUnreliable;
	pfnEngSrc_GetMousePos_t					pfnGetMousePos;
	pfnEngSrc_SetMousePos_t					pfnSetMousePos;
	pfnEngSrc_SetMouseEnable_t				pfnSetMouseEnable;
	pfnEngSrc_GetFirstCVarPtr_t				GetFirstCvarPtr;
	pfnEngSrc_GetFirstCmdFunctionHandle_t	GetFirstCmdFunctionHandle;
	pfnEngSrc_GetNextCmdFunctionHandle_t	GetNextCmdFunctionHandle;
	pfnEngSrc_GetCmdFunctionName_t			GetCmdFunctionName;
	pfnEngSrc_GetClientOldTime_t			hudGetClientOldTime;
	pfnEngSrc_GetServerGravityValue_t		hudGetServerGravityValue;
	pfnEngSrc_GetModelByIndex_t				hudGetModelByIndex;
	pfnEngSrc_pfnSetFilterMode_t			pfnSetFilterMode;
	pfnEngSrc_pfnSetFilterColor_t			pfnSetFilterColor;
	pfnEngSrc_pfnSetFilterBrightness_t		pfnSetFilterBrightness;
	pfnEngSrc_pfnSequenceGet_t				pfnSequenceGet;
	pfnEngSrc_pfnSPR_DrawGeneric_t			pfnSPR_DrawGeneric;
	pfnEngSrc_pfnSequencePickSentence_t		pfnSequencePickSentence;
	pfnEngSrc_pfnDrawString_t				pfnDrawString;
	pfnEngSrc_pfnDrawStringReverse_t				pfnDrawStringReverse;
	pfnEngSrc_LocalPlayerInfo_ValueForKey_t		LocalPlayerInfo_ValueForKey;
	pfnEngSrc_pfnVGUI2DrawCharacter_t		pfnVGUI2DrawCharacter;
	pfnEngSrc_pfnVGUI2DrawCharacterAdd_t	pfnVGUI2DrawCharacterAdd;
	pfnEngSrc_COM_GetApproxWavePlayLength	COM_GetApproxWavePlayLength;
	pfnEngSrc_pfnGetCareerUI_t				pfnGetCareerUI;
	pfnEngSrc_Cvar_Set_t					Cvar_Set;
	pfnEngSrc_pfnIsPlayingCareerMatch_t		pfnIsCareerMatch;
	pfnEngSrc_pfnPlaySoundVoiceByName_t	pfnPlaySoundVoiceByName;
	pfnEngSrc_pfnPrimeMusicStream_t		pfnPrimeMusicStream;
	pfnEngSrc_GetAbsoluteTime_t				GetAbsoluteTime;
	pfnEngSrc_pfnProcessTutorMessageDecayBuffer_t		pfnProcessTutorMessageDecayBuffer;
	pfnEngSrc_pfnConstructTutorMessageDecayBuffer_t		pfnConstructTutorMessageDecayBuffer;
	pfnEngSrc_pfnResetTutorMessageDecayData_t		pfnResetTutorMessageDecayData;
	pfnEngSrc_pfnPlaySoundByNameAtPitch_t	pfnPlaySoundByNameAtPitch;
	pfnEngSrc_pfnFillRGBABlend_t					pfnFillRGBABlend;
	pfnEngSrc_pfnGetAppID_t					pfnGetAppID;
	pfnEngSrc_pfnGetAliases_t				pfnGetAliasList;
	pfnEngSrc_pfnVguiWrap2_GetMouseDelta_t pfnVguiWrap2_GetMouseDelta;
} cl_enginefunc_t;