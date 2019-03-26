
cmake_minimum_required(VERSION 3.1)
project(C_MAKE_DEMO)

if(APPLE)
    # no warning
    # https://stackoverflow.com/questions/31561309/cmake-warnings-under-os-x-macosx-rpath-is-not-specified-for-the-following-targe
    set(CMAKE_MACOSX_RPATH 0)
endif()

#link_directories( <asan dir>)

set(bin1 json_tests)

set(Source_files 
	${CMAKE_CURRENT_LIST_DIR}/call.c
	${CMAKE_CURRENT_LIST_DIR}/crt_dbg_leak.c
	${CMAKE_CURRENT_LIST_DIR}/crt_dbg_leak.c
	${CMAKE_CURRENT_LIST_DIR}/rdtsc.c
	${CMAKE_CURRENT_LIST_DIR}/rdtsc.h
	)

# more files, you can use if()
#list(APPEND Source_files )

add_executable(${bin1} ${Source_files})

# include other directories
#target_include_directories(${bin1} PRIVATE ${CMAKE_CURRENT_LIST_DIR})

## defines options link-libraries
#target_compile_definitions(${bin1} PRIVATE "_USE_DATA2")
if (WIN32)
  # visual studio file encoding
  target_compile_options(${bin1} PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
#elseif(APPLE)
else()
  target_compile_options(${bin1} PRIVATE -fvisibility=hidden)
  target_compile_options(${bin1} PRIVATE -fno-common )
  #target_link_libraries(${bin1} dl) # dlopen
endif()
if(APPLE)
endif()

# For include .cmake
list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}")
#if (NOT TARGET cjson)
#    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/cJSON  build_cjson)
#endif ()


include(cjson)
target_link_libraries(${bin1} cjson)

if(NOT TARGET rapidjson)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/rapid-json-wrapper build_rapidjson)
endif()
target_link_libraries(${bin1} rapidjson)

#if(NOT TARGET jansson)
#    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/jansson-2.11 build_jasson)
#endif()

include(jansson)
target_link_libraries(${bin1} jansson)

# visual studio project tree
source_group(files FILES ${Source_files})






set(bin2 json_serialization)
add_executable(${bin2}
        ${CMAKE_CURRENT_LIST_DIR}/timing_serialization.c
        )

        
#target_compile_options(${bin2} PRIVATE -fno-omit-frame-pointer -fsanitize=address)

#target_link_libraries(${bin2} clang_rt)

target_link_libraries(${bin2} cjson)
target_link_libraries(${bin2} rapidjson)
target_link_libraries(${bin2} jansson)
include(json-c)
target_link_libraries(${bin2} json-c)
