#######################################################################
# CMakeLists.txt
# Copyright (c) 2020 Nicolas Ivan Hamblenne
#######################################################################

set(HEADERS ncl_value.h ncl_interp.h ncl_hash_table.h)
add_library(ncl ncl_value.c ncl_interp.c ncl_hash_table.c ncl_parser.c ncl_lexer.c ncl_symset.c)
set_project_warnings(ncl PRIVATE)
target_link_libraries(ncl PRIVATE ngc)
target_include_directories(ncl INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include/ncl>)

if(LIBNCL_INSTALL)
    install(TARGETS ncl EXPORT libnclConfig DESTINATION lib)
    install(FILES ${HEADERS} DESTINATION include/ncl)
    install(EXPORT libnclConfig DESTINATION lib/cmake/libncl)
endif()
