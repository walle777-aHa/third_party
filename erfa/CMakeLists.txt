# CMakeLists.txt - 编译 ERFA 为动态库和静态库
cmake_minimum_required(VERSION 3.10)
project(erfa_build
    VERSION 2.0.0
    LANGUAGES C
    DESCRIPTION "ERFA (Essential Routines for Fundamental Astronomy) library build"
)

# 设置项目选项
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_STATIC_LIBS "Build static libraries" ON)
option(BUILD_TESTING "Build test programs" OFF)
option(INSTALL_LIBRARY "Install library after build" ON)

# 设置 C 标准
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Windows 特定设置
if(WIN32)
    # 设置运行时库（MT/MTd/MD/MDd）
    if(MSVC)
        # 默认使用动态链接运行时库
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
        
        # 禁用特定警告
        add_compile_options(/wd4244)  # 从 'type1' 转换到 'type2'，可能丢失数据
        add_compile_options(/wd4996)  # 函数或变量可能不安全
        
        # 设置默认编译类型
        if(NOT CMAKE_BUILD_TYPE)
            set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
        endif()
        
        # 根据编译类型设置优化选项
        string(TOUPPER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_UPPER)
        if(BUILD_TYPE_UPPER STREQUAL "RELEASE")
            # add_compile_options(/O2 /Ob2 /DNDEBUG)
        elseif(BUILD_TYPE_UPPER STREQUAL "DEBUG")
            # add_compile_options(/Od /Ob0 /Zi)
        endif()
    endif()
    
    # 定义导出宏
    add_definitions(-DERFA_DLL_BUILD)
endif()

# 添加包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# 查找源文件
file(GLOB_RECURSE ERFA_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

# 如果需要，排除特定文件
set(EXCLUDED_SOURCES "")
list(FILTER ERFA_SOURCES EXCLUDE REGEX ".*test.*\\.c$")
list(FILTER ERFA_SOURCES EXCLUDE REGEX ".*example.*\\.c$")

message(STATUS "找到 ${ERFA_SOURCES} 个源文件")

# 创建静态库
if(BUILD_STATIC_LIBS)
    add_library(erfa_static STATIC ${ERFA_SOURCES})
    
    # 设置静态库属性
    set_target_properties(erfa_static PROPERTIES
        OUTPUT_NAME "erfa"
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
    )
    
    # Windows 下设置静态库后缀
    if(WIN32)
        set_target_properties(erfa_static PROPERTIES
            OUTPUT_NAME "erfa_static"
            PREFIX "lib"
            SUFFIX ".lib"
        )
    else()
        set_target_properties(erfa_static PROPERTIES
            OUTPUT_NAME "erfa"
            PREFIX "lib"
            SUFFIX ".a"
        )
    endif()
    
    # 添加定义（静态库不需要导出符号）
    target_compile_definitions(erfa_static PRIVATE 
        ERFA_STATIC_DEFINE
        $<$<CONFIG:Debug>:ERFA_DEBUG>
    )
    
    # 设置包含目录
    target_include_directories(erfa_static PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:/src>
    )
endif()

# 创建动态库
if(BUILD_SHARED_LIBS)
    add_library(erfa_shared SHARED ${ERFA_SOURCES})
    
    # 设置动态库属性
    set_target_properties(erfa_shared PROPERTIES
        OUTPUT_NAME "erfa"
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
    )
    
    # Windows 下设置导出定义
    if(WIN32)
        # 添加导出宏定义
        target_compile_definitions(erfa_shared PRIVATE 
            ERFA_EXPORTS
            $<$<CONFIG:Debug>:ERFA_DEBUG>
        )
        
        # 设置 Windows DLL 属性
        set_target_properties(erfa_shared PROPERTIES
            WINDOWS_EXPORT_ALL_SYMBOLS OFF
            SUFFIX ".dll"
        )
    else()
        # Linux/macOS 设置
        set_target_properties(erfa_shared PROPERTIES
            PREFIX "lib"
            SUFFIX ".so"
        )
    endif()
    
    # 设置包含目录
    target_include_directories(erfa_shared PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:/src>
    )
endif()

# 安装目标
# if(INSTALL_LIBRARY)
#     # 安装头文件
#     install(DIRECTORY include/ DESTINATION include
#             FILES_MATCHING PATTERN "*.h")
    
#     # 安装静态库
#     if(BUILD_STATIC_LIBS)
#         install(TARGETS erfa_static
#                 ARCHIVE DESTINATION lib
#                 LIBRARY DESTINATION lib
#                 RUNTIME DESTINATION bin)
#     endif()
    
#     # 安装动态库
#     if(BUILD_SHARED_LIBS)
#         install(TARGETS erfa_shared
#                 ARCHIVE DESTINATION lib
#                 LIBRARY DESTINATION lib
#                 RUNTIME DESTINATION bin)
#     endif()
    
#     # 生成配置文件
#     include(CMakePackageConfigHelpers)
#     write_basic_package_version_file(
#         "${CMAKE_CURRENT_BINARY_DIR}/erfa-config-version.cmake"
#         VERSION ${PROJECT_VERSION}
#         COMPATIBILITY SameMajorVersion
#     )
    
#     install(FILES
#         "${CMAKE_CURRENT_BINARY_DIR}/erfa-config-version.cmake"
#         DESTINATION lib/cmake/erfa
#     )
    
#     # 导出目标
#     # install(EXPORT erfa-targets
#     #         FILE erfa-targets.cmake
#     #         NAMESPACE erfa::
#     #         DESTINATION lib/cmake/erfa)
# endif()

# 添加测试（可选）
# if(BUILD_TESTING)
#     enable_testing()
    
#     # 查找测试文件
#     file(GLOB TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/test/*.c")
    
#     if(TEST_SOURCES)
#         foreach(test_source ${TEST_SOURCES})
#             get_filename_component(test_name ${test_source} NAME_WE)
            
#             add_executable(${test_name}_test ${test_source})
            
#             # 链接库
#             if(BUILD_SHARED_LIBS)
#                 target_link_libraries(${test_name}_test erfa_shared)
#             elseif(BUILD_STATIC_LIBS)
#                 target_link_libraries(${test_name}_test erfa_static)
#             endif()
            
#             add_test(NAME ${test_name}_test COMMAND ${test_name}_test)
#         endforeach()
#     endif()
# endif()

# 输出信息
message(STATUS "ERFA 构建配置:")
message(STATUS "  构建静态库: ${BUILD_STATIC_LIBS}")
message(STATUS "  构建动态库: ${BUILD_SHARED_LIBS}")
message(STATUS "  安装库: ${INSTALL_LIBRARY}")
message(STATUS "  构建测试: ${BUILD_TESTING}")
message(STATUS "  C 标准: ${CMAKE_C_STANDARD}")
if(MSVC)
    message(STATUS "  MSVC 运行时: ${CMAKE_MSVC_RUNTIME_LIBRARY}")
endif()