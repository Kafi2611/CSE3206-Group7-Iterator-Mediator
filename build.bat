@echo off
REM ==========================================================
REM  CSE 3206 Lab 3 - Group 7 : one-click build for Windows
REM  Needs g++ (MinGW-w64 / MSYS2 / Code::Blocks MinGW) in PATH
REM  Usage:  build.bat          (build everything)
REM          build.bat run      (build, then run both demos)
REM ==========================================================
setlocal
set FLAGS=-std=c++17 -Wall -Wextra -Wpedantic
if not exist bin mkdir bin

if exist iterator\main.cpp (
    echo [1/4] Building iterator_demo ...
    g++ %FLAGS% -Iiterator\include iterator\src\CampusConnect.cpp iterator\src\FeedIterators.cpp iterator\src\FeedScreen.cpp iterator\main.cpp -o bin\iterator_demo.exe || goto :error
)
if exist mediator\main.cpp (
    echo [2/4] Building mediator_demo ...
    g++ %FLAGS% -Imediator\include mediator\src\User.cpp mediator\src\Member.cpp mediator\src\SupportBot.cpp mediator\src\MessengerServer.cpp mediator\main.cpp -o bin\mediator_demo.exe || goto :error
)
if exist tests\test_iterator.cpp if exist iterator\main.cpp (
    echo [3/4] Building test_iterator ...
    g++ %FLAGS% -Iiterator\include -Itests iterator\src\CampusConnect.cpp iterator\src\FeedIterators.cpp iterator\src\FeedScreen.cpp tests\test_iterator.cpp -o bin\test_iterator.exe || goto :error
    bin\test_iterator.exe || goto :error
)
if exist tests\test_mediator.cpp if exist mediator\main.cpp (
    echo [4/4] Building test_mediator ...
    g++ %FLAGS% -Imediator\include -Itests mediator\src\User.cpp mediator\src\Member.cpp mediator\src\SupportBot.cpp mediator\src\MessengerServer.cpp tests\test_mediator.cpp -o bin\test_mediator.exe || goto :error
    bin\test_mediator.exe || goto :error
)

echo.
echo Build OK. Executables are in the bin folder.
if /I "%1"=="run" (
    if exist bin\iterator_demo.exe bin\iterator_demo.exe
    if exist bin\mediator_demo.exe bin\mediator_demo.exe
)
exit /b 0

:error
echo.
echo *** BUILD OR TEST FAILED - see the messages above ***
exit /b 1
