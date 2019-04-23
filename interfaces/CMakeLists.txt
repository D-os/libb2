set(interfaceFiles
    "${CMAKE_CURRENT_SOURCE_DIR}/app/ICommand.idl"
    "${CMAKE_CURRENT_SOURCE_DIR}/app/IProgress.idl"
    "${CMAKE_CURRENT_SOURCE_DIR}/app/ITerminalView.idl"
    "${CMAKE_CURRENT_SOURCE_DIR}/package/IInstallHandler.idl"
    "${CMAKE_CURRENT_SOURCE_DIR}/services/IInformant.idl"
    "${CMAKE_CURRENT_SOURCE_DIR}/storage/IReferable.idl"
    "${CMAKE_CURRENT_SOURCE_DIR}/support/ICatalogPermissions.idl"
    "${CMAKE_CURRENT_SOURCE_DIR}/support/ICatalog.idl"
    "${CMAKE_CURRENT_SOURCE_DIR}/support/IDatum.idl"
    "${CMAKE_CURRENT_SOURCE_DIR}/support/IIterable.idl"
    "${CMAKE_CURRENT_SOURCE_DIR}/support/IIterator.idl"
    "${CMAKE_CURRENT_SOURCE_DIR}/support/INib.idl"
    "${CMAKE_CURRENT_SOURCE_DIR}/support/INode.idl"
    "${CMAKE_CURRENT_SOURCE_DIR}/support/INodeObserver.idl"
    "${CMAKE_CURRENT_SOURCE_DIR}/support/IProcess.idl"
    "${CMAKE_CURRENT_SOURCE_DIR}/support/IProcessManager.idl"
    "${CMAKE_CURRENT_SOURCE_DIR}/support/IRandomIterator.idl"
    "${CMAKE_CURRENT_SOURCE_DIR}/support/ISelector.idl"
    "${CMAKE_CURRENT_SOURCE_DIR}/support/ITable.idl"
    "${CMAKE_CURRENT_SOURCE_DIR}/support/IUuid.idl"
    "${CMAKE_CURRENT_SOURCE_DIR}/support/IVirtualMachine.idl"
)
set(interfaces_install_dir /headers/os)
install(FILES ${interfaceFiles} DESTINATION ${interfaces_install_dir})

foreach(IF ${interfaceFiles})
    file(RELATIVE_PATH REL_IF ${CMAKE_CURRENT_SOURCE_DIR} ${IF})
    get_filename_component(NAME_IF ${REL_IF} NAME_WE)
    get_filename_component(DIR_IF ${REL_IF} DIRECTORY)
    set(HEADER_DIR "${INTERFACE_HEADERS_DIR}/${DIR_IF}")
    set(CPP_DIR "${CMAKE_CURRENT_BINARY_DIR}/${DIR_IF}")
    set(OUT_HEADER "${HEADER_DIR}/${NAME_IF}.h")
    set(OUT_CPP "${CPP_DIR}/${NAME_IF}.cpp")
    file(MAKE_DIRECTORY ${HEADER_DIR})
    add_custom_command(
        OUTPUT ${OUT_HEADER} ${OUT_CPP}
        COMMAND pidgen_bs -I "${CMAKE_CURRENT_SOURCE_DIR}" -O "${CPP_DIR}" -S "${HEADER_DIR}" "${IF}"
        MAIN_DEPENDENCY "${IF}"
        DEPENDS pidgen_bs
        COMMENT "Compiling IDL ${REL_IF}"
        )
    list (APPEND interfaceSources "${OUT_CPP}")
endforeach()

file(GLOB_RECURSE interfaceHeaders "${INTERFACE_HEADERS_DIR}/*.h")
install(FILES ${interfaceHeaders} DESTINATION ${interfaces_install_dir})

set(interfaceFiles ${interfaceFiles} PARENT_SCOPE)
set(interfaceSources ${interfaceSources} PARENT_SCOPE)
set(interfaceHeaders ${interfaceHeaders} PARENT_SCOPE)

add_library(interfaces STATIC ${interfaceSources})
target_compile_options(interfaces PRIVATE ${GLOBAL_CFLAGS} -frtti -fexceptions)
