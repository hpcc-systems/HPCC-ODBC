     README
     LexisNexis HPCC Systems
     HPCC-ODBC Connector
     Version 1.0

***********************************************************************
This README file contains last minute information about the HPCC-ODBC Connector
***********************************************************************

Disk Space Requirements
-------------------------------
    Windows 32-bit driver: 61 MB


Enhancement Requests and Fixes
-----------------------------------------


Installation and Configuration
-----------------------------------------
After successfully installing the HPCC-ODBC Connector, you must set the IP address of the WsSQL service using using the Microsoft Management Console (MMC). You can also configure other features with this console.

1) Install the HPCC-ODBC plugin.
2) From the Windows Start menu, select "HPCC Systems" > "HPCC-ODBC Connector" > "Management Console."
3) In the MMC, select "File" > "Add/Remove Snap-in..."
4) Double-click the  "HPCC Systems HPCC-ODBC Connector Manager 1.0 Local."
5) Press the "Finish" button and then press the "Ok" button.
6) Expand the "Console Root" > "Manager" >  "C:\Program Files (x86)\HPCCSystems\HPCC-ODBC Connector\cfg\oadm.ini" > "Services" > "HPCC-ODBC_ConnSvc" > "Data Source Settings" > "HPCC-ODBC_ConnDS"
7) Select  "IP Parameters"
8) Double-click the "DataSourceIPCustomProperties" and edit any of the values in the key/value pairs listed to settings that you prefer.
The default settings are : PROTOCOL=http;WSSQLPORT=8510;WSSQLIP=nnn.nnn.nnn.nnn;CLUSTER=hthor;DEFAULTQUERYSET=thor;MAXROWBUFFCOUNT=10000;
9) You must change the WSSQLIP value to the IP address of the HPCC ESP Server to which the WsSQL service is bound.
10)Select OK, the "File > "Save" and agree to all the prompts
11) Exit the MMC


Notes, Known Problems, and Restrictions
---------------------------------------------------



***********************************************************************
February 2014

End of README

