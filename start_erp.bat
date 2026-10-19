@echo off
title chronosMIS - 24/7 Biometric Cloud ERP Server
echo ========================================================
echo   CHRONOS MIS - 24/7 BIOMETRIC CLOUD ERP PORTAL
echo ========================================================
echo Starting local web server on port 5000...
python serve.py
if %ERRORLEVEL% NEQ 0 (
    echo Python server failed to start, launching index.html directly...
    start index.html
)
pause
