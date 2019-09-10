set(IDL_HEADERS_DIR "${CMAKE_CURRENT_BINARY_DIR}/headers")
include(AIDL)
add_aidl(interfaces ${IDL_HEADERS_DIR}
    "${CMAKE_CURRENT_SOURCE_DIR}/example/IExample.aidl"
)

include_directories(${IDL_HEADERS_DIR})

add_executable(exampleservice
    main.cpp
    ${interfaces}
)
target_link_libraries(exampleservice PUBLIC binder)
