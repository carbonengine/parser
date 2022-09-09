set(GENERATED_SOURCES_DIR ${CMAKE_CURRENT_BINARY_DIR}/GeneratedSources)


add_library(CcpParser STATIC
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
target_sources(CcpParser PUBLIC
    include/ccpparser.h
    include/externals.h
    include/parserresult.h
    include/stringview.h
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


add_executable(CcpParserTest
    tests/basic.cpp
    tests/functions.cpp
    tests/helpers.cpp
    tests/operators.cpp
    tests/pch.cpp
    tests/variables.cpp
)
target_sources(CcpParserTest PRIVATE
    tests/helpers.h
    tests/pch.h
)

target_precompile_headers(CcpParserTest PRIVATE tests/pch.h)
target_link_libraries(CcpParserTest PRIVATE CcpParser GTest::GTest GTest::Main)
set_target_properties(CcpParserTest PROPERTIES FOLDER "Tests")

