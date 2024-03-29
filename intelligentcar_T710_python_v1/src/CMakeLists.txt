cmake_minimum_required(VERSION 3.4...3.18)
project(intelligentCar_v2.0)

set (CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O3 -Wall -mcpu=native -flto -pthread")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(INCLUDE_PATH "/usr/local/include")
set(LIB_PATH "/usr/local/lib")
#---------------------------------------------------------------------
#       [ Include、Lib  : Define ] ==> []
#---------------------------------------------------------------------

set(COMMON_LIB_DIR "${PROJECT_SOURCE_DIR}/lib/")

set(COMMON_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include"
                        "${PROJECT_SOURCE_DIR}/config"
                        "${PROJECT_SOURCE_DIR}/demo" 
                        "${PROJECT_SOURCE_DIR}/driver" 
                        "${PROJECT_SOURCE_DIR}/lib" 
                        "${PROJECT_SOURCE_DIR}/src"
                        "${PROJECT_SOURCE_DIR}/tool"          )

link_directories(${COMMON_LIB_DIR})
include_directories( ${COMMON_INCLUDE_DIR} )

#---------------------------------------------------------------------
#       [ Include、Lib  : Path ] ==> [ glib opencv ]
#---------------------------------------------------------------------
find_package(PkgConfig)
pkg_search_module(GLIB REQUIRED glib-2.0)
include_directories(${GLIB_INCLUDE_DIRS})

find_package( OpenCV REQUIRED )    
include_directories( ${OpenCV_INCLUDE_DIRS} )

pkg_search_module(SERIAL REQUIRED libserial)
include_directories(${SERIAL_INCLUDE_DIRS})
link_directories(${SERIAL_LIBRARY_DIRS})

# find ppnc
pkg_search_module(PPNC REQUIRED ppnc)
include_directories(${PPNC_INCLUDE_DIRS})
link_directories(${PPNC_LIBRARY_DIRS})

# find onnx
pkg_search_module(ONNX REQUIRED onnx)
include_directories(${ONNX_INCLUDE_DIRS})
link_directories(${ONNX_LIBRARY_DIRS})

# 添加Python包
find_package(PythonLibs 3.6 REQUIRED)
include_directories(${PYTHON_INCLUDE_DIR})

# 添加Boost包
find_package(Boost COMPONENTS python3 REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

#---------------------------------------------------------------------
#              动态链接库生成
#---------------------------------------------------------------------
# 生成动态链接库：
set(ICAR_PROJECT_NAME "intelligentCar_sou")
set(ICAR_PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/src/icar.cpp)
add_library(${ICAR_PROJECT_NAME} SHARED ${ICAR_PROJECT_SOURCES})
target_link_libraries(${ICAR_PROJECT_NAME} ${Boost_LIBRARIES})
target_link_libraries(${ICAR_PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries(${ICAR_PROJECT_NAME} pthread )
target_link_libraries(${ICAR_PROJECT_NAME} ${SERIAL_LIBRARIES})
target_link_libraries(${ICAR_PROJECT_NAME} ${PPNC_LIBRARIES})
target_link_libraries(${ICAR_PROJECT_NAME} ${ONNX_LIBRARIES})
set_target_properties(${ICAR_PROJECT_NAME} PROPERTIES PREFIX "") # 去掉前缀，否则会生成lib***.so

set(AICAR_PROJECT_NAME "carrun")
set(AICAR_PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/src/main.cpp)
add_library(${AICAR_PROJECT_NAME} SHARED ${AICAR_PROJECT_SOURCES})
target_link_libraries(${AICAR_PROJECT_NAME} ${Boost_LIBRARIES})
target_link_libraries(${AICAR_PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries(${AICAR_PROJECT_NAME} pthread )
target_link_libraries(${AICAR_PROJECT_NAME} ${SERIAL_LIBRARIES})
target_link_libraries(${AICAR_PROJECT_NAME} ${PPNC_LIBRARIES})
target_link_libraries(${AICAR_PROJECT_NAME} ${ONNX_LIBRARIES})
set_target_properties(${AICAR_PROJECT_NAME} PROPERTIES PREFIX "") # 去掉前缀，否则会生成lib***.so

install(TARGETS ${ICAR_PROJECT_NAME} 
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX})
