@echo off
setlocal
title chronosMIS - 24/7 Firebase Online Deployer
color 0B
echo ========================================================
echo   CHRONOS MIS - 24/7 ONLINE DEPLOYMENT ENGINE
echo ========================================================
echo.
echo Target URL: https://chronosmis.web.app/
echo.

set "PATH=C:\Program Files\nodejs;%APPDATA%\npm;%PATH%"

echo --------------------------------------------------------
echo  STEP 1: Log in to Firebase CLI
echo --------------------------------------------------------
call npx -y firebase-tools login

echo.
echo --------------------------------------------------------
echo  STEP 2: Your Available Projects in Firebase Console
echo --------------------------------------------------------
call npx -y firebase-tools projects:list

echo.
echo --------------------------------------------------------
echo  STEP 3: Uploading chronosMIS to Firebase Hosting...
echo --------------------------------------------------------
call npx -y firebase-tools deploy --only hosting --non-interactive

echo.
echo ========================================================
echo  Deployment process complete!
echo ========================================================
echo.
pause
