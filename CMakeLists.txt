cmake_minimum_required(VERSION 3.16)

project(HighPerformanceECGQml2 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)
find_package(Qt6 REQUIRED COMPONENTS Graphs)
find_package(Qt6 REQUIRED COMPONENTS Concurrent)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Multimedia)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appHighPerformanceECGQml2
    main.cpp
)

qt_add_qml_module(appHighPerformanceECGQml2
    URI HighPerformanceECGQml2
    SOURCES
    SOURCES modules/EggModel.h modules/EggModel.cpp
    RESOURCES qml.qrc
    SOURCES modules/RingBuffer.h
    SOURCES modules/EggSensor.h modules/EggSensor.cpp
    SOURCES modules/TimeTrace.h
    SOURCES main.h
    SOURCES modules/AudioInput.h modules/AudioInput.cpp
    SOURCES modules/AudioVisualizerModel.h modules/AudioVisualizerModel.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appHighPerformanceECGQml2 PROPERTIES
    #    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appHighPerformanceECGQml2
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_include_directories(appHighPerformanceECGQml2 PRIVATE "D:/Work/AJA/HighPerformanceECGQml/fftw")

target_link_libraries(appHighPerformanceECGQml2 PRIVATE Qt6::Quick)
target_link_libraries(appHighPerformanceECGQml2 PRIVATE Qt6::Graphs)
target_link_libraries(appHighPerformanceECGQml2 PRIVATE Qt6::Concurrent)
target_link_libraries(appHighPerformanceECGQml2 PRIVATE Qt6::Core)
target_link_libraries(appHighPerformanceECGQml2 PRIVATE Qt6::Multimedia)
target_link_libraries(appHighPerformanceECGQml2 PRIVATE "D:/Work/AJA/HighPerformanceECGQml/fftw/libfftw3-3.lib")

include(GNUInstallDirs)
install(TARGETS appHighPerformanceECGQml2
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
