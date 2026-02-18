cmake_minimum_required(VERSION 3.5)

project(Client_Rock_Paper_Scissors LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

# تنظیمات UI و Resource
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network)

# ----------------------------------------------------------------
# تنظیمات OpenCV (روش اصلاح شده با آدرس مستقیم)
# ----------------------------------------------------------------
# آدرس اصلی پوشه بیلد OpenCV
if(DEFINED ENV{OPENCV_ROOT})
    set(OPENCV_ROOT $ENV{OPENCV_ROOT})
    message(STATUS "Using OpenCV from environment variable: ${OPENCV_ROOT}")
else()
    set(OPENCV_ROOT "D:/openCV/opencv/build")
    message(STATUS "OPENCV_ROOT not set, using default: ${OPENCV_ROOT}")
endif()

# هدر فایل‌ها
include_directories(${OPENCV_ROOT}/include)

add_executable(Client_Rock_Paper_Scissors
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    cameraengine.cpp
    cameraengine.h
    cameraworker.cpp
    cameraworker.h
    handdetector.cpp
    handdetector.h
    handdetectorai.cpp
    handdetectorai.h
    stylemanager.cpp
    stylemanager.h
    framerenderer.cpp
    framerenderer.h
    appconstants.h
    protocol.h
    gamedata.h
    networkmanager.cpp
    networkmanager.h
    gamecontroller.cpp
    gamecontroller.h
    loginform.cpp
    loginform.h
    loginform.ui
)

# لینک کردن کتابخانه‌ها (این بخش تغییر کرده است)
target_link_libraries(Client_Rock_Paper_Scissors PRIVATE 
    Qt6::Widgets 
    Qt6::Network
    
    # فایل مخصوص Debug (حتما باید آخرش d داشته باشد)
    debug "${OPENCV_ROOT}/x64/vc16/lib/opencv_world4120d.lib"
    
    # فایل مخصوص Release (نباید d داشته باشد)
    optimized "${OPENCV_ROOT}/x64/vc16/lib/opencv_world4120.lib"
)

# ----------------------------------------------------------------
# دستورات کپی کردن خودکار DLLها بعد از بیلد
# ----------------------------------------------------------------

# 1. کپی کردن DLLهای Qt (با windeployqt)
add_custom_command(TARGET Client_Rock_Paper_Scissors POST_BUILD
    COMMAND "${CMAKE_PREFIX_PATH}/bin/windeployqt.exe"
        $<$<CONFIG:Debug>:--debug>
        $<$<CONFIG:Release>:--release>
        --no-translations
        --no-opengl-sw
        --compiler-runtime
        "$<TARGET_FILE:Client_Rock_Paper_Scissors>"
    COMMENT "Running windeployqt..."
)

# 2. کپی کردن DLL مناسب OpenCV بر اساس نوع بیلد (Debug یا Release)
add_custom_command(TARGET Client_Rock_Paper_Scissors POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${OPENCV_ROOT}/x64/vc16/bin/opencv_world4120$<$<CONFIG:Debug>:d>.dll"
        "$<TARGET_FILE_DIR:Client_Rock_Paper_Scissors>"
    COMMENT "Copying OpenCV DLL..."
)

# 3. کپی کردن فایل مدل هوش مصنوعی (ONNX) به کنار فایل اجرایی
add_custom_command(TARGET Client_Rock_Paper_Scissors POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/hand_landmark_sparse_Nx3x224x224.onnx"
        "$<TARGET_FILE_DIR:Client_Rock_Paper_Scissors>"
    COMMENT "Copying AI Model (ONNX)..."
)