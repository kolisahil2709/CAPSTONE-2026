@echo off
setlocal
title chronosMIS - 24/7 Live Biometric Sync Daemon
color 0A
echo ========================================================
echo   CHRONOS MIS - 24/7 LIVE BIOMETRIC SYNC DAEMON
echo ========================================================
echo.
set "PATH=C:\Program Files\nodejs;%APPDATA%\npm;%PATH%"

set "DEV_IP=192.168.0.108"
set /p USER_INPUT="Enter your ESP32 Device IP [Press Enter for default: %DEV_IP%]: "
if not "%USER_INPUT%"=="" set "DEV_IP=%USER_INPUT%"

echo.
echo Connecting to ESP32 at: http://%DEV_IP%
echo Streaming live punches to: https://chronosmis-default-rtdb.firebaseio.com
echo.
echo [NOTE] Minimize this window to keep live punch streaming active 24/7!
echo --------------------------------------------------------
node live_sync_daemon.js %DEV_IP%
pause
