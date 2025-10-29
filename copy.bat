
@echo off
setlocal enabledelayedexpansion

:: 参数检查
if "%~1"=="" (
    echo 用法: %~nx0 源文件夹 目标文件夹
    exit /b 1
)

:: 设置路径变量
set "source=%~f1"
set "dest=%~f2"

:: 创建目标根目录
if not exist "%dest%\" (
    md "%dest%"
    echo 已创建目录: %dest%
)

:: 递归复制函数
for /r "%source%" %%a in (*) do (
    set "filepath=%%a"
    set "relpath=!filepath:%source%=!"
    set "targetpath=%dest%!relpath!"
    
    :: 创建目标子目录
    for %%b in ("!targetpath!\..") do (
        if not exist "%%~fb" (
            md "%%~fb"
            echo 已创建目录: %%~fb
        )
    )
    
    :: 复制文件
    copy /y "%%a" "!targetpath!" >nul
    echo 已复制: %%a -^> !targetpath!
)

echo 操作完成
endlocal
