if "%PROGRESS_SDK64_DIR%".=="". set PROGRESS_SDK64_DIR=C:\Program Files\Progress\DataDirect\oaodbclocal72
if "%HPCCODBC_SRC_DIR%".=="". set HPCCODBC_SRC_DIR=C:\hpcc\HPCC-ODBC
if "%HPCC_BUILD_DIR_64%".=="". set HPCC_BUILD_DIR_64=C:\hpcc\HPCC-Lib64

@echo on
@echo ===============================================================================
@echo This batch file builds the installer for the 64 bit HPCC ODBC Connector.
@echo This assumes the "Progress DataDirect OEM SDK Local, version 7.2"
@echo ===============================================================================

@echo ------------------------------------------------------------------------------
@echo Copy Management Console plug-in that was branded by brand_hpcc_odbc64.bat
copy "%PROGRESS_SDK64_DIR%\admin\mmcoa.dll" "%PROGRESS_SDK64_DIR%\custom\install\image\admin"
copy "%PROGRESS_SDK64_DIR%\admin\prepmsc.reg" "%PROGRESS_SDK64_DIR%\custom\install\image\admin"
copy "%PROGRESS_SDK64_DIR%\admin\unprepmsc.reg" "%PROGRESS_SDK64_DIR%\custom\install\image\admin"

@echo ------------------------------------------------------------------------------
@echo Copy IP (Interface Provider)
copy "%HPCCODBC_SRC_DIR%\Release_x64\hpcc_odbc_64.dll" "%PROGRESS_SDK64_DIR%\custom\install\image\ip\bin" /y
if not exist "%PROGRESS_SDK64_DIR%\custom\install\image\ip\bin\hpcc_odbc_64.dll" goto :MISSING_IP

@echo ------------------------------------------------------------------------------
@echo Copy License files
copy "%PROGRESS_SDK64_DIR%\ipe\ISLV.INI" "%PROGRESS_SDK64_DIR%\custom\install\image\ipe" /y
copy "%PROGRESS_SDK64_DIR%\custom\install\image\bin\HS.SHA" "%PROGRESS_SDK64_DIR%\custom\install\image\ipe" /y
if not exist "%PROGRESS_SDK64_DIR%\custom\install\image\ipe\ISLV.INI" goto :MISSING_LICENSE
if not exist "%PROGRESS_SDK64_DIR%\custom\install\image\ipe\HS.SHA" goto :MISSING_LICENSE

@echo ------------------------------------------------------------------------------
@echo Copy HPCC dependencies
copy "%HPCC_BUILD_DIR_64%\jlib.dll" "%PROGRESS_SDK64_DIR%\custom\install\image\ip\bin" /y
copy "%HPCC_BUILD_DIR_64%\esphttp.dll" "%PROGRESS_SDK64_DIR%\custom\install\image\ip\bin" /y
if not exist "%PROGRESS_SDK64_DIR%\custom\install\image\ip\bin\jlib.dll" goto :MISSING_HPCC
if not exist "%PROGRESS_SDK64_DIR%\custom\install\image\ip\bin\esphttp.dll" goto :MISSING_HPCC

@echo ------------------------------------------------------------------------------
@echo Copy the contents of the image directory to the basic directory. This is the "install image," which a user can run the setup.exe to install
xcopy "%PROGRESS_SDK64_DIR%\custom\install\image\*.*" "%PROGRESS_SDK64_DIR%\custom\install\basic" /s /y /q

@echo ------------------------------------------------------------------------------
@echo Copy INI/INP
copy "%HPCCODBC_SRC_DIR%\progress\64bit\custom.ini" "%PROGRESS_SDK64_DIR%\custom\install\basic\custom.ini"  /y
copy "%HPCCODBC_SRC_DIR%\progress\64bit\cla.inp" "%PROGRESS_SDK64_DIR%\custom\install\basic\cla.inp"  /y
copy "%HPCCODBC_SRC_DIR%\progress\64bit\setup.ini" "%PROGRESS_SDK64_DIR%\custom\install\basic\setup.ini"  /y
copy "%HPCCODBC_SRC_DIR%\progress\64bit\readme.txt" "%PROGRESS_SDK64_DIR%\custom\install\basic\readme.txt"  /y
copy "%HPCCODBC_SRC_DIR%\progress\HPCC.bmp" "%PROGRESS_SDK64_DIR%\custom\install\basic\setup.bmp"  /y
goto :DONE

:MISSING_IP
@echo ERROR! HPCC IP driver 'hpcc_odbc_64.dll' appears to be missing !!!
goto :DONE

:MISSING_LICENSE
@echo ERROR! License files (ISLV.INI and HS.SHA) appear to be missing !!!
goto :DONE

:MISSING_HPCC
@echo ERROR! HPCC Dependencies (jlib.dll and esphttp.dll) appear to be missing !!!
goto :DONE

:DONE
