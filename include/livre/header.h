#pragma once

#include <stdint.h>
#include <string>

#ifdef VOXEL_BUILD
#   define DLLOUT __declspec(dllexport)
#else
#   define DLLOUT
#endif