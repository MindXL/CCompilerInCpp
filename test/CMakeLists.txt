CMAKE_MINIMUM_REQUIRED(VERSION 3.19)
project(test)

set(CMAKE_CXX_STANDARD 20)

include_directories(. ../src)

add_executable(${PROJECT_NAME} main.cpp test.cpp ../src/Lexer.cpp ../src/Parser.cpp ../src/AstNode.cpp ../src/PrintVisitor.cpp)