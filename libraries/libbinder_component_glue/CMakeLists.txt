include_directories(../libbinder_glue)
add_library(ob_component_glue STATIC binder_component_glue.cpp)
add_dependencies(ob_component_glue interfaces)
