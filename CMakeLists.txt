cmake_minimum_required(VERSION 3.16)

project(Qtv2 VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets PrintSupport)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets PrintSupport)

# Unity Build 활성화 (한 줄 추가로 빌드 속도 대폭 개선!)
set(CMAKE_UNITY_BUILD ON)

# qcustomplot을 소스에서 직접 빌드
add_library(qcustomplot STATIC
    "${CMAKE_CURRENT_SOURCE_DIR}/qcustomplot/qcustomplot.cpp"
)
target_compile_options(qcustomplot PRIVATE
    $<$<CONFIG:Release>:-O2>
    $<$<CONFIG:Debug>:-O1>
)

# qcustomplot에도 Unity Build 적용 qlfem qlfem
set_target_properties(qcustomplot PROPERTIES UNITY_BUILD ON)


target_include_directories(qcustomplot PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/qcustomplot"
)
target_link_libraries(qcustomplot PUBLIC
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::PrintSupport
)



set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        sqlite3.c
        sqlite3.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    find_package(Qt6 REQUIRED COMPONENTS Core)

    qt_add_executable(Qtv2
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        graphwidget.h graphwidget.cpp
        inputwidget.h inputwidget.cpp
        dataManagement.h dataManagement.cpp
        slideedit.h slideedit.cpp

        settingstorage.h settingstorage.cpp
        rmsgraphwidget.h rmsgraphwidget.cpp
        resources.qrc
        phasorwidget.h phasorwidget.cpp
        frequencytracker.h frequencytracker.cpp
        harmonicwidget.h harmonicwidget.cpp
        onesecmainwindow.h onesecmainwindow.cpp
        inputmainwindow.h inputmainwindow.cpp
        inputsettingwidget.h inputsettingwidget.cpp
        show_hide_check_widget.h show_hide_check_widget.cpp
        settingdialog.h settingdialog.cpp
        a3700n_gui.h a3700n_gui.cpp
        a3700n_phasor.h a3700n_phasor.cpp
        a3700n_wave.h a3700n_wave.cpp
        a3700n_harm.h a3700n_harm.cpp




    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Qtv2 APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Qtv2 SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Qtv2
            ${PROJECT_SOURCES}
        )
    endif()
endif()


target_include_directories(Qtv2 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/sqlite_modern_cpp/hdr
    ${CMAKE_CURRENT_SOURCE_DIR}  # sqlite3.h 경로
)

target_link_libraries(Qtv2 PRIVATE
    qcustomplot
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::PrintSupport
)
target_link_libraries(Qtv2 PRIVATE Qt6::Core)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Qtv2)
endif()
set_target_properties(Qtv2 PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Qtv2
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Qtv2)
endif()
