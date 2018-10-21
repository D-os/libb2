include_directories(../libbinder_glue)
add_library(binder_component_glue STATIC binder_component_glue.cpp)
add_dependencies(binder_component_glue interfaces)
