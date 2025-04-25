set_plat("mingw")

add_rules("mode.debug")

add_requires("fmt", {configs = {cxx17 = true}})

set_languages("c99", "cxx20")

target("main")
    set_kind("binary")
    set_toolchains("mingw")
    add_cxxflags("-O2")
    add_includedirs(
        "include"
    )
    add_files(
        "src/main.cpp",
        "src/wfc.cpp"
    )
    add_packages("fmt")
target_end()