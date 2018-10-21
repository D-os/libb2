set(RESOURCES_DIR "${CMAKE_CURRENT_BINARY_DIR}/resources")
add_custom_command(
    OUTPUT ${RESOURCES_DIR}
    COMMAND mkdir -p "${RESOURCES_DIR}"
    COMMAND makestrings -O "${RESOURCES_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/Strings.xml"
    MAIN_DEPENDENCY Strings.xml
    DEPENDS makestrings
    COMMENT "Strings: Strings.xml"
)
add_custom_target(generate-strings DEPENDS "${RESOURCES_DIR}")

add_library(Component SHARED
    SampleComponent.cpp
    main_samplecomponent.cpp
    ${RESOURCES_DIR}
)
set_target_properties(Component PROPERTIES PREFIX "")
target_link_libraries(Component PUBLIC ob)
