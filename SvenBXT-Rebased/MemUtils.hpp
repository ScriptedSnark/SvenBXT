/**
 * Copyright - xWhitey, 2022.
 * MemUtils.hpp - Some utilities for DOS/NT images. (and maybe some others? XD)
 */

#ifdef MEMUTILS_HPP_RECURSE_GUARD
#error Recursive header files inclusion detected in MemUtils.hpp
#else //MEMUTILS_HPP_RECURSE_GUARD

#define MEMUTILS_HPP_RECURSE_GUARD

#ifndef MEMUTILS_HPP_GUARD
#define MEMUTILS_HPP_GUARD
#pragma once

//We don't want to overload MemUtils with useless STL.
#include <Windows.h>

typedef struct MemUtils {
	static unsigned long GetModuleSize(_In_ void* _Module) {
		IMAGE_DOS_HEADER* header = reinterpret_cast<IMAGE_DOS_HEADER*>(_Module);
		IMAGE_NT_HEADERS* nt_header = reinterpret_cast<IMAGE_NT_HEADERS*>(header + header->e_lfanew);
		return nt_header->OptionalHeader.SizeOfImage;
	}

	static unsigned long GetModuleBase(_In_ void* _Module) {
		IMAGE_DOS_HEADER* header = reinterpret_cast<IMAGE_DOS_HEADER*>(_Module);
		IMAGE_NT_HEADERS* nt_header = reinterpret_cast<IMAGE_NT_HEADERS*>(header + header->e_lfanew);
		return nt_header->OptionalHeader.ImageBase;
	}
} MemUtils;

#endif //MEMUTILS_HPP_GUARD

#undef MEMUTILS_HPP_RECURSE_GUARD
#endif //MEMUTILS_HPP_RECURSE_GUARD
