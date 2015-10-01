project('hello', ['cpp', 'c'])
add_global_arguments('-std=c99', '-Wno-multichar', language : 'c')
add_global_arguments('-std=c++98', '-Wno-multichar', language : 'cpp')

subdir('libbe')

#executable('hello', 'hello.cpp',
#    include_directories : libbe_inc,
#    link_with : libbe)

executable('thread', 'thread.cpp',
    include_directories : libbe_inc,
    link_with : libbe)
