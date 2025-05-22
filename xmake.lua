set_plat("mingw")

add_rules("mode.debug", "mode.release")

set_languages("c99", "cxx20")

add_requires("fmt", {configs = {cxx20 = true}})
add_requires("sfml >=3.0.0", {configs = {window = true, graphics = true, audio = true}})

target("main")
    set_kind("binary")
    add_packages("fmt", "sfml")
    add_defines(
        "SFML_STATIC",
        "SFML_NO_DEPRECATED_WARNINGS"
    )
    add_cxxflags("-O2")
    add_includedirs("include")
    add_files(
        "src/main.cpp",
        "src/renderer.cpp",
        "src/wfc.cpp"
    )
    after_build(
        function (target)
            os.cp("assets", target:targetdir())
        end
    )
target_end()