@echo off

::set SCRIPT_DIR_FULL=%~1
::set SCRIPT_DIR=%SCRIPT_DIR_FULL:~0,-1%
set SCRIPT_DIR=%~1
set BLD_DIR=%~2

set CATCH2_DIR=%SCRIPT_DIR%\ext\Catch2\
set GLFW_DIR=%SCRIPT_DIR%\ext\glfw\
set GLM_DIR=%SCRIPT_DIR%\ext\glm\
set GLAD_DIR=%SCRIPT_DIR%\ext\glad\
set NLOHMANN_JSON_DIR=%SCRIPT_DIR%\ext\nlohmann-json\
set TINYGLTF_DIR=%SCRIPT_DIR%\ext\tinygltf\
set STB_DIR=%SCRIPT_DIR%\ext\stb\
set BIGECC=%BLD_DIR%\.bigecc

:: if exist "%BLD_DIR%\.bigecc" rd /s /q "%BLD_DIR%\.bigecc"
:: if exist "%BIGECC%" rd /s /q "%BIGECC%"
mkdir "%BLD_DIR%"
mkdir "%BIGECC%"

call :Build_GLAD
call :Build_STB
call :Build_TinyGLTF
call :Build_Catch2
call :Build_GLFW
call :Build_GLM
call :Build_JSON

cd "%BLD_DIR%"
call :WriteCMakeLists
call :CreateProject
exit /b

:WriteCMakeLists
:: Write CMakeLists.txt
call :DeleteExcept "%BLD_DIR%" ".bigecc" "CMakeLists.txt"
set BIGECC_DIR_CM=%BIGECC:\=/%
set BIGECC_ROOT=%SCRIPT_DIR:\=/%
echo cmake_minimum_required(VERSION 3.25) > CMakeLists.txt
echo project(^"BiGE^") >> CMakeLists.txt
echo set(BIGE_ROOT_DIR ^"%BIGECC_ROOT%^") >> CMakeLists.txt
echo set(BIGECC_DIR ^"%BIGECC_DIR_CM%^") >> CMakeLists.txt
::
echo set(CMAKE_CXX_STANDARD 20) >> CMakeLists.txt
echo set(CMAKE_CXX_STANDARD_REQUIRED YES) >> CMakeLists.txt
::
echo include(^"${BIGE_ROOT_DIR}/bige.cmake^") >> CMakeLists.txt
exit /b


:DeleteExcept
set TARGET=%~1
set KEEP1=%~2
set KEEP2=%~3

:: Delete files
for %%F in ("%TARGET%\*") do (
    del /q "%%F"
)

:: Delete folders
for /d %%D in ("%TARGET%\*") do (
    if /i not "%%~nxD"=="%KEEP1%" (
        if /i not "%%~nxD"=="%KEEP2%" (
            rd /s /q "%%D"
        )
    )
)
exit /b

:CreateProject
cd "%BLD_DIR%"
cmake . -A x64
exit /b



:Build_GLAD
    :: Copy GLAD
    echo "- Building GLAD"
    if exist "%BIGECC%\glad\_in_progress_" (
        rd /s /q "%BIGECC%\glad\include"
        rd /s /q "%BIGECC%\glad\src"
    )

    if exist "%BIGECC%\glad\include" (
        if exist "%BIGECC%\glad\src" (
            echo "----- GLAD already built"
            exit /b
        )
    )
    mkdir "%BIGECC%\glad"
    cd "%BIGECC%\glad"
    echo "" >> _in_progress_
    xcopy /e /i /y "%GLAD_DIR%\include" "%BIGECC%\glad\include"
    xcopy /e /i /y "%GLAD_DIR%\src" "%BIGECC%\glad\src"
    del _in_progress_
    exit /b


:Build_STB
    echo "- Building STB"
    if exist "%BIGECC%\stb\_in_progress_" (
        rd /s /q "%BIGECC%\stb"
    )

    if exist "%BIGECC%\stb" (
        echo "----- STB already built"
        exit /b
    )

    mkdir "%BIGECC%\stb"
    cd "%BIGECC%\stb"
    echo "" >> _in_progress_
    xcopy /y "%STB_DIR%\*.h" "%BIGECC%\stb"
    del _in_progress_


:Build_TinyGLTF
    echo "- Building TinyGLTF"
    if exist "%BIGECC%\tinygltf\_in_progress_" (
        rd /s /q "%BIGECC%\tinygltf"
    )

    if exist "%BIGECC%\tinygltf\include" (
        if exist "%BIGECC%\tinygltf\lib" (
            echo "----- TinyGLTF already built"
            exit /b
        )
    )

    mkdir "%BIGECC%\tinygltf"
    cd "%BIGECC%\tinygltf"
    echo "" >> _in_progress_
    cmake %TINYGLTF_DIR% -DCMAKE_INSTALL_PREFIX="%BIGECC%\tinygltf" -DTINYGLTF_BUILD_LOADER_EXAMPLE=OFF ^
                        -DTINYGLTF_HEADER_ONLY=OFF -DTINYGLTF_INSTALL_VENDOR=OFF -DTINYGLTF_USE_CUSTOM_JSON=ON ^
                        -DCMAKE_DEBUG_POSTFIX=d

    cmake --build "%BIGECC%\tinygltf" --config DEBUG --parallel
    cmake --install "%BIGECC%\tinygltf" --config DEBUG

    cmake --build "%BIGECC%\tinygltf" --config Release --parallel
    cmake --install "%BIGECC%\tinygltf" --config Release
    call :DeleteExcept "%BIGECC%\tinygltf" "include" "lib"
    del _in_progress_
    exit /b


:Build_Catch2
    echo "- Building Catch2"
    if exist "%BIGECC%\Catch2\_in_progress_" (
        rd /s /q "%BIGECC%\Catch2"
    )

    if exist "%BIGECC%\Catch2\include" (
        if exist "%BIGECC%\Catch2\lib" (
            echo "----- Catch2 already built"
            exit /b
        )
    )

    mkdir "%BIGECC%\Catch2"
    cd "%BIGECC%\Catch2"
    echo "" >> _in_progress_
    cmake %CATCH2_DIR% -DCMAKE_INSTALL_PREFIX="%BIGECC%\Catch2" -DCATCH_ENABLE_REPRODUCIBLE_BUILD=OFF
    cmake --build "%BIGECC%\Catch2" --config Release --parallel
    cmake --install "%BIGECC%\Catch2" --config Release
    call :DeleteExcept "%BIGECC%\Catch2" "include" "lib"
    del _in_progress_
    exit /b

:Build_GLFW
    echo "- Building GLFW"
    if exist "%BIGECC%\glfw\_in_progress_" (
        rd /s /q "%BIGECC%\glfw"
    )

    if exist "%BIGECC%\glfw\include" (
        if exist "%BIGECC%\glfw\lib" (
            echo "----- GLFW already built"
            exit /b
        )
    )

    mkdir "%BIGECC%\glfw"
    cd "%BIGECC%\glfw"
    echo "" >> _in_progress_
    cmake %GLFW_DIR% -DCMAKE_INSTALL_PREFIX="%BIGECC%\glfw"
    cmake --build "%BIGECC%\glfw" --config Release --parallel
    cmake --install "%BIGECC%\glfw" --config Release
    call :DeleteExcept "%BIGECC%\glfw" "include" "lib"
    del _in_progress_
    exit /b

:Build_GLM
    echo "- Building GLM"
    if exist "%BIGECC%\glm\_in_progress_" (
        rd /s /q "%BIGECC%\glm"
    )

    if exist "%BIGECC%\glm\include" (
        if exist "%BIGECC%\glm\lib" (
            echo "----- GLM already built"
            exit /b
        )
    )

    mkdir "%BIGECC%\glm"
    cd "%BIGECC%\glm"
    echo "" >> _in_progress_
    cmake %GLM_DIR% -DCMAKE_INSTALL_PREFIX="%BIGECC%\glm"
    cmake --build "%BIGECC%\glm" --config Release --parallel
    cmake --install "%BIGECC%\glm" --config Release
    call :DeleteExcept "%BIGECC%\glm" "include" "lib"
    del _in_progress_
    exit /b


:Build_JSON
    echo "- Building JSON"
    if exist "%BIGECC%\nlohmann-json\_in_progress_" (
        rd /s /q "%BIGECC%\nlohmann-json"
    )

    if exist "%BIGECC%\nlohmann-json\include" (
        echo "----- GLM already built"
        exit /b
    )

    mkdir "%BIGECC%\nlohmann-json"
    cd "%BIGECC%\nlohmann-json"
    echo "" >> _in_progress_
    cmake %NLOHMANN_JSON_DIR% -DCMAKE_INSTALL_PREFIX="%BIGECC%\nlohmann-json" -DJSON_BuildTests=OFF -DJSON_MultipleHeaders=OFF
    cmake --build "%BIGECC%\nlohmann-json" --config Release --parallel
    cmake --install "%BIGECC%\nlohmann-json" --config Release
    call :DeleteExcept "%BIGECC%\nlohmann-json" "include" "lib"
    del _in_progress_
    exit /b