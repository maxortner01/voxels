#pragma once

#ifdef VOXEL_BUILD
#   define DLLOUT __declspec(dllexport)
#else
#   define DLLOUT
#endif