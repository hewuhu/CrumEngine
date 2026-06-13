add_rules("mode.debug", "mode.release")

set_plat("mingw")
set_toolchains("clang")

set_policy("build.c++.modules", true)

set_languages("c++23")

set_runtimes("c++_static")

target("crumb_engine")
    set_kind("static")
    add_includedirs("engine/source", {public=true})
    add_files("engine/**.cppm",{public=true})
    add_files("engine/**.cpp")

    add_files("vendor/**.cpp")

    add_deps("vendor")


target("sandbox")
    set_kind("binary")
    add_files("sandbox/**.cpp")
    add_deps("crumb_engine")

    before_build(function (target)
        local slangc = "C:/third_party/vulkan/Bin/slangc.exe"
        local outdir = path.join(target:targetdir(), "assets/shaders")
        os.mkdir(outdir)
        for _, src in ipairs(os.files("engine/shader/*.slang")) do
            os.runv(slangc, {
                src,
                "-target", "spirv",
                "-profile", "spirv_1_4",
                "-emit-spirv-directly",
                "-fvk-use-entrypoint-name",
                "-entry", "VSMain", "-stage", "vertex",
                "-entry", "PSMain", "-stage", "fragment",
                "-o", path.join(outdir, path.basename(src) .. ".spv")
            })
        end
    end)

     after_build(function (target)
        os.cp("C:/third_party/sdl3/bin/SDL3.dll", target:targetdir())
        os.cp("$(projectdir)/sandbox/assets/player.png", path.join(target:targetdir(), "assets"))
        os.cp("$(projectdir)/sandbox/assets/config.ini", path.join(target:targetdir(), "assets"))
    end)

target("vendor")
    set_kind("moduleonly")
    add_files("vendor/**.cppm")

    add_links("spdlog", "fmt", {public=true})

    add_includedirs("C:/third_party/glm/include", {public=true})

    add_includedirs("C:/third_party/VulkanMemoryAllocator-Hpp/include",  {public=true})

    add_includedirs("C:/third_party/vulkan/Include", {public=true})
    add_linkdirs("C:/third_party/vulkan/Lib", {public=true})
    add_links("vulkan-1", {public=true})

    add_includedirs("C:/third_party/sdl3/include", {public=true})
    add_linkdirs("C:/third_party/sdl3/lib", {public=true})
    add_links("SDL3", {public=true})

    add_includedirs("C:/third_party/imgui", {public=true})

    add_includedirs("vendor/json11", {public=true})
    add_includedirs("vendor/stb", {public=true})

    -- add_includedirs("C:/third_party/glad/include", {public=true}) -- 暂时不使用
