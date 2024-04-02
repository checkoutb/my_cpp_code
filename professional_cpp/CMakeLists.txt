cmake_minimum_required(VERSION 3.25)

project(professional_cpp)

# find_package(ECM "5.68.0" REQUIRED NO_MODULE)
# set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

# include(KDEInstallDirs)
# include(KDECMakeSettings)
# include(KDECompilerSettings NO_POLICY_SCOPE)
# include(ECMQtDeclareLoggingCategory)
# include(FeatureSummary)

# find_package(KDevPlatform 5.2.40 REQUIRED)

add_subdirectory(src)


# ecm_qt_install_logging_categories(
#     EXPORT PROFESSIONAL_CPP
#     FILE professional_cpp.categories
#     DESTINATION "${KDE_INSTALL_LOGGINGCATEGORIESDIR}"
# )

# feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)



########### mine #########

# https://cmake.org/cmake/help/latest/manual/cmake-cxxmodules.7.html
# g， 3.28 加入的。
# 我的 cmake version 3.27.7
# 而且，编译器需要 GCC 14
#       GCC 14 (for the in-development branch, after 2023-09-20) and newer
# 我的 gcc (GCC) 13.2.1 20231011 (Red Hat 13.2.1-4)



# 这个需要自定义 CMake 的 function的， 虽然他提供了， 但是 还是想找一个官方点的。
# # https://github.com/rafagaitan/cpp-modules-cmake/blob/main/CMakeLists.txt
#
# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)
# set(PREBUILT_MODULE_PATH ${CMAKE_BINARY_DIR}/modules)
#
# add_compile_options(-fmodules)
# add_compile_options(-stdlib=libc++)
# add_compile_options(-fbuiltin-module-map)
# add_compile_options(-fimplicit-module-maps)


add_executable(professional_cpp src/ch01/main.cpp)

# 这个放 add_executable 上面的话，会报错，无法找到 professional_cpp
# 确实， 我在 add_executable 增加 __a 后，这里也需要增加，不然 报错。
# 这个是 microsoft的，好像不能用于其他的IDE
# https://devblogs.microsoft.com/cppblog/cpp20-modules-in-cmake-with-vs/
# set_property(TARGET professional_cpp_aa PROPERTY CXX_STANDARD 20)


# 也没用。
# string(CONCAT CMAKE_EXPERIMENTAL_CXX_SCANDEP_SOURCE
#   "<CMAKE_CXX_COMPILER> <DEFINES> <INCLUDES> <FLAGS> <SOURCE>"
#   " -MT <DYNDEP_FILE> -MD -MF <DEP_FILE>"
#   " ${flags_to_scan_deps} -fdep-file=<DYNDEP_FILE> -fdep-output=<OBJECT>"
#   )
#
# set(CMAKE_EXPERIMENTAL_CXX_MODULE_MAP_FORMAT "gcc")
# set(CMAKE_EXPERIMENTAL_CXX_MODULE_MAP_FLAG
#   "${compiler_flags_for_module_map} -fmodule-mapper=<MODULE_MAP_FILE>")







