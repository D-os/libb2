add_library(BinderShell SHARED
    BinderCommands.cpp
    bsh.cpp
    CV.cpp
    Parser.cpp
    SyntaxTree.cpp
    Test.cpp
    vm.cpp
)
set_target_properties(BinderShell PROPERTIES PREFIX "")
target_link_libraries(BinderShell PUBLIC ob)
