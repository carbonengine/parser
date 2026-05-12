# Copyright © 2022 CCP ehf.

cmake_minimum_required(VERSION 3.16)
project(carbon-parser VERSION 1.0.0)

include(cmake/CcpBuildConfigurations.cmake)
include(cmake/CcpTargetConfigurations.cmake)
include(cmake/CcpPackageConfigHelpers.cmake)
include(cmake/CcpMonolithVendorConfig.cmake)
include(CMakePackageConfigHelpers)

find_program(Lemon lemon REQUIRED)
find_program(Re2c re2c REQUIRED)

set(LEMON_DIRECTORY "")
cmake_path(GET Lemon PARENT_PATH LEMON_DIRECTORY)
set(LEMON_TEMPLATE_PATH ${LEMON_DIRECTORY})
cmake_path(APPEND LEMON_TEMPLATE_PATH "lempar.c")

set(GENERATED_SOURCES_DIR ${CMAKE_CURRENT_BINARY_DIR}/GeneratedSources)


ccp_add_library(CcpParser STATIC
    src/ccpparser.cpp
    src/codebuilder.cpp
    src/parserstate.cpp
    src/parser.y
    src/pch.cpp
    src/parserresult.cpp
    src/program.cpp
    src/scanner.re
    src/stdfunctions.cpp
    src/stringview.cpp
        
    ${GENERATED_SOURCES_DIR}/scanner.cpp
)
target_sources(CcpParser PRIVATE
    src/code.h
    src/codebuilder.h
    src/parserstate.h
    src/pch.h
    src/stdfunctions.h
)

target_precompile_headers(CcpParser PRIVATE src/pch.h)

target_include_directories(CcpParser
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        ${GENERATED_SOURCES_DIR}
)

add_custom_command(
    OUTPUT ${GENERATED_SOURCES_DIR}/parser.c ${GENERATED_SOURCES_DIR}/parser.h
    COMMAND ${CMAKE_COMMAND} -E make_directory ${GENERATED_SOURCES_DIR}
    COMMAND ${Lemon} -d${GENERATED_SOURCES_DIR} -T${LEMON_TEMPLATE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/src/parser.y
    MAIN_DEPENDENCY src/parser.y
)
add_custom_command(
    OUTPUT ${GENERATED_SOURCES_DIR}/scanner.cpp
    COMMAND ${CMAKE_COMMAND} -E make_directory ${GENERATED_SOURCES_DIR}
    COMMAND ${Re2c} -o ${GENERATED_SOURCES_DIR}/scanner.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/scanner.re
    MAIN_DEPENDENCY src/scanner.re
)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    option(BUILD_TESTING "Build and run tests. Enabled by default." ON)
    if(BUILD_TESTING)
        include(CTest)
        add_subdirectory(tests)
    endif()
endif()

# Provide an install target iff this is the top level project only
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    option(INSTALL_TO_MONOLITH "Install this package to a perforce branch vendor folder - provided for backwards compatability" OFF)

    install(DIRECTORY include/ TYPE INCLUDE)

    if(INSTALL_TO_MONOLITH)

        # Install rule to ensure that our runtime and linker files are in the expected, platform-specific folders
        install(
            TARGETS CcpParser
            CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES}
            ARCHIVE DESTINATION lib/${CCP_PLATFORM}/${CCP_ARCHITECTURE}/${CCP_TOOLSET}/
            LIBRARY DESTINATION lib/${CCP_PLATFORM}/${CCP_ARCHITECTURE}/${CCP_TOOLSET}/
        )

        # Install config file so that other projects know where to find libraries headers etc.
        configure_ccp_vendor_config_file(
            TARGET CcpParser
            DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/CcpParserConfig.cmake"
        )
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/CcpParserConfig.cmake DESTINATION .)
    else()

        install(
                TARGETS CcpParser
                EXPORT CcpParserTargets
                CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES}
                ARCHIVE DESTINATION lib/
        )

        install(
                EXPORT CcpParserTargets
                DESTINATION share/carbon-parser
                FILE carbon-parserConfig.cmake
        )
    endif()
endif()

