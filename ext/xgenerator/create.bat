@rem rd /s /q ./build/*.*
@rem @echo off	�ر�֮����������Ļ��ԣ���Ȼbat�ļ���ÿ��ָ�����cmd�������ʾ
@rem @echo off

del /f /q "build\*.*"

mkdir build
@rem echo �ļ��� "build" �Ѵ�����

cmake -G "Visual Studio 17 2022" -Thost=x64 -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" -B build -S .

pause