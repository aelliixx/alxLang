cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 20)

add_executable(Arithmetic Arithmetic.cpp)

target_link_libraries(Arithmetic Compiler Codegen Parser Tokeniser AST Utils Print Colour)



add_test(NAME Addition COMMAND Arithmetic "0")
add_test(NAME Subtraction COMMAND Arithmetic "1")

add_test(NAME BoolToLongZeroExtension COMMAND Arithmetic "6")
add_test(NAME CharToLongSignedExtension COMMAND Arithmetic "5")
add_test(NAME ShortToLongSignedExtension COMMAND Arithmetic "2")
add_test(NAME IntToLongSignedExtension COMMAND Arithmetic "3")
add_test(NAME LongToLong COMMAND Arithmetic "25")


add_test(NAME BoolToIntZeroExtension COMMAND Arithmetic "8")
add_test(NAME CharToIntSignedExtension COMMAND Arithmetic "10")
add_test(NAME ShortToIntSignedExtension COMMAND Arithmetic "12")
add_test(NAME IntToInt COMMAND Arithmetic "26")
add_test(NAME LongToInt COMMAND Arithmetic "4")

add_test(NAME BoolToShortZeroExtension COMMAND Arithmetic "20")
add_test(NAME CharToShortZeroExtension COMMAND Arithmetic "21")
add_test(NAME ShortToShortZeroExtension COMMAND Arithmetic "22")
add_test(NAME IntToShort COMMAND Arithmetic "23")
add_test(NAME LongToShort COMMAND Arithmetic "24")

add_test(NAME BoolToCharZeroExtension COMMAND Arithmetic "15")
add_test(NAME CharToCharZeroExtension COMMAND Arithmetic "18")
add_test(NAME ShortToCharZeroExtension COMMAND Arithmetic "16")
add_test(NAME IntToChar COMMAND Arithmetic "7")
add_test(NAME LongToChar COMMAND Arithmetic "17")

add_test(NAME BoolToBoolZeroExtension COMMAND Arithmetic "19")
add_test(NAME CharToBoolSetByte COMMAND Arithmetic "11")
add_test(NAME ShortToBoolSetByte COMMAND Arithmetic "13")
add_test(NAME IntToBoolSetByte COMMAND Arithmetic "9")
add_test(NAME LongToBoolSetByte COMMAND Arithmetic "14")


add_test(NAME BinaryAssignment_BoolToLongZeroExtension COMMAND Arithmetic "6_binary_assignment")
add_test(NAME BinaryAssignment_CharToLongSignedExtension COMMAND Arithmetic "5_binary_assignment")
add_test(NAME BinaryAssignment_ShortToLongSignedExtension COMMAND Arithmetic "2_binary_assignment")
add_test(NAME BinaryAssignment_IntToLongSignedExtension COMMAND Arithmetic "3_binary_assignment")
add_test(NAME BinaryAssignment_LongToLongSignedExtension COMMAND Arithmetic "25_binary_assignment")

add_test(NAME BinaryAssignment_BoolToIntZeroExtension COMMAND Arithmetic "8_binary_assignment")
add_test(NAME BinaryAssignment_CharToIntSignedExtension COMMAND Arithmetic "10_binary_assignment")
add_test(NAME BinaryAssignment_ShortToIntSignedExtension COMMAND Arithmetic "12_binary_assignment")
add_test(NAME BinaryAssignment_LongToInt COMMAND Arithmetic "4_binary_assignment")
add_test(NAME BinaryAssignment_IntToInt COMMAND Arithmetic "26_binary_assignment")

add_test(NAME BinaryAssignment_BoolToShortZeroExtension COMMAND Arithmetic "20_binary_assignment")
add_test(NAME BinaryAssignment_CharToShortZeroExtension COMMAND Arithmetic "21_binary_assignment")
add_test(NAME BinaryAssignment_ShortToShortZeroExtension COMMAND Arithmetic "22_binary_assignment")
add_test(NAME BinaryAssignment_IntToShort COMMAND Arithmetic "23_binary_assignment")
add_test(NAME BinaryAssignment_LongToShort COMMAND Arithmetic "24_binary_assignment")

add_test(NAME BinaryAssignment_BoolToCharZeroExtension COMMAND Arithmetic "15_binary_assignment")
add_test(NAME BinaryAssignment_CharToCharZeroExtension COMMAND Arithmetic "18_binary_assignment")
add_test(NAME BinaryAssignment_ShortToCharZeroExtension COMMAND Arithmetic "16_binary_assignment")
add_test(NAME BinaryAssignment_IntToChar COMMAND Arithmetic "7_binary_assignment")
add_test(NAME BinaryAssignment_LongToChar COMMAND Arithmetic "17_binary_assignment")

add_test(NAME BinaryAssignment_BoolToBoolZeroExtension COMMAND Arithmetic "19_binary_assignment")
add_test(NAME BinaryAssignment_CharToBoolSetByte COMMAND Arithmetic "11_binary_assignment")
add_test(NAME BinaryAssignment_ShortToBoolSetByte COMMAND Arithmetic "13_binary_assignment")
add_test(NAME BinaryAssignment_IntToBoolSetByte COMMAND Arithmetic "9_binary_assignment")
add_test(NAME BinaryAssignment_LongToBoolSetByte COMMAND Arithmetic "14_binary_assignment")



