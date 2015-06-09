/*##############################################################################

    HPCC SYSTEMS software Copyright (C) 2015 HPCC Systems.

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
#include "hpcc_util.hpp"

/************************************************************************
Function:       hpcc_add_row()
Description:    Add the given IPropertyTree row to the given DAM_HSTMT, using the
                column attributes in the given CColumn object
Return:         DAM_SUCCESS on Success
                DAM_FAILURE on Failure
************************************************************************/
int hpcc_add_row(HPCC_STMT_DA *pStmtDA, DAM_HROW hrow, IPropertyTree * pRow, CColumn * pCol)
{
    int             iRetCode;
    int64           i64Val;
    double          dVal;

    if (pCol->m_hcol == (DAM_HCOL)UNINITIALIZED)//Progress column doesnt exist. This happens on COUNT(*) and similar, so lets ignore it for now
        return DAM_SUCCESS;

    const char * value = pRow->queryProp(pCol->m_alias);//Alias takes precedence over name
    if (NULL == value)
        value = pRow->queryProp(pCol->m_name);
    if (NULL == value)
    {
        //No value provided. Inject empty string or "XO_NULL_DATA"
        switch (pCol->m_iXOType)
        {
        case XO_TYPE_LONGVARCHAR:
        case XO_TYPE_WLONGVARCHAR:
        case XO_TYPE_CHAR:
        case XO_TYPE_WCHAR:
            iRetCode = dam_addValToRow(pStmtDA->dam_hstmt, hrow, pCol->m_hcol, pCol->m_iXOType, "", XO_NTS);
            return iRetCode;
        }
        //this is unlikely to happen since in ECL there are no "empty" or unpopulated columns
        iRetCode = dam_addValToRow(pStmtDA->dam_hstmt, hrow, pCol->m_hcol, pCol->m_iXOType, NULL, XO_NULL_DATA);
        return iRetCode;
    }

    switch (pCol->m_iXOType)
    {
    case XO_TYPE_LONGVARCHAR:
    case XO_TYPE_WLONGVARCHAR:
    case XO_TYPE_VARBINARY:
        pCol->m_char_max_length = sl_strlen(value) + 1;
        pCol->m_numeric_precision = pCol->m_char_max_length;//fall through
    case XO_TYPE_CHAR:
    case XO_TYPE_WCHAR:
    case XO_TYPE_BINARY:
        iRetCode = dam_addValToRow(pStmtDA->dam_hstmt, hrow, pCol->m_hcol, pCol->m_iXOType, (void*)value, XO_NTS);
        return iRetCode;
    case XO_TYPE_TINYINT:
    case XO_TYPE_SMALL:
    case XO_TYPE_INTEGER:
    case XO_TYPE_BIGINT:
        i64Val = atol(value);
        iRetCode = dam_addValToRow(pStmtDA->dam_hstmt, hrow, pCol->m_hcol, pCol->m_iXOType, &i64Val, pCol->m_char_max_length);
        return iRetCode;
    case XO_TYPE_REAL:
    case XO_TYPE_FLOAT:
        dVal = atof(value);
        iRetCode = dam_addValToRow(pStmtDA->dam_hstmt, hrow, pCol->m_hcol, pCol->m_iXOType, (void*)&dVal, pCol->m_char_max_length);
        return iRetCode;
    default:
        tm_trace(hpcc_tm_Handle, UL_TM_ERRORS, "HPCC_Conn:hpcc_add_row unknown data type %u\n", (pCol->m_iXOType));
    }

    StringBuffer err;
    err.setf("HPCC_Conn:hpcc_add_row : Unsupported pCol->m_iXOType type '%d'", pCol->m_iXOType);
    dam_addError(pStmtDA->pConnDA->dam_hdbc, NULL, DAM_IP_ERROR, 0, (char*)err.str());

    return DAM_FAILURE;
}

/************************************************************************
Function:       hpcc_exec()
Description:    Execute the given query. Calls ws_sql to get rows, then adds them to OpenAccess
Return:         DAM_SUCCESS on Success
                DAM_FAILURE on Failure
************************************************************************/
int             hpcc_exec(const char * sqlQuery, const char * targetQuerySet, HPCC_STMT_DA *pStmtDA, int *piNumRows)
{
    StringBuffer sbErrors;
    HPCCdb *    pHPCCdb = pStmtDA->pConnDA->pHPCCdb;
    DAM_HROW    hrow;
    int         iRetCode;

    //call ws_sql to get row(s)
    sbErrors.set("HPCC_Conn:hpcc_exec : ");
    if (!pHPCCdb->executeSQL(sqlQuery, targetQuerySet, sbErrors, pStmtDA))
    {
        dam_addError(pStmtDA->pConnDA->dam_hdbc, pStmtDA->dam_hstmt, DAM_IP_ERROR, 0, (char *)sbErrors.str());
        return DAM_FAILURE;
    }
    //Iterate over all rows, adding to OpenAccess
    IPropertyTree * pRow;
    bool bRowAvail = pHPCCdb->ReadFirstRow(&pRow);
    while (bRowAvail)
    {
        DUMPPTREE(pRow);
        hrow = dam_allocRow(pStmtDA->dam_hstmt);//allocate a new row
        aindex_t iNumColumns = pHPCCdb->queryOutputColumnDescriptorCount();
        for (aindex_t col = 0; col < iNumColumns; col++)//iterate over each column in this row
        {
            CColumn * pCol = pHPCCdb->queryOutputColumnDescriptor(col);
            assert(pCol);
            iRetCode = hpcc_add_row(pStmtDA, hrow, pRow, pCol);//add column to hrow
            if (iRetCode != DAM_SUCCESS)
            {
                tm_trace(hpcc_tm_Handle, UL_TM_ERRORS, "HPCC_Conn:hpcc_exec() hpcc_add_row() returned error %d\n", (iRetCode));
                dam_freeRow(hrow);
                pRow->Release();
                return iRetCode;
            }
        }
        pRow->Release();
        iRetCode = dam_addRowToTable(pStmtDA->dam_hstmt, hrow);//add hrow to hstmt
        if (iRetCode != DAM_SUCCESS)
        {
            tm_trace(hpcc_tm_Handle, UL_TM_ERRORS, "HPCC_Conn:hpcc_exec() dam_addRowToTable() returned error %d\n", (iRetCode));
            return iRetCode;
        }
        (*piNumRows)++;
         bRowAvail = pHPCCdb->ReadNextRow(&pRow);
    }
    tm_trace(hpcc_tm_Handle, UL_TM_F_TRACE, "HPCC_Conn:Read '%ld' rows\n", (*piNumRows));
    return DAM_SUCCESS;
}


/************************************************************************
Function:       queryColumnDetails
Description:    Get the table name
************************************************************************/
bool queryColumnDetails(/*input*/void *pStmtDA, /*input*/aindex_t colIdx,
                        StringAttr &tblName, int * piColNum, int * piXOType, DAM_HCOL * phcol)
{
    aindex_t idx = 0;
    DAM_HCOL hcol = dam_getFirstCol(((HPCC_STMT_DA*)pStmtDA)->dam_hstmt, DAM_COL_IN_USE);
    while (hcol && idx < colIdx)
    {
        hcol = dam_getNextCol(((HPCC_STMT_DA*)pStmtDA)->dam_hstmt);
        ++idx;
    }

    if (hcol && idx == colIdx)
    {
        char sColName[DAM_MAX_ID_LEN+1];
        int colType;
        dam_describeCol(hcol, piColNum, sColName, piXOType, &colType);
        tblName.set(((HPCC_STMT_DA*)pStmtDA)->sTableName);
        *phcol = hcol;
        return true;
    }
    return false;
}
