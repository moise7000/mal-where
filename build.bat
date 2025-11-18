@echo off
setlocal

:: Compiler and flags
set CXX=g++

set WINDRES=windres
set CXXFLAGS=-lz
set RESOURCES_RC=resources.rc
set RESOURCES_OBJ=resources.o


:: Executable names
set CIPHER=cipher.exe
set SYS_ENV=system_env.exe
set STUB=stub.exe
set PACKING=packing.exe
set COMPRESSOR=compressor.exe
SET COMPOSE=compose.exe
set FAKE_REC=fake_rec.exe
set SHA=sha.exe
set CALL=call_printf.exe
set DEVIL=devil.exe
set ANTI_DEBUG=anti_debug.exe
set CUSTOM=custom.exe

:: Check first argument
if "%1"=="clean" goto clean

if "%1"=="resources" goto compile_resources
if "%1"=="tests" (
    if "%2"=="" (
        goto tests_all
    ) else (
        goto test_single
    )
)
if "%1"=="test" goto test_single
if "%1"=="" goto tests_all

echo Usage: build.bat [test [TEST_NAME]^|tests^|resources^|clean]
echo Available tests: CIPHER, SYS_ENV, STUB, PACKING, COMPRESSOR, COMPOSE, FAKE_REC, CALL, DEVIL, ANTI_DEBUG, CUSTOM
echo.
echo Commands:
echo   build.bat                - Compile and run all tests
echo   build.bat resources      - Compile only resources.rc
echo   build.bat test TEST_NAME - Compile and run specific test
echo   build.bat clean          - Remove all executables and resources
goto end



:compile_resources
echo ********************************************
echo *       Compiling resources.rc...          *
echo ********************************************
if not exist %RESOURCES_RC% (
    echo ERROR: %RESOURCES_RC% not found!
    goto error
)
%WINDRES% %RESOURCES_RC% -O coff -o %RESOURCES_OBJ%
if errorlevel 1 (
    echo ERROR: Failed to compile resources
    goto error
)
echo Resources compiled successfully: %RESOURCES_OBJ%
goto end





:test_single
set TEST_NAME=%2
if "%TEST_NAME%"=="" (
    echo ERROR: Please specify a test name
    echo Available tests: CIPHER, SYS_ENV, STUB, PACKING, COMPRESSOR
    goto error
)


:: Compile resources first
call :compile_resources
if errorlevel 1 goto error

call :compile_%TEST_NAME%
if errorlevel 1 goto error
call :run_%TEST_NAME%
goto end

:tests_all
echo ********************************************
echo *          Compiling tests...              *
echo ********************************************
echo.

:: Compile resources first

call :compile_resources
if errorlevel 1 goto error

echo.

:: Compile each executable
call :compile_CIPHER
if errorlevel 1 goto error

call :compile_SYS_ENV
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



call :compile_SHA
if errorlevel 1 goto error

call :compile_CALL
if errorlevel 1 goto error

call :compile_DEVIL
if errorlevel 1 goto error

call :compile_ANTI_DEBUG
if errorlevel 1 goto error

call :compile_CUSTOM
if errorlevel 1 goto error

echo.
echo ********************************************
echo *          Running tests...                *
echo ********************************************
echo.

call :run_CIPHER
call :run_SYS_ENV
call :run_STUB
call :run_PACKING
call :run_COMPRESSOR
call :run_COMPOSE
call :run_FAKE_REC
call :run_SHA
call :run_CALL
call :run_DEVIL
call :run_ANTI_DEBUG
call :run_CUSTOM


echo.
echo ********************************************
echo *           All tests completed            *
echo ********************************************
goto end

:: Compilation functions
:compile_CIPHER
echo Compiling %CIPHER%...
%CXX% -o %CIPHER% packer/Cipher.cpp  env/SystemEnvironment.cpp tests/TestingTools.cpp tests/test_cipher.cpp  %CXXFLAGS%
%CXX% -o %CIPHER% packer/Cipher.cpp env/SystemEnvironment.cpp tests/TestingTools.cpp tests/test_cipher.cpp %RESOURCES_OBJ% %CXXFLAGS%
exit /b %errorlevel%

:compile_SYS_ENV
echo Compiling %SYS_ENV%...
%CXX% -o %SYS_ENV% env/SystemEnvironment.cpp tests/TestingTools.cpp tests/test_sys_env.cpp  %CXXFLAGS%
%CXX% -o %SYS_ENV% env/SystemEnvironment.cpp tests/TestingTools.cpp tests/test_sys_env.cpp %RESOURCES_OBJ% %CXXFLAGS%
exit /b %errorlevel%

:compile_STUB
echo Compiling %STUB%...
%CXX% -o %STUB% packer/Cipher.cpp packer/Stub.cpp packer/Compressor.cpp tests/TestingTools.cpp tests/test_stub.cpp  %CXXFLAGS%
%CXX% -o %STUB% packer/Cipher.cpp packer/Stub.cpp packer/Compressor.cpp tests/TestingTools.cpp tests/test_stub.cpp %RESOURCES_OBJ% %CXXFLAGS%
exit /b %errorlevel%

:compile_PACKING
echo Compiling %PACKING%...
%CXX% -o %PACKING% packer/Cipher.cpp packer/Stub.cpp packer/Compressor.cpp packer/Packer.cpp tests/TestingTools.cpp tests/test_packing_mechanism.cpp  %CXXFLAGS%
%CXX% -o %PACKING% packer/Cipher.cpp packer/Stub.cpp packer/Compressor.cpp packer/Packer.cpp tests/TestingTools.cpp tests/test_packing_mechanism.cpp %RESOURCES_OBJ% %CXXFLAGS%
exit /b %errorlevel%

:compile_COMPRESSOR
echo Compiling %COMPRESSOR%...
%CXX% -o %COMPRESSOR% packer/Compressor.cpp tests/TestingTools.cpp tests/test_compressor.cpp  %CXXFLAGS%
%CXX% -o %COMPRESSOR% packer/Compressor.cpp tests/TestingTools.cpp tests/test_compressor.cpp %RESOURCES_OBJ% %CXXFLAGS%
exit /b %errorlevel%

:compile_COMPOSE
echo Compiling %COMPOSE%...
%CXX% -o %COMPOSE% obfuscation_methods/compose.cpp tests/TestingTools.cpp tests/test_compose.cpp  %CXXFLAGS%
%CXX% -o %COMPOSE% obfuscation_methods/compose.cpp tests/TestingTools.cpp tests/test_compose.cpp %RESOURCES_OBJ% %CXXFLAGS%
exit /b %errorlevel%

:compile_FAKE_REC
echo Compiling %FAKE_REC%...
%CXX% -o %FAKE_REC% obfuscation_methods/fake_rec.cpp tests/TestingTools.cpp tests/test_fake_rec.cpp  %CXXFLAGS%
%CXX% -o %FAKE_REC% obfuscation_methods/fake_rec.cpp tests/TestingTools.cpp tests/test_fake_rec.cpp %RESOURCES_OBJ% %CXXFLAGS%
exit /b %errorlevel%



:compile_SHA
echo Compiling %SHA%...
%CXX% -o %SHA% crypto/hash_function.cpp  tests/test_hash_function.cpp  %CXXFLAGS%
exit /b %errorlevel%


:compile_CALL
echo Compiling %CALL%...
%CXX% -o %CALL% obfuscation_methods/LoadPrintfFunction.cpp env/SystemEnvironment.cpp packer/Cipher.cpp tests/TestingTools.cpp tests/test_call_printf.cpp  %CXXFLAGS%
%CXX% -o %CALL% obfuscation_methods/LoadPrintfFunction.cpp env/SystemEnvironment.cpp packer/Cipher.cpp tests/TestingTools.cpp tests/test_call_printf.cpp %RESOURCES_OBJ% %CXXFLAGS%
exit /b %errorlevel%

:compile_DEVIL
echo Compiling %DEVIL%...
%CXX% -o %DEVIL% devil/DevilCode.cpp devil/Rickroll.cpp -lwinmm %CXXFLAGS%
%CXX% -o %DEVIL% devil/DevilCode.cpp devil/Rickroll.cpp -lwinmm  %RESOURCES_OBJ% %CXXFLAGS%
exit /b %errorlevel%

:compile_ANTI_DEBUG
echo Compiling %ANTI_DEBUG%...
%CXX% -o %ANTI_DEBUG% tests/test_anti_debug.cpp -lpsapi %CXXFLAGS%
exit /b %errorlevel%

:compile_CUSTOM
echo Compiling %CUSTOM%...
%CXX% -o %CUSTOM% tests/test_custom.cpp devil/custom.cpp env/SystemEnvironment.cpp packer/Cipher.cpp crypto/hash.cpp obfuscation_methods/anti_debug/anti_debug.cpp -lpsapi %CXXFLAGS% %RESOURCES_OBJ%
exit /b %errorlevel%

:: Run functions
:run_CIPHER
echo *** Test Cipher ***
%CIPHER%
echo.
exit /b 0

:run_SYS_ENV
echo *** Testing System Environment namespace ***
%SYS_ENV%
echo.
exit /b 0

:run_STUB
echo *** Test Stub ***
%STUB%
echo.
exit /b 0

:run_PACKING
echo *** Test Packing ***
%PACKING%
echo.
exit /b 0

:run_COMPRESSOR
echo *** Test Compressor ***
%COMPRESSOR%
echo.
exit /b 0

:run_COMPOSE
echo *** Test Compose ***
%COMPOSE%
echo.
exit /b 0

:run_FAKE_REC
echo *** Test Fake rec ***
%FAKE_REC%
echo.
exit /b 0



:run_SHA
echo *** Test sha function ***
%SHA%
echo.
exit /b 0

:run_CALL
echo *** Test call printf ***
%CALL%
echo.
exit /b 0

:run_DEVIL
echo *** Test devil ***
%DEVIL%
echo.
exit /b 0

:run_ANTI_DEBUG
echo *** Test Anti-Debug ***
%ANTI_DEBUG%
echo.
exit /b 0

:run_CUSTOM
echo *** Test Custom Function ***
%CUSTOM%
echo.
exit /b 0


:clean
echo Cleaning executables...
if exist %CIPHER% del %CIPHER%
if exist %SYS_ENV% del %SYS_ENV%
if exist %STUB% del %STUB%
if exist %PACKING% del %PACKING%
if exist %COMPRESSOR% del %COMPRESSOR%
if exist %COMPOSE% del %COMPOSE%
if exist %FAKE_REC% del %FAKE_REC%
if exist %SHA% del %SHA%
if exist %CALL% del %CALL%
if exist %ANTI_DEBUG% del %ANTI_DEBUG%
if exist %CUSTOM% del %CUSTOM%

if exist %RESOURCES_OBJ% del %RESOURCES_OBJ%
echo Cleaning completed.
goto end

:error
echo.
echo ********************************************
echo *        ERROR: Compilation failed         *
echo ********************************************
exit /b 1

:end
endlocal