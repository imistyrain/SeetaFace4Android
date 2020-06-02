cmake_minimum_required(VERSION 3.4.1)
project(seetaface_jni CXX)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall -O3 -s")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
FIND_PACKAGE( OpenMP REQUIRED)
if(OPENMP_FOUND)
    message("OPENMP FOUND")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    add_definitions(-DUSE_OPENMP)
endif()

set(LIB_SRCS src/classifier/lab_boosted_classifier.cpp src/classifier/mlp.cpp src/classifier/surf_mlp.cpp
        src/feat/lab_feature_map.cpp src/feat/surf_feature_map.cpp
        src/Identification/aligner.cpp src/Identification/bias_adder_net.cpp src/Identification/blob.cpp
        src/Identification/bn_net.cpp src/Identification/common_net.cpp src/Identification/conv_net.cpp
        src/Identification/eltwise_net.cpp src/Identification/face_identification.cpp src/Identification/inner_product_net.cpp
        src/Identification/log.cpp src/Identification/math_functions.cpp src/Identification/max_pooling_net.cpp
        src/Identification/net.cpp src/Identification/pad_net.cpp src/Identification/spatial_transform_net.cpp src/Identification/tform_maker_net.cpp
        src/io/lab_boost_model_reader.cpp src/io/surf_mlp_model_reader.cpp
        src/util/image_pyramid.cpp src/util/nms.cpp
        src/cfan.cpp src/face_alignment.cpp src/face_detection.cpp src/fust.cpp src/sift.cpp
        )
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_library(seetaface ${LIB_SRCS})

set(OPENCV_DIR ${CMAKE_CURRENT_SOURCE_DIR}/opencv)
add_library(opencv SHARED IMPORTED)
set_target_properties(opencv PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libopencv_java3.so)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)
set(SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/seetaface_jni.cpp)
add_library(seetaface_jni SHARED ${SRC_FILES})

target_link_libraries(seetaface_jni seetaface opencv log)