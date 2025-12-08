# Tests CMakeLists.txt
cmake_minimum_required(VERSION 3.16)

# 查找 Qt Test 组件
find_package(Qt6 QUIET COMPONENTS Test)
if(NOT Qt6_FOUND)
    find_package(Qt5 REQUIRED COMPONENTS Test)
endif()

# 启用测试
enable_testing()

# 设置测试编译选项
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含目录
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# 收集所有测试源文件
file(GLOB_RECURSE TEST_SOURCES
    "unit/*_test.cpp"
    "integration/*_test.cpp"
    "performance/*_test.cpp"
)

# 收集 Mock 源文件
file(GLOB_RECURSE MOCK_SOURCES "mocks/*.cpp")

# 创建测试可执行文件
foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)

    add_executable(${TEST_NAME}
        ${TEST_SOURCE}
        ${MOCK_SOURCES}
    )

    # 链接 Qt Test
    if(Qt6_FOUND)
        target_link_libraries(${TEST_NAME}
            Qt6::Test
            Qt6::Core
            Qt6::Network
            Qt6::Widgets
        )
    else()
        target_link_libraries(${TEST_NAME}
            Qt5::Test
            Qt5::Core
            Qt5::Network
            Qt5::Widgets
        )
    endif()

    # 添加测试到 CTest
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

    # 设置测试工作目录
    set_tests_properties(${TEST_NAME} PROPERTIES
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )

    # 为单元测试添加标签
    if(${TEST_SOURCE} MATCHES "unit/")
        set_tests_properties(${TEST_NAME} PROPERTIES
            LABELS "unit"
        )
    endif()

    # 为集成测试添加标签
    if(${TEST_SOURCE} MATCHES "integration/")
        set_tests_properties(${TEST_NAME} PROPERTIES
            LABELS "integration"
        )
    endif()

    # 为性能测试添加标签
    if(${TEST_SOURCE} MATCHES "performance/")
        set_tests_properties(${TEST_NAME} PROPERTIES
            LABELS "performance"
        )
    endif()
endforeach()

# 创建测试套件
# 单元测试套件
add_custom_target(test_unit
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --label-regex "unit"
    DEPENDS ${TEST_NAMES}
    COMMENT "Running unit tests"
)

# 集成测试套件
add_custom_target(test_integration
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --label-regex "integration"
    DEPENDS ${TEST_NAMES}
    COMMENT "Running integration tests"
)

# 性能测试套件
add_custom_target(test_performance
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --label-regex "performance"
    DEPENDS ${TEST_NAMES}
    COMMENT "Running performance tests"
)

# 运行所有测试
add_custom_target(test_all
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS ${TEST_NAMES}
    COMMENT "Running all tests"
)

# 生成测试覆盖率报告（如果支持）
if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    find_program(GCOV_PATH gcov)
    find_program(LCOV_PATH lcov)
    find_program(GENHTML_PATH genhtml)

    if(GCOV_PATH AND LCOV_PATH AND GENHTML_PATH)
        # 启用覆盖率
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")

        # 生成覆盖率报告
        add_custom_target(coverage
            COMMAND ${LCOV_PATH} --directory . --capture --output-file coverage.info
            COMMAND ${LCOV_PATH} --remove coverage.info '/usr/*' '${CMAKE_SOURCE_DIR}/tests/*' --output-file coverage.info
            COMMAND ${GENHTML_PATH} -o coverage coverage.info
            COMMAND ${CMAKE_COMMAND} -E remove coverage.info
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating code coverage report"
        )
    endif()
endif()

# 安装测试（可选）
if(INSTALL_TESTS)
    install(TARGETS ${TEST_NAMES}
        RUNTIME DESTINATION bin/tests
    )
endif()