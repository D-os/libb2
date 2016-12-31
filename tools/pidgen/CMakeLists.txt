add_executable(pidgen
    idlstruct.cpp
    InterfaceRec.cpp
    AST.cpp
    main.cpp
    OutputCPP.cpp
    OutputI.cpp
    OutputUtil.cpp
    symbol.cpp
    symbolstack.cpp
    TypeBank.cpp
    yacc.y
    scanner.l
#    WSDL.cpp
#    WsdlOutput.cpp
)
target_link_libraries(pidgen utils)
