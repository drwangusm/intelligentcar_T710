cmake_minimum_required(VERSION 3.4...3.18)
project(patrol_car)
set(CMAKE_CXX_STANDARD 17)
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
#---------------------------------------------------------------------
#               [ bin ] ==> [ demo ]
#---------------------------------------------------------------------
# ImageCollection
set(COLLECTION_PROJECT_NAME "image_collection")
set(COLLECTION_PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/demo/image_collection.cpp)
add_executable(${COLLECTION_PROJECT_NAME} ${COLLECTION_PROJECT_SOURCES})
target_link_libraries(${COLLECTION_PROJECT_NAME} pthread )
target_link_libraries(${COLLECTION_PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries(${COLLECTION_PROJECT_NAME} ${SERIAL_LIBRARIES})

# CameraDisplay
set(CAMERA_DISPLAY_PROJECT_NAME "camera_display")
set(CAMERA_DISPLAY_PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/demo/camera_display.cpp)
add_executable(${CAMERA_DISPLAY_PROJECT_NAME} ${CAMERA_DISPLAY_PROJECT_SOURCES})
target_link_libraries(${CAMERA_DISPLAY_PROJECT_NAME} pthread )
target_link_libraries(${CAMERA_DISPLAY_PROJECT_NAME} ${OpenCV_LIBS})

# CameraCalibration
set(CALOBRATION_PROJECT_NAME "camera_calibrate")
set(CALOBRATION_PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/demo/camera_calibrate.cpp)
add_executable(${CALOBRATION_PROJECT_NAME} ${CALOBRATION_PROJECT_SOURCES})
target_link_libraries(${CALOBRATION_PROJECT_NAME} pthread )
target_link_libraries(${CALOBRATION_PROJECT_NAME} ${OpenCV_LIBS})

# ImagePreprocess
set(PREPROCESS_PROJECT_NAME "image_preprocess")
set(PREPROCESS_PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/demo/image_preprocess.cpp)
add_executable(${PREPROCESS_PROJECT_NAME} ${PREPROCESS_PROJECT_SOURCES})
target_link_libraries(${PREPROCESS_PROJECT_NAME} pthread )
target_link_libraries(${PREPROCESS_PROJECT_NAME} ${OpenCV_LIBS})

# PerspectiveMapping
set(PERSPECTIVE_PROJECT_NAME "perspective_mapping")
set(PERSPECTIVE_PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/demo/perspective_mapping.cpp)
add_executable(${PERSPECTIVE_PROJECT_NAME} ${PERSPECTIVE_PROJECT_SOURCES})
target_link_libraries(${PERSPECTIVE_PROJECT_NAME} pthread )
target_link_libraries(${PERSPECTIVE_PROJECT_NAME} ${OpenCV_LIBS})

# Track_Recognition
set(TRACKRES_PROJECT_NAME "track_recognition")
set(TRACKRES_PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/demo/track_recognition.cpp)
add_executable(${TRACKRES_PROJECT_NAME} ${TRACKRES_PROJECT_SOURCES})
target_link_libraries(${TRACKRES_PROJECT_NAME} pthread )
target_link_libraries(${TRACKRES_PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries(${TRACKRES_PROJECT_NAME} ${PPNC_LIBRARIES})
target_link_libraries(${TRACKRES_PROJECT_NAME} ${ONNX_LIBRARIES})

# RingRecognition
set(RINGRES_PROJECT_NAME "ring_recognition")
set(RINGRES_PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/demo/ring_recognition.cpp)
add_executable(${RINGRES_PROJECT_NAME} ${RINGRES_PROJECT_SOURCES})
target_link_libraries(${RINGRES_PROJECT_NAME} pthread )
target_link_libraries(${RINGRES_PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries(${RINGRES_PROJECT_NAME} ${PPNC_LIBRARIES})
target_link_libraries(${RINGRES_PROJECT_NAME} ${ONNX_LIBRARIES})

# CrossRecognition
set(CROSSRES_PROJECT_NAME "cross_recognition")
set(CROSSRES_PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/demo/cross_recognition.cpp)
add_executable(${CROSSRES_PROJECT_NAME} ${CROSSRES_PROJECT_SOURCES})
target_link_libraries(${CROSSRES_PROJECT_NAME} pthread )
target_link_libraries(${CROSSRES_PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries(${CROSSRES_PROJECT_NAME} ${PPNC_LIBRARIES})
target_link_libraries(${CROSSRES_PROJECT_NAME} ${ONNX_LIBRARIES})

# GarageRecognition
set(GARAGERES_PROJECT_NAME "garage_recognition")
set(GARAGERES_PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/demo/garage_recognition.cpp)
add_executable(${GARAGERES_PROJECT_NAME} ${GARAGERES_PROJECT_SOURCES})
target_link_libraries(${GARAGERES_PROJECT_NAME} pthread )
target_link_libraries(${GARAGERES_PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries(${GARAGERES_PROJECT_NAME} ${PPNC_LIBRARIES})
target_link_libraries(${GARAGERES_PROJECT_NAME} ${ONNX_LIBRARIES})
target_link_libraries(${GARAGERES_PROJECT_NAME} ${PPNC_LIBRARIES})
target_link_libraries(${GARAGERES_PROJECT_NAME} ${ONNX_LIBRARIES})

# FreezoneRecognition
set(FREEZONERES_PROJECT_NAME "freezone_recognition")
set(FREEZONERES_PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/demo/freezone_recognition.cpp)
add_executable(${FREEZONERES_PROJECT_NAME} ${FREEZONERES_PROJECT_SOURCES})
target_link_libraries(${FREEZONERES_PROJECT_NAME} pthread )
target_link_libraries(${FREEZONERES_PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries(${FREEZONERES_PROJECT_NAME} ${PPNC_LIBRARIES})
target_link_libraries(${FREEZONERES_PROJECT_NAME} ${ONNX_LIBRARIES})

# MotionController
set(MOTION_PROJECT_NAME "motion_controller")
set(MOTION_PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/demo/motion_controller.cpp)
add_executable(${MOTION_PROJECT_NAME} ${MOTION_PROJECT_SOURCES})
target_link_libraries(${MOTION_PROJECT_NAME} pthread )
target_link_libraries(${MOTION_PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries(${MOTION_PROJECT_NAME} ${SERIAL_LIBRARIES})
target_link_libraries(${MOTION_PROJECT_NAME} ${PPNC_LIBRARIES})
target_link_libraries(${MOTION_PROJECT_NAME} ${ONNX_LIBRARIES})

# DepotDetection
set(DEPOTDET_PROJECT_NAME "depot_detection")
set(DEPOTDET_PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/demo/depot_detection.cpp)
add_executable(${DEPOTDET_PROJECT_NAME} ${DEPOTDET_PROJECT_SOURCES})
target_link_libraries(${DEPOTDET_PROJECT_NAME} pthread)
target_link_libraries(${DEPOTDET_PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries(${DEPOTDET_PROJECT_NAME} ${PPNC_LIBRARIES})
target_link_libraries(${DEPOTDET_PROJECT_NAME} ${ONNX_LIBRARIES})

# SlowzoneDetection
set(SLOWZONEDET_PROJECT_NAME "slowzone_detection")
set(SLOWZONEDET_PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/demo/slowzone_detection.cpp)
add_executable(${SLOWZONEDET_PROJECT_NAME} ${SLOWZONEDET_PROJECT_SOURCES})
target_link_libraries(${SLOWZONEDET_PROJECT_NAME} pthread)
target_link_libraries(${SLOWZONEDET_PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries(${SLOWZONEDET_PROJECT_NAME} ${PPNC_LIBRARIES})
target_link_libraries(${SLOWZONEDET_PROJECT_NAME} ${ONNX_LIBRARIES})

# BridgeDetection
set(BRIDGEDET_PROJECT_NAME "bridge_detection")
set(BRIDGEDET_PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/demo/bridge_detection.cpp)
add_executable(${BRIDGEDET_PROJECT_NAME} ${BRIDGEDET_PROJECT_SOURCES})
target_link_libraries(${BRIDGEDET_PROJECT_NAME} pthread)
target_link_libraries(${BRIDGEDET_PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries(${BRIDGEDET_PROJECT_NAME} ${PPNC_LIBRARIES})
target_link_libraries(${BRIDGEDET_PROJECT_NAME} ${ONNX_LIBRARIES})

# GranaryDetection
set(GRANARYDET_PROJECT_NAME "granary_detection")
set(GRANARYDET_PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/demo/granary_detection.cpp)
add_executable(${GRANARYDET_PROJECT_NAME} ${GRANARYDET_PROJECT_SOURCES})
target_link_libraries(${GRANARYDET_PROJECT_NAME} pthread)
target_link_libraries(${GRANARYDET_PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries(${GRANARYDET_PROJECT_NAME} ${PPNC_LIBRARIES})
target_link_libraries(${GRANARYDET_PROJECT_NAME} ${ONNX_LIBRARIES})

# FarmlandDetection
set(FARMLANDDET_PROJECT_NAME "farmland_detection")
set(FARMLANDDET_PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/demo/farmland_detection.cpp)
add_executable(${FARMLANDDET_PROJECT_NAME} ${FARMLANDDET_PROJECT_SOURCES})
target_link_libraries(${FARMLANDDET_PROJECT_NAME} pthread)
target_link_libraries(${FARMLANDDET_PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries(${FARMLANDDET_PROJECT_NAME} ${PPNC_LIBRARIES})
target_link_libraries(${FARMLANDDET_PROJECT_NAME} ${ONNX_LIBRARIES})

# Image2video
set(IMAGE2VIDEO_PROJECT_NAME "image2video")
set(IMAGE2VIDEO_PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/demo/image2video.cpp)
add_executable(${IMAGE2VIDEO_PROJECT_NAME} ${IMAGE2VIDEO_PROJECT_SOURCES})
target_link_libraries(${IMAGE2VIDEO_PROJECT_NAME} pthread )
target_link_libraries(${IMAGE2VIDEO_PROJECT_NAME} ${OpenCV_LIBS})

#---------------------------------------------------------------------
#               [ bin ] ==> [ main ]
#---------------------------------------------------------------------
set(ICAR_PROJECT_NAME "main")
set(ICAR_PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/src/main.cpp)
add_executable(${ICAR_PROJECT_NAME} ${ICAR_PROJECT_SOURCES})
target_link_libraries(${ICAR_PROJECT_NAME} ${PPNC_LIBRARIES})
target_link_libraries(${ICAR_PROJECT_NAME} ${ONNX_LIBRARIES})
target_link_libraries(${ICAR_PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries(${ICAR_PROJECT_NAME} pthread )
target_link_libraries(${ICAR_PROJECT_NAME} ${SERIAL_LIBRARIES})



