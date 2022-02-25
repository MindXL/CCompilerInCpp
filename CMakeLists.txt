cmake_minimum_required(VERSION 3.19)
project(CCompilerInCpp)

set(CMAKE_CXX_STANDARD 20)

add_executable(CCompilerInCpp main.cpp Lexer.cpp Lexer.h Parser.cpp Parser.h AstNode.cpp AstNode.h PrintVisitor.cpp PrintVisitor.h)