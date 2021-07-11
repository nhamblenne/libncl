#######################################################################
# CMakeLists.txt
# Copyright (c) 2020 Nicolas Ivan Hamblenne
#######################################################################

add_library(testlib node-dumper.c file-reader.c)
set_project_warnings(testlib PUBLIC)
target_link_libraries(testlib ncl)

add_executable(test-lexer test-lexer.c)
target_link_libraries(test-lexer ncl testlib)
add_test(NAME test-lexer-ok
         COMMAND bash ntt -v -d ${CMAKE_CURRENT_LIST_DIR} -n test-lexer-ok $<TARGET_FILE:test-lexer> ${CMAKE_CURRENT_LIST_DIR}/test-lexer-ok.ncl)
add_test(NAME test-lexer-err
         COMMAND bash ntt -v -d ${CMAKE_CURRENT_LIST_DIR} -n test-lexer-err $<TARGET_FILE:test-lexer> ${CMAKE_CURRENT_LIST_DIR}/test-lexer-err.ncl)

add_executable(test-parser test-parser.c)
target_link_libraries(test-parser ncl testlib)
add_test(NAME test-parser-ok
        COMMAND bash ntt -v -d ${CMAKE_CURRENT_LIST_DIR} -n test-parser-ok $<TARGET_FILE:test-parser> ${CMAKE_CURRENT_LIST_DIR}/test-parser-ok.ncl)
add_test(NAME test-parser-err
        COMMAND bash ntt -v -d ${CMAKE_CURRENT_LIST_DIR} -n test-parser-err $<TARGET_FILE:test-parser> ${CMAKE_CURRENT_LIST_DIR}/test-parser-err.ncl)

add_executable(test1 test1.c)
target_link_libraries(test1 ncl testlib)
add_test(NAME test1
         COMMAND test1)

add_executable(test2 test2.c)
target_link_libraries(test2 ncl testlib)
add_test(NAME test2
         COMMAND test2)

add_executable(test3 test3.c)
target_link_libraries(test3 ncl testlib)
add_test(NAME test3
         COMMAND bash ntt -v -d ${CMAKE_CURRENT_LIST_DIR} $<TARGET_FILE:test3>)

add_executable(test4 test4.c)
target_link_libraries(test4 ncl testlib)
add_test(NAME test4
         COMMAND bash ntt -v -d ${CMAKE_CURRENT_LIST_DIR} $<TARGET_FILE:test4>)

add_executable(test5 test5.c)
target_link_libraries(test5 ncl testlib)
add_test(NAME test5
        COMMAND test5)

add_executable(test6 test6.c)
target_link_libraries(test6 ncl testlib)
add_test(NAME test6
        COMMAND bash ntt -v -d ${CMAKE_CURRENT_LIST_DIR} $<TARGET_FILE:test6>)
