@rem rd /s /q ./build/*.*
@rem @echo off	关闭之后所有命令的回显，不然bat文件中每条指令会在cmd命令窗口显示
@rem @echo off

del /f /q "build\*.*"

mkdir build
@rem echo 文件夹 "build" 已创建！

cmake -B build -DCMAKE_BUILD_TYPE=Debug -G  "Visual Studio 17 2022"

pause