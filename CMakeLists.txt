cmake_minimum_required(VERSION 3.16)
project(carbon-parser VERSION 1.0.0)

if(NOT DEFINED ENV{CCP_EVE_PERFORCE_BRANCH_PATH})
    message(FATAL_ERROR "Missing required environment variable CCP_EVE_PERFORCE_BRANCH_PATH")
elseif(NOT IS_ABSOLUTE $ENV{CCP_EVE_PERFORCE_BRANCH_PATH})
    message(FATAL_ERROR "CCP_EVE_PERFORCE_BRANCH_PATH needs to be an absolute path")
elseif(NOT EXISTS $ENV{CCP_EVE_PERFORCE_BRANCH_PATH})
    message(FATAL_ERROR "CCP_EVE_PERFORCE_BRANCH_PATH is pointing at a non-existing location $ENV{CCP_EVE_PERFORCE_BRANCH_PATH}")
elseif(NOT IS_DIRECTORY $ENV{CCP_EVE_PERFORCE_BRANCH_PATH})
    message(FATAL_ERROR "CCP_EVE_PERFORCE_BRANCH_PATH needs to be a directory")
else()
    message(STATUS "Using CCP_EVE_PERFORCE_BRANCH_PATH located at $ENV{CCP_EVE_PERFORCE_BRANCH_PATH}")
endif()

file(TO_CMAKE_PATH "$ENV{CCP_EVE_PERFORCE_BRANCH_PATH}" BRANCH_ROOT_DIR)
list(INSERT CMAKE_MODULE_PATH 0 ${BRANCH_ROOT_DIR}/cmake)

include(CcpGlobalSettings)
include(CcpPackageConfigHelpers)

find_package(Lemon REQUIRED NO_CMAKE_PATH)
find_package(re2c REQUIRED NO_CMAKE_PATH)

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
set(PUBLIC_HEADER_FILES
    include/ccpparser.h
    include/externals.h
    include/parserresult.h
    include/stringview.h
)
target_sources(CcpParser PUBLIC
    ${PUBLIC_HEADER_FILES}
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
    COMMAND ${LEMON_PATH} -d${GENERATED_SOURCES_DIR} -T${LEMON_TEMPLATE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/src/parser.y
    MAIN_DEPENDENCY src/parser.y
)
add_custom_command(
    OUTPUT ${GENERATED_SOURCES_DIR}/scanner.cpp
    COMMAND ${CMAKE_COMMAND} -E make_directory ${GENERATED_SOURCES_DIR}
    COMMAND ${RE2C_PATH} -o ${GENERATED_SOURCES_DIR}/scanner.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/scanner.re
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
    # Install rule for available public headers
    install(FILES ${PUBLIC_HEADER_FILES} DESTINATION include)

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
        DEPENDENCIES ""
    )
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/CcpParserConfig.cmake DESTINATION .)
endif()

