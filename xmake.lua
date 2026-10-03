set_project("twase")
set_arch("arm64")

set_languages("c++23")
set_optimize("faster")
set_warnings("allextra")
set_encodings("utf-8")

add_rules("mode.debug", "mode.release")

-- Game root for post-build deployment (can be overridden with the TWASE_GAMEROOT env var)
local gameroot = os.getenv("TWASE_GAMEROOT") or
    path.join(os.getenv("HOME"), "Library/Application Support/Steam/steamapps/common/Total War Attila")

add_requires("fmt", "toml11", "dobby")
add_requires("spdlog", {configs = {fmt_external = true}})

if is_mode("debug") then
    add_defines("_DEBUG")
end

option("disable_copy", {default = false, description = "Disable copy build files to game directory after build."})

target("twase")
    set_basename("TWASE")
    set_kind("shared")

    -- files
    add_files("src/dylib/*.cpp")
    add_headerfiles("src/dylib/*.hpp")
    for _, dir in ipairs(os.dirs("src/dylib/*")) do
        add_files(path.join(dir, "*.cpp"))
        add_headerfiles(path.join(dir, "*.hpp"))
    end
    add_includedirs("src/dylib")

    -- sdk
    add_headerfiles("src/sdk/Attila/*.hpp")

    -- precompiled header
    set_pcxxheader("src/dylib/stdafx.hpp")

    -- links
    add_packages("fmt", "spdlog", "toml11", "dobby")
    -- Foundation makes dyld initialize it before our constructor runs
    add_frameworks("CoreFoundation", "Foundation")

    -- Post-build: copy libTWASE.dylib to <gameroot>/TWASE and the launcher to <gameroot>
    after_build(function (target)
        if has_config("disable_copy") then
            print("[twase] Skipping copy to game directory (disable_copy is enabled).")
            return
        end

        -- Only proceed if gameroot exists to avoid accidental folder creation in a wrong path
        if not os.isdir(gameroot) then
            print(string.format("[twase] Gameroot '%s' not found; skipping deploy copy.", gameroot))
            return
        end

        local destdir = path.join(gameroot, "TWASE")
        os.mkdir(destdir)

        local dylib = target:targetfile()
        os.cp(dylib, destdir)
        print(string.format("[twase] Copied %s -> %s", dylib, destdir))

        local launcher = path.join(os.projectdir(), "scripts", "twase-launch.command")
        os.cp(launcher, gameroot)
        os.exec("chmod +x \"%s\"", path.join(gameroot, "twase-launch.command"))
        print(string.format("[twase] Copied %s -> %s", launcher, gameroot))
    end)

    on_run(function (target)
        os.execv(path.join(gameroot, "twase-launch.command"), {})
    end)
