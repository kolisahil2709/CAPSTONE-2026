@echo off
echo ==================================================
echo   MySQL Root Password Resetter via Service Manager
echo ==================================================
echo.

:: 1. Create the init file in C:\
echo ALTER USER 'root'@'localhost' IDENTIFIED BY 'admin123'; > C:\mysql-init.txt

echo 2. Stopping MySQL service...
net stop MySQL80

echo.
echo 3. Configuring service to load the password reset file...
sc config MySQL80 binPath= "\"C:\Program Files\MySQL\MySQL Server 8.0\bin\mysqld.exe\" --defaults-file=\"C:\ProgramData\MySQL\MySQL Server 8.0\my.ini\" --init-file=\"C:\mysql-init.txt\" MySQL80"

echo.
echo 4. Starting service temporarily (this runs the reset code)...
net start MySQL80

echo.
echo 5. Waiting 5 seconds for execution...
timeout /t 5 /nobreak > nul

echo.
echo 6. Stopping MySQL service again...
net stop MySQL80

echo.
echo 7. Restoring original service configuration...
sc config MySQL80 binPath= "\"C:\Program Files\MySQL\MySQL Server 8.0\bin\mysqld.exe\" --defaults-file=\"C:\ProgramData\MySQL\MySQL Server 8.0\my.ini\" MySQL80"

echo.
echo 8. Restarting MySQL service normally...
net start MySQL80

:: Clean up the temp file
del C:\mysql-init.txt

echo.
echo ==================================================
echo   Password successfully reset to: admin123
echo ==================================================
pause
