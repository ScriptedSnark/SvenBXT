/**
 * Copyright - xWhitey ~ ScriptedSnark, 2022.
 * includes.hpp - basic includes for our project.
 */

#ifdef INCLUDES_HPP_RECURSE_GUARD
#error Recursive header files inclusion detected in includes.hpp
#else //INCLUDES_HPP_RECURSE_GUARD

#define INCLUDES_HPP_RECURSE_GUARD

#ifndef INCLUDES_HPP_GUARD
#define INCLUDES_HPP_GUARD
#pragma once

//WinAPI
#include <Windows.h>
#include <psapi.h>

//STL
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <functional>
#include <array>

//Patterns
#include "patterns.hpp"

//MinHook
#include "MinHook/MinHook.h"

//HLSDK
#include "hlsdk_mini.hpp"
#include "cl_util.h"

//OpenGL
#include <gl/GL.h>

#pragma comment(lib, "opengl32.lib")

#endif //INCLUDES_HPP_GUARD

#undef INCLUDES_HPP_RECURSE_GUARD
#endif //INCLUDES_HPP_RECURSE_GUARD
