set(interfaceFiles
    "${CMAKE_CURRENT_SOURCE_DIR}/os/support/IContextManager.aidl"
    "${CMAKE_CURRENT_SOURCE_DIR}/os/support/ICatalog.aidl"
    "${CMAKE_CURRENT_SOURCE_DIR}/os/support/INode.aidl"
    "${CMAKE_CURRENT_SOURCE_DIR}/os/support/IDatum.aidl"
    "${CMAKE_CURRENT_SOURCE_DIR}/os/support/IIterable.aidl"
    "${CMAKE_CURRENT_SOURCE_DIR}/os/support/IIterator.aidl"
    "${CMAKE_CURRENT_SOURCE_DIR}/os/support/IRandomIterator.aidl"
    "${CMAKE_CURRENT_SOURCE_DIR}/os/support/IByteInput.aidl"
    "${CMAKE_CURRENT_SOURCE_DIR}/os/support/IByteOutput.aidl"
    "${CMAKE_CURRENT_SOURCE_DIR}/os/support/IByteSeekable.aidl"
)
set(interfaces_install_dir /headers/os)
install(FILES ${interfaceFiles} DESTINATION ${interfaces_install_dir})

foreach(IF ${interfaceFiles})
    file(RELATIVE_PATH REL_IF ${CMAKE_CURRENT_SOURCE_DIR} ${IF})
    get_filename_component(NAME_IF ${REL_IF} NAME_WE)
    get_filename_component(DIR_IF ${REL_IF} DIRECTORY)
    set(OUT_CPP "${CMAKE_CURRENT_BINARY_DIR}/${DIR_IF}/${NAME_IF}.cpp")
    add_custom_command(
        OUTPUT "${OUT_CPP}"
        COMMAND aidl-cpp -I"${CMAKE_CURRENT_SOURCE_DIR}" "${IF}" "${CMAKE_CURRENT_BINARY_DIR}" "${OUT_CPP}"
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
