#pragma once

#include <Windows.h>
#include <iostream>
#include <string>
#include "Utils.hpp"
#include "ptrns.hpp"
#include "hlsdk_mini.hpp"
#include "MemUtils.h"
#include "stdio.h"

typedef struct SvenBXT {
    void AddHWStuff();
    void AddCLStuff();
    static void Main();
} SvenBXT;

cl_enginefunc_t* pEngfuncs;

extern SvenBXT* svenbxt = new SvenBXT();

