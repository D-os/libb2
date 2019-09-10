function(add_aidl generatedSources headersDir)
  list(REMOVE_AT ARGV 0)
  list(REMOVE_AT ARGV 0)
  foreach(IF ${ARGV})
      file(RELATIVE_PATH REL_IF ${CMAKE_CURRENT_SOURCE_DIR} ${IF})
      get_filename_component(NAME_IF ${REL_IF} NAME_WE)
      get_filename_component(DIR_IF ${REL_IF} DIRECTORY)
      set(CPP_DIR "${CMAKE_CURRENT_BINARY_DIR}/${DIR_IF}")
      set(OUT_CPP "${CPP_DIR}/${NAME_IF}.cpp")
      set(HEADERS_DIR "${headersDir}/${DIR_IF}")
      file(MAKE_DIRECTORY ${HEADERS_DIR})
      add_custom_command(
          OUTPUT ${OUT_CPP}
          COMMAND aidl-cpp -I"${CMAKE_CURRENT_SOURCE_DIR}" "${IF}" "${headersDir}" "${OUT_CPP}"
          MAIN_DEPENDENCY "${IF}"
          DEPENDS aidl-cpp
          COMMENT "Compiling IDL ${REL_IF}"
          COMMENT "aidl-cpp -I${CMAKE_CURRENT_SOURCE_DIR} ${IF} ${headersDir} ${OUT_CPP}"
      )
      list (APPEND SOURCES "${OUT_CPP}")
  endforeach()
  set(${generatedSources} ${${generatedSources}} ${SOURCES} PARENT_SCOPE)
endfunction(add_aidl)
