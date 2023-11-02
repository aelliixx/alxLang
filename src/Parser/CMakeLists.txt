cmake_minimum_required(VERSION 3.24)


add_library(Parser Parser.cpp ParseFunction.cpp
        ParseVariable.cpp
        ParseStatement.cpp
        ParseReturn.cpp
        ParseLiteral.cpp
        ParseBinaryExpression.cpp
        ParseConditionalStatement.cpp
        ParseUnaryExpression.cpp
        ParseClass.cpp
        ParseMemberExpression.cpp)