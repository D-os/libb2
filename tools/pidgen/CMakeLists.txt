find_package(BISON)
find_package(FLEX)

BISON_TARGET(yacc_y
    "yacc.y" ${CMAKE_CURRENT_BINARY_DIR}/yacc_y.cpp)
FLEX_TARGET(scanner_l
    "scanner.l" ${CMAKE_CURRENT_BINARY_DIR}/scanner_l.cpp)
ADD_FLEX_BISON_DEPENDENCY(scanner_l yacc_y)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

add_executable(pidgen
    ${BISON_yacc_y_OUTPUTS}
    ${FLEX_scanner_l_OUTPUTS}
    "idlstruct.cpp"
    "InterfaceRec.cpp"
    "AST.cpp"
    "main.cpp"
    "OutputCPP.cpp"
    "OutputI.cpp"
    "OutputUtil.cpp"
    "symbol.cpp"
    "symbolstack.cpp"
    "TypeBank.cpp"
    "yacc.y"
    "scanner.l"
    "WSDL.cpp"
    "WsdlOutput.cpp"
)
target_compile_options(pidgen PRIVATE
${GLOBAL_CFLAGS} -frtti
# Tragically, the code is riddled with unused parameters.
-Wno-unused-parameter
# yacc dumps a lot of code *just in case*.
-Wno-unused-function
-Wno-unneeded-internal-declaration
# yacc is a tool from a more civilized age.
-Wno-deprecated-register
# yacc also has a habit of using char* over const char*.
-Wno-writable-strings
)
target_link_libraries(pidgen PUBLIC ob_bootstrap)
