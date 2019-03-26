
# overwrite the options
#set(BUILD_SHARED_LIBS  OFF CACHE BOOL "" FORCE)

if(WIN32)
endif()


if(NOT TARGET json-c)
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/json-c build_json-c)
endif()

