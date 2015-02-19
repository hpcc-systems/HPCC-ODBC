     LexisNexis HPCC Systems
     HPCC-ODBC Connector
     Version 1.0
     Disk Space Requirements :   Windows 32-bit driver: 55  MB

***********************************************************************
This README file contains last minute information about the HPCC-ODBC Connector
***********************************************************************

Requirements
------------------

The HPCC-ODBC connector is a 32 bit Windows ODBC driver that interfaces with the HPCC Platform through the HPCC WsSQL service. WsSQL must be configured and running on the HPCC Platform, and you will need to know the IP address and port of that service.  This is typically the same IP as your ECL Watch service, and usually port 8510.


Installation and Configuration
-----------------------------------------

When executed, the downloaded installer program "HPCC-ODBCConnector.exe" created an "HPCC-ODBCConnector" folder in your current directory. You may delete this folder, and the program, once installation is complete.

After successfully installing the HPCC-ODBC Connector, you must first add the HPCC-ODBC Connector plug-in to the Microsoft Management Console (MMC). The MMC is needed to set and configure various features of the HPCC-ODBC Connector.

	1) From the Windows Start menu, select "HPCC Systems" > "HPCC-ODBC Connector" > "Management Console."
	2) In the MMC, select "File" > "Add/Remove Snap-in..."
	3) Select the  "HPCC Systems HPCC-ODBC Connector Manager 1.0 Local" and press the "Add" button.
	4) Press the "Finish" button and then press the "Ok" button.

Once the plug-in is installed, you must set the IP address of the HPCC WsSQL service, which must be running on your HPCC Platform. 

	1) In the MMC console, expand the "Console Root" > "Manager" >  "C:\Program Files (x86)\HPCCSystems\HPCC-ODBC Connector\cfg\oadm.ini" > "Services" > "HPCC-ODBC_ConnSvc" > "Data Source Settings" > "HPCC-ODBC_ConnDS"
	2) Select "IP Parameters"
	3) Double-click the "DataSourceIPCustomProperties" and edit any of the values in the key/value pairs listed to settings that you prefer. 
	     The default settings are : PROTOCOL=http;WSSQLPORT=8510;WSSQLIP=127.0.0.1;CLUSTER=hthor;DEFAULTQUERYSET=thor;MAXROWBUFFCOUNT=10000;
	4) You must change the WSSQLIP value to the IP address of the HPCC ESP Server to which the WsSQL service is bound.
	5) Ensure the WSSQLPORT is correct. Check with your HPCC system admin.
	6) Select OK, the "File > "Save" and agree to all the prompts
	7) Exit the MMC



Notes, Known Problems, and Restrictions
---------------------------------------------------



***********************************************************************
February 2014

End of README

