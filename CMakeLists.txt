cmake_minimum_required(VERSION 3.19)
#project(CCompilerInCpp)
project(ccc)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} src/main.cpp src/Lexer.cpp src/Lexer.h src/Parser.cpp src/Parser.h src/AstNode.cpp src/AstNode.h src/PrintVisitor.cpp src/PrintVisitor.h src/CodeGenerator.cpp src/CodeGenerator.h src/diagnose.h)