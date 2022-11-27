#ifdef CL_UTIL_H_RECURSE_GUARD
#error Recursive header files inclusion detected in cl_util.h
#else //CL_UTIL_H_RECURSE_GUARD

#define CL_UTIL_H_RECURSE_GUARD

#ifndef CL_UTIL_H_GUARD
#define CL_UTIL_H_GUARD
#pragma once

extern cl_enginefunc_t* pEngfuncs;

inline float CVAR_GET_FLOAT(const char* x) { return pEngfuncs->pfnGetCvarFloat((char*)x); }
inline char* CVAR_GET_STRING(const char* x) { return pEngfuncs->pfnGetCvarString((char*)x); }
inline struct cvar_s* CVAR_CREATE(const char* cv, const char* val, const int flags) { return pEngfuncs->pfnRegisterVariable((char*)cv, (char*)val, flags); }

#endif //CL_UTIL_H_GUARD

#undef CL_UTIL_H_RECURSE_GUARD
#endif //CL_UTIL_H_RECURSE_GUARD