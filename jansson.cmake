

# overwrite the options
set(JANSSON_STATIC_CRT  ON CACHE "" BOOL FORCE)
set(JANSSON_EXAMPLES  OFF CACHE "" BOOL FORCE)
set(JANSSON_BUILD_DOCS  OFF CACHE "" BOOL FORCE)
set(JANSSON_WITHOUT_TESTS  ON CACHE "" BOOL FORCE)
set(JANSSON_INSTALL  OFF CACHE "" BOOL FORCE)

if(WIN32)
endif()



if(NOT TARGET jansson)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/jansson build_jansson)
endif()

# For other include
target_include_directories(jansson INTERFACE ${jansson_BINARY_DIR}/include)

#message(STATUS "jansson include "  ${CMAKE_CURRENT_BINARY_DIR}/include)
#-- jansson include /Users/hujianfei/Desktop/git_src/c_json_rdtsc/build/include

#message(STATUS "jansson include "  ${jansson_BINARY_DIR}/include)
#-- jansson include /Users/hujianfei/Desktop/git_src/c_json_rdtsc/build/build_jansson/include



