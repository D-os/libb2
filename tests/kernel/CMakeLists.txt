add_executable(thread thread.cpp)
target_link_libraries(thread root)

add_executable(team team.cpp)
target_link_libraries(team root)

add_executable(sem sem.cpp)
target_link_libraries(sem root)

add_executable(area area.cpp)
target_link_libraries(area root)

add_executable(port port.cpp)
target_link_libraries(port root)
