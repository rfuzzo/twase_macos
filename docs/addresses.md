# Address table

Build: Feral 1.6.1 RC2, `CFBundleVersion 480285.103778`, `LC_UUID 392D6F66-9183-329E-8A98-8C90FC828326`.

Addresses are **unslid VAs** as shown in Binary Ninja (image base `0x100000000`); runtime address = VA + ASLR slide.
All of them are also named in the bndb.

## Lua

The binary contains **two** Lua 5.1 copies. The game scripting uses the one at `0x103f0xxxx`–`0x103f2xxxx` (base library `luaL_Reg` table at `0x104d84c70`). The other copy at `0x100e9xxxx` (table at `0x104b18698`) is Feral's; it is named `feralLua_*` in the bndb. Don't mix them.

| Name | PC RVA | Mac VA | Found via |
|---|---|---|---|
| luaL_loadbuffer | `0x012C7E20` | `0x103f137d8` | `luaB_loadstring`; verified `(L, buf, size, name)` → `lua_load` `0x103f2a35c` |
| lua_tolstring | `0x012C7380` | `0x103f283b8` | `luaB_print` |
| lua_settop | `0x012C7210` | `0x103f270f8` | `luaB_next` |
| lua_gettop | `0x012C67B0` | `0x103f270e8` | `luaB_pcall` |
| lua_getfield | `0x012C66D0` | `0x103f291d4` | `luaB_print` (`getfield(L, GLOBALSINDEX, "tostring")`) |
| lua_pcall | `0x012C6B00` | `0x103f2a100` | `luaB_pcall` |
| lua_next | `0x012C6A50` | `0x103f2a550` | `luaB_next` |
| lua_pushnil | `0x012C6CE0` | `0x103f28c80` | `luaB_next` |
| lua_pushvalue | `0x012C6D80` | `0x103f2755c` | `luaB_print` |
| lua_type | `0x012C74E0` | `0x103f27654` | `luaB_type` |
| lua_getmetatable | `0x012C6730` | `0x103f295bc` | `luaB_getmetatable` |
| lua_remove | `0x012C6F60` | `0x103f271a0` | `luaL_getmetafield` |

Also named: `lua_insert` `0x103f272a4`, `lua_pushboolean` `0x103f2903c`, `lua_pushstring` `0x103f28d48`, `lua_setfield` `0x103f29964`, `lua_call` `0x103f2a0a4`, `lua_rawget` `0x103f2931c`, `luaL_error` `0x103f11e80`, `luaB_print` `0x103f09540`.

## Lua log (PC `LuaLog` hook)

On Mac the Lua output goes through a global function pointer, the **sink**:

| Name | Mac VA | Notes |
|---|---|---|
| g_LuaLogSink | `0x1056d6588` | `void (*)(const char*)`; `print`, `out`, the error handler (`0x1038b1420`, `"Lua Error: [%s], %s\n%s\n"`) and `lua_writestring` `0x103f08c20` call it, or `fputs(stdout)` if it is null |
| SetLuaLogSink | `0x103f25de8` | `adrp x8; str x0, [x8, #0x588]; ret`. Called from both ScriptingEnv constructors (`0x102c40904`, `0x102c40e04`) |
| ScriptLog_NoOp | `0x1038af268` | the sink the game installs: a lone `ret` (logging stripped in retail) |

The sink function is a single instruction, so it can't take an inline hook. Instead:
1. Set `g_LuaLogSink` to our function at startup.
2. Patch the `str` in `SetLuaLogSink` at `0x103f25dec` to a `nop`, so the constructors can't reinstall the no-op. Expected bytes `00 c5 02 f9`, patch `1f 20 03 d5`.

## Script runtimes (PC `LuaGameEnvironment`)

| Name | PC RVA | Mac VA | Notes |
|---|---|---|---|
| GetLuaState | `0x1626AA0` | `0x1038b377c` | `lua_State* (void* scriptInterface)` = `GetScriptRuntime(this)->L`. Lazily creates the runtime, like PC |
| GetScriptRuntime | – | `0x1038b3794` | |
| RegisterScriptRuntime | – | `0x1038b261c` | creates `g_RuntimeLuaState` on first use and appends a `RuntimeLuaNode`. Also registers the `out` global |
| g_RuntimeLuaListHead | `0x01DA4230` | `0x10560b7c8` | `RuntimeLuaNode*` (first node, or `&sentinel` when empty) |
| g_RuntimeLuaListSentinel | `0x01DA4234` | `0x10560b7d0` | sentinel node; iterate `node = *head; while (node != sentinel) node = node->next` |
| g_RuntimeLuaState | `0x0291DE00` | `0x10560b7c0` | master state (unused by TWASE) |
| ScriptInterface_ctor | – | `0x1038b3690` | |
| g_ScriptingEnvArray | – | `0x105603548` | `{int capacity; int count; ScriptingEnv** data}`, filled by the ScriptingEnv constructors (not used yet) |

64-bit layouts (from the code above):

```cpp
struct ScriptInterface { void* vtable; ScriptRuntime* runtime; lua_State** parentState; uint64_t field_18;
                         void (*registerBindings)(lua_State*); uint32_t owned; };      // 0x30
struct ScriptRuntime   { lua_State* L; bool initialized; void* scriptInterface; uint64_t owned;
                         uint64_t field_20, field_28; };                                // 0x30
struct RuntimeLuaNode  { RuntimeLuaNode* prev; RuntimeLuaNode* next; ScriptRuntime* runtime;
                         lua_State* baseL; int registry_ref; };                          // 0x28
```

## Patches

| Name | PC RVA | Mac VA | Found via | Notes |
|---|---|---|---|---|
| BitSetCrashAddr (unit size patch) | `0x0091CB57` (`cmp edi,40h; jnb`) | `0x101a51c58` (`b.eq`) | xref to `"bitset test argument out of range"` (5 sites). Only this one has the `mov x8,#-1` mask → vcall(vtable+0x30, &mask) → list walk → `cmp x20,#0x40` shape | expected `40 26 00 54` (`b.eq 0x101a52120` → throw). Patch `80 00 00 54` (`b.eq 0x101a51c68`, include branch) |

## Still to find

`RunStartupPath`, `VFS_GetInstance`, `VFS_SearchFiles`, `CName_ctor`, `tw_free`, `ScriptingEnv` path fields (Phase 4); diplomacy and UI functions/offsets (Phase 5).
