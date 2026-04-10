@echo off
cls
if not exist version.txt (
    echo 1 > version.txt
)
set /p VERSION=<version.txt
set /a VERSION=%VERSION%+1
echo %VERSION% > version.txt
set INDEX_FILE=index.h

if exist "%INDEX_FILE%" (
    (
        for /f "delims=" %%A in (%INDEX_FILE%) do (
            echo %%A | findstr /C:"#define APP_VERSION" >nul
            if not errorlevel 1 (
                echo #define APP_VERSION %VERSION%
            ) else (
                echo %%A
            )
        )
    ) > "%INDEX_FILE%.tmp"

    move /Y "%INDEX_FILE%.tmp" "%INDEX_FILE%" >nul
    echo APP_VERSION updated to %VERSION%.
) else (
    echo ERROR : %INDEX_FILE% not found !
)

docker build -t uvk5 .
del .\compiled-firmware\*.bin
docker run --rm -v %CD%:/app uvk5 /bin/bash -c "make clean && make -j$(nproc) ENABLE_FULL_BAND=1 ENABLE_EEPROM_512K=1 TARGET=robzyl_512k && make -j$(nproc) ENABLE_FULL_BAND=1 ENABLE_EEPROM_512K=0 TARGET=robzyl_8k && cp *.bin ./compiled-firmware/"

time /t
pause