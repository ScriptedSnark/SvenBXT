/**
 * Copyright - ScriptedSnark, 2022.
 * tri.hpp - client-side drawing through TriAPI
 */

#ifdef TRI_HPP_RECURSE_GUARD
#error Recursive header files inclusion detected in tri.hpp
#else //TRI_HPP_RECURSE_GUARD

#define TRI_HPP_RECURSE_GUARD

#ifndef TRI_HPP_GUARD
#define TRI_HPP_GUARD
#pragma once

#include "includes.hpp"

namespace CustomTri
{
	void DrawTriggers(triangleapi_s* pTriAPI);
}

#endif //TRI_HPP_GUARD

#undef TRI_HPP_RECURSE_GUARD
#endif //TRI_HPP_RECURSE_GUARD