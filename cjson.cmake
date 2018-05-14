
# overwrite the options
set(BUILD_SHARED_LIBS  OFF CACHE "" BOOL FORCE)
set(ENABLE_CJSON_TEST  OFF CACHE "" BOOL FORCE)

if(WIN32)
endif()



if(NOT TARGET cjson)
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/cJSON build_cjson)
endif()



# For other include
#target_include_directories(cjson INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/include)



