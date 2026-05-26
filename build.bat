if not exist "build" (
    mkdir build
)
del /q /s build\*
for /d %%p in (build\*) do rd /s /q "%%p"

cmake -S . -B build
cmake --build build

:: Just in case
timeout /t 1 /nobreak > nul

.\build\lyte.exe
