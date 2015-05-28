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

/* This file is adapted from Progress Software 'Pass Through Mode' sample code  xxxformat.c
   Copyright (c) 1995-2013 Progress Software Corporation. All Rights Reserved.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ipdef.h"

int     ip_format_select_query(DAM_HQUERY    hquery, char *pSqlBuffer, int *piWriteOffset);
int     ip_format_table_list(DAM_HQUERY    hquery, char *pSqlBuffer, int *piWriteOffset);
int     ip_format_valexp_list(DAM_HQUERY    hquery, DAM_HVALEXP_LIST hValExpList, char *pSqlBuffer, int *piWriteOffset);
int     ip_format_valexp(DAM_HQUERY    hquery, DAM_HVALEXP hValExp, char *pSqlBuffer, int *piWriteOffset);
int     ip_format_scalar_valexp(DAM_HQUERY    hquery, DAM_HSCALAR_VALEXP   hScalarValExp, char *pSqlBuffer, int *piWriteOffset);
int     ip_format_case_valexp(DAM_HQUERY hquery, DAM_HCASE_VALEXP hCaseValExp, char *pSqlBuffer, int *piWriteOffset);
int     ip_format_case_elem_list(DAM_HQUERY hquery, DAM_HCASE_ELEM_LIST hCaseElemList, char *pSqlBuffer, int *piWriteOffset);
int     ip_format_val(DAM_HQUERY    hquery, DAM_HVAL hVal, char *pSqlBuffer, int *piWriteOffset);
int     ip_format_col(DAM_HQUERY    hquery, DAM_HCOL hCol, char *pSqlBuffer, int *piWriteOffset);
int     ip_format_group_list(DAM_HQUERY    hquery, DAM_HVALEXP_LIST hValExpList, char *pSqlBuffer, int *piWriteOffset);
int     ip_format_order_list(DAM_HQUERY    hquery, DAM_HVALEXP_LIST hValExpList, char *pSqlBuffer, int *piWriteOffset);
int     ip_format_logexp(DAM_HQUERY    hquery, DAM_HLOGEXP hLogExp, char *pSqlBuffer, int *piWriteOffset);
int     ip_format_cond(DAM_HQUERY    hquery, DAM_HCOND hCond, char *pSqlBuffer, int *piWriteOffset);

/************************************************************************
Function:       ip_format_query()
Description:
Return:
************************************************************************/
int				ip_format_query(DAM_HQUERY hquery, char *pSqlBuffer, int *piWriteOffset)
{
    if (damex_getQueryType(hquery) != DAM_SELECT)
        return DAM_FAILURE;
    ip_format_select_query(hquery, pSqlBuffer, piWriteOffset);
    return DAM_SUCCESS;
}



int     ip_format_select_query(DAM_HQUERY hquery, char *pSqlBuffer, int *piWriteOffset)
{
    int               iSetQuantifier;
    DAM_HVALEXP_LIST  hSelectValExpList, hGroupValExpList, hOrderValExpList;
    DAM_HLOGEXP       hSearchExp;
    DAM_HLOGEXP       hHavingExp;
    int				  iUnionType;
    DAM_HQUERY		  hUnionQuery;
    int               iTopResRows;
    int               bTopPercent;

    damex_describeSelectQuery(hquery, &iSetQuantifier,
        &hSelectValExpList,
        &hSearchExp,
        &hGroupValExpList,
        &hHavingExp,
        &hOrderValExpList);
    damex_describeSelectTopClause(hquery, &iTopResRows, &bTopPercent);


    /* get the table list */
    {
        DAM_HTABLE        htable;

        htable = damex_getFirstTable(hquery);
        while (htable) {
            int     iTableNum;
            char    sTableName[DAM_MAX_ID_LEN+1];
            DAM_HCOL    hCol;
            char    sColName[DAM_MAX_ID_LEN+1];

            damex_describeTable(htable, &iTableNum, NULL, NULL, sTableName, NULL, NULL);
            /*
            printf("Table:%s. Columns Used: ", sTableName);
            */
            hCol = damex_getFirstCol(htable, DAM_COL_IN_USE);
            while (hCol) {
                damex_describeCol(hCol, NULL, NULL, sColName, NULL, NULL, NULL, NULL);
                /*
                printf("%s,", sColName);
                */
                hCol = damex_getNextCol(htable);
            }
            /*
            printf("\n");
            */

            htable = damex_getNextTable(hquery);
        }
    }

    *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "SELECT ");
    if (iSetQuantifier == SQL_SELECT_DISTINCT)
        *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "DISTINCT ");
#ifdef __SUPPORT_SQL_TOP
    if (iTopResRows != DAM_NOT_SET) {
        *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "TOP %d ", iTopResRows);
        if (bTopPercent)
            *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "PERCENT ");
    }
#endif
    ip_format_valexp_list(hquery, hSelectValExpList, pSqlBuffer, piWriteOffset);

    *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " FROM ");
    ip_format_table_list(hquery, pSqlBuffer, piWriteOffset);
    if (hSearchExp) {
        *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "WHERE ");
        ip_format_logexp(hquery, hSearchExp, pSqlBuffer, piWriteOffset);
    }
    if (hGroupValExpList) {
        *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " GROUP BY ");
        ip_format_group_list(hquery, hGroupValExpList, pSqlBuffer, piWriteOffset);
    }
    if (hHavingExp) {
        *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " HAVING ");
        ip_format_logexp(hquery, hHavingExp, pSqlBuffer, piWriteOffset);
    }

    /* check if query has a UNION clause */
    damex_describeUnionQuery(hquery, &iUnionType, &hUnionQuery);
    if (hUnionQuery) {
        *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " UNION ");
        if (iUnionType)
            *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " ALL ");
        ip_format_select_query(hUnionQuery, pSqlBuffer, piWriteOffset);
    }

    if (hOrderValExpList) {
        *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " ORDER BY ");
        ip_format_order_list(hquery, hOrderValExpList, pSqlBuffer, piWriteOffset);
    }

#ifndef __SUPPORT_SQL_TOP
    if (iTopResRows != DAM_NOT_SET) {
        *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "LIMIT %d ", iTopResRows);
        if (bTopPercent)
            *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "PERCENT ");
    }
#endif
    return DAM_SUCCESS;

}


int     ip_format_col_in_use(DAM_HQUERY hquery, char *pSqlBuffer, int *piWriteOffset)
{
    DAM_HTABLE        htable;
    int               iFirst = TRUE;

    htable = damex_getFirstTable(hquery);
    while (htable) {
        int     iTableNum;
        char    sTableName[DAM_MAX_ID_LEN+1];
        DAM_HCOL    hCol;

        damex_describeTable(htable, &iTableNum, NULL, NULL, sTableName, NULL, NULL);
        hCol = damex_getFirstCol(htable, DAM_COL_IN_USE);

        while (hCol) {
            if(!iFirst)
                *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, ", ");
            ip_format_col(hquery, hCol, pSqlBuffer, piWriteOffset);
            hCol = damex_getNextCol(htable);
            iFirst = FALSE;
        }
        htable = damex_getNextTable(hquery);
    }
    return DAM_SUCCESS;
}

int     ip_format_table_list(DAM_HQUERY hquery, char *pSqlBuffer, int *piWriteOffset)
{
    DAM_HTABLE        htable;
    int               iFirst = TRUE;
    int				  iTableNum;
    char			  sTableName[DAM_MAX_ID_LEN+1];
    int				  iJoinType;
    DAM_HLOGEXP		  hJoinExp;

    htable = damex_getFirstTable(hquery);
    while (htable) {
        hJoinExp = NULL;
        damex_describeTable(htable, &iTableNum, NULL, NULL, sTableName, NULL, NULL);
        damex_describeTableJoinInfo(htable, &iJoinType, &hJoinExp);

        switch (iJoinType) {
            case SQL_JOIN_LEFT_OUTER:
                *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "%s", " LEFT OUTER JOIN ");
                break;
            case SQL_JOIN_RIGHT_OUTER:
                *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "%s", " RIGHT OUTER JOIN ");
                break;
            case SQL_JOIN_FULL_OUTER:
                *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "%s", " FULL OUTER JOIN ");
                break;
            case SQL_JOIN_INNER:
                *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "%s", " INNER JOIN ");
                break;
            case SQL_JOIN_OLD_STYLE:
                if (!iFirst)
                    *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, ", ");
                break;
        }
//      *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "%s T%d_Q%lx", sTableName, iTableNum, (long)hquery);
        *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "%s", sTableName);
        if (hJoinExp) {
            *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "%s", " ON ");
            ip_format_logexp(hquery, hJoinExp, pSqlBuffer, piWriteOffset);
        }

        iFirst = FALSE;
        htable = damex_getNextTable(hquery);
    }

    *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " ");
    return DAM_SUCCESS;
}


int     ip_format_valexp_list(DAM_HQUERY hquery, DAM_HVALEXP_LIST hValExpList, char *pSqlBuffer, int *piWriteOffset)
{
    DAM_HVALEXP     hValExp;
    int             iFirst = TRUE;

    if (!hValExpList)
    {
        *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "*");
        return DAM_SUCCESS;
    }
    hValExp = damex_getFirstValExp(hValExpList);
    while (hValExp) {
        if (!iFirst)
            *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, ", ");
        else
            iFirst = FALSE;

        ip_format_valexp(hquery, hValExp, pSqlBuffer, piWriteOffset);

        hValExp = damex_getNextValExp(hValExpList);
    }

    *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " ");
    return DAM_SUCCESS;
}

int     ip_format_valexp(DAM_HQUERY hquery, DAM_HVALEXP hValExp, char *pSqlBuffer, int *piWriteOffset)
{
    int         iType; /* literal value, column, +, -, *, / etc   */
    int         iFuncType;
    DAM_HVALEXP	hLeftValExp;
    DAM_HVALEXP hRightValExp;
    DAM_HVAL    hVal;
    DAM_HSCALAR_VALEXP  hScalarValExp;
    DAM_HCASE_VALEXP    hCaseValExp;

    damex_describeValExp(hValExp, &iType, /* literal value, column, +, -, *, / etc   */
        &iFuncType,
        &hLeftValExp,
        &hRightValExp,
        &hVal,
        &hScalarValExp,
        &hCaseValExp
        );

    /* function type */
    if (iFuncType & SQL_F_COUNT_ALL) *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "COUNT(*) ");
    if (iFuncType & SQL_F_COUNT) *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "COUNT ");
    if (iFuncType & SQL_F_AVG) *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "AVG ");
    if (iFuncType & SQL_F_MAX) *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "MAX ");
    if (iFuncType & SQL_F_MIN) *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "MIN ");
    if (iFuncType & SQL_F_SUM) *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "SUM ");
    if (iFuncType & SQL_F_VAR) *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "VAR_SAMP ");
    if (iFuncType & SQL_F_VARP) *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "VAR_POP ");
    if (iFuncType & SQL_F_STDDEV) *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "STDDEV_SAMP ");
    if (iFuncType & SQL_F_STDDEVP) *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "STDDEV_POP ");

    if (iFuncType && iFuncType != SQL_F_COUNT_ALL)
        *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "( ");
    if (iFuncType & SQL_F_DISTINCT) *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "DISTINCT ");
    switch (iType) {
        case SQL_VAL_EXP_VAL:
            ip_format_val(hquery, hVal, pSqlBuffer, piWriteOffset);
            break;
        case SQL_VAL_EXP_ADD:
            ip_format_valexp(hquery, hLeftValExp, pSqlBuffer, piWriteOffset);
            *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "+");
            ip_format_valexp(hquery, hRightValExp, pSqlBuffer, piWriteOffset);

            break;
        case SQL_VAL_EXP_SUBTRACT:
            ip_format_valexp(hquery, hLeftValExp, pSqlBuffer, piWriteOffset);
            *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "-");
            ip_format_valexp(hquery, hRightValExp, pSqlBuffer, piWriteOffset);

            break;

        case SQL_VAL_EXP_MULTIPLY:
            ip_format_valexp(hquery, hLeftValExp, pSqlBuffer, piWriteOffset);
            *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "*");
            ip_format_valexp(hquery, hRightValExp, pSqlBuffer, piWriteOffset);

            break;

        case SQL_VAL_EXP_DIVIDE:
            ip_format_valexp(hquery, hLeftValExp, pSqlBuffer, piWriteOffset);
            *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "/");
            ip_format_valexp(hquery, hRightValExp, pSqlBuffer, piWriteOffset);

            break;

        case SQL_VAL_EXP_SCALAR:
            ip_format_scalar_valexp(hquery, hScalarValExp, pSqlBuffer, piWriteOffset);
            break;

        case SQL_VAL_EXP_CASE:
            ip_format_case_valexp(hquery, hCaseValExp, pSqlBuffer, piWriteOffset);
            break;

        default:
            *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "Invalid Value Expression Type:%d", iType);
            break;
    }

    if (iFuncType && iFuncType != SQL_F_COUNT_ALL)
        *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, ")");


    return DAM_SUCCESS;
}


int     ip_format_scalar_valexp(DAM_HQUERY hquery, DAM_HSCALAR_VALEXP   hScalarValExp, char *pSqlBuffer, int *piWriteOffset)
{
    char        sName[DAM_MAX_ID_LEN+1];
    DAM_HVALEXP_LIST    hValExpList;

    damex_describeScalarValExp(hScalarValExp, sName, &hValExpList);
    *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "%s", sName);

    /* check if scalar function refers to a special @@ identifier */
    if (!strncmp(sName, "@@", 2))
        return DAM_SUCCESS;

    *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "( ");
    if (hValExpList) ip_format_valexp_list(hquery, hValExpList, pSqlBuffer, piWriteOffset);
    *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, ") ");

    return DAM_SUCCESS;
}

int     ip_format_case_valexp(DAM_HQUERY hquery, DAM_HCASE_VALEXP hCaseValExp, char *pSqlBuffer, int *piWriteOffset)
{
    DAM_HVALEXP             hInputValExp;
    DAM_HCASE_ELEM_LIST     hCaseElemList;
    DAM_HVALEXP             hElseValExp;

    damex_describeCaseValExp(hCaseValExp, &hInputValExp, &hCaseElemList, &hElseValExp);
    *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "CASE ");
    if (hInputValExp) ip_format_valexp(hquery, hInputValExp, pSqlBuffer, piWriteOffset);
    *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " ");
    ip_format_case_elem_list(hquery, hCaseElemList, pSqlBuffer, piWriteOffset);
    if (hElseValExp) {
        *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " ELSE ");
        ip_format_valexp(hquery, hElseValExp, pSqlBuffer, piWriteOffset);
    }
    *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " END ");

    return DAM_SUCCESS;
}

int     ip_format_case_elem_list(DAM_HQUERY hquery, DAM_HCASE_ELEM_LIST hCaseElemList, char *pSqlBuffer, int *piWriteOffset)
{
    DAM_HCASE_ELEM          hCaseElem;
    DAM_HVALEXP             hWhenValExp;
    DAM_HLOGEXP             hWhenBoolExp;
    DAM_HVALEXP             hResValExp;

    hCaseElem = damex_getFirstCaseElem(hCaseElemList);
    while (hCaseElem) {
        *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " WHEN ");

        damex_describeCaseElem(hCaseElem, &hWhenValExp, &hWhenBoolExp, &hResValExp);
        if (hWhenValExp) ip_format_valexp(hquery, hWhenValExp, pSqlBuffer, piWriteOffset);
        if (hWhenBoolExp) ip_format_logexp(hquery, hWhenBoolExp, pSqlBuffer, piWriteOffset);
        *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " THEN ");
        ip_format_valexp(hquery, hResValExp, pSqlBuffer, piWriteOffset);

        hCaseElem = damex_getNextCaseElem(hCaseElemList);
    }

    return DAM_SUCCESS;
}

int     ip_format_val(DAM_HQUERY   hquery, DAM_HVAL hVal, char *pSqlBuffer, int *piWriteOffset)
{
    int 	    iType; /* literal value, column */
    int 	    iXoType; /* type of literal value - INTEGER, CHAR etc */
    void	    *pData;
    int         iValLen;
    DAM_HCOL	hCol;
    DAM_HQUERY  hSubQuery;
    xo_tm       *pxoTime;

    damex_describeVal(hVal, &iType,
        &iXoType,
        &pData,
        &iValLen,
        &hCol,
        &hSubQuery);


    switch (iType) {

            case SQL_VAL_DATA_CHAIN:
                *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "?");
                break;
            case SQL_VAL_NULL:
                *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "NULL"); break;
            case SQL_VAL_QUERY: /* query */
                *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "( ");
                *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " \"nested query\" ");
                *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " )");
                break;
            case SQL_VAL_COL: /* value is the column value */
                ip_format_col(hquery, hCol, pSqlBuffer, piWriteOffset); break;
            case SQL_VAL_INTERVAL:
                break;
            case SQL_VAL_LITERAL: /* value is a Xo Type literal */
                switch (iXoType) {
            case XO_TYPE_CHAR: /* pVal is a char literal */
            case XO_TYPE_VARCHAR:
            case XO_TYPE_NUMERIC:
            case XO_TYPE_DECIMAL:
                *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "'%s'", (char *)pData);
                break;
            case XO_TYPE_INTEGER:  /* pVal is a integer literal */
                *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "%ld", *(long *)pData);
                break;
            case XO_TYPE_SMALLINT: /* pVal is small integer literal */
                *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "%d", (int)(*(short *)pData));
                break;
            case XO_TYPE_FLOAT: /* pVal is a double literal */
            case XO_TYPE_DOUBLE:
                *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "%.15G", *(double *)pData);
                break;
            case XO_TYPE_REAL: /* pVal is a float literal */
                *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "%.7G", (double)(*(float *)pData));
                break;
            case XO_TYPE_DATE:
                pxoTime = (xo_tm *)pData;
                *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "{d '%d-%02d-%02d'}", pxoTime->tm_year, pxoTime->tm_mon+1, pxoTime->tm_mday);
                break;
            case XO_TYPE_TIME:
                pxoTime = (xo_tm *)pData;
                *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "{t '%02d:%02d:%02d'}", pxoTime->tm_hour, pxoTime->tm_min, pxoTime->tm_sec);
                break;
            case XO_TYPE_TIMESTAMP:
                pxoTime = (xo_tm *)pData;
                if (pxoTime->tm_frac > 0) {
                    int		frac;

                    frac = (int) (pxoTime->tm_frac * 0.000001);
                    *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "{ts '%d-%02d-%02d %02d:%02d:%02d.%03d'}",
                        pxoTime->tm_year, pxoTime->tm_mon+1, pxoTime->tm_mday,
                        pxoTime->tm_hour, pxoTime->tm_min, pxoTime->tm_sec, frac);
                }
                else
                    *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "{ts '%d-%02d-%02d %02d:%02d:%02d'}",
                    pxoTime->tm_year, pxoTime->tm_mon+1, pxoTime->tm_mday,
                    pxoTime->tm_hour, pxoTime->tm_min, pxoTime->tm_sec);
                break;
            default:
                *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "Invalid Xo Value Type:%d", iXoType);
                break;
                }
                break;
            default:
                *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "Invalid Value Type:%d", iType); break;
    }
    return DAM_SUCCESS;

}

int     ip_format_col(DAM_HQUERY hquery, DAM_HCOL hCol, char *pSqlBuffer, int *piWriteOffset)
{
    int         iTableNum, iColNum;
    char        sColName[DAM_MAX_ID_LEN+1];
    DAM_HQUERY  hParentQuery;

    damex_describeCol(hCol,
        &iTableNum,
        &iColNum,
        sColName, NULL, NULL, NULL, NULL);

    /* check if TableNum is valid. For COUNT(*) the column has iTableNum not set */
    if (iTableNum == DAM_NOT_SET)
        return DAM_SUCCESS;

    if(damex_getQueryType(hquery) == DAM_SELECT){
        if (damex_isCorrelatedCol(hCol)) {
            hParentQuery = damex_getParentQuery(hquery);
//          *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "T%d_Q%lx.%s", iTableNum, (long)hParentQuery, sColName);
            *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "%s", sColName);
        }
        else
//          *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "T%d_Q%lx.%s", iTableNum, (long)hquery, sColName);
            *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "%s", sColName);
    }
    else {
        *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "%s", sColName);
    }

    return DAM_SUCCESS;

}

int     ip_format_group_list(DAM_HQUERY hquery, DAM_HVALEXP_LIST hValExpList, char *pSqlBuffer, int *piWriteOffset)
{

    ip_format_valexp_list(hquery, hValExpList, pSqlBuffer, piWriteOffset);
    return DAM_SUCCESS;
}


int     ip_format_order_list(DAM_HQUERY   hquery, DAM_HVALEXP_LIST hValExpList, char *pSqlBuffer, int *piWriteOffset)
{
    int             iFirst = TRUE;
    DAM_HVALEXP     hValExp;
    int             iResultColNum;
    int             iSortOrder;

    if (!hValExpList) return DAM_SUCCESS;

    hValExp = damex_getFirstValExp(hValExpList);
    while (hValExp) {
        if (!iFirst)
            *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, ", ");
        else
            iFirst = FALSE;

        damex_describeOrderByExp(hValExp, &iResultColNum, &iSortOrder);


        if (iResultColNum != DAM_NOT_SET) /* use the result column number */
            *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "%d", iResultColNum+1);
        else
            ip_format_valexp(hquery, hValExp, pSqlBuffer, piWriteOffset);

        if (iSortOrder == SQL_ORDER_ASC)
            *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " ASC");
        else if (iSortOrder == SQL_ORDER_DESC)
            *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " DESC");

        hValExp = damex_getNextValExp(hValExpList);
    }

    *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " ");

    return DAM_SUCCESS;
}


int     ip_format_logexp(DAM_HQUERY hquery, DAM_HLOGEXP hLogExp, char *pSqlBuffer, int *piWriteOffset)
{
    int         iType; /* AND, OR , NOT or CONDITION */
    DAM_HLOGEXP	hLeft, hRight;
    DAM_HCOND   hCond;

    damex_describeLogicExp(hLogExp,
        &iType, /* AND, OR , NOT or CONDITION */
        &hLeft,
        &hRight,
        &hCond);

    switch (iType) {
        case SQL_EXP_COND:
            *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "( ");
            ip_format_cond(hquery, hCond, pSqlBuffer, piWriteOffset);
            *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " )");
            break;
        case SQL_EXP_AND:
            *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "( ");
            ip_format_logexp(hquery, hLeft, pSqlBuffer, piWriteOffset);
            *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " AND ");
            ip_format_logexp(hquery, hRight, pSqlBuffer, piWriteOffset);
            *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " )");

            break;
        case SQL_EXP_OR:
            *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "( ");
            ip_format_logexp(hquery, hLeft, pSqlBuffer, piWriteOffset);
            *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " OR ");
            ip_format_logexp(hquery, hRight, pSqlBuffer, piWriteOffset);
            *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " )");

            break;
        case SQL_EXP_NOT:
            *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "( ");
            *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " NOT ");
            ip_format_logexp(hquery, hLeft, pSqlBuffer, piWriteOffset);
            *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " )");
            break;
        default:
            *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "Invalid Expression Type:%d", iType);
            break;
    }

    return DAM_SUCCESS;
}

int     ip_format_cond(DAM_HQUERY hquery, DAM_HCOND hCond, char *pSqlBuffer, int *piWriteOffset)
{
    int             iType;
    DAM_HVALEXP     hLeft, hRight, hExtra;

    damex_describeCond(hCond,
        &iType, /* >, <, =, BETWEEN etc.*/
        &hLeft,
        &hRight,
        &hExtra); /* used for BETWEEN */

    /* EXISTS and UNIQUE predicates */
    if (iType & (SQL_OP_EXISTS | SQL_OP_UNIQUE)) {

        if (iType & SQL_OP_NOT) *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " NOT ");
        if (iType & SQL_OP_EXISTS) *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " EXISTS ");
        if (iType & SQL_OP_UNIQUE) *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " UNIQUE ");

        ip_format_valexp(hquery, hLeft, pSqlBuffer, piWriteOffset);
    }

    /* conditional predicates */
    if (iType & ( SQL_OP_SMALLER | SQL_OP_GREATER |  SQL_OP_EQUAL)) {
        ip_format_valexp(hquery, hLeft, pSqlBuffer, piWriteOffset);

        if (iType & SQL_OP_NOT) {
            if (iType & SQL_OP_EQUAL) *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " <> ");
        }
        else {
            *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " ");
            if (iType & SQL_OP_SMALLER) *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "<");
            if (iType & SQL_OP_GREATER) *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, ">");
            if (iType & SQL_OP_EQUAL) *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, "=");
            *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " ");
        }

        if (iType & (SQL_OP_QUANTIFIER_ALL | SQL_OP_QUANTIFIER_SOME | SQL_OP_QUANTIFIER_ANY)) {
            if (iType & SQL_OP_QUANTIFIER_ALL) *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " ALL ");
            if (iType & SQL_OP_QUANTIFIER_SOME) *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " SOME ");
            if (iType & SQL_OP_QUANTIFIER_ANY) *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " ANY ");
        }
        ip_format_valexp(hquery, hRight, pSqlBuffer, piWriteOffset);
    }

    /* like predicate */
    if (iType & SQL_OP_LIKE) {
        ip_format_valexp(hquery, hLeft, pSqlBuffer, piWriteOffset);
        if (iType & SQL_OP_NOT) *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " NOT ");
        *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " LIKE ");
        ip_format_valexp(hquery, hRight, pSqlBuffer, piWriteOffset);

        if (hExtra) {
            *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " ESCAPE ");
            ip_format_valexp(hquery, hExtra, pSqlBuffer, piWriteOffset);
        }

    }

    /* Is NULL predicate */
    if (iType & SQL_OP_ISNULL) {
        ip_format_valexp(hquery, hLeft, pSqlBuffer, piWriteOffset);
        if (iType & SQL_OP_NOT)
            *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " IS NOT NULL ");
        else
            *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " IS NULL ");
    }

    /* IN predicate */
    if (iType & SQL_OP_IN) {
        ip_format_valexp(hquery, hLeft, pSqlBuffer, piWriteOffset);
        if (iType & SQL_OP_NOT) *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " NOT ");
        *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " IN (");
        ip_format_valexp(hquery, hRight, pSqlBuffer, piWriteOffset);
        *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " )");
    }

    /* BETWEEN predicate */
    if (iType & SQL_OP_BETWEEN) {

        /* check if the between is a form of ( >= and < ) OR (> and <)
        OR (> and <=)
        */
        if ((iType & SQL_OP_BETWEEN_OPEN_LEFT) || (iType & SQL_OP_BETWEEN_OPEN_RIGHT)) {
            /* format it as two conditions */
            ip_format_valexp(hquery, hLeft, pSqlBuffer, piWriteOffset);
            if (iType & SQL_OP_BETWEEN_OPEN_LEFT)
                *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " > ");
            else
                *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " >= ");
            ip_format_valexp(hquery, hRight, pSqlBuffer, piWriteOffset);

            *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " AND ");

            ip_format_valexp(hquery, hLeft, pSqlBuffer, piWriteOffset);
            if (iType & SQL_OP_BETWEEN_OPEN_RIGHT)
                *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " < ");
            else
                *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " <= ");
            ip_format_valexp(hquery, hExtra, pSqlBuffer, piWriteOffset);
        }
        else {
            /* standard BETWEEN pattern */
            ip_format_valexp(hquery, hLeft, pSqlBuffer, piWriteOffset);

            if (iType & SQL_OP_NOT) *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " NOT ");
            *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " BETWEEN ");

            ip_format_valexp(hquery, hRight, pSqlBuffer, piWriteOffset);
            *piWriteOffset += sprintf(pSqlBuffer + *piWriteOffset, " AND ");
            ip_format_valexp(hquery, hExtra, pSqlBuffer, piWriteOffset);
        }
    }

    return DAM_SUCCESS;
}
