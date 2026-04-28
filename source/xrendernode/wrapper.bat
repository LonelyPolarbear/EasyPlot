@echo off
rem 获取当前脚本所在目录
set "SCRIPT_DIR=%~dp0"

rem 提取当前目录的名称作为目标名称
for %%I in ("%SCRIPT_DIR:~0,-1%") do set "TARGET_NAME=%%~nI"

rem 判断当前目录下是否包含文件名为XScan.h的文件，如果不存在，则不做处理
set SCAN_NAME=XScan.md
if not exist "%SCRIPT_DIR%%SCAN_NAME%" (
    echo %SCAN_NAME% not found in %SCRIPT_DIR%, skipping generation.
    goto :EOF
)else (
    echo %SCAN_NAME% found in %SCRIPT_DIR%, proceeding with generation.
    rem 扫描XScan.h 下的内容，提取其中#include "source/target_name/xxx.h"的行，并将其中的路径部分提取出来，替换为template.h，并将结果输出到compile_commands.json中

    E:\work\EasyPlot\ext\\xgenerator\output\bin\scanjson.exe --json=E:\work\EasyPlot\compile_commands.json --scan=%SCRIPT_DIR%\%SCAN_NAME% --out=%SCRIPT_DIR%\1.json

    rem call configureClang %TARGET_NAME%
    rem 检查 template_compile_command.json 是否生成成功
    if not exist "%SCRIPT_DIR%1.json" (
        echo [ERROR] 1.json not found after configureClang.
        goto :EOF
    )
  
    goto :SUC
)

:EOF
    echo error: %SCAN_NAME% not found, skipping generation.
    pause

:SUC
    echo [INFO] 1.json generated successfully.
    rem 1.json执行成功，开始扫描代码
   E:\work\EasyPlot\ext\xgenerator\output\bin\xmetagenerator.exe --json=%SCRIPT_DIR%1.json
    pause   