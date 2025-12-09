# 智能医疗管理系统测试框架的CMakeLists.txt

cmake_minimum_required(VERSION 3.16)

# 设置项目名称
project(INTELLIGENT_HEALTHCARE_Tests VERSION 0.1 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置Qt自动处理
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 查找Qt组件（与主项目保持一致）
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network Test)

# 启用测试
enable_testing()

# 定义测试源文件
set(TEST_SOURCES
    unit/TcpClient_test.cpp
    unit/UserSession_test.cpp
    unit/JsonMessageBuilder_test.cpp
    unit/StateManager_test.cpp
    # unit/Function_test.cpp  # 暂时跳过，因为依赖客户端类
    unit/DataManager_test.cpp
)

# 定义Mock源文件
set(MOCK_SOURCES
    mocks/MockTcpClient.cpp
    mocks/MockTcpClient.h
    config/test_config.cpp
    config/test_config.h
)

# 定义需要测试的项目源文件
set(PROJECT_SOURCES_FOR_TEST
    ../NetWork/tcpclient.cpp
    ../NetWork/tcpclient.h
    ../Instance/UserSession.h
    ../Instance/StateManager.h
    ../Instance/StateManager.cpp
    ../Fun./JsonMessageBuilder.h
    ../Fun./JsonMessageBuilder.cpp
    ../Fun./DataManager.h
    ../Fun./DataManager.cpp
    ../Standard.h
)

# 为每个测试创建可执行文件
foreach(TEST_SOURCE ${TEST_SOURCES})
    # 获取测试文件名（不含路径和扩展名）
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)

    # 创建测试可执行文件
    if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
        qt_add_executable(${TEST_NAME}
            MANUAL_FINALIZATION
            ${TEST_SOURCE}
            ${MOCK_SOURCES}
            ${PROJECT_SOURCES_FOR_TEST}
        )
    else()
        add_executable(${TEST_NAME}
            ${TEST_SOURCE}
            ${MOCK_SOURCES}
            ${PROJECT_SOURCES_FOR_TEST}
        )
    endif()

    # 链接Qt库
    target_link_libraries(${TEST_NAME} PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Network
        Qt${QT_VERSION_MAJOR}::Test
    )

    # 设置包含目录
    target_include_directories(${TEST_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

    # 添加测试
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

    # 设置测试工作目录
    set_tests_properties(${TEST_NAME} PROPERTIES
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..
    )

    # 为测试设置环境变量（如果需要）
    if(WIN32)
        set_tests_properties(${TEST_NAME} PROPERTIES
            ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
        )
    endif()
endforeach()

# 创建一个总测试可执行文件（运行所有测试）
# 使用QProcess方式运行各个测试，避免main函数冲突
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(run_all_tests
        MANUAL_FINALIZATION
        tests_main.cpp
    )
else()
    add_executable(run_all_tests
        tests_main.cpp
    )
endif()

target_link_libraries(run_all_tests PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Test
)

target_include_directories(run_all_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 创建自定义目标来运行所有测试
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose
    DEPENDS ${TEST_NAMES}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running all tests"
)

# 创建自定义目标来运行特定测试
add_custom_target(test_tcpclient
    COMMAND TcpClient_test
    DEPENDS TcpClient_test
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running TcpClient tests"
)

add_custom_target(test_usersession
    COMMAND UserSession_test
    DEPENDS UserSession_test
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running UserSession tests"
)

add_custom_target(test_jsonbuilder
    COMMAND JsonMessageBuilder_test
    DEPENDS JsonMessageBuilder_test
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running JsonMessageBuilder tests"
)

add_custom_target(test_statemanager
    COMMAND StateManager_test
    DEPENDS StateManager_test
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running StateManager tests"
)

add_custom_target(test_function
    COMMAND Function_test
    DEPENDS Function_test
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running Function tests"
)

add_custom_target(test_datamanager
    COMMAND DataManager_test
    DEPENDS DataManager_test
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running DataManager tests"
)

# 设置测试输出格式
set(CTEST_OUTPUT_ON_FAILURE TRUE)

# 打印测试信息
message(STATUS "智能医疗管理系统测试框架")
message(STATUS "Qt版本: Qt${QT_VERSION_MAJOR}")
message(STATUS "测试文件数量: ${CMAKE_LIST_LENGTH}")
if(DEFINED TEST_NAMES)
    message(STATUS "测试可执行文件: ${TEST_NAMES}")
endif()