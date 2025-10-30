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

:: Check first argument
if "%1"=="clean" goto clean
if "%1"=="tests" goto tests
if "%1"=="" goto tests

echo Usage: build.bat [tests^|clean]
goto end

:tests
echo ========================================
echo Compiling tests...
echo ========================================
echo.

:: Compile each executable
echo Compiling %CIPHER%...
%CXX% -o %CIPHER% packer/Cipher.cpp tests/TestingTools.cpp tests/test_cipher.cpp %CXXFLAGS%
if errorlevel 1 goto error

echo Compiling %COMPUTER%...
%CXX% -o %COMPUTER% packer/ComputerName.cpp tests/TestingTools.cpp tests/test_computer_name.cpp %CXXFLAGS%
if errorlevel 1 goto error

echo Compiling %STUB%...
%CXX% -o %STUB% packer/Cipher.cpp packer/Stub.cpp packer/Compressor.cpp tests/TestingTools.cpp tests/test_stub.cpp %CXXFLAGS%
if errorlevel 1 goto error

echo Compiling %PACKING%...
%CXX% -o %PACKING% packer/Cipher.cpp packer/Stub.cpp packer/Compressor.cpp packer/Packer.cpp tests/TestingTools.cpp tests/test_packing_mechanism.cpp %CXXFLAGS%
if errorlevel 1 goto error

echo Compiling %COMPRESSOR%...
%CXX% -o %COMPRESSOR% packer/Compressor.cpp tests/TestingTools.cpp tests/test_compressor.cpp %CXXFLAGS%
if errorlevel 1 goto error

echo.
echo ========================================
echo Running tests...
echo ========================================
echo.

echo === Test Cipher ===
%CIPHER%
echo.

echo === Test Computer Name ===
%COMPUTER%
echo.

echo === Test Stub ===
%STUB%
echo.

echo === Test Packing ===
%PACKING%
echo.

echo === Test Compressor ===
%COMPRESSOR%
echo.

echo ========================================
echo All tests completed
echo ========================================
goto end

:clean
echo Cleaning executables...
if exist %CIPHER% del %CIPHER%
if exist %COMPUTER% del %COMPUTER%
if exist %STUB% del %STUB%
if exist %PACKING% del %PACKING%
if exist %COMPRESSOR% del %COMPRESSOR%
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