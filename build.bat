@echo off
setlocal

:: Compiler and flags
set CXX=g++
set CXXFLAGS=-lz

:: Executable names
set CIPHER=cipher.exe
set COMPUTER=computer.exe
set STUB=stub.exe
set PACKING=packing.exe
set COMPRESSOR=compressor.exe
SET COMPOSE=compose.exe
set FAKE_REC=fake_rec.exe

:: Check first argument
if "%1"=="clean" goto clean
if "%1"=="tests" (
    if "%2"=="" (
        goto tests_all
    ) else (
        goto test_single
    )
)
if "%1"=="test" goto test_single
if "%1"=="" goto tests_all

echo Usage: build.bat [test [TEST_NAME]^|tests^|clean]
echo Available tests: CIPHER, COMPUTER, STUB, PACKING, COMPRESSOR, COMPOSE, FAKE_REC
goto end

:test_single
set TEST_NAME=%2
if "%TEST_NAME%"=="" (
    echo ERROR: Please specify a test name
    echo Available tests: CIPHER, COMPUTER, STUB, PACKING, COMPRESSOR
    goto error
)
call :compile_%TEST_NAME%
if errorlevel 1 goto error
call :run_%TEST_NAME%
goto end

:tests_all
echo ========================================
echo Compiling tests...
echo ========================================
echo.

:: Compile each executable
call :compile_CIPHER
if errorlevel 1 goto error

call :compile_COMPUTER
if errorlevel 1 goto error

call :compile_STUB
if errorlevel 1 goto error

call :compile_PACKING
if errorlevel 1 goto error

call :compile_COMPRESSOR
if errorlevel 1 goto error

call :compile_COMPOSE
if errorlevel 1 goto error

call :compile_FAKE_REC
if errorlevel 1 goto error

echo.
echo ========================================
echo Running tests...
echo ========================================
echo.

call :run_CIPHER
call :run_COMPUTER
call :run_STUB
call :run_PACKING
call :run_COMPRESSOR
call :run_COMPOSE
call :run_FAKE_REC


echo.
echo ========================================
echo All tests completed
echo ========================================
goto end

:: Compilation functions
:compile_CIPHER
echo Compiling %CIPHER%...
%CXX% -o %CIPHER% packer/Cipher.cpp tests/TestingTools.cpp tests/test_cipher.cpp %CXXFLAGS%
exit /b %errorlevel%

:compile_COMPUTER
echo Compiling %COMPUTER%...
%CXX% -o %COMPUTER% packer/ComputerName.cpp tests/TestingTools.cpp tests/test_computer_name.cpp %CXXFLAGS%
exit /b %errorlevel%

:compile_STUB
echo Compiling %STUB%...
%CXX% -o %STUB% packer/Cipher.cpp packer/Stub.cpp packer/Compressor.cpp tests/TestingTools.cpp tests/test_stub.cpp %CXXFLAGS%
exit /b %errorlevel%

:compile_PACKING
echo Compiling %PACKING%...
%CXX% -o %PACKING% packer/Cipher.cpp packer/Stub.cpp packer/Compressor.cpp packer/Packer.cpp tests/TestingTools.cpp tests/test_packing_mechanism.cpp %CXXFLAGS%
exit /b %errorlevel%

:compile_COMPRESSOR
echo Compiling %COMPRESSOR%...
%CXX% -o %COMPRESSOR% packer/Compressor.cpp tests/TestingTools.cpp tests/test_compressor.cpp %CXXFLAGS%
exit /b %errorlevel%

:compile_COMPOSE
echo Compiling %COMPOSE%...
%CXX% -o %COMPOSE% obfuscation_methods/compose.cpp tests/TestingTools.cpp tests/test_compose.cpp %CXXFLAGS%
exit /b %errorlevel%

:compile_FAKE_REC
echo Compiling %FAKE_REC%...
%CXX% -o %FAKE_REC% obfuscation/fake_rec.cpp tests/TestingTools.cpp tests/test_fake_rec.cpp %CXXFLAGS%
exit /b %errorlevel%

:: Run functions
:run_CIPHER
echo === Test Cipher ===
%CIPHER%
echo.
exit /b 0

:run_COMPUTER
echo === Test Computer Name ===
%COMPUTER%
echo.
exit /b 0

:run_STUB
echo === Test Stub ===
%STUB%
echo.
exit /b 0

:run_PACKING
echo === Test Packing ===
%PACKING%
echo.
exit /b 0

:run_COMPRESSOR
echo === Test Compressor ===
%COMPRESSOR%
echo.
exit /b 0

:run_COMPOSE
echo === Test Compose ===
%COMPOSE%
echo.
exit /b 0

:run_FAKE_REC
echo === Test Fake rec ===
%FAKE_REC%
echo.
exit /b 0

:clean
echo Cleaning executables...
if exist %CIPHER% del %CIPHER%
if exist %COMPUTER% del %COMPUTER%
if exist %STUB% del %STUB%
if exist %PACKING% del %PACKING%
if exist %COMPRESSOR% del %COMPRESSOR%
if exist %COMPOSE% del %COMPOSE%
if exist %FAKE_REC% del %FAKE_REC%
echo Cleaning completed.
goto end

:error
echo.
echo ========================================
echo ERROR: Compilation failed
echo ========================================
exit /b 1

:end
endlocal