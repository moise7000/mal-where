@echo off
setlocal

:: Compilateur et flags
set CXX=g++
set CXXFLAGS=

:: Noms des exécutables
set CIPHER=cipher.exe
set COMPUTER=computer.exe
set STUB=stub.exe
set PACKING=packing.exe
set COMPRESSOR=compressor.exe

:: Vérifier le premier argument
if "%1"=="clean" goto clean
if "%1"=="tests" goto tests
if "%1"=="" goto tests

echo Usage: build.bat [tests^|clean]
goto end

:tests
echo ========================================
echo Compilation of tests...
echo ========================================
echo.

:: Compilation de chaque exécutable
echo Compilation of %CIPHER%...
%CXX% %CXXFLAGS% -o %CIPHER% packer/Cipher.cpp tests/TestingTools.cpp tests/test_cipher.cpp
if errorlevel 1 goto error

echo Compilation of %COMPUTER%...
%CXX% %CXXFLAGS% -o %COMPUTER% packer/ComputerName.cpp tests/TestingTools.cpp tests/test_computer_name.cpp
if errorlevel 1 goto error

echo Compilation of %STUB%...
%CXX% %CXXFLAGS% -o %STUB% packer/Cipher.cpp packer/Stub.cpp packer/Compressor.cpp tests/TestingTools.cpp tests/test_stub.cpp
if errorlevel 1 goto error

echo Compilation of %PACKING%...
%CXX% %CXXFLAGS% -o %PACKING% packer/Cipher.cpp packer/Stub.cpp packer/Compressor.cpp packer/Packer.cpp tests/TestingTools.cpp tests/test_packing_mechanism.cpp
if errorlevel 1 goto error

echo Compilation of %COMPRESSOR%...
%CXX% %CXXFLAGS% -o %COMPRESSOR% packer/Compressor.cpp tests/TestingTools.cpp tests/test_compressor.cpp
if errorlevel 1 goto error

echo.
echo ========================================
echo Execution of the tests...
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
echo All tests are complete.
echo ========================================
goto end

:clean
echo Nettoyage des executables...
if exist %CIPHER% del %CIPHER%
if exist %COMPUTER% del %COMPUTER%
if exist %STUB% del %STUB%
if exist %PACKING% del %PACKING%
if exist %COMPRESSOR% del %COMPRESSOR%
echo Nettoyage termine.
goto end

:error
echo.
echo ========================================
echo ERROR: Compilation failed
echo ========================================
exit /b 1

:end
endlocal