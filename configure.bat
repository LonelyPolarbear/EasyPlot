@echo off
echo ================================
echo Configuring EasyPlot with MSVC x64
echo ================================
echo.

REM 切换到脚本所在目录
cd /d "%~dp0"
echo [1/5] Working directory: %CD%
echo.

REM 检查 ninja.exe 是否存在
if not exist ".\ninja.exe" (
    echo [ERROR] ninja.exe not found in %CD%!
    echo Please place ninja.exe in the project root directory.
    pause
    exit /b 1
)
echo [2/5] ninja.exe found.
echo.

REM 设置 Visual Studio 2022 环境（x64）
echo [3/5] Setting up Visual Studio 2022 environment (x64)...
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Failed to set up Visual Studio environment!
    pause
    exit /b 1
)
echo Visual Studio environment ready.
echo.

REM 执行 CMake 配置
echo [4/5] Running CMake configuration (Ninja, Debug)...
echo This may take a moment...
cmake -B build -G Ninja -DCMAKE_MAKE_PROGRAM=.\ninja.exe -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
if errorlevel 1 (
    echo.
    echo [ERROR] CMake configuration failed!
    pause
    exit /b 1
)
echo CMake configuration succeeded.
echo.

REM 复制 compile_commands.json 到根目录
echo [5/5] Copying compile_commands.json to project root...
if exist "build\compile_commands.json" (
    copy /Y "build\compile_commands.json" . >nul
    echo compile_commands.json copied successfully.
) else (
    echo [WARNING] compile_commands.json not found in build directory.
)
echo.

echo ================================
echo Configuration completed successfully!
echo ================================
pause