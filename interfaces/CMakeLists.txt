file(GLOB_RECURSE interfaceFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.aidl")
set(interfaces_install_dir /headers/os)
install(FILES ${interfaceFiles} DESTINATION ${interfaces_install_dir})

foreach(IF ${interfaceFiles})
    file(RELATIVE_PATH REL_IF ${CMAKE_CURRENT_SOURCE_DIR} ${IF})
    get_filename_component(NAME_IF ${REL_IF} NAME_WE)
    get_filename_component(DIR_IF ${REL_IF} DIRECTORY)
    set(OUT_CPP "${CMAKE_CURRENT_BINARY_DIR}/${DIR_IF}/${NAME_IF}.cpp")
    add_custom_command(
        OUTPUT "${OUT_CPP}"
        COMMAND aidl-cpp "${IF}" "${CMAKE_CURRENT_BINARY_DIR}" "${OUT_CPP}"
        MAIN_DEPENDENCY "${IF}"
        DEPENDS aidl-cpp
        COMMENT "Compiling AIDL ${REL_IF}"
        )
    list (APPEND interfaceSources "${OUT_CPP}")
endforeach()

file(GLOB_RECURSE interfaceHeaders "${CMAKE_CURRENT_BINARY_DIR}/*.h")
install(FILES ${interfaceHeaders} DESTINATION ${interfaces_install_dir})

set(interfaceFiles ${interfaceFiles} PARENT_SCOPE)
set(interfaceSources ${interfaceSources} PARENT_SCOPE)
set(interfaceHeaders ${interfaceHeaders} PARENT_SCOPE)

add_library(interfaces STATIC ${interfaceSources})
set_property(TARGET interfaces PROPERTY POSITION_INDEPENDENT_CODE ON)
