@echo off
setlocal enabledelayedexpansion
echo ================================
echo Configuring EasyPlot with clang-cl (custom path)
echo ================================
echo.


REM 定义变量
set "CMAKELIST_PATH=E:\work\EasyPlot"

REM 切换到脚本所在目录
cd /d "%~dp0"
echo [1/5] Working directory: %CD%
echo.

REM 检查 ninja.exe 是否存在
if not exist "!CMAKELIST_PATH!\ninja.exe" (
    echo [ERROR] ninja.exe not found in %CD%!
    echo Please place ninja.exe in the project root directory.
    pause
    exit /b 1
)
echo [2/5] ninja.exe found.
echo.

REM === 设置 clang-cl 的完整路径 ===
REM 请将下面的路径修改为你的 clang-cl.exe 实际位置
set CLANG_CL_PATH=!CMAKELIST_PATH!\3rdParty\clang_llvm\bin\clang-cl.exe

REM 检查 clang-cl.exe 是否存在
if not exist "%CLANG_CL_PATH%" (
    echo [ERROR] clang-cl.exe not found at %CLANG_CL_PATH%!
    pause
    exit /b 1
)
echo [3/5] clang-cl found at %CLANG_CL_PATH%.
echo.

REM 设置 Visual Studio 2022 环境
echo [4/5] Setting up Visual Studio 2022 environment (x64)...
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Failed to set up Visual Studio environment!
    pause
    exit /b 1
)
echo Visual Studio environment ready.
echo.

REM 执行 CMake 配置，强制使用自定义路径的 clang-cl
set NINJA_PATH=!CMAKELIST_PATH!\ninja.exe
echo [5/5] Running CMake configuration (Ninja, Debug, clang-cl)...
cmake -S !CMAKELIST_PATH! -B !CMAKELIST_PATH!\build2 -G Ninja ^
       -DCMAKE_MAKE_PROGRAM="%NINJA_PATH%" ^
      -DCMAKE_C_COMPILER="%CLANG_CL_PATH%" ^
      -DCMAKE_CXX_COMPILER="%CLANG_CL_PATH%" ^
      -DCMAKE_BUILD_TYPE=Debug ^
      -DCMAKE_CXX_STANDARD=17 ^
      -DCMAKE_CXX_STANDARD_REQUIRED=ON ^
      -DCMAKE_CXX_EXTENSIONS=OFF ^
      -DCMAKE_CXX_FLAGS="/Zc:__cplusplus /utf-8 /EHsc /D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS -Wno-inconsistent-missing-override -Wignored-attributes" ^
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
      REM /D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS 标准库会关闭这些特定的弃用警告，让编译输出更干净
if errorlevel 1 (
    echo.
    echo [ERROR] CMake configuration failed!
    pause
    exit /b 1
)
echo CMake configuration succeeded.
echo.

REM 复制 compile_commands.json 到根目录
if exist "!CMAKELIST_PATH!\build2\compile_commands.json" (
   copy /Y "!CMAKELIST_PATH!\build2\compile_commands.json" . >nul
    echo compile_commands.json copied successfully.


    set TARGET_NAME=%~1

   if not "!TARGET_NAME!"=="" (
        echo Filtering compile_commands.json for target: !TARGET_NAME!

        REM 构造带目标名的输出文件名
        set OUTPUT_FILE=compile_commands_!TARGET_NAME!.json

        powershell -Command "$json = Get-Content compile_commands.json -Raw | ConvertFrom-Json; $filtered = $json | Where-Object { $_.file -like '*source/!TARGET_NAME!/*' }; $filtered | ConvertTo-Json -Depth 10 | Set-Content '!OUTPUT_FILE!'"
        if exist "!OUTPUT_FILE!" (
            echo Filtered file created: !OUTPUT_FILE!
        ) else (
             echo [WARNING] No entries found for target !TARGET_NAME!, no filtered file generated.
        )
    ) else (
        echo No target specified, keeping full compile_commands.json.
    )
) else (
    echo [WARNING] compile_commands.json not found in build2 directory.
)
echo.

REM ========== 转换为 arguments 格式并删除 command 字段 ==========
REM echo Converting compile_commands.json to arguments format...
REM powershell -Command "$json = Get-Content compile_commands.json -Raw | ConvertFrom-Json; foreach ($entry in $json) { if ($entry.command -and -not $entry.arguments) { $argsList = $entry.command -split ' '; $entry | Add-Member -NotePropertyName arguments -NotePropertyValue $argsList -Force }; if ($entry.PSObject.Properties['command']) { $entry.PSObject.Properties.Remove('command') } }; $json | ConvertTo-Json -Depth 10 | Set-Content compile_commands.json"
REM if errorlevel 1 (
REM   echo [ERROR] Conversion failed!
REM    pause
REM    exit /b 1
REM )
REM echo Conversion succeeded.

rem
rem echo.
rem 
rem echo ================================
rem echo Generating template from !OUTPUT_FILE! with source file changed to "template.h" and output field empty
rem echo ================================
rem 
rem if "!OUTPUT_FILE!"=="" (
rem     echo [ERROR] OUTPUT_FILE is not set. Please set OUTPUT_FILE before running this script.
rem     pause
rem     exit /b 1
rem )
rem 
rem if not exist "!OUTPUT_FILE!" (
rem     echo [ERROR] File !OUTPUT_FILE! not found!
rem     pause
rem     exit /b 1
rem )
rem 
rem powershell -Command "$inputFile = $env:OUTPUT_FILE; $json = Get-Content \"$inputFile\" -Raw | ConvertFrom-Json; if ($json -is [array]) { $entry = $json[0] } else { $entry = $json }; if ($entry) { $entry.PSObject.Properties.Remove('output'); $entry | Add-Member -MemberType NoteProperty -Name 'output' -Value '' -Force; $cmd = $entry.command.Replace('\\', '/'); $cmd = $cmd -replace '\s+/Fo\S+', ''; $cmd = $cmd -replace '\s+/Fd\S*', ''; $cmd = $cmd -replace '\s+-c\s+', ' '; $cmd = $cmd -replace '\s+--\s+\S+$', ''; $parts = $cmd -split '\s+'; for ($i=$parts.Count-1; $i -ge 0; $i--) { if ($parts[$i] -notmatch '^[/-]') { $parts[$i] = 'template.h'; break; } } $cmd = $parts -join ' '; $entry.command = $cmd.Trim(); $entry | ConvertTo-Json -Depth 10 | Set-Content 'template_compile_command.json'; Write-Host \"Success: template_compile_command.json created from $inputFile (source file renamed to template.h, output field empty).\"; } else { Write-Host '[ERROR] Input file contains no valid compile command!'; exit 1; }"
rem 
rem if errorlevel 1 (
rem     echo [ERROR] Failed to generate template compile command from !OUTPUT_FILE!
rem     pause
rem     exit /b 1
rem )
rem 
rem echo Template saved to template_compile_command.json
rem echo.

rem end
echo ================================
echo Configuration completed successfully!
echo ================================