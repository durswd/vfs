cmake_minimum_required(VERSION 3.1)
project(VFS)

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_C_STANDARD 11) 

find_package(CUDA REQUIRED)
find_package(OpenCV REQUIRED)

cuda_add_library(stereotgv
    stereotgv/warpImage.cu
    stereotgv/upscale.cu
    stereotgv/updatePrimalVariables.cu
    stereotgv/updateDualVariables.cu
    stereotgv/thresholdingL1.cu
    stereotgv/stereotgv.cu
    stereotgv/scalar.cu
    stereotgv/preconditioning.cu
    stereotgv/medianFilter.cu
    stereotgv/limitRange.cu
    stereotgv/imageConverter.cu
    stereotgv/flowToHsv.cu
    stereotgv/downscale.cu
    stereotgv/copyClone.cu
    stereotgv/conversion.cu
    stereotgv/computeDerivatives.cu
    stereotgv/calcTensor.cu)

add_executable(test_vfs
    test_vfs/main.cpp
)

target_include_directories(stereotgv
    PUBLIC
    ${OpenCV_INCLUDE_DIRS}
    thirdparty/CUDA/inc/)

target_link_libraries(stereotgv
    ${OpenCV_LIBRARIES})

target_include_directories(test_vfs
    PUBLIC
    ./)

target_link_libraries(test_vfs
    PRIVATE
    stereotgv
    ${OpenCV_LIBRARIES})
