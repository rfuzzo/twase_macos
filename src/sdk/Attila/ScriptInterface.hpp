#pragma once

#include "Lua/LuaDefs.hpp"

#include <cstddef>

namespace sdk::Attila
{
// created lazily by GetScriptRuntime (0x1038b3794)
struct ScriptRuntime
{
    lua_State* L;           // +0x00
    bool initialized;       // +0x08
    void* scriptInterface;  // +0x10
    uint64_t owned;         // +0x18
    uint64_t field_20;      // +0x20
    uint64_t field_28;      // +0x28
};
static_assert(sizeof(ScriptRuntime) == 0x30);

// ScriptInterface_ctor (0x1038b3690)
struct ScriptInterface
{
    void* vtable;                          // +0x00
    ScriptRuntime* runtime;                // +0x08
    lua_State** parentState;               // +0x10
    uint64_t field_18;                     // +0x18
    void (*registerBindings)(lua_State*);  // +0x20
    uint32_t owned;                        // +0x28
};
static_assert(offsetof(ScriptInterface, registerBindings) == 0x20);
static_assert(offsetof(ScriptInterface, owned) == 0x28);

// appended to g_RuntimeLuaListHead by RegisterScriptRuntime (0x1038b261c)
struct RuntimeLuaNode
{
    RuntimeLuaNode* prev;    // +0x00
    RuntimeLuaNode* next;    // +0x08
    ScriptRuntime* runtime;  // +0x10
    lua_State* baseL;        // +0x18
    int registry_ref;        // +0x20
};
static_assert(sizeof(RuntimeLuaNode) == 0x28);

} // namespace sdk::Attila
