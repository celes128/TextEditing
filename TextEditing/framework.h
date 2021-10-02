#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclure les en-têtes Windows rarement utilisés

#include <cstdint>
#include <cassert>

#define IN
#define OUT

using uint = unsigned int;
using u8 = uint8_t;

using Result = int;

const Result kSuccess = 0;
const Result kFailure = -1;

inline bool ok(int result) { return kSuccess == result; };
inline bool failed(int result) { return !ok(result); };