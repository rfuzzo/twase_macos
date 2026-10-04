#pragma once

#include <cstdint>

// Unslid addresses as shown in Binary Ninja (image base 0x100000000), resolve with Image::Resolve.
// See docs/addresses.md for how each one was found.
namespace sdk::Attila::Addresses
{
// Feral 1.6.1 RC2 (CFBundleVersion 480285.103778)
constexpr const char* SupportedUUID = "392D6F66-9183-329E-8A98-8C90FC828326";

// STARTUP
// int64_t (*)(void* instance, void* prevInstance, const char16_t* commandLine), CA's WinMain on the WinMain thread
constexpr uint64_t WinMain = 0x1032BC10C;

// GAME LOOP
// bool (*)(void* app, int, int, int, int), called once per frame by the run loop on the WinMain (game/Lua) thread
constexpr uint64_t GameTick = 0x1010ECD24;

// LUA LOG
// void (*)(const char*), called by print, out and the Lua error handler, falls back to fputs(stdout) if null
constexpr uint64_t g_LuaLogSink = 0x1056D6588;

// void (*)(void (*sink)(const char*)), the str instruction that stores the sink
constexpr uint64_t SetLuaLogSink_Store = 0x103F25DEC;

// SCRIPT RUNTIMES
// Linked list of all active lua states
constexpr uint64_t g_RuntimeLuaListHead = 0x10560B7C8;
constexpr uint64_t g_RuntimeLuaListSentinel = 0x10560B7D0;

// lua_State* (*)(void* scriptInterface)
constexpr uint64_t GetLuaState = 0x1038B377C;

// PATCHES
constexpr uint64_t BitSetCrashAddr = 0x101A51C58;

} // namespace sdk::Attila::Addresses
