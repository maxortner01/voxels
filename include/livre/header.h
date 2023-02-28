#pragma once

#include <stdint.h>

#ifdef VOXEL_BUILD
#   define DLLOUT __declspec(dllexport)
#else
#   define DLLOUT
#endif