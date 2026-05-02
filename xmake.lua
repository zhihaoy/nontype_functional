option("modules", { defines = "NONTYPE_FUNCTIONAL_MODULE", default = false })

option("tests", { default = false })

target("nontype_functional")
    set_languages("c++23")
    if get_config("modules") then
        set_kind("moduleonly")
        add_files("modules/nontype_functional.cppm")
    else
        set_kind("headeronly")
    end
    add_headerfiles("include/(**.h)")
    add_includedirs("include", {public = true})
    add_options("modules")

if get_config("tests") then
    for _, test_name in ipairs("function_ref", "move_only_function", "function") do
        target(test_name)
            set_kind("binary")
            set_languages("c++23")

            add_files(path.join("tests", test_name, "**.cpp"))
            add_headerfiles(path.join("tests", test_name, "**.h"))
            add_includedirs("tests/include")

            add_cxflags("clang::-Wno-self-move", "clang::fsized-deallocation")
            add_deps("nontype_functional")
            add_options("modules")

            set_policy("build.c++.modules", get_config("modules"))
    end
end



