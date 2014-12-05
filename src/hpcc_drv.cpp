/*##############################################################################

    HPCC SYSTEMS software Copyright (C) 2014 HPCC Systems.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
############################################################################## */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "damip.h"
#include "hpcc_util.hpp"


#pragma warning (disable : 538)

TM_ModuleCB     hpcc_tm_Handle;
IP_SUPPORT_ARRAY    hpcc_support_array =
    {
        0,
        1, /* IP_SUPPORT_SELECT */
        0, /* IP_SUPPORT_INSERT */
        0, /* IP_SUPPORT_UPDATE */
        0, /* IP_SUPPORT_DELETE */
        1, /* IP_SUPPORT_SCHEMA */      //HPCC Dynamic schema
        0, /* IP_SUPPORT_PRIVILEGES */
        1, /* IP_SUPPORT_OP_EQUAL */
        0, /* IP_SUPPORT_OP_NOT   */
        0, /* IP_SUPPORT_OP_GREATER */
        0, /* IP_SUPPORT_OP_SMALLER */
        0, /* IP_SUPPORT_OP_BETWEEN */
        0, /* IP_SUPPORT_OP_LIKE    */
        0, /* IP_SUPPORT_OP_NULL    */
        0, /* IP_SUPPORT_SELECT_FOR_UPDATE */
        0, /* IP_SUPPORT_START_QUERY */
        0, /* IP_SUPPORT_END_QUERY */
        0, /* IP_SUPPORT_UNION_CONDLIST */
        0, /* IP_SUPPORT_CREATE_TABLE */
        0, /* IP_SUPPORT_DROP_TABLE */
        0, /* IP_SUPPORT_CREATE_INDEX */
        0, /* IP_SUPPORT_DROP_INDEX */
        1, /* IP_SUPPORT_PROCEDURE */       //HPCC supports stored procedures
        0, /* IP_SUPPORT_CREATE_VIEW */
        0, /* IP_SUPPORT_DROP_VIEW */
        0, /* IP_SUPPORT_QUERY_VIEW */
        0, /* IP_SUPPORT_CREATE_USER */
        0, /* IP_SUPPORT_DROP_USER */
        0, /* IP_SUPPORT_CREATE_ROLE */
        0, /* IP_SUPPORT_DROP_ROLE */
        0, /* IP_SUPPORT_GRANT */
        0, /* IP_SUPPORT_REVOKE */
        0,  /* IP_SUPPORT_PUSHDOWN_QUERY */
        0,  /* IP_SUPPORT_NATIVE_COMMAND */
        0,  /* IP_SUPPORT_ALTER_TABLE */
        0,  /* IP_SUPPORT_BLOCK_JOIN */
        0,  /* IP_SUPPORT_XA */
        0,  /* IP_SUPPORT_QUERY_MODE_SELECTION */
        0,  /* IP_SUPPORT_VALIDATE_SCHEMAOBJECTS_IN_USE */
        0,  /* IP_SUPPORT_UNICODE_INFO */
        0,  /* Reserved for future use */
        0,  /* Reserved for future use */
        0,  /* Reserved for future use */
        0,  /* Reserved for future use */
        0,  /* Reserved for future use */
        0,  /* Reserved for future use */
        0,  /* Reserved for future use */
        0,  /* Reserved for future use */
        0,  /* Reserved for future use */
        0   /* Reserved for future use */
    };

#define HPCC_QUALIFIER_NAME "SCHEMA"
#define HPCC_CATALOG_NAME   "SCHEMA"
#define HPCC_USER_NAME      "HPCCUSER"   //table is managed by user(us)

static int     bAllowSchemaSearchPattern = FALSE;

static int      hpcc_is_matching_table(damobj_table *pSearchObj,
                                        char *table_qualifier,
                                        char *table_owner,
                                        char *table_name);
static int      hpcc_is_matching_column(damobj_column *pSearchObj,
                                        char *table_qualifier,
                                        char *table_owner,
                                        char *table_name);
int				ip_format_query(DAM_HQUERY hquery,
                                        char *pSqlBuffer,
                                        int *piWriteOffset);

/************************************************************************
Function:       OAIP_init()
Description:    Initialize the Interface Provider and the data source being supported
and return handle to the context of the driver.

Return:         DAM_SUCCESS on Success
                DAM_FAILURE on Failure
                DAM_NOT_AVAILABLE if Data source is not available or No Driver
************************************************************************/
int             OAIP_init(TM_ModuleCB tmHandle, XM_Tree *pMemTree, IP_HENV *phenv)
{
    InitModuleObjects();
    HPCC_ENV_DA      *pEnvDA;
    hpcc_tm_Handle = tmHandle;//save the trace module handle
    tm_trace(hpcc_tm_Handle, UL_TM_F_TRACE, "HPCC_Conn:OAIP_init() has been called\n", ());//allocate the environment handle
    if(!(pEnvDA = (HPCC_ENV_DA *)xm_allocItem(pMemTree, sizeof(HPCC_ENV_DA), XM_NOFLAGS)))
    {
        return DAM_FAILURE;
    }

    pEnvDA->pMemTree = pMemTree;    //save the memory tree handle
    *phenv = pEnvDA;                //set the return value
    return DAM_SUCCESS;
}

/************************************************************************
Function:       OAIP_exit()
Description:    Shutdown the Interface Provider. Close the data source.
Return:         DAM_SUCCESS on Success
                DAM_FAILURE on Failure
************************************************************************/
int             OAIP_exit(IP_HENV henv)
{
    HPCC_ENV_DA  *pEnv = (HPCC_ENV_DA *)henv;

    tm_trace(hpcc_tm_Handle, UL_TM_F_TRACE, "HPCC_Conn:OAIP_exit() has been called\n", ());
    releaseAtoms();
    ExitModuleObjects();
    return DAM_SUCCESS;
}

/************************************************************************
Function:       OAIP_getInfo()
Description:    Return value for the requested information
Return:         DAM_SUCCESS on Success
                DAM_FAILURE on Failure
************************************************************************/
int             OAIP_getInfo(IP_HENV ip_henv, IP_HDBC ip_hdbc, IP_HSTMT hstmt, int iInfoType,
                             void *pInfoValue, int iInfoValueMax, int *piInfoValueLen)
{
    char            sFunctionName[]="OAIP_getInfo";
    HPCC_ENV_DA    *pEnvDA = (HPCC_ENV_DA *)ip_henv;
    HPCC_CONN_DA   *pConnDA = (HPCC_CONN_DA *)ip_hdbc;
    short          *pShortIntValue = (short *)pInfoValue;
    char           *pStrValue = (char *)pInfoValue;
    char            sBuf[128];

    tm_trace(hpcc_tm_Handle, UL_TM_TRIVIA, "HPCC_Conn:%s() called for InfoType.\n",(sFunctionName, iInfoType));

    switch (iInfoType) {
    case IP_INFO_QUALIFIER_TERM:    /* return the Qualifier term used in Schema information */
        strcpy(pStrValue, "database");
        break;

    case IP_INFO_OWNER_TERM:        /* return the Owner term used in Schema information */
        strcpy(pStrValue, "owner");
        break;

    case IP_INFO_QUALIFIER_NAME:    /* return the Qualifier name used in Schema information */
        strcpy(pStrValue, pConnDA->sQualifier);
        break;

    case IP_INFO_OWNER_NAME:
        strcpy(pStrValue, pConnDA->sUserName);
        break;

    case IP_INFO_FILTER_VIEWS_WITH_QUALIFIER_NAME:
        *pShortIntValue = FALSE;
        break;

    case IP_INFO_VALIDATE_TABLE_WITH_OWNER: /* we do not support multiple schemas */
        *pShortIntValue = FALSE;
        break;

    case IP_INFO_SUPPORT_SCHEMA_SEARCH_PATTERN:
        sprintf(sBuf, "%s(): IP_INFO_SUPPORT_SCHEMA_SEARCH_PATTERN=%d", sFunctionName, bAllowSchemaSearchPattern);
        tm_trace(hpcc_tm_Handle, UL_TM_MAJOR_EV, "HPCC_Conn:%s\n", (sBuf));
        *pShortIntValue = (short)bAllowSchemaSearchPattern;
        break;

    default:
        /* we will not report it as error, since the ip_getInfo() is an optional implementation */
        sprintf(sBuf, "%s(): Information type:%d is out of range", sFunctionName, iInfoType);
        tm_trace(hpcc_tm_Handle, UL_TM_TRIVIA, "HPCC_Conn:%s\n", (sBuf));
        return DAM_NOT_AVAILABLE;
        break;
    }

    return DAM_SUCCESS;
}

/************************************************************************
Function:       OAIP_setInfo()
Description:    Return value for the requested information
Return:         DAM_SUCCESS on Success
                DAM_FAILURE on Failure
************************************************************************/
int             OAIP_setInfo(IP_HENV ip_henv, IP_HDBC ip_hdbc, IP_HSTMT hstmt, int iInfoType,
                             void *pInfoValue, int iInfoValueLen)
{
    char        sFunctionName[]="OAIP_setInfo";

    HPCC_ENV_DA      *pEnvDA = (HPCC_ENV_DA *)ip_henv;
    HPCC_CONN_DA     *pConnDA = (HPCC_CONN_DA *)ip_hdbc;
    char            *pStrValue = (char *)pInfoValue;
    char            sBuf[128];

    tm_trace(hpcc_tm_Handle, UL_TM_F_TRACE, "HPCC_Conn:%s() called for InfoType.\n",(sFunctionName, iInfoType));

    switch (iInfoType) {
    case IP_INFO_QUALIFIER_NAME:
        /* Save the Qualifier name */
        strcpy(pConnDA->sQualifier, pStrValue);
        break;

    default:
        /* we will not report it as error, since the ip_setInfo() is an optional implementation */
        sprintf(sBuf, "%s(): Information type:%d is out of range", sFunctionName, iInfoType);
        tm_trace(hpcc_tm_Handle, UL_TM_MAJOR_EV, "HPCC_Conn:%s\n", (sBuf));
        return DAM_NOT_AVAILABLE;
        break;
    }

    return DAM_SUCCESS;
}

/************************************************************************
Function:       OAIP_getSupport()
Description:    Return value for the requested information
Return:         DAM_SUCCESS on Success
                DAM_FAILURE on Failure
************************************************************************/
int             OAIP_getSupport(IP_HDBC hdbc, int iSupportType,
                                int *pSupportExists)
{
    HPCC_CONN_DA     *pConnDA = (HPCC_CONN_DA *)hdbc;
    *pSupportExists = hpcc_support_array[iSupportType];
    return DAM_SUCCESS;
}

/************************************************************************
Function:       OAIP_connect()
Description:    Connect to the data source being specified using the
                authentication information
Return:         DAM_SUCCESS on Success
                DAM_FAILURE on Failure
************************************************************************/
int             OAIP_connect(DAM_HDBC dam_hdbc, IP_HENV henv,
                             XM_Tree *pMemTree, char *sDatasourceName, char *sUserName, char *sPassword,
                             char *sCurrentCatalog, char *sIPProperties, char *sIPCustomProperties,
                             IP_HDBC *phdbc)
{
    HPCC_ENV_DA      *pEnvDA = (HPCC_ENV_DA *)henv;
    HPCC_CONN_DA     *pConnDA;

    tm_trace(hpcc_tm_Handle, UL_TM_F_TRACE, "HPCC_Conn:OAIP_connect() for user %s\n", (sUserName));

    /* allocate the connection da */
    if(!(pConnDA = (HPCC_CONN_DA *)xm_allocItem(pMemTree, sizeof(HPCC_CONN_DA), XM_NOFLAGS)))
        return DAM_FAILURE;

    /* initialize the ConnDA */
    pConnDA->pMemTree = pMemTree;
    pConnDA->pEnvDA = pEnvDA;
    pConnDA->dam_hdbc = dam_hdbc;

    //Isolate Protocol
    StringBuffer protocol;
    const char * p = sl_strstr(sIPCustomProperties, "PROTOCOL=");
    if (p)
    {
        int x = 0;
        p += 9;
        while (*p && *p != ';')
            protocol.append(*(p++));
    }
    else
        protocol.set("http");

    //Isolate ws_sql port
    StringBuffer wssqlPort;
    p = sl_strstr(sIPCustomProperties, "WSSQLPORT=");
    if (p)
    {
        int x = 0;
        p += 10;
        while (*p && *p != ';')
            wssqlPort.append(*(p++));
    }
    else
        wssqlPort.set("8510");

    //Isolate ESP IP
    StringBuffer espIP;
    p = sl_strstr(sIPCustomProperties, "ESP=");
    if (p)
    {
        int x = 0;
        p += 4;
        while (*p && *p != ';')
            espIP.append(*(p++));
    }

    //Isolate Target Cluster
    StringBuffer targetCluster;
    p = sl_strstr(sIPCustomProperties, "CLUSTER=");
    if (p)
    {
        int x = 0;
        p += 8;
        while (*p && *p != ';')
            targetCluster.append(*(p++));
    }
    else
        targetCluster.set("roxie");


    //Isolate Default QuerySet, or targets
    StringBuffer defaultQuerySet;
    p = sl_strstr(sIPCustomProperties, "DEFAULTQUERYSET=");
    if (p)
    {
        int x = 0;
        p += 16;
        while (*p && *p != ';')
            defaultQuerySet.append(*(p++));
    }
    else
        defaultQuerySet.set("roxie");

    //Isolate Default QuerySet
    aindex_t maxFetchRowCount;
    p = sl_strstr(sIPCustomProperties, "MAXROWBUFFCOUNT=");
    if (p)
        maxFetchRowCount = p ? atol(p+16) : -1;//-1 if no limit

    /* initialize the IP data source */
    pConnDA->pHPCCdb = new HPCCdb(hpcc_tm_Handle, protocol.str(), wssqlPort.str(), sUserName, sPassword, espIP.str(), targetCluster.str(), defaultQuerySet.str(), maxFetchRowCount);
    if (!pConnDA->pHPCCdb->ping())
    {
        delete pConnDA->pHPCCdb;
        tm_trace(hpcc_tm_Handle, UL_TM_PARM, "HPCC_Conn:OAIP_connect() failed, unable to connect to %s:wssql\n", (espIP.str()));
        return DAM_FAILURE;
    }

    /* We will use Qualifier and User name that matches with Schema */
    sl_strcpy(pConnDA->sQualifier, HPCC_QUALIFIER_NAME);
    sl_strcpy(pConnDA->sUserName, HPCC_USER_NAME);

    *phdbc = pConnDA;/* set the return value */

    return DAM_SUCCESS;
}

/************************************************************************
Function:       OAIP_disconnect()
Description:    Disconnect from the data source. Close open file handles etc.
Return:         DAM_SUCCESS on Success
                DAM_FAILURE on Failure
************************************************************************/
int             OAIP_disconnect(DAM_HDBC dam_hdbc, IP_HDBC hdbc)
{
    HPCC_CONN_DA     *pConnDA = (HPCC_CONN_DA *)hdbc;

    tm_trace(hpcc_tm_Handle, UL_TM_F_TRACE, "HPCC_Conn:OAIP_disconnect() has been called\n", ());

    assert(pConnDA->dam_hdbc == dam_hdbc);

    /* disconnect from the data source */
    if (pConnDA->pHPCCdb)
    {
        delete pConnDA->pHPCCdb;
        pConnDA->pHPCCdb = NULL;
    }

    /* free the connection da */
    xm_freeItem(pConnDA);

    return DAM_SUCCESS;
}

/************************************************************************
Function:       OAIP_startTransaction()
Description:    Start a new transaction on the connection
Return:         DAM_SUCCESS on Success
                DAM_FAILURE on Failure
************************************************************************/
int             OAIP_startTransaction(DAM_HDBC dam_hdbc, IP_HDBC hdbc)
{
//    HPCC_CONN_DA     *pConnDA = (HPCC_CONN_DA *)hdbc;
    return DAM_SUCCESS;
}

/************************************************************************
Function:       OAIP_endTransaction()
Description:    End the transaction on the connection. Use the iType to
                either commit or rollback
Return:         DAM_SUCCESS on Success
                DAM_FAILURE on Failure
************************************************************************/
int             OAIP_endTransaction(DAM_HDBC dam_hdbc, IP_HDBC hdbc, int iType)
{
//    HPCC_CONN_DA     *pConnDA = (HPCC_CONN_DA *)hdbc;
    return DAM_SUCCESS;
}

/************************************************************************
Function:       OAIP_execute()
Description:    Execute the given statement
Return:         DAM_SUCCESS on Success
                DAM_FAILURE on Failure
************************************************************************/
int             OAIP_execute(IP_HDBC hdbc,
                             DAM_HSTMT hstmt,
                             int iStmtType,
                             DAM_HCOL hSearchCol,
                             int *piNumResRows)
{
    char            sFunctionName[]="OAIP_execute";
    HPCC_CONN_DA *  pConnDA = (HPCC_CONN_DA *)hdbc;
    HPCC_STMT_DA *  pStmtDA;
    XM_Tree      *  pMemTree;
    DAM_HCOL        hcol;

    tm_trace(hpcc_tm_Handle, UL_TM_F_TRACE, "HPCC_Conn:OAIP_execute() has been called", (0));

    if(iStmtType != DAM_SELECT)
    {
        tm_trace(hpcc_tm_Handle, UL_TM_MINOR_EV, "HPCC_Conn:Only SELECT statements supported", (0));
        return DAM_FAILURE;
    }

    pMemTree = dam_getMemTree(hstmt);/* get the memory tree to be used */
    if(!(pStmtDA = (HPCC_STMT_DA *)xm_allocItem(pMemTree, sizeof(HPCC_STMT_DA), XM_NOFLAGS)))/* allocate a new stmt */
        return DAM_FAILURE;

    /* initialize the StmtDA */
    pStmtDA->pMemTree = pMemTree;
    pStmtDA->pConnDA = pConnDA;
    pStmtDA->dam_hstmt = hstmt;
    pStmtDA->iType = iStmtType;

    /* get the table information */
    CTable *pTable = NULL;
    dam_describeTable(pStmtDA->dam_hstmt, NULL, NULL, pStmtDA->sTableName, NULL, (char*)NULL);
    pConnDA->pHPCCdb->getTableSchema(pStmtDA->sTableName, &pTable);
    pConnDA->pHPCCdb->clearOutputColumnDescriptors();

    //-----------------------------------------------------------
    //The following calls retrieve the output table column format.
    //Look up these columns in our hpcc CColumn cache, and save them
    //in an ordered array of CColumn descriptors.  Once data is retrieved (below)
    //we can iterate over these columns to add the columns to OpenAcces rows
    //-----------------------------------------------------------
    hcol = dam_getFirstCol(pStmtDA->dam_hstmt, DAM_COL_IN_USE);
    while (hcol)
    {
        char    sColName[DAM_MAX_ID_LEN+1];
        int     iColNum;
        int     iXOType;
        int     iColType;

        dam_describeCol(hcol,       //DAM_HCOL  hcol,
                        &iColNum,   //int *piColNum,
                        sColName,   //char *pColName,
                        &iXOType,   //int *piXOType,
                        &iColType); //int *piColType);

        CColumn *pCol = pTable->queryColumn(iColNum);//find this hpcc CColumn descriptor
        assert(0 == strcmp(sColName, pCol->m_name));
//        pCol->m_iColNum     = iColNum;
        assert(pCol->m_iXOType = iXOType);
        pCol->m_hcol        = hcol;
        pConnDA->pHPCCdb->addOutputColumnDescriptor(pCol);//save in ordered array of column descriptors
        hcol = dam_getNextCol(pStmtDA->dam_hstmt);
    }

    //-----------------------------------
    //Build the SQL SELECT query string
    //-----------------------------------
    int     iRetCode;
    char    szSqlBuffer[4*1024];
    int     iWriteOffset = 0;
    iRetCode = ip_format_query(dam_getQuery(hstmt), szSqlBuffer, &iWriteOffset);
#ifdef _DEBUG
    DUMP(szSqlBuffer);
#endif

    //-----------------------------------
    //execute the SELECT query
    //-----------------------------------
    *piNumResRows = 0;
    if (DAM_SUCCESS == iRetCode && szSqlBuffer[0] != NULL)
        iRetCode = hpcc_exec(szSqlBuffer, NULL, pStmtDA, piNumResRows);

    tm_trace(hpcc_tm_Handle, UL_TM_INFO, "HPCC_Conn:OAIP_execute() Query %s\n",(iRetCode == DAM_SUCCESS ? "executed successfully" : "failed"));

    return iRetCode;
}

/************************************************************************
Function:       OAIP_procedure
Description:

Return:         DAM_SUCCESS on Success
                DAM_FAILURE on Failure
************************************************************************/
int             OAIP_procedure(IP_HDBC hdbc, DAM_HSTMT hstmt, int iType, int *piNumResRows)
{
    HPCC_STMT_DA *  pStmtDA;
    XM_Tree      *  pMemTree;

    pMemTree = dam_getMemTree(hstmt);/* get the memory tree to be used */
    if(!(pStmtDA = (HPCC_STMT_DA *)xm_allocItem(pMemTree, sizeof(HPCC_STMT_DA), XM_NOFLAGS)))/* allocate a new stmt */
        return DAM_FAILURE;

    /* initialize the StmtDA */
    pStmtDA->pMemTree = pMemTree;
    pStmtDA->pConnDA = (HPCC_CONN_DA *)hdbc;
    pStmtDA->dam_hstmt = hstmt;
    pStmtDA->iType = -1;

    //Get stored procedure name
    char sProcName[DAM_MAX_ID_LEN+1];
    dam_describeProcedure(hstmt, NULL, NULL, sProcName, NULL);

    //Get HPCC descriptor for requested stored procedure
    HPCC_CONN_DA   *pConnDA = (HPCC_CONN_DA *)hdbc;
    CMyQuerySetQuery * pQuery = NULL;
    pQuery = pConnDA->pHPCCdb->queryStoredProcedure(pConnDA->pHPCCdb->queryCurrentQuerySet(), sProcName);//find specified stored procedure
    if (!pQuery)
        return DAM_FAILURE;

    CTable *pTable = pQuery->queryOutputDataset(0);//wssql only supports single output dataset
    aindex_t numCols = pTable->queryNumColumns();

    pConnDA->pHPCCdb->clearOutputColumnDescriptors();
    for (aindex_t idx = 0; idx < numCols; idx++)
    {
        CColumn *pCol = pTable->queryColumn(idx);//find this hpcc CColumn descriptor
        pCol->m_hcol =  dam_getCol(hstmt, (char*)pCol->m_name.get());
        pConnDA->pHPCCdb->addOutputColumnDescriptor(pCol);//save in ordered array of column descriptors
    }

    //Build the SQL SELECT query string
    char    szSqlBuffer[4*1024];
    sprintf(szSqlBuffer, "CALL %s", sProcName);

    //Get value of the input parameters
    DAM_HROWELEM hRowElem;
    int     iParamNum;
    int     iNumInputs = 0;
    int     iRetCode;
    DAM_HROW hrow = dam_getInputRow(hstmt);

    for ( hRowElem = dam_getFirstValueSet(hstmt, hrow), iParamNum=1; hRowElem != NULL;
        hRowElem = dam_getNextValueSet(hstmt), iParamNum++)
    {
        DAM_HCOL hcol = dam_getColToSet(hRowElem);
        int     iColumnType;
        char    szValueName[50];
        int     iXOType;
        dam_describeCol(hcol,       //DAM_HCOL  hcol,
                        NULL,       //int * piColNum,
                        szValueName,//char* pColName,
                        &iXOType,   //int * piXOType,
                        NULL);      //int * piColType)
        dam_describeColDetail(hcol, NULL, NULL, &iColumnType, NULL);
        if (iColumnType != SQL_PARAM_INPUT)
            continue;

        //get the value portion of a value object.
        int iValLen;
        void * pVal;
        iRetCode = dam_getValueToSet(hRowElem,
                                     iXOType,   //XoType of the value returned
                                     &pVal,     //Reference to the value pointer being returned
                                     &iValLen); //Length of the return value
        if (iRetCode != DAM_SUCCESS)
            return DAM_FAILURE;

        if (0 == iNumInputs)
            strcat(szSqlBuffer, " (");
        else
            strcat(szSqlBuffer, ", ");
        switch (iXOType)
        {
        case XO_TYPE_CHAR:
        case XO_TYPE_VARCHAR:
        case XO_TYPE_NUMERIC:
        case XO_TYPE_DECIMAL:
        case XO_TYPE_TIMESTAMP:
        case XO_TYPE_LONGVARCHAR:
            strcat(szSqlBuffer, "'");
            strcat(szSqlBuffer, (char*)pVal);
            strcat(szSqlBuffer, "'");
            break;
        default:
            strcat(szSqlBuffer, (char*)pVal);
            break;
        }
        ++iNumInputs;
    }
    if (iNumInputs)
        strcat(szSqlBuffer, ");");
#ifdef _DEBUG
    DUMP(szSqlBuffer);
#endif

    //----------------------------------------------------------
    //Call HPCC to execute the stored procedure (Deployed Query)
    //----------------------------------------------------------
    *piNumResRows = 0;
    if (DAM_SUCCESS == iRetCode && szSqlBuffer[0] != NULL)
        iRetCode = hpcc_exec(szSqlBuffer, pConnDA->pHPCCdb->queryTargetCluster(), pStmtDA, piNumResRows);

    return DAM_SUCCESS;
}


/************************************************************************
Function:       populateOAtypes
Description:    given an HPCC ECL value type, deduce and set the following CColumn attributes
    short       data_type;          //OpenAccess data type (OpenAccess SDK Prog Guide, chap 5, Specifying Data Types)
    StringAttr  type_name;          //The X/Open column type (OpenAccess SQL Engine Programmers Reference, dam_add_damobj_column method)
    unsigned    char_max_length;    //Length in bytes of data transferred to the client in its default format
    unsigned    numeric_precision;  //Maximum number of digits used by the data in the column.
************************************************************************/
void populateOAtypes(CColumn * pColumn)
{
    if (pColumn->m_iXOType != UNINITIALIZED)
        return;//already populated

    char * p;
    _strupr((char*)pColumn->m_hpccType.get());

    if (0 == sl_strncmp(pColumn->m_hpccType.get(), "BOOLEAN",7))
    {
        pColumn->m_type_name.set("TINYINT");//possibly "bit" ?
        pColumn->m_iXOType = XO_TYPE_TINYINT;
        pColumn->m_char_max_length = 1;
        pColumn->m_numeric_precision = 3;
    }
    else if ((p = sl_strstr((char*)pColumn->m_hpccType.get(), "INTEGER")) ||
             (p = sl_strstr((char*)pColumn->m_hpccType.get(), "UNSIGNED")))
    {
        int idx = 0;
        while (*(p + idx) && !isdigit((int)*(p + idx)))//search for optional precision
            idx++;
        pColumn->m_char_max_length = 8;//num bytes
        if (*(p + idx) && isdigit((int)*(p + idx)))
            pColumn->m_char_max_length = atoi(p + idx);
        switch (pColumn->m_char_max_length)
        {
        case 1:
/*
            pColumn->m_type_name.set("TINYINT");
            pColumn->m_iXOType = XO_TYPE_TINYINT;
            pColumn->m_numeric_precision = 3;
            break;
*/
        case 2:
/*
            pColumn->m_type_name.set("SMALLINT");
            pColumn->m_iXOType = XO_TYPE_SMALL;
            pColumn->m_numeric_precision = 5;
            break;
*/
        case 3:
        case 4:
            pColumn->m_type_name.set("INTEGER");
            pColumn->m_iXOType = XO_TYPE_INTEGER;
            pColumn->m_char_max_length = sizeof(int);
            pColumn->m_numeric_precision = 10;
            break;
        case 5:
        case 6:
        case 7:
        case 8:
            pColumn->m_type_name.set("BIGINT");
            pColumn->m_iXOType = XO_TYPE_BIGINT;
            pColumn->m_numeric_precision = 19;
            break;
        }
    }
    else if (0 == sl_strncmp(pColumn->m_hpccType.get(), "REAL",4))
    {
        int idx = 4;
        while (*(pColumn->m_hpccType.get()+idx) && !isdigit((int)*(pColumn->m_hpccType.get() + idx)))//search for optional precision
            idx++;
        if (*(pColumn->m_hpccType.get() + idx) && isdigit((int)*(pColumn->m_hpccType.get() + idx)))
            pColumn->m_char_max_length = atoi(pColumn->m_hpccType.get() + idx);
        else
            pColumn->m_char_max_length = 4;
/*
        switch (pColumn->m_char_max_length)
        {

        case 4:
            pColumn->m_type_name.set("REAL");
            pColumn->m_iXOType = XO_TYPE_REAL;
            pColumn->m_numeric_precision = 7;
            break;
        case 8:

        default:
*/
            pColumn->m_type_name.set("FLOAT");//or DOUBLE
            pColumn->m_iXOType = XO_TYPE_FLOAT;
            pColumn->m_char_max_length = 8;
            pColumn->m_numeric_precision = 15;
/*
            break;
        }
*/
    }
    else if ((p = sl_strstr((char*)pColumn->m_hpccType.get(), "DECIMAL"))  ||//DECIMALn, UNSIGNED DECIMALn, UDECIMALn
             (p = sl_strstr((char*)pColumn->m_hpccType.get(), "UDECIMAL")))
    {
        int idx = 0;
        int iNumDigits = 0;
        int iNumDecimalPlaces = 0;
        while (*(p + idx) && !isdigit((int)*(p + idx)))//search for (required) num digits
            idx++;
        if (*(p + idx) && isdigit((int)*(p + idx)))
        {
            iNumDigits = atoi(p + idx);
            while (*(p + idx) &&   '_' != *(p + idx))//search for decimal places
                idx++;
            if (*(p + idx) &&   '_' == *(p + idx))
            {
                iNumDecimalPlaces = atoi(p + idx + 1);
/*
                if (iNumDecimalPlaces < 8)
                {
                    pColumn->m_type_name.set("REAL");
                    pColumn->m_iXOType = XO_TYPE_REAL;
                    pColumn->m_char_max_length = 4;
                    pColumn->m_numeric_precision = 7;//iNumDecimalPlaces;
                }
                else
*/
                {
                    pColumn->m_type_name.set("FLOAT");//or DOUBLE
                    pColumn->m_iXOType = XO_TYPE_FLOAT;
                    pColumn->m_char_max_length = 8;
                    pColumn->m_numeric_precision = 15;//iNumDecimalPlaces;
                }
            }
            else
            {
                pColumn->m_type_name.set("INTEGER");
                pColumn->m_iXOType = XO_TYPE_INTEGER;
                pColumn->m_char_max_length = 5;
                pColumn->m_numeric_precision = 10;
            }
        }
    }
    else if (0 == sl_strncmp(pColumn->m_hpccType.get(), "STRING",6) ||
             0 == sl_strncmp(pColumn->m_hpccType.get(), "VARSTRING",9))
    {
        int iLen = -1;
        const char * p = pColumn->m_hpccType.get();
        int idx = 0;
        while (*(p + idx) && !isdigit((int)*(p + idx)))//search for num digits
            idx++;
        if (isdigit((int)*(p + idx)))
            iLen = atoi(p + idx);
        if (iLen != -1 && iLen < 254)
        {
            //Fixed length string
            pColumn->m_type_name.set("CHAR");
            pColumn->m_iXOType = XO_TYPE_CHAR;
            pColumn->m_char_max_length = iLen;
            pColumn->m_numeric_precision = iLen;
        }
        else
        {
            //Variable length string
            pColumn->m_type_name.set("LONGVARCHAR");
            pColumn->m_iXOType = XO_TYPE_LONGVARCHAR;
            pColumn->m_char_max_length = 16000000;
            pColumn->m_numeric_precision = 16000000;
        }
    }
    else if (0 == sl_strncmp(pColumn->m_hpccType.get(), "QSTRING",7))
    {
        tm_trace(hpcc_tm_Handle, UL_TM_PARM, "HPCC_Conn:Data type 'QSTRING' not supported\n", ());
    }
    else if (0 == sl_strncmp(pColumn->m_hpccType.get(), "UNICODE",7))
    {
        int iLen = -1;
        if (isdigit((int)pColumn->m_hpccType.get()[7]))
            iLen = atoi(&pColumn->m_hpccType.get()[7]);
        if (iLen != -1)
        {
            //Fixed length unicode string
            pColumn->m_type_name.set("WCHAR");
            pColumn->m_iXOType = XO_TYPE_WCHAR;
            pColumn->m_char_max_length = iLen;
            pColumn->m_numeric_precision = DAMOBJ_NOTSET;
        }
        else
        {
            //Fixed length string
            pColumn->m_type_name.set("WLONGVARCHAR");
            pColumn->m_iXOType = XO_TYPE_WLONGVARCHAR;
            pColumn->m_char_max_length = DAMOBJ_NOTSET;
            pColumn->m_numeric_precision = DAMOBJ_NOTSET;
        }
    }
    else if (0 == sl_strncmp(pColumn->m_hpccType.get(), "DATA",4))
    {
        int iLen = -1;
        if (isdigit((int)pColumn->m_hpccType.get()[4]))
            iLen = atoi(&pColumn->m_hpccType.get()[4]);
        if (iLen != -1)
        {
            //Fixed length packed hexadecimal data
            pColumn->m_type_name.set("BINARY");
            pColumn->m_iXOType = XO_TYPE_BINARY;
            pColumn->m_char_max_length = iLen;
            pColumn->m_numeric_precision = DAMOBJ_NOTSET;
        }
        else
        {
            //Fixed length string
            pColumn->m_type_name.set("VARBINARY");
            pColumn->m_iXOType = XO_TYPE_VARBINARY;
            pColumn->m_char_max_length = DAMOBJ_NOTSET;
            pColumn->m_numeric_precision = DAMOBJ_NOTSET;
        }
    }
    else if (0 == sl_strncmp(pColumn->m_hpccType.get(), "VARUNICODE",10))
    {
        int iLen = -1;
        if (isdigit((int)pColumn->m_hpccType.get()[9]))
            iLen = atoi(&pColumn->m_hpccType.get()[9]);
        if (iLen != -1)
        {
            //Fixed length unicode string
            pColumn->m_type_name.set("WCHAR");
            pColumn->m_iXOType = XO_TYPE_WCHAR;
            pColumn->m_char_max_length = iLen+2;
            pColumn->m_numeric_precision = iLen+2;
        }
        else
        {
            //Fixed length string
            pColumn->m_type_name.set("WLONGVARCHAR");
            pColumn->m_iXOType = XO_TYPE_WLONGVARCHAR;
            pColumn->m_char_max_length = DAMOBJ_NOTSET;
            pColumn->m_numeric_precision = DAMOBJ_NOTSET;
        }
    }
    else
    {
        tm_trace(hpcc_tm_Handle, UL_TM_PARM, "HPCC_Conn:Unknown Data type '%s' specified\n", (pColumn->m_hpccType.get()));
    }
}

/************************************************************************
Function:       OAIP_schema()
Description:    Return the requested schema objects
                Calls out to HPCC to retrieve the requested object
Return:         DAM_SUCCESS on Success
                DAM_FAILURE on Failure
************************************************************************/
int     OAIP_schema(DAM_HDBC dam_hdbc,
                    IP_HDBC hdbc,
                    XM_Tree *pMemTree,
                    int iType,
                    DAM_OBJ_LIST pList,
                    DAM_OBJ pSearchObj)
{
    tm_trace(hpcc_tm_Handle, UL_TM_F_TRACE, "HPCC_Conn:OAIP_schema() has been called \n", ());

    int rc;
    HPCC_CONN_DA *pConnDA = (HPCC_CONN_DA *)hdbc;

    switch(iType)
    {
    case DAMOBJ_TYPE_TABLE://retrieve list of all tables (HPCC Files)
        {
            damobj_table * pSearchTableObj = (damobj_table *)pSearchObj;
            if(pSearchTableObj)
            {
                tm_trace(hpcc_tm_Handle,UL_TM_MAJOR_EV,"HPCC_Conn:Dynamic Schema for table :(%s.%s.%s) is being requested.  Table Schema requested with %s\n",
                    (pSearchTableObj->table_qualifier, pSearchTableObj->table_owner, pSearchTableObj->table_name,
                    dam_isSearchPatternObject(pSearchObj) ? "Pattern SearchObject" : "Non-Pattern SearchObject"));
                CTable * table;
                if (pConnDA->pHPCCdb->getTableSchema(pSearchTableObj->table_name, &table))
                {
                    if (hpcc_is_matching_table(pSearchTableObj, HPCC_QUALIFIER_NAME, HPCC_USER_NAME, (char*)table->queryName()))
                    {
                        rc = dam_add_damobj_table(
                            pMemTree,                   //XM_Tree *     pMemTree,
                            pList,                      //DAM_OBJ_LIST  pList,
                            pSearchObj,                 //DAM_OBJ       pSearchObj,
                            HPCC_QUALIFIER_NAME,        //char   *      table_qualifier,
                            HPCC_USER_NAME,             //char   *      table_owner,
                            pSearchTableObj->table_name,//char   *      table_name,
                            "TABLE",                    //char   *      table_type, (TABLE == managed by IP)
                            NULL,                       //char   *      table_path,
                            NULL,                       //char   *      table_userdata
                            NULL,                       //char   *      function_support, IP_TABLE_SUPPORT_SELECT
                            NULL);                      //char   *      remarks
                        if (rc != DAM_SUCCESS)
                        {
                            return DAM_FAILURE;
                        }
                    }
                }
                else
                {
                    tm_trace(hpcc_tm_Handle,UL_TM_MAJOR_EV,"Table not found/not supported '%s'\n",(pSearchTableObj->table_name));
                    return DAM_FAILURE;
                }
            }
            else
            {
                tm_trace(hpcc_tm_Handle,UL_TM_MAJOR_EV,"HPCC_Conn:Dynamic Schema for all tables is being requested\n",());
                return DAM_FAILURE;
            }
        }
        break;

    case DAMOBJ_TYPE_COLUMN://retrieve list of columns for the given table
        {
            damobj_column *pSearchColumnObj;

            pSearchColumnObj = (damobj_column *)pSearchObj;

            if(pSearchColumnObj)
            {
                tm_trace(hpcc_tm_Handle,UL_TM_MAJOR_EV,"HPCC_Conn:Dynamic Schema for columns of table :(%s.%s.%s) is being requested. Column Schema requested with %s \n",
                    (pSearchColumnObj->table_qualifier,pSearchColumnObj->table_owner,pSearchColumnObj->table_name,
                    dam_isSearchPatternObject(pSearchObj) ? "Pattern SearchObject" : "Non-Pattern SearchObject"));

                CTable * table;
                if (pConnDA->pHPCCdb->getTableSchema(pSearchColumnObj->table_name, &table))//find specified table
                {
                    aindex_t numCols = table->queryNumColumns();
                    for (aindex_t colIdx = 0; colIdx < numCols; colIdx++)//iterate over all columns in specified table
                    {
                        CColumn *col = table->queryColumn(colIdx);
                        if (hpcc_is_matching_column(pSearchColumnObj, HPCC_QUALIFIER_NAME, HPCC_USER_NAME, (char*)table->queryName()))
                        {
                            populateOAtypes(col);//map HPCC data types to OpenAccess dam_add_damobj_column types
                            rc = dam_add_damobj_column(
                                    pMemTree,               //XM_Tree     *pMemTree,
                                    pList,                  //DAM_OBJ_LIST pList,
                                    pSearchObj,             //DAM_OBJ      pSearchObj,
                                    HPCC_QUALIFIER_NAME,    //char   *     table_qualifier,
                                    HPCC_USER_NAME,         //char   *     table_owner,
                                    (char*)table->queryName(),//char  *   table_name
                                    (char*)col->m_name.get(),//char   *     column_name,
                                    col->m_iXOType,         //short        data_type,
                                    (char*)col->m_type_name.get(),//char *  type_name,
                                    col->m_char_max_length,  //long         char_max_length,"Length in bytes of data transferred to the client in its default format."
                                    col->m_numeric_precision,//long         numeric_precision,  "Maximum number of digits used by the data in the column."
                                    DAMOBJ_NOTSET,          //short        numeric_precision_radix,
                                    DAMOBJ_NOTSET,          //short        numeric_scale,
                                    XO_NULLABLE,            //short        nullable,
                                    DAMOBJ_NOTSET,          //short        scope,
                                    NULL,                   //char   *     userdata,
                                    NULL,                   //char   *     operator_support,
                                    SQL_PC_NOT_PSEUDO,      //short        pseudo_column,
                                    0,                      //short        column_type,
                                    NULL);                  //char   *     remarks
                            if (rc != DAM_SUCCESS)
                            {
                                return DAM_FAILURE;
                            }

                        }
                    }
                }
                else
                {
                    tm_trace(hpcc_tm_Handle,UL_TM_MAJOR_EV,"Table not found/not supported '%s'\n",(pSearchColumnObj->table_name));
                    return DAM_FAILURE;
                }
            }
            else
            {
                tm_trace(hpcc_tm_Handle,UL_TM_MAJOR_EV,"HPCC_Conn:Dynamic Schema for all columns is being requested\n",());
                return DAM_FAILURE;
            }
        }
        break;

    case DAMOBJ_TYPE_PROC://retrieve information for given stored procedure (HPCC deployed query)
        {
            damobj_proc    *pSearchProcObj;

            pSearchProcObj = (damobj_proc *)pSearchObj;
            // check what information is being requested even if we do not use the info
            if (pSearchProcObj)
            {
                tm_trace(hpcc_tm_Handle, UL_TM_MAJOR_EV, "HPCC_Conn:Dynamic Schema for Procedure:(%s.%s.%s) is being requested\n",
                    (DAMOBJ_IS_SET(pSearchProcObj->qualifier_len) ? pSearchProcObj->qualifier : "",
                    DAMOBJ_IS_SET(pSearchProcObj->owner_len) ? pSearchProcObj->owner : "",
                    DAMOBJ_IS_SET(pSearchProcObj->name_len) ? pSearchProcObj->name : "")
                    );

                const char * pQS = pSearchProcObj->owner[0] ? pSearchProcObj->owner : pConnDA->pHPCCdb->queryDefaultQuerySet();
                CMyQuerySetQuery * pQuery = pConnDA->pHPCCdb->queryStoredProcedure(pQS, pSearchProcObj->name);//find specified stored procedure within the querySet
                if (pQuery)
                {
                    pConnDA->pHPCCdb->setCurrentQuertSet(pQS);
                    rc = dam_add_damobj_proc(pMemTree, // XM_Tree *    pMemTree,
                            pList,                  // DAM_OBJ_LIST pList,
                            pSearchObj,             // DAM_OBJ      pSearchObj,
                            HPCC_CATALOG_NAME,      // char   *proc_qualifier,
                            HPCC_USER_NAME,         // char   *proc_owner,
                            pSearchProcObj->name,   // char   *proc_name,
                            (long)pQuery->queryNumInputs(),// long   num_input_params,Not used at this time
                            (long)DAMOBJ_NOTSET,    // long   num_output_params,Not used at this time
                            (long)pQuery->queryNumOutputDatasets(),// long   num_result_sets,
                            SQL_PT_PROCEDURE,       // short  proc_type, does not have a return value.
                            (char*)pQuery,        // char   *userdata,
                            NULL);                  // char   *remarks
                    if (rc != DAM_SUCCESS)
                    {
                        return DAM_FAILURE;
                    }
                }
                else
                {
                    tm_trace(hpcc_tm_Handle, UL_TM_MAJOR_EV, "HPCC_Conn:Query %s not found in HPCC cache\n", (pSearchProcObj->name));
                    return DAM_FAILURE;
                }
            }
            else
            {
                tm_trace(hpcc_tm_Handle, UL_TM_MAJOR_EV, "HPCC_Conn:Dynamic Schema for all Procedures is being requested\n", ());
                return DAM_FAILURE;
            }
        }
        break;

    case DAMOBJ_TYPE_PROC_COLUMN://retrieve list of columns for given stored procedure (HPCC deployed query)
        {
            damobj_proc_column    *pSearchProcColumnObj;
            pSearchProcColumnObj = (damobj_proc_column *)pSearchObj;
            if (pSearchProcColumnObj)
            {
                CMyQuerySetQuery * pQuery = pConnDA->pHPCCdb->queryStoredProcedure(pConnDA->pHPCCdb->queryCurrentQuerySet(), pSearchProcColumnObj->name);//find specified stored procedure
                if (!pQuery)
                {
                    tm_trace(hpcc_tm_Handle, UL_TM_MAJOR_EV, "HPCC_Conn:Query %s not found in HPCC cache\n", (pSearchProcColumnObj->name));
                    return DAM_FAILURE;
                }

                //---------------------------
                //First describe all inputs
                //---------------------------
                aindex_t numInputs = pQuery->queryNumInputs();
                for (aindex_t idx = 0; idx < numInputs; idx++)
                {
                    CColumn * pCol = pQuery->queryInput(idx);
                    if (pCol)
                    {
                        populateOAtypes(pCol);//map HPCC data types to OpenAccess dam_add_damobj_column types
                        int rc = dam_add_damobj_proc_column(pMemTree, // XM_Tree      *pMemTree,
                                    pList,                      //   DAM_OBJ_LIST       pList,
                                    pSearchObj,                 //   DAM_OBJ            pSearchObj,
                                    HPCC_CATALOG_NAME,          //   char   *qualifier,
                                    HPCC_USER_NAME,             //   char   *owner,
                                    pSearchProcColumnObj->name, //   char   *name,
                                    (char*)pCol->m_name.get(),  //   char   *column_name,
                                    SQL_PARAM_INPUT,            //   short  column_type
                                    pCol->m_iXOType,            //   short  data_type,
                                    (char*)pCol->m_type_name.get(),// char   *type_name,
                                    pCol->m_numeric_precision,  //   long   precision,
                                    DAMOBJ_NOTSET,              //   long   length,
                                    DAMOBJ_NOTSET,              //   short  scale,
                                    DAMOBJ_NOTSET,              //   short  radix,
                                    XO_NULLABLE,                //   short  nullable,
                                    NULL,                       //   char   *userdata,
                                    NULL);                      //   char   *remarks
                        if (rc != DAM_SUCCESS)
                        {
                            return DAM_FAILURE;
                        }
                    }
                    else
                    {
                        tm_trace(hpcc_tm_Handle,UL_TM_MAJOR_EV,"Input column descriptor not found/not supported '%s'\n",(pSearchProcColumnObj->name));
                        return DAM_FAILURE;
                    }
                }

                //---------------------------
                //Describe all outputs
                //---------------------------
                aindex_t numResultSets = pQuery->queryNumOutputDatasets();
                for (aindex_t idx = 0; idx < numResultSets; idx++)
                {
                    CTable * pResSet = pQuery->queryOutputDataset(idx);
                    if (pResSet)
                    {
                        aindex_t numCols = pResSet->queryNumColumns();
                        for (aindex_t idx = 0; idx < numCols; idx++)
                        {
                            CColumn * pCol = pResSet->queryColumn(idx);
                            if (pCol)
                            {
                                populateOAtypes(pCol);//map HPCC data types to OpenAccess dam_add_damobj_column types
                                int rc = dam_add_damobj_proc_column(pMemTree, // XM_Tree      *pMemTree,
                                            pList,                      //   DAM_OBJ_LIST       pList,
                                            pSearchObj,                 //   DAM_OBJ            pSearchObj,
                                            HPCC_CATALOG_NAME,          //   char   *qualifier,
                                            HPCC_USER_NAME,             //   char   *owner,
                                            pSearchProcColumnObj->name, //   char   *name,
                                            (char*)pCol->m_name.get(),  //   char   *column_name,
                                            SQL_RESULT_COL,             //   short  column_type
                                            pCol->m_iXOType,            //   short  data_type,
                                            (char*)pCol->m_type_name.get(),// char   *type_name,
                                            pCol->m_numeric_precision,  //   long   precision,
                                            DAMOBJ_NOTSET,              //   long   length,
                                            DAMOBJ_NOTSET,              //   short  scale,
                                            DAMOBJ_NOTSET,              //   short  radix,
                                            XO_NULLABLE,                //   short  nullable,
                                            NULL,                       //   char   *userdata,
                                            NULL);                      //   char   *remarks
                                if (rc != DAM_SUCCESS)
                                {
                                    return DAM_FAILURE;
                                }
                            }
                        }
                    }
                    else
                    {
                        tm_trace(hpcc_tm_Handle,UL_TM_MAJOR_EV,"Output column descriptor not found/not supported '%s'\n",(pSearchProcColumnObj->name));
                        return DAM_FAILURE;
                    }
                }
            }
            break;
        }

    default:
        break;
    }

    return DAM_SUCCESS;
}

/**********************************************************************
Function:       hpcc_is_matching_table()
Description:    Return the requested schema objects
Return:         TRUE if search object will match the given search pattern
                FALSE otherwise
************************************************************************/
static int      hpcc_is_matching_table(damobj_table *pSearchObj,
                                       char *table_qualifier,
                                       char *table_owner,
                                       char *table_name)
{
    if (!pSearchObj)
        return TRUE;

    if (!bAllowSchemaSearchPattern || !dam_isSearchPatternObject(pSearchObj))
    {
        /* match the search pattern */
        if (DAMOBJ_IS_SET(pSearchObj->table_qualifier_len) && sl_stricmp(pSearchObj->table_qualifier, table_qualifier))
            return FALSE;
        if (DAMOBJ_IS_SET(pSearchObj->table_owner_len) && sl_stricmp(pSearchObj->table_owner, table_owner))
            return FALSE;
        if (DAMOBJ_IS_SET(pSearchObj->table_name_len) && sl_stricmp(pSearchObj->table_name, table_name))
            return FALSE;
    }
    else
    {
        /* match the search pattern */
        if (DAMOBJ_IS_SET(pSearchObj->table_qualifier_len) && dam_strlikecmp(pSearchObj->table_qualifier, table_qualifier))
            return FALSE;
        if (DAMOBJ_IS_SET(pSearchObj->table_owner_len) && dam_strlikecmp(pSearchObj->table_owner, table_owner))
            return FALSE;
        if (DAMOBJ_IS_SET(pSearchObj->table_name_len) && dam_strlikecmp(pSearchObj->table_name, table_name))
            return FALSE;
    }

    return TRUE;
}


/************************************************************************
Function:       hpcc_is_matching_column()
Description:    Return the requested schema objects
Return:         TRUE if search object will match the given search pattern
                FALSE otherwise
************************************************************************/
static int      hpcc_is_matching_column(damobj_column *pSearchObj,
                                        char *table_qualifier,
                                        char *table_owner,
                                        char *table_name)
{
    if (!pSearchObj)
        return TRUE;

    if (!bAllowSchemaSearchPattern || !dam_isSearchPatternObject(pSearchObj))
    {
        /* match the search pattern */
        if (DAMOBJ_IS_SET(pSearchObj->table_qualifier_len) && sl_stricmp(pSearchObj->table_qualifier, table_qualifier))
            return FALSE;
        if (DAMOBJ_IS_SET(pSearchObj->table_owner_len) && sl_stricmp(pSearchObj->table_owner, table_owner))
            return FALSE;
        if (DAMOBJ_IS_SET(pSearchObj->table_name_len) && sl_stricmp(pSearchObj->table_name, table_name))
            return FALSE;
    }
    else
    {

        /* match the search pattern */
        if (DAMOBJ_IS_SET(pSearchObj->table_qualifier_len) &&
            dam_strlikecmp(pSearchObj->table_qualifier, table_qualifier))
            return FALSE;
        if (DAMOBJ_IS_SET(pSearchObj->table_owner_len) &&
            dam_strlikecmp(pSearchObj->table_owner, table_owner))
            return FALSE;
        if (DAMOBJ_IS_SET(pSearchObj->table_name_len) &&
            dam_strlikecmp(pSearchObj->table_name, table_name))
            return FALSE;
    }

    return TRUE;
}