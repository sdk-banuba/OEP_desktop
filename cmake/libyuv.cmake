cmake_minimum_required(VERSION 3.5)

include(ExternalProject)


ExternalProject_Add(
  libyuv
  GIT_REPOSITORY https://chromium.googlesource.com/libyuv/libyuv
  GIT_TAG 312c02a5aad4adda67cb2e0cc93a497d12845522
  UPDATE_COMMAND ""
  INSTALL_COMMAND ""
  PATCH_COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/libyuv/CMakeLists.txt <SOURCE_DIR>/CMakeLists.txt
  LOG_DOWNLOAD ON
  LOG_CONFIGURE ON
  LOG_BUILD ON)

ExternalProject_Get_Property(libyuv source_dir)
set(LIBYUV_INCLUDE_DIRS ${source_dir}/include)

ExternalProject_Get_Property(libyuv binary_dir)
set(LIBYUV_LIBRARY_PATH ${binary_dir})
set(LIBYUV_LIBRARY yuv)
add_library(${LIBYUV_LIBRARY} UNKNOWN IMPORTED)
if(CMAKE_GENERATOR STREQUAL Xcode)
  set_target_properties(${LIBYUV_LIBRARY} PROPERTIES
    IMPORTED_LOCATION ${LIBYUV_LIBRARY_PATH}/${CMAKE_FIND_LIBRARY_PREFIXES}yuv.a
    IMPORTED_LOCATION_RELEASE ${LIBYUV_LIBRARY_PATH}/Release/${CMAKE_FIND_LIBRARY_PREFIXES}yuv.a
    IMPORTED_LOCATION_DEBUG ${LIBYUV_LIBRARY_PATH}/Debug/${CMAKE_FIND_LIBRARY_PREFIXES}yuv.a
    )
elseif(MSVC)
  set_target_properties(${LIBYUV_LIBRARY} PROPERTIES
    IMPORTED_LOCATION_DEBUG "${LIBYUV_LIBRARY_PATH}/${CMAKE_FIND_LIBRARY_PREFIXES}/Debug/yuv.lib"
    IMPORTED_LOCATION_RELEASE "${LIBYUV_LIBRARY_PATH}/${CMAKE_FIND_LIBRARY_PREFIXES}/Release/yuv.lib"
  )
else()
  set_target_properties(${LIBYUV_LIBRARY} PROPERTIES
    IMPORTED_LOCATION ${LIBYUV_LIBRARY_PATH}/${CMAKE_FIND_LIBRARY_PREFIXES}libyuv.a
    )
endif()

add_dependencies(${LIBYUV_LIBRARY} libyuv)