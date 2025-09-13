@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

REM 智能医疗管理系统 - Windows 构建脚本

REM 颜色定义
set "RED=[91m"
set "GREEN=[92m"
set "YELLOW=[93m"
set "BLUE=[94m"
set "NC=[0m"

REM 默认配置
set "BUILD_TYPE=Release"
set "BUILD_DIR=build"
set "CLEAN_BUILD=false"
set "BUILD_SERVER=false"
set "BUILD_CLIENT=true"
set "PACKAGE=false"

REM 日志函数
:log_info
echo %BLUE%[INFO]%NC% %~1
goto :eof

:log_success
echo %GREEN%[SUCCESS]%NC% %~1
goto :eof

:log_warning
echo %YELLOW%[WARNING]%NC% %~1
goto :eof

:log_error
echo %RED%[ERROR]%NC% %~1
goto :eof

REM 显示帮助信息
:show_help
echo 智能医疗管理系统构建脚本
echo.
echo 用法: %~nx0 [选项]
echo.
echo 选项:
echo     -h, --help          显示此帮助信息
echo     -c, --clean         清理构建目录
echo     -d, --debug         构建调试版本
echo     -r, --release       构建发布版本 (默认)
echo     -s, --server        只构建服务器
echo     -t, --client        只构建客户端 (默认)
echo     -b, --both          构建服务器和客户端
echo     -p, --package       打包应用程序
echo     -o, --output DIR    指定输出目录
echo.
echo 示例:
echo     %~nx0                  # 构建客户端发布版本
echo     %~nx0 -c -b -p         # 清理并构建打包所有组件
echo     %~nx0 -d -s            # 构建调试版本服务器
goto :eof

REM 解析命令行参数
:parse_args
if "%~1"=="" goto :eof
if "%~1"=="-h" set "SHOW_HELP=true" & shift & goto :parse_args
if "%~1"=="--help" set "SHOW_HELP=true" & shift & goto :parse_args
if "%~1"=="-c" set "CLEAN_BUILD=true" & shift & goto :parse_args
if "%~1"=="--clean" set "CLEAN_BUILD=true" & shift & goto :parse_args
if "%~1"=="-d" set "BUILD_TYPE=Debug" & shift & goto :parse_args
if "%~1"=="--debug" set "BUILD_TYPE=Debug" & shift & goto :parse_args
if "%~1"=="-r" set "BUILD_TYPE=Release" & shift & goto :parse_args
if "%~1"=="--release" set "BUILD_TYPE=Release" & shift & goto :parse_args
if "%~1"=="-s" set "BUILD_SERVER=true" & shift & goto :parse_args
if "%~1"=="--server" set "BUILD_SERVER=true" & shift & goto :parse_args
if "%~1"=="-t" set "BUILD_CLIENT=false" & shift & goto :parse_args
if "%~1"=="--client" set "BUILD_CLIENT=false" & shift & goto :parse_args
if "%~1"=="-b" set "BUILD_SERVER=true" & shift & goto :parse_args
if "%~1"=="--both" set "BUILD_SERVER=true" & shift & goto :parse_args
if "%~1"=="-p" set "PACKAGE=true" & shift & goto :parse_args
if "%~1"=="--package" set "PACKAGE=true" & shift & goto :parse_args
if "%~1"=="-o" set "BUILD_DIR=%~2" & shift & shift & goto :parse_args
if "%~1"=="--output" set "BUILD_DIR=%~2" & shift & shift & goto :parse_args
call :log_error "未知选项: %~1"
call :show_help
exit /b 1

REM 检查依赖
:check_dependencies
call :log_info "检查构建依赖..."

REM 检查 CMake
where cmake >nul 2>&1
if %errorlevel% neq 0 (
    call :log_error "未找到 CMake，请安装 CMake 3.16+"
    exit /b 1
)

REM 检查编译器
where cl >nul 2>&1
if %errorlevel% neq 0 (
    where g++ >nul 2>&1
    if %errorlevel% neq 0 (
        call :log_error "未找到 C++ 编译器，请安装 Visual Studio 或 MinGW"
        exit /b 1
    )
)

REM 检查 Qt
where qmake >nul 2>&1
if %errorlevel% neq 0 (
    call :log_warning "未找到 Qt，可能会影响构建"
)

call :log_success "依赖检查完成"
goto :eof

REM 清理构建目录
:clean_build
if exist "%BUILD_DIR%" (
    call :log_info "清理构建目录: %BUILD_DIR%"
    rmdir /s /q "%BUILD_DIR%" >nul 2>&1
)
goto :eof

REM 创建构建目录
:setup_build_dir
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
cd /d "%BUILD_DIR%"
goto :eof

REM 构建客户端
:build_client
call :log_info "构建客户端 (构建类型: %BUILD_TYPE%)..."

REM 创建客户端构建目录
if not exist "client" mkdir client
cd client

REM 配置 CMake
cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ..\..

REM 编译
nmake

REM 检查编译结果
if exist "INTELLIGENT_HEALTHCARE_Client.exe" (
    call :log_success "客户端构建成功"
) else (
    call :log_error "客户端构建失败"
    exit /b 1
)

cd ..
goto :eof

REM 构建服务器
:build_server
call :log_info "构建服务器..."

REM 创建服务器构建目录
if not exist "server" mkdir server
cd server

REM 编译服务器 (需要 MySQL Connector/C 和 Boost)
if exist "C:\Program Files\MySQL\MySQL Connector C 8.0\include" (
    set "MYSQL_INCLUDE=C:\Program Files\MySQL\MySQL Connector C 8.0\include"
    set "MYSQL_LIB=C:\Program Files\MySQL\MySQL Connector C 8.0\lib\opt"
) else if exist "C:\Program Files\MySQL\MySQL Connector C 6.1\include" (
    set "MYSQL_INCLUDE=C:\Program Files\MySQL\MySQL Connector C 6.1\include"
    set "MYSQL_LIB=C:\Program Files\MySQL\MySQL Connector C 6.1\lib\opt"
) else (
    call :log_error "未找到 MySQL Connector/C，请安装并设置路径"
    exit /b 1
)

if exist "C:\local\boost\include" (
    set "BOOST_INCLUDE=C:\local\boost\include"
    set "BOOST_LIB=C:\local\boost\lib"
) else if exist "C:\Boost\include" (
    set "BOOST_INCLUDE=C:\Boost\include"
    set "BOOST_LIB=C:\Boost\lib"
) else (
    call :log_error "未找到 Boost 库，请安装并设置路径"
    exit /b 1
)

REM 使用 Visual Studio 编译器
cl /EHsc /std:c++17 /O2 ^
    /I"%MYSQL_INCLUDE%" ^
    /I"%BOOST_INCLUDE%" ^
    /I"..\..\Server" ^
    "..\..\Server\server.cpp" ^
    /link /LIBPATH:"%MYSQL_LIB%" /LIBPATH:"%BOOST_LIB%" ^
    libmysql.lib boost_system-vc142-mt-x64-1_83.lib ^
    /OUT:server.exe

if %errorlevel% equ 0 (
    call :log_success "服务器构建成功"
) else (
    call :log_error "服务器构建失败"
    exit /b 1
)

cd ..
goto :eof

REM 打包应用程序
:package_app
call :log_info "打包应用程序..."

set "PACKAGE_DIR=%BUILD_DIR%\package"
set "VERSION=1.0.0"

REM 创建包目录
if not exist "%PACKAGE_DIR%" mkdir "%PACKAGE_DIR%"

REM 复制可执行文件
if "%BUILD_CLIENT%"=="true" (
    if exist "%BUILD_DIR%\client\INTELLIGENT_HEALTHCARE_Client.exe" (
        copy "%BUILD_DIR%\client\INTELLIGENT_HEALTHCARE_Client.exe" "%PACKAGE_DIR%\" >nul
    )
)

if "%BUILD_SERVER%"=="true" (
    if exist "%BUILD_DIR%\server\server.exe" (
        copy "%BUILD_DIR%\server\server.exe" "%PACKAGE_DIR%\" >nul
        REM 复制 MySQL 运行时库
        if exist "%MYSQL_LIB%\libmysql.dll" (
            copy "%MYSQL_LIB%\libmysql.dll" "%PACKAGE_DIR%\" >nul
        )
    )
)

REM 复制资源文件
if exist "..\..\resources" (
    xcopy "..\..\resources" "%PACKAGE_DIR%\resources\" /E /I /Y >nul
)

REM 复制文档
copy "..\..\README.md" "%PACKAGE_DIR%\" >nul
copy "..\..\LICENSE" "%PACKAGE_DIR%\" >nul

REM 创建启动批处理文件
echo @echo off > "%PACKAGE_DIR%\start_client.bat"
echo cd /d "%%~dp0" >> "%PACKAGE_DIR%\start_client.bat"
echo start "" "INTELLIGENT_HEALTHCARE_Client.exe" >> "%PACKAGE_DIR%\start_client.bat"

echo @echo off > "%PACKAGE_DIR%\start_server.bat"
echo cd /d "%%~dp0" >> "%PACKAGE_DIR%\start_server.bat"
echo start "" "server.exe" >> "%PACKAGE_DIR%\start_server.bat"

REM 创建配置文件模板
echo # 数据库配置示例 > "%PACKAGE_DIR%\config.example"
echo DB_HOST=localhost >> "%PACKAGE_DIR%\config.example"
echo DB_USER=your_username >> "%PACKAGE_DIR%\config.example"
echo DB_PASSWORD=your_password >> "%PACKAGE_DIR%\config.example"
echo DB_NAME=SmartMedical >> "%PACKAGE_DIR%\config.example"
echo DB_PORT=3306 >> "%PACKAGE_DIR%\config.example"
echo. >> "%PACKAGE_DIR%\config.example"
echo # 服务器配置 >> "%PACKAGE_DIR%\config.example"
echo SERVER_PORT=1437 >> "%PACKAGE_DIR%\config.example"
echo SERVER_HOST=0.0.0.0 >> "%PACKAGE_DIR%\config.example"

call :log_success "打包完成"
goto :eof

REM 主函数
:main
call :log_info "开始构建智能医疗管理系统..."

REM 解析参数
:parse_loop
if "%~1"=="" goto :parse_done
call :parse_args %*
goto :parse_loop
:parse_done

if "%SHOW_HELP%"=="true" (
    call :show_help
    exit /b 0
)

call :check_dependencies

if "%CLEAN_BUILD%"=="true" (
    call :clean_build
)

call :setup_build_dir

if "%BUILD_CLIENT%"=="true" (
    call :build_client
)

if "%BUILD_SERVER%"=="true" (
    call :build_server
)

if "%PACKAGE%"=="true" (
    call :package_app
)

call :log_success "构建完成！"
call :log_info "输出目录: %BUILD_DIR%"

goto :eof

REM 运行主函数
call :main %*