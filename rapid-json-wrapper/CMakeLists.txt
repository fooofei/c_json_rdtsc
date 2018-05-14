
cmake_minimum_required(VERSION 3.1)
project(rapidjson-wrapper)

if(APPLE)
    # no warning
    # https://stackoverflow.com/questions/31561309/cmake-warnings-under-os-x-macosx-rpath-is-not-specified-for-the-following-targe
    set(CMAKE_MACOSX_RPATH 0)
endif()

set(self_library_name rapidjson)

set(Source_files 
	${CMAKE_CURRENT_LIST_DIR}/rapidJSON_api.cpp
	${CMAKE_CURRENT_LIST_DIR}/rapidJSON_api.h

	)

add_library(${self_library_name} STATIC  ${Source_files})

# include other directories
target_include_directories(${self_library_name} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../rapidjson/include)

# for others module include
#target_include_directories(${self_library_name} INTERFACE|PUBLIC ${CMAKE_CURRENT_LIST_DIR})

## defines options link-libraries
#target_compile_definitions(${self_library_name} PRIVATE "__STDC_FORMAT_MACROS")
if (WIN32)
  # visual studio file encoding
  target_compile_options(${self_library_name} PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
#elseif(APPLE)
else()
  # fPIC for other library link
  target_compile_options(${self_library_name} PRIVATE -fPIC)
  target_compile_options(${self_library_name} PRIVATE -fvisibility=hidden)
  target_compile_options(${self_library_name} PRIVATE -fno-common )
  #target_link_libraries(${self_library_name} dl) # dlopen  
endif()
if(APPLE)
endif()



source_group(files FILES ${Source_files})

