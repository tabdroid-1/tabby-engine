#pragma once

#include "Tabby/Core/PlatformDetection.h"

#ifdef TB_PLATFORM_WINDOWS
#ifndef NOMINMAX
// See github.com/skypjack/entt/wiki/Frequently-Asked-Questions#warning-c4003-the-min-the-max-and-the-macro
#define NOMINMAX
#endif
#endif

#include <unordered_set>
#include <unordered_map>
#include <shared_mutex>
#include <filesystem>
#include <functional>
#include <typeindex>
#include <algorithm>
#include <optional>
#include <iostream>
#include <fstream>
#include <utility>
#include <sstream>
#include <cassert>
#include <cstdint>
#include <atomic>
#include <random>
#include <memory>
#include <vector>
#include <thread>
#include <string>
#include <cstdio>
#include <cmath>
#include <mutex>
#include <tuple>
#include <array>
#include <queue>
#include <any>
#include <bit>
#include <map>

#include <Tabby/Core/Base.h>
#include <Tabby/Debug/Instrumentor.h>
#include <Tabby/Core/Log/Log.h>
#include <Tabby/Core/Assert.h>
#include <Tabby/Core/UUID.h>

#ifdef TB_PLATFORM_WINDOWS
#include <windows.h>
#endif
