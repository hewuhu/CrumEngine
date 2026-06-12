add_rules("mode.debug", "mode.release")

set_plat("mingw")
set_toolchains("clang")

set_policy("build.c++.modules", true)

set_languages("c++23")

set_runtimes("c++_static")

target("crumb_engine")
    set_kind("static")
    add_files("engine/**.cppm",{public=true})
    add_files("engine/**.cpp")

    add_files("vendor/**.cpp")

    add_deps("vendor")


target("sandbox")
    set_kind("binary")
    add_files("sandbox/**.cpp")
    add_deps("crumb_engine")

     after_build(function (target)
        os.cp("C:/third_party/sdl3/bin/SDL3.dll", target:targetdir())
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
    add_includedirs("C:/third_party/tinyobjloader/examples/viewer", {public=true})

    -- add_includedirs("C:/third_party/glad/include", {public=true}) -- 暂时不使用