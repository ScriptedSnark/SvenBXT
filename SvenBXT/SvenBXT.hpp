#pragma once

#include <Windows.h>
#include <iostream>
#include <string>
#include "Utils.hpp"
#include "ptrns.hpp"
#include "hlsdk_mini.hpp"
#include "hud_hook.h"
#include "conutils.hpp"
#include "MemUtils.h"
#include "stdio.h"

typedef struct SvenBXT {
    void AddHWStuff();
    void AddCLStuff();
    static void Main();
} SvenBXT;

extern SvenBXT* svenbxt = new SvenBXT();

