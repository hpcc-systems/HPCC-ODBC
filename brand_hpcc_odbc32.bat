rem if "%PROGRESS_SDK_DIR%".==. set PROGRESS_SDK_DIR=C:\Program Files (x86)\Progress\DataDirect\oaodbclocal72

@echo on
@echo ==============================================================================
@echo This batch file performs the branding of the HPCC-ODBC Connector and the Management
@echo Console plug-in. This assumes the "Progress DataDirect OEM SDK Local, version 7.2"
@echo ==============================================================================

@echo Ensure SDK is properly installed and mirrored
if not exist "%PROGRESS_SDK_DIR%\admin.ORIGINAL\mmcoa.dll" goto :MISSING_SDK_BRANCH
if not exist "%PROGRESS_SDK_DIR%\custom\install.ORIGINAL\basic\setup.ini" goto :MISSING_SDK_BRANCH

@echo ------------------------------------------------------------------------------
@echo Restore original Progress DataDirect SDK "image" folder
@echo ------------------------------------------------------------------------------
rmdir "%PROGRESS_SDK_DIR%\custom\install" /S /Q
xcopy "%PROGRESS_SDK_DIR%\custom\install.ORIGINAL" "%PROGRESS_SDK_DIR%\custom\install\*.*" /s /y /q
del "%PROGRESS_SDK_DIR%\custom\install\image\cla*.inp"
del "%PROGRESS_SDK_DIR%\custom\install\image\custom*.ini"

@echo ------------------------------------------------------------------------------
@echo Brand the MMC management console plug-in
@echo Refer to the "OpenAccess™ SDK Distribution Guide" for details
@echo ------------------------------------------------------------------------------
rmdir "%PROGRESS_SDK_DIR%\admin" /S /Q
xcopy "%PROGRESS_SDK_DIR%\admin.ORIGINAL" "%PROGRESS_SDK_DIR%\admin\*.*" /s /y /q
"%PROGRESS_SDK_DIR%\custom\branding\brandmmc" "%PROGRESS_SDK_DIR%\admin\mmcoa.dll" local -c "HPCC Systems" -s "HPCC Systems" -p "HPCC-ODBC Connector" -v 0.50
if not exist "%PROGRESS_SDK_DIR%\admin\prepmsc.reg" goto :MISSING_MMC

@echo ------------------------------------------------------------------------------
@echo Brand the drivers. We specify the "HS" prefix which is registered with Progress
@echo Refer to the "OpenAccess™ SDK Distribution Guide" for details
@echo ------------------------------------------------------------------------------
"%PROGRESS_SDK_DIR%\custom\branding\brandodbc" "%PROGRESS_SDK_DIR%\custom\install\image\bin"  local -N HS -P HPCC4Me  -X n -S n  -c "HPCC Systems" -s  "HPCC Systems"  -p "HPCC-ODBC Connector"  -v 0.50
if not exist "%PROGRESS_SDK_DIR%\custom\install\image\bin\HSoal25.dll" goto :MISSING_DRIVER

@echo ------------------------------------------------------------------------------
@echo Please ensure the following branded files were created for the Local 
@echo ODBC client, in the %PROGRESS_SDK_DIR%\custom\install\image\bin folder
@echo .
@echo HSoal25.dll - Branded Local ODBC client Driver 
@echo HSoald25.dll - Branded Local ODBC driver dialog file 
@echo HSoals25.dll - Branded Local ODBC driver setup file 
@echo HSoal25m.dll - Branded Local ODBC driver message file 
@echo .
@echo HSmgan25.dll - Branded “Anonymous” authentication MGSS module 
@echo HSmghu25.dll - Branded “Host User” authentication MGSS module 
@echo HSmgsp25.dll - Branded “SSPI (NTLM or Kerberos)” authentication MGSS module 
@echo .
@echo HSoal25.ini - Branded ODBC Driver config file. References are modified with the branded DLL names. 
@echo HS.SHA - Branded Product license file 
@echo ------------------------------------------------------------------------------
goto :DONE

:MISSING_SDK_BRANCH
@echo ERROR! "Progress DataDirect OEM SDK Local, version 7.2" missing or not mirrored. Please ensure it
@echo is installed, and create "%PROGRESS_SDK_DIR%\admin.ORIGINAL" and "%PROGRESS_SDK_DIR%\custom\install.ORIGINAL" !!!
goto :DONE

:MISSING_MMC
@echo ERROR! Branded MMC files appear to be missing (mmcoa.dll, prepmsc.reg, unprepmsc.reg) !!!
goto :DONE

:MISSING_DRIVER
@echo ERROR! Branded Driver files appear to be missing (HS*.dll) !!!
goto :DONE

:DONE
