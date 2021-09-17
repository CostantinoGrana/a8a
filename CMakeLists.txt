cmake_minimum_required(VERSION 3.12)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

project(a8a)

if(MSVC)
    add_compile_definitions(-D_CRT_SECURE_NO_WARNINGS)
	set(FLEX_ADDITIONAL_FLAGS "--wincompat")
endif()

find_package(FLEX)
find_package(BISON)

FLEX_TARGET(lexer lexer.l "${CMAKE_CURRENT_BINARY_DIR}/lexer.c" COMPILE_FLAGS "${FLEX_ADDITIONAL_FLAGS} -L -i")
BISON_TARGET(parser parser.y "${CMAKE_CURRENT_BINARY_DIR}/parser.c" COMPILE_FLAGS "-d -l")

add_executable(a8a 
	lexer.l parser.y ${FLEX_lexer_OUTPUTS} ${BISON_parser_OUTPUTS}
	sym_tab.c sym_tab.h
	main.c
	)


