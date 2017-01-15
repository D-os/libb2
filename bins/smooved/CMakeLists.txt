add_executable(smooved
    smoove.cpp
    ContextManagerService.h
    ContextManagerService.cpp
)
target_link_libraries(smooved binder b2)
