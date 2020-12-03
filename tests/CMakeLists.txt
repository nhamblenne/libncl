#######################################################################
# CMakeLists.txt
# Copyright (c) 2020 Nicolas Ivan Hamblenne
#######################################################################

add_executable(test1 test1.c)
target_link_libraries(test1 ncl)

add_executable(test2 test2.c)
target_link_libraries(test2 ncl)

add_executable(test3 test3.c)
target_link_libraries(test3 ncl)

add_executable(test4 test4.c)
target_link_libraries(test4 ncl)