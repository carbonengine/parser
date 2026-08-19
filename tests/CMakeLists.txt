# Copyright © 2022 CCP ehf.

find_package(GTest REQUIRED)

ccp_add_executable(CcpParserTest
    basic.cpp
    functions.cpp
    helpers.cpp
    operators.cpp
    pch.cpp
    variables.cpp
)
target_sources(CcpParserTest PRIVATE
    helpers.h
    pch.h
)

target_precompile_headers(CcpParserTest PRIVATE pch.h)
target_link_libraries(CcpParserTest PRIVATE CcpParser GTest::gtest GTest::gtest_main)
set_target_properties(CcpParserTest PROPERTIES FOLDER "Tests")
gtest_discover_tests(CcpParserTest)

if (WIN32)
    add_custom_command(
            TARGET CcpParserTest PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_RUNTIME_DLLS:CcpParserTest>
            $<TARGET_FILE_DIR:CcpParserTest>
            COMMAND_EXPAND_LISTS
            VERBATIM
    )
endif()
