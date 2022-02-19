#pragma once

#include <Windows.h>
#include <iostream>
#include <string>
#include "Utils.hpp"
#include "ptrns.hpp"
#include "MemUtils.h"
#include "stdio.h"

typedef struct SvenBXT {
    void AddBXTStuff();
    static void Main();
} SvenBXT;

extern SvenBXT* svenbxt = new SvenBXT();