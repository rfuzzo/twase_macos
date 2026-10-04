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

// MOD LOADER
// void* (*)(ScriptingEnv* self), EPISODIC_SCRIPTING_ENV virtual slot 3, loads and runs campaigns/<name>/scripting.lua
constexpr uint64_t RunStartupPath = 0x102170104;

// VFS
// void* (*)(), returns the static VFS instance
constexpr uint64_t VFS_GetInstance = 0x10387C0A8;

// void (*)(void* vfs, CName* dir, CName* pattern, VFSSearchResults* out, int flags, int mode), a virtual of the VFS
constexpr uint64_t VFS_SearchFiles_VtableOffset = 0x58;

// CName* (*)(CName* self, const char* str)
constexpr uint64_t CName_ctor = 0x10109240C;

// void (*)(void* ptr), the game allocator's free
constexpr uint64_t tw_free = 0x1010ACB70;

// PATCHES
constexpr uint64_t BitSetCrashAddr = 0x101A51C58;

} // namespace sdk::Attila::Addresses
