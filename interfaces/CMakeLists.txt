set(interfaceFiles
    # "${CMAKE_CURRENT_SOURCE_DIR}/app/ICommand.idl"
    # "${CMAKE_CURRENT_SOURCE_DIR}/app/IProgress.idl"
    # "${CMAKE_CURRENT_SOURCE_DIR}/app/ITerminalView.idl"
    # "${CMAKE_CURRENT_SOURCE_DIR}/package/IInstallHandler.idl"
    # "${CMAKE_CURRENT_SOURCE_DIR}/services/IInformant.idl"
    # "${CMAKE_CURRENT_SOURCE_DIR}/storage/IReferable.idl"
    # "${CMAKE_CURRENT_SOURCE_DIR}/support/ICatalogPermissions.idl"
    "${CMAKE_CURRENT_SOURCE_DIR}/os/support/ICatalog.aidl"
    "${CMAKE_CURRENT_SOURCE_DIR}/os/support/IDatum.aidl"
    # "${CMAKE_CURRENT_SOURCE_DIR}/support/IIterable.idl"
    # "${CMAKE_CURRENT_SOURCE_DIR}/support/IIterator.idl"
    # "${CMAKE_CURRENT_SOURCE_DIR}/support/INib.idl"
    "${CMAKE_CURRENT_SOURCE_DIR}/os/support/INode.aidl"
    "${CMAKE_CURRENT_SOURCE_DIR}/os/support/INodeObserver.aidl"
    # "${CMAKE_CURRENT_SOURCE_DIR}/support/IProcess.idl"
    # "${CMAKE_CURRENT_SOURCE_DIR}/support/IProcessManager.idl"
    # "${CMAKE_CURRENT_SOURCE_DIR}/support/IRandomIterator.idl"
    # "${CMAKE_CURRENT_SOURCE_DIR}/support/ISelector.idl"
    # "${CMAKE_CURRENT_SOURCE_DIR}/support/ITable.idl"
    # "${CMAKE_CURRENT_SOURCE_DIR}/support/IUuid.idl"
    # "${CMAKE_CURRENT_SOURCE_DIR}/support/IVirtualMachine.idl"
)
set(interfaces_install_dir /headers/os)
install(FILES ${interfaceFiles} DESTINATION ${interfaces_install_dir})

include(AIDL)
add_aidl(interfaceSources ${INTERFACE_HEADERS_DIR} ${interfaceFiles})

file(GLOB_RECURSE interfaceHeaders "${INTERFACE_HEADERS_DIR}/*.h")
install(FILES ${interfaceHeaders} DESTINATION ${interfaces_install_dir})

set(interfaceFiles ${interfaceFiles} PARENT_SCOPE)
set(interfaceSources ${interfaceSources} PARENT_SCOPE)
set(interfaceHeaders ${interfaceHeaders} PARENT_SCOPE)

add_library(interfaces STATIC ${interfaceSources})
# target_compile_options(interfaces PRIVATE ${GLOBAL_CFLAGS} -frtti -fexceptions)
