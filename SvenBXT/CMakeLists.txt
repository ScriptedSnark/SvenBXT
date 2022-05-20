set(PROJECT_NAME SvenBXT)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "cmd_wrapper.hpp"
    "conutils.cpp"
    "conutils.hpp"
    "cvars.cpp"
    "cvars.hpp"
    "DetoursUtils.cpp"
    "DetoursUtils.hpp"
    "hlsdk_mini.hpp"
    "hud_custom.cpp"
    "hud_custom.hpp"
    "hud_hook.h"
    "MemUtils.h"
    "MinHook/buffer.c"
    "MinHook/buffer.h"
    "MinHook/hde/hde32.c"
    "MinHook/hde/hde32.h"
    "MinHook/hde/hde64.c"
    "MinHook/hde/hde64.h"
    "MinHook/hde/pstdint.h"
    "MinHook/hde/table32.h"
    "MinHook/hde/table64.h"
    "MinHook/hook.c"
    "MinHook/MinHook.h"
    "MinHook/trampoline.c"
    "MinHook/trampoline.h"
    "opengl_utils.cpp"
    "opengl_utils.hpp"
    "patterns.hpp"
    "patterns_macros.hpp"
    "pengfuncs.h"
    "ptrns.hpp"
    "sptlib-stdafx.hpp"
    "sven_sdk.h"
    "SvenBXT.cpp"
    "SvenBXT.hpp"
    "Utils.hpp"
)
source_group("" FILES ${no_group_source_files})

set(udis86
    "udis86/src/decode.c"
    "udis86/src/itab.c"
    "udis86/src/syn-att.c"
    "udis86/src/syn-intel.c"
    "udis86/src/syn.c"
    "udis86/src/udis86.c"
)
source_group("udis86" FILES ${udis86})

set(ALL_FILES
    ${no_group_source_files}
    ${udis86}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} SHARED ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE SvenBXT)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
)
################################################################################
# Compile definitions
################################################################################
target_compile_definitions(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Debug>:"
        "_DEBUG"
    ">"
    "$<$<CONFIG:Release>:"
        "NDEBUG"
    ">"
    "WIN32;"
    "IDK_EXPORTS;"
    "_WINDOWS;"
    "_USRDLL;"
    "UNICODE;"
    "_UNICODE"
)

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:
            /Oi;
            /Gy
        >
        /permissive-;
        /MP;
        /std:c++17;
        /sdl-;
        /W3;
        ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
        ${DEFAULT_CXX_EXCEPTION_HANDLING}
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /DEBUG;
            /INCREMENTAL
        >
        $<$<CONFIG:Release>:
            /OPT:REF;
            /OPT:ICF;
            /INCREMENTAL:NO
        >
        /SUBSYSTEM:WINDOWS
    )
endif()

################################################################################
# Dependencies
################################################################################
set(ADDITIONAL_LIBRARY_DEPENDENCIES
    "opengl32;"
    "kernel32;"
    "user32;"
    "gdi32;"
    "winspool;"
    "comdlg32;"
    "advapi32;"
    "shell32;"
    "ole32;"
    "oleaut32;"
    "uuid;"
    "odbc32;"
    "odbccp32;"
    "OpenGL32"
)
target_link_libraries(${PROJECT_NAME} PUBLIC "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

