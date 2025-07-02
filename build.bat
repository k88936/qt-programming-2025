set QT_DIR=C:\Qt\6.8.3\mingw_64
::cmake -DCMAKE_BUILD_TYPE=Debug  -DCMAKE_TOOLCHAIN_FILE=%QT_DIR%\lib\cmake\Qt6\qt.toolchain.cmake  -S . -B build
::cmake --build build --target lucknight
%QT_DIR%\bin\windeployqt.exe build\lucknight.exe
:: mkdir build\plugins
:: robocopy build\multimedia build\plugins /E
