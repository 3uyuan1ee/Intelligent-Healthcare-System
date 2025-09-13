cmake_minimum_required(VERSION 3.16)

project(INTELLIGENT_HEALTHCARE_Client VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

#-----------------------网络配置------------------------------



#-----------------------网络配置------------------------------

#-----------------------资源配置------------------------------

set(RESOURCE_FILES resources.qrc)
#

#-----------------------资源配置------------------------------

set(PROJECT_SOURCES
        main.cpp
        login_widget.cpp
        login_widget.h
        login_widget.ui
        resources.qrc  # 必须明确添加
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(INTELLIGENT_HEALTHCARE_Client
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}

        enroll.h enroll.cpp enroll.ui
        Instance/UserSession.h
        Client/doctor_client.h Client/doctor_client.cpp Client/doctor_client.ui
        Client/patient_client.h Client/patient_client.cpp Client/patient_client.ui
        Client/admin_client.h Client/admin_client.cpp Client/admin_client.ui
        Fun./function.h Fun./function.cpp
        NetWork/tcpclient.h NetWork/tcpclient.cpp
        Instance/StateManager.h Instance/StateManager.cpp
        resources.qrc
        Fun./JsonMessageBuilder.h Fun./JsonMessageBuilder.cpp
        Fun./DataManager.h Fun./DataManager.cpp
        infoClient/patientinfo.h infoClient/patientinfo.cpp infoClient/patientinfo.ui
        infoClient/doctorinfo.h infoClient/doctorinfo.cpp infoClient/doctorinfo.ui


        appointClient/patientappoint.h appointClient/patientappoint.cpp appointClient/patientappoint.ui
        Standard.h
        caseClient/patientcasemenuclient.h caseClient/patientcasemenuclient.cpp caseClient/patientcasemenuclient.ui
        adviceClient/patientadviceclient.h adviceClient/patientadviceclient.cpp adviceClient/patientadviceclient.ui
        QuestionClient/patienthealthquestionclient.h QuestionClient/patienthealthquestionclient.cpp QuestionClient/patienthealthquestionclient.ui
        checkInClient/doctorcheckinclient.h checkInClient/doctorcheckinclient.cpp checkInClient/doctorcheckinclient.ui
        caseClient/doctorcaseclient.h caseClient/doctorcaseclient.cpp caseClient/doctorcaseclient.ui
        caseClient/doctorcaseeditclient.h caseClient/doctorcaseeditclient.cpp caseClient/doctorcaseeditclient.ui
        adviceClient/doctoradviceclient.h adviceClient/doctoradviceclient.cpp adviceClient/doctoradviceclient.ui
        adviceClient/doctoradvicemenuclient.h adviceClient/doctoradvicemenuclient.cpp adviceClient/doctoradvicemenuclient.ui
        Notice/adminnoticeclient.h Notice/adminnoticeclient.cpp Notice/adminnoticeclient.ui
        infoClient/admininfomenuclient.h infoClient/admininfomenuclient.cpp infoClient/admininfomenuclient.ui
        checkInClient/admincheckinclient.h checkInClient/admincheckinclient.cpp checkInClient/admincheckinclient.ui
        checkInClient/admincheckinresultclient.h checkInClient/admincheckinresultclient.cpp checkInClient/admincheckinresultclient.ui
        infoClient/admininfoclient.h infoClient/admininfoclient.cpp infoClient/admininfoclient.ui
        infoClient/admininfodoctorclient.h infoClient/admininfodoctorclient.cpp infoClient/admininfodoctorclient.ui
        Notice/doctornoticeclient.h Notice/doctornoticeclient.cpp Notice/doctornoticeclient.ui
        Notice/patientnoticeclient.h Notice/patientnoticeclient.cpp Notice/patientnoticeclient.ui







    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET INTELLIGENT_HEALTHCARE_Client APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(INTELLIGENT_HEALTHCARE_Client SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(INTELLIGENT_HEALTHCARE_Client
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(INTELLIGENT_HEALTHCARE_Client PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.INTELLIGENT_HEALTHCARE_Client)
endif()
set_target_properties(INTELLIGENT_HEALTHCARE_Client PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS INTELLIGENT_HEALTHCARE_Client
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(INTELLIGENT_HEALTHCARE_Client)
endif()

# ---------------------调试---------------------------
message(STATUS "资源文件: ${RESOURCE_FILES}")
message(STATUS "AUTORCC状态: ${CMAKE_AUTORCC}")

# 确保资源文件被处理
if(CMAKE_AUTORCC)
    message(STATUS "自动资源编译已启用")
else()
    message(WARNING "自动资源编译未启用，手动处理")
    qt6_add_resources(MANUAL_RESOURCES resources.qrc)
    target_sources(INTELLIGENT_HEALTHCARE_Client PRIVATE ${MANUAL_RESOURCES})
endif()
