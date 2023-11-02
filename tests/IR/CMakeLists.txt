cmake_minimum_required(VERSION 3.24)


add_executable(IRTests IRTests.cpp)

target_link_libraries(IRTests Compiler Codegen Parser Tokeniser IR AST Utils Print Colour)

add_test(NAME EmptyFunction COMMAND IRTests "EmptyFunction")
add_test(NAME IntegerTypes COMMAND IRTests "IntegerTypes")
add_test(NAME IntegerAssignment COMMAND IRTests "IntegerAssignment")
add_test(NAME IntegerAddition COMMAND IRTests "IntegerAddition")
add_test(NAME IntegerSubtraction COMMAND IRTests "IntegerSubtraction")
add_test(NAME IntegerMultiplication COMMAND IRTests "IntegerMultiplication")
add_test(NAME IntegerSignedDivision COMMAND IRTests "IntegerSignedDivision")









