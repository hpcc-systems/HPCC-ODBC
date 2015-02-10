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

#include "damip.h"
#include "damschem.h"
/****************** OA_TYPES definition ******************************/

#define EMPTY_STR    ""
#define EMPTY_VAL    0
#define DAM_MAX_AFFIX_LEN 32

typedef struct _damobj_col_types {
    short   type_name_len;
    char    type_name[DAM_MAX_ID_LEN+1];
    short   data_type_len;
    short   data_type;
    short   oa_precision_len;
    long    oa_precision;
    short   literal_prefix_len;
    char    literal_prefix[DAM_MAX_AFFIX_LEN+1];
    short   literal_suffix_len;
    char    literal_suffix[DAM_MAX_AFFIX_LEN+1];
    short   create_params_len;
    char    create_params[DAM_MAX_AFFIX_LEN+1];
    short   oa_nullable_len;
    short   oa_nullable;
    short   case_sensitive_len;
    short   case_sensitive;
    short   oa_searchable_len;
    short   oa_searchable;
    short   unsigned_attrib_len;
    short   unsigned_attrib;
    short   oa_money_len;
    short   oa_money;
    short   auto_increment_len;
    short   auto_increment;
    short   minimum_scale_len;
    short   minimum_scale;
    short   maximum_scale_len;
    short   maximum_scale;
    short   local_type_name_len;
    char    local_type_name[DAM_MAX_ID_LEN+1];
} damobj_col_types;

damobj_col_types gColArray_Types[] = {
    /* TYPE_NAME    DATA_TYPE    OA_PRECISION LITERAL_PREFIX
        LITERAL_SUFFIX  CREATE_PARAMS   OA_NULLABLE  CASE_SENSITIVE
        OA_SEARCHABLE   UNSIGNED_ATTRIB OA_MONEY  AUTO_INCREMENT
        MINIMUM_SCALE   MAXIMUM_SCALE   LOCAL_TYPE_NAME */
        {DAMOBJ_SET, "CHAR",  DAMOBJ_SET, 1,  DAMOBJ_SET, 4096, DAMOBJ_SET, "'",
        DAMOBJ_SET, "'", DAMOBJ_SET,  "length", DAMOBJ_SET, 1, DAMOBJ_SET, 1,
        DAMOBJ_SET, 3, DAMOBJ_SET, 0, DAMOBJ_SET, 0, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_SET, "CHAR"},

        {DAMOBJ_SET, "NUMERIC",  DAMOBJ_SET, 2,  DAMOBJ_SET, 40, DAMOBJ_NOTSET, EMPTY_STR,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET,  "precision,scale", DAMOBJ_SET, 1, DAMOBJ_SET, 0,
        DAMOBJ_SET, 2, DAMOBJ_SET, 0, DAMOBJ_SET, 0, DAMOBJ_SET, 0,
        DAMOBJ_SET,  0, DAMOBJ_SET,  32, DAMOBJ_SET, "NUMERIC"},

        {DAMOBJ_SET, "DECIMAL",  DAMOBJ_SET, 3,  DAMOBJ_SET, 40, DAMOBJ_NOTSET, EMPTY_STR,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET,  "precision,scale", DAMOBJ_SET, 1, DAMOBJ_SET, 0,
        DAMOBJ_SET, 2, DAMOBJ_SET, 0, DAMOBJ_SET, 0, DAMOBJ_SET, 0,
        DAMOBJ_SET,  0, DAMOBJ_SET,  32, DAMOBJ_SET, "DECIMAL"},

        {DAMOBJ_SET, "INTEGER",  DAMOBJ_SET, 4,  DAMOBJ_SET, 10, DAMOBJ_NOTSET, EMPTY_STR,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_NOTSET,  EMPTY_STR, DAMOBJ_SET, 1, DAMOBJ_SET, 0,
        DAMOBJ_SET, 2, DAMOBJ_SET, 0, DAMOBJ_SET, 0, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_SET, "INTEGER"},

        {DAMOBJ_SET, "BIGINT",  DAMOBJ_SET, XO_TYPE_BIGINT,  DAMOBJ_SET, 19, DAMOBJ_NOTSET, EMPTY_STR,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_NOTSET,  EMPTY_STR, DAMOBJ_SET, 1, DAMOBJ_SET, 0,
        DAMOBJ_SET, 2, DAMOBJ_SET, 0, DAMOBJ_SET, 0, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_SET, "BIGINT"},

        {DAMOBJ_SET, "SMALLINT",  DAMOBJ_SET, 5,  DAMOBJ_SET, 5, DAMOBJ_NOTSET, EMPTY_STR,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_NOTSET,  EMPTY_STR, DAMOBJ_SET, 1, DAMOBJ_SET, 0,
        DAMOBJ_SET, 2, DAMOBJ_SET, 0, DAMOBJ_SET, 0, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_SET, "SMALLINT"},

/* ATIqa02857

        {DAMOBJ_SET, "FLOAT",  DAMOBJ_SET, 6,  DAMOBJ_SET, 15, DAMOBJ_NOTSET, EMPTY_STR,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_NOTSET,  EMPTY_STR, DAMOBJ_SET, 1, DAMOBJ_SET, 0,
        DAMOBJ_SET, 2, DAMOBJ_SET, 0, DAMOBJ_SET, 0, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_SET, "FLOAT"},
*/
        {DAMOBJ_SET, "REAL",  DAMOBJ_SET, 7,  DAMOBJ_SET, 7, DAMOBJ_NOTSET, EMPTY_STR,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_NOTSET,  EMPTY_STR, DAMOBJ_SET, 1, DAMOBJ_SET, 0,
        DAMOBJ_SET, 2, DAMOBJ_SET, 0, DAMOBJ_SET, 0, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_SET, "REAL"},

        {DAMOBJ_SET, "DOUBLE",  DAMOBJ_SET, 8,  DAMOBJ_SET, 15, DAMOBJ_NOTSET, EMPTY_STR,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_NOTSET,  EMPTY_STR, DAMOBJ_SET, 1, DAMOBJ_SET, 0,
        DAMOBJ_SET, 2, DAMOBJ_SET, 0, DAMOBJ_SET, 0, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_SET, "DOUBLE"},

        {DAMOBJ_SET, "BINARY",  DAMOBJ_SET, -2,  DAMOBJ_SET, 4096, DAMOBJ_SET, "0x",
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET,  "length", DAMOBJ_SET, 1, DAMOBJ_SET, 0,
        DAMOBJ_SET, 0, DAMOBJ_SET, 0, DAMOBJ_SET, 0, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_SET, "BINARY"},

        {DAMOBJ_SET, "VARBINARY",  DAMOBJ_SET, -3,  DAMOBJ_SET, 4096, DAMOBJ_SET, "0x",
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET,  "max length", DAMOBJ_SET, 1, DAMOBJ_SET, 0,
        DAMOBJ_SET, 0, DAMOBJ_SET, 0, DAMOBJ_SET, 0, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_SET, "VARBINARY"},

        {DAMOBJ_SET, "LONGVARBINARY",  DAMOBJ_SET, -4,  DAMOBJ_SET, 2147483647, DAMOBJ_SET, "0x",
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET,  "max length", DAMOBJ_SET, 1, DAMOBJ_SET, 0,
        DAMOBJ_SET, 0, DAMOBJ_SET, 0, DAMOBJ_SET, 0, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_SET, "LONGVARBINARY"},

        {DAMOBJ_SET, "VARCHAR",  DAMOBJ_SET, 12,  DAMOBJ_SET, 4096, DAMOBJ_SET, "'",
        DAMOBJ_SET, "'", DAMOBJ_SET,  "max length", DAMOBJ_SET, 1, DAMOBJ_SET, 1,
        DAMOBJ_SET, 3, DAMOBJ_SET, 0, DAMOBJ_SET, 0, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_SET, "VARCHAR"},

        {DAMOBJ_SET, "LONGVARCHAR",  DAMOBJ_SET, -1,  DAMOBJ_SET, 2147483647, DAMOBJ_SET, "'",
        DAMOBJ_SET, "'", DAMOBJ_SET,  "max length", DAMOBJ_SET, 1, DAMOBJ_SET, 1,
        DAMOBJ_SET, 3, DAMOBJ_SET, 0, DAMOBJ_SET, 0, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_SET, "LONGVARCHAR"},

        {DAMOBJ_SET, "DATE",  DAMOBJ_SET, 91,  DAMOBJ_SET, 10, DAMOBJ_SET, "'",
        DAMOBJ_SET, "'", DAMOBJ_NOTSET,  EMPTY_STR, DAMOBJ_SET, 1, DAMOBJ_SET, 0,
        DAMOBJ_SET, 2, DAMOBJ_SET, 0, DAMOBJ_SET, 0, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_SET, "DATE"},

        {DAMOBJ_SET, "TIME",  DAMOBJ_SET, 92,  DAMOBJ_SET, 8, DAMOBJ_SET, "'",
        DAMOBJ_SET, "'", DAMOBJ_NOTSET,  EMPTY_STR, DAMOBJ_SET, 1, DAMOBJ_SET, 0,
        DAMOBJ_SET, 2, DAMOBJ_SET, 0, DAMOBJ_SET, 0, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_SET, "TIME"},

        {DAMOBJ_SET, "TIMESTAMP",  DAMOBJ_SET, 93,  DAMOBJ_SET, 19, DAMOBJ_SET, "'",
        DAMOBJ_SET, "'", DAMOBJ_NOTSET,  EMPTY_STR, DAMOBJ_SET, 1, DAMOBJ_SET, 0,
        DAMOBJ_SET, 2, DAMOBJ_SET, 0, DAMOBJ_SET, 0, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_SET, "TIMESTAMP"},

        {DAMOBJ_SET, "BIT",  DAMOBJ_SET, XO_TYPE_BIT,  DAMOBJ_SET, 1, DAMOBJ_NOTSET, EMPTY_STR,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_NOTSET,  EMPTY_STR, DAMOBJ_SET, 1, DAMOBJ_SET, 0,
        DAMOBJ_SET, 2, DAMOBJ_SET, 0, DAMOBJ_SET, 0, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_SET, "BIT"},

        {DAMOBJ_SET, "TINYINT",  DAMOBJ_SET, XO_TYPE_TINYINT,  DAMOBJ_SET, 3, DAMOBJ_NOTSET, EMPTY_STR,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_NOTSET,  EMPTY_STR, DAMOBJ_SET, 1, DAMOBJ_SET, 0,
        DAMOBJ_SET, 2, DAMOBJ_SET, 1, DAMOBJ_SET, 0, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_SET, "TINYINT"},

        {DAMOBJ_SET, "NULL",  DAMOBJ_SET, XO_TYPE_NULL,  DAMOBJ_SET, 1, DAMOBJ_NOTSET, EMPTY_STR,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_NOTSET,  EMPTY_STR, DAMOBJ_SET, 1, DAMOBJ_SET, 0,
        DAMOBJ_SET, 2, DAMOBJ_SET, 0, DAMOBJ_SET, 0, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_SET, "NULL"},

        {DAMOBJ_SET, "WCHAR",  DAMOBJ_SET, -8,  DAMOBJ_SET, 4096, DAMOBJ_SET, "N'",
        DAMOBJ_SET, "'", DAMOBJ_SET,  "length", DAMOBJ_SET, 1, DAMOBJ_SET, 1,
        DAMOBJ_SET, 3, DAMOBJ_SET, 0, DAMOBJ_SET, 0, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_SET, "WCHAR"},

        {DAMOBJ_SET, "WVARCHAR",  DAMOBJ_SET, -9,  DAMOBJ_SET, 4096, DAMOBJ_SET, "N'",
        DAMOBJ_SET, "'", DAMOBJ_SET,  "max length", DAMOBJ_SET, 1, DAMOBJ_SET, 1,
        DAMOBJ_SET, 3, DAMOBJ_SET, 0, DAMOBJ_SET, 0, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_SET, "WVARCHAR"},

        {DAMOBJ_SET, "WLONGVARCHAR",  DAMOBJ_SET, -10,  DAMOBJ_SET, 2147483647, DAMOBJ_SET, "N'",
        DAMOBJ_SET, "'", DAMOBJ_SET,  "max length", DAMOBJ_SET, 1, DAMOBJ_SET, 1,
        DAMOBJ_SET, 3, DAMOBJ_SET, 0, DAMOBJ_SET, 0, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_NOTSET,  EMPTY_VAL, DAMOBJ_SET, "WLONGVARCHAR"},
};

#define COL_ARRAY_SIZE_TYPES  (sizeof(gColArray_Types) / sizeof(damobj_col_types))

int     OAIP_execute_dataTypes(DAM_HDBC hdbc, DAM_HSTMT hstmt, int iStmtType, int *piNumResRows)
{
    DAM_HCOL    hcol_type_name, hcol_data_type,
                hcol_precision, hcol_literal_prefix,
                hcol_literal_suffix, hcol_create_params,
                hcol_oa_nullable, hcol_case_sensitive,
                hcol_oa_searchable, hcol_unsigned_attrib,
                hcol_oa_money, hcol_auto_increment,
                hcol_minimum_scale, hcol_maximum_scale,
                hcol_local_type_name;
    unsigned int  iRowCount;
    int         iRetCode;
    int         iRowStatus;


    *piNumResRows = 0;
    if (iStmtType != DAM_SELECT) return DAM_SUCCESS;

    /* get all column handles */
    hcol_type_name = dam_getCol(hstmt, "type_name");
    hcol_data_type = dam_getCol(hstmt, "data_type");
    hcol_precision = dam_getCol(hstmt, "oa_precision");
    hcol_literal_prefix = dam_getCol(hstmt, "literal_prefix");
    hcol_literal_suffix = dam_getCol(hstmt, "literal_suffix");
    hcol_create_params = dam_getCol(hstmt, "create_params");
    hcol_oa_nullable = dam_getCol(hstmt, "oa_nullable");
    hcol_case_sensitive = dam_getCol(hstmt, "case_sensitive");
    hcol_oa_searchable = dam_getCol(hstmt, "oa_searchable");
    hcol_unsigned_attrib = dam_getCol(hstmt, "unsigned_attrib");
    hcol_oa_money = dam_getCol(hstmt, "oa_money");
    hcol_auto_increment = dam_getCol(hstmt, "auto_increment");
    hcol_minimum_scale = dam_getCol(hstmt, "minimum_scale");
    hcol_maximum_scale = dam_getCol(hstmt, "maximum_scale");
    hcol_local_type_name = dam_getCol(hstmt, "local_type_name");
    if (!hcol_type_name || !hcol_data_type ||
                !hcol_precision || !hcol_literal_prefix ||
                !hcol_literal_suffix || !hcol_create_params ||
                !hcol_oa_nullable || !hcol_case_sensitive ||
                !hcol_oa_searchable || !hcol_unsigned_attrib ||
                !hcol_oa_money || !hcol_auto_increment ||
                !hcol_minimum_scale || !hcol_maximum_scale ||
                !hcol_local_type_name)
        return DAM_FAILURE;

    iRowCount = 0;

   while (iRowCount < COL_ARRAY_SIZE_TYPES) {
        DAM_HROW            hrow;
        damobj_col_types    *pObj;

        pObj = &gColArray_Types[iRowCount];

        /* skip deprecated data/time types */
        if (1) {
            if (pObj->data_type == XO_TYPE_DATE || pObj->data_type == XO_TYPE_TIME || pObj->data_type == XO_TYPE_TIMESTAMP) {
                iRowCount++;
                continue;
                }
            }

        /* allocate a new row */
        hrow = dam_allocRow(hstmt);
        if(!hrow) return DAM_FAILURE;

        dam_addValToRow(hstmt, hrow, hcol_type_name, XO_TYPE_CHAR, pObj->type_name,
            (pObj->type_name_len != DAMOBJ_NOTSET) ? XO_NTS : XO_NULL_DATA);

        dam_addValToRow(hstmt, hrow, hcol_data_type, XO_TYPE_SMALLINT, &pObj->data_type,
            (pObj->data_type_len != DAMOBJ_NOTSET) ? (long)sizeof(pObj->data_type) : XO_NULL_DATA);

        dam_addValToRow(hstmt, hrow, hcol_precision, XO_TYPE_INTEGER, &pObj->oa_precision,
            (pObj->oa_precision_len != DAMOBJ_NOTSET) ? (long)sizeof(pObj->oa_precision) : XO_NULL_DATA);

        dam_addValToRow(hstmt, hrow, hcol_literal_prefix, XO_TYPE_CHAR, pObj->literal_prefix,
            (pObj->literal_prefix_len != DAMOBJ_NOTSET) ? XO_NTS : XO_NULL_DATA);

        dam_addValToRow(hstmt, hrow, hcol_literal_suffix, XO_TYPE_CHAR, pObj->literal_suffix,
            (pObj->literal_suffix_len != DAMOBJ_NOTSET) ? XO_NTS : XO_NULL_DATA);

        dam_addValToRow(hstmt, hrow, hcol_create_params, XO_TYPE_CHAR, pObj->create_params,
            (pObj->create_params_len != DAMOBJ_NOTSET) ? XO_NTS : XO_NULL_DATA);

        dam_addValToRow(hstmt, hrow, hcol_oa_nullable, XO_TYPE_SMALLINT, &pObj->oa_nullable,
            (pObj->oa_nullable_len != DAMOBJ_NOTSET) ? (long)sizeof(pObj->oa_nullable) : XO_NULL_DATA);

        dam_addValToRow(hstmt, hrow, hcol_case_sensitive, XO_TYPE_SMALLINT, &pObj->case_sensitive,
            (pObj->case_sensitive_len != DAMOBJ_NOTSET) ? (long)sizeof(pObj->case_sensitive) : XO_NULL_DATA);

        dam_addValToRow(hstmt, hrow, hcol_oa_searchable, XO_TYPE_SMALLINT, &pObj->oa_searchable,
            (pObj->oa_searchable_len != DAMOBJ_NOTSET) ? (long)sizeof(pObj->oa_searchable) : XO_NULL_DATA);

        dam_addValToRow(hstmt, hrow, hcol_unsigned_attrib, XO_TYPE_SMALLINT, &pObj->unsigned_attrib,
            (pObj->unsigned_attrib_len != DAMOBJ_NOTSET) ? (long)sizeof(pObj->unsigned_attrib) : XO_NULL_DATA);

        dam_addValToRow(hstmt, hrow, hcol_oa_money, XO_TYPE_SMALLINT, &pObj->oa_money,
            (pObj->oa_money_len != DAMOBJ_NOTSET) ? (long)sizeof(pObj->oa_money) : XO_NULL_DATA);

        dam_addValToRow(hstmt, hrow, hcol_auto_increment, XO_TYPE_SMALLINT, &pObj->auto_increment,
            (pObj->auto_increment_len != DAMOBJ_NOTSET) ? (long)sizeof(pObj->auto_increment) : XO_NULL_DATA);

        dam_addValToRow(hstmt, hrow, hcol_minimum_scale, XO_TYPE_SMALLINT, &pObj->minimum_scale,
            (pObj->minimum_scale_len != DAMOBJ_NOTSET) ? (long)sizeof(pObj->minimum_scale) : XO_NULL_DATA);

        dam_addValToRow(hstmt, hrow, hcol_maximum_scale, XO_TYPE_SMALLINT, &pObj->maximum_scale,
            (pObj->maximum_scale_len != DAMOBJ_NOTSET) ? (long)sizeof(pObj->maximum_scale) : XO_NULL_DATA);

        dam_addValToRow(hstmt, hrow, hcol_local_type_name, XO_TYPE_CHAR, pObj->local_type_name,
            (pObj->local_type_name_len != DAMOBJ_NOTSET) ? XO_NTS : XO_NULL_DATA);

        iRowStatus = dam_isTargetRow(hstmt, hrow);
        if (iRowStatus == DAM_TRUE) {
            iRetCode = dam_addRowToTable(hstmt, hrow);
            if (iRetCode != DAM_SUCCESS) return iRetCode;
            (*piNumResRows)++;
            }
        else if (iRowStatus == DAM_ERROR)
            return DAM_FAILURE;
        else
            dam_freeRow(hrow);

        iRowCount++;
        }

    return DAM_SUCCESS;
}

/****************** OA_INFO definition ******************************/

typedef struct _damobj_col_info {
    short   info_name_len;
    char    info_name[DAM_MAX_ID_LEN+1];
    short   info_num_len;
    short   info_num;
    short   info_int_len;
    short   info_int;
    short   info_bitmask_len;
    long    info_bitmask;
    short   info_text_len;
    char    info_text[DAM_MAX_ID_LEN+1];
    short   remarks_len;
    char    remarks[DAM_MAX_ID_LEN+1];
} damobj_col_info;

damobj_col_info gColArray_Info[] = {
        /* INFO_NAME    INFO_NUM
        INFO_INT    INFO_BITMASK
        INFO_TEXT     REMARKS */

        {DAMOBJ_SET, "SQL_ACTIVE_STATEMENTS", DAMOBJ_SET,     1,
        DAMOBJ_SET, 0, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "The maximum number of statements supported"},

        {DAMOBJ_SET, "SQL_ROW_UPDATES", DAMOBJ_SET, 11,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_SET, "N", DAMOBJ_SET, "Y if driver can detect row changes between multiple fetches of"},

        {DAMOBJ_SET, "SQL_ODBC_SQL_CONFORMANCE", DAMOBJ_SET,     15,
        DAMOBJ_SET, 0, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "SQL Grammar supported by the driver"},

        {DAMOBJ_SET, "SQL_SEARCH_PATTERN_ESCAPE", DAMOBJ_SET, 14,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_SET, "\\", DAMOBJ_NOTSET, EMPTY_STR},

        {DAMOBJ_SET, "SQL_DBMS_NAME", DAMOBJ_SET, 17,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_SET, "OpenAccess", DAMOBJ_NOTSET, EMPTY_STR },

        {DAMOBJ_SET, "SQL_DBMS_VER", DAMOBJ_SET, 18,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_SET, "07.02", DAMOBJ_SET, "Version of current DBMS product of the form ##.## ex: 01.00"},

        {DAMOBJ_SET, "SQL_ACCESSIBLE_TABLES", DAMOBJ_SET, 19,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_SET, "Y", DAMOBJ_SET, "Y if user is guaranted access to all tables returned by SQL_TAB"},

        {DAMOBJ_SET, "SQL_ACCESSIBLE_PROCEDURES", DAMOBJ_SET, 20,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_SET, "Y", DAMOBJ_SET, "Y if dat source supports procedures"},

        {DAMOBJ_SET, "SQL_PROCEDURES", DAMOBJ_SET, 21,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_SET, "Y", DAMOBJ_SET, "Y if user can execute returned by SQL_PROCEDURES"},

        {DAMOBJ_SET, "SQL_CONCAT_NULL_BEHAVIOR", DAMOBJ_SET,     22,
        DAMOBJ_SET, 0, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "0 if string+NULL=NULL / 1 if result is string"},

        {DAMOBJ_SET, "SQL_DATA_SOURCE_READ_ONLY", DAMOBJ_SET, 25,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_SET, "N", DAMOBJ_SET, "Y if data source set to read only"},

        {DAMOBJ_SET, "SQL_EXPRESSIONS_IN_ORDERBY", DAMOBJ_SET, 27,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_SET, "Y", DAMOBJ_SET, "Y if driver supports ORDER BY expression"},

        {DAMOBJ_SET, "SQL_IDENTIFIER_CASE", DAMOBJ_SET, 28,
        DAMOBJ_SET, 4, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "1= case insensitive(stored upper), 2 = lower case, 3 = case sensitive, stored mixed, 4 = case insensitive, stored mixed"},

        {DAMOBJ_SET, "SQL_IDENTIFIER_QUOTE_CHAR", DAMOBJ_SET, 29,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_SET, "\"", DAMOBJ_SET, "the character string used to surround a delimiter identifier. b"},

        {DAMOBJ_SET, "SQL_MAX_COLUMN_NAME_LEN", DAMOBJ_SET, 30,
        DAMOBJ_SET, 128, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "Max length of a column name in the data source"},

        {DAMOBJ_SET, "SQL_MAX_OWNER_NAME_LEN", DAMOBJ_SET, 32,
        DAMOBJ_SET, 128, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "Max length of an owner name in the data source"},

        {DAMOBJ_SET, "SQL_MAX_PROCEDURE_NAME_LEN", DAMOBJ_SET, 33,
        DAMOBJ_SET, 128, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "Max length of a procedure name in the data source"},

        {DAMOBJ_SET, "SQL_MAX_QUALIFIER_NAME_LEN", DAMOBJ_SET, 34,
        DAMOBJ_SET, 128, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "Max length of a qualifier name in the data source"},

        {DAMOBJ_SET, "SQL_MAX_TABLE_NAME_LEN", DAMOBJ_SET, 35,
        DAMOBJ_SET, 128, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "Max length of a table name in the data source"},

        {DAMOBJ_SET, "SQL_MULT_RESULT_SETS", DAMOBJ_SET, 36,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_SET, "Y", DAMOBJ_SET, "Y if databases support multiple result sets"},

        {DAMOBJ_SET, "SQL_MULTIPLE_ACTIVE_TXN", DAMOBJ_SET, 37,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_SET, "Y", DAMOBJ_SET, "Y if transact. on multiple connection are allowed"},

        {DAMOBJ_SET, "SQL_OUTER_JOINS", DAMOBJ_SET, 38,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_SET, "Y", DAMOBJ_SET, "Y if data source supports outer joins."},

        {DAMOBJ_SET, "SQL_PROCEDURE_TERM", DAMOBJ_SET, 40,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_SET, "procedure", DAMOBJ_SET, "the vendor's name for procedure"},

        {DAMOBJ_SET, "SQL_QUALIFIER_NAME_SEPARATOR", DAMOBJ_SET, 41,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_SET, ".", DAMOBJ_SET, "the character string defines as a separator between the qualifi"},

        {DAMOBJ_SET, "SQL_TABLE_TERM", DAMOBJ_SET, 45,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_SET, "table", DAMOBJ_SET, "the vendor's name for table"},

        {DAMOBJ_SET, "SQL_TXN_CAPABLE", DAMOBJ_SET, 46,
        DAMOBJ_SET, 1, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "0= transact not supported. 1 = transact contains only DML state, 2=DML or DDL"},

        {DAMOBJ_SET, "SQL_CONVERT_FUNCTIONS", DAMOBJ_SET, 48,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_NOTSET, EMPTY_STR },

        {DAMOBJ_SET, "SQL_NUMERIC_FUNCTIONS", DAMOBJ_SET, 49,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 16777215,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "bitmask enumerating scalar numeric functions supported by the d"},

        {DAMOBJ_SET, "SQL_STRING_FUNCTIONS", DAMOBJ_SET, 50,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 16547839,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "bitmask enumerating scalar string functions supported by the dr"},

        {DAMOBJ_SET, "SQL_SYSTEM_FUNCTIONS", DAMOBJ_SET, 51,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 3,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "bitmask enumerating scalar system functions supported by the dr"},

        {DAMOBJ_SET, "SQL_TIMEDATE_FUNCTIONS", DAMOBJ_SET, 52,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 630783,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "bitmask enumerating scalar time and date functions supported by"},

        {DAMOBJ_SET, "SQL_CONVERT_BIGINT", DAMOBJ_SET,     53,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_NOTSET, EMPTY_STR },

        {DAMOBJ_SET, "SQL_CONVERT_BINARY", DAMOBJ_SET,     54,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_NOTSET, EMPTY_STR },

        {DAMOBJ_SET, "SQL_CONVERT_BIT", DAMOBJ_SET,         55,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_NOTSET, EMPTY_STR },

        {DAMOBJ_SET, "SQL_CONVERT_CHAR", DAMOBJ_SET,     56,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_NOTSET, EMPTY_STR },

        {DAMOBJ_SET, "SQL_CONVERT_DATE", DAMOBJ_SET, 57,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_NOTSET, EMPTY_STR },

        {DAMOBJ_SET, "SQL_CONVERT_DECIMAL", DAMOBJ_SET, 58,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_NOTSET, EMPTY_STR },

        {DAMOBJ_SET, "SQL_CONVERT_DOUBLE", DAMOBJ_SET, 59,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_NOTSET, EMPTY_STR },

        {DAMOBJ_SET, "SQL_CONVERT_FLOAT", DAMOBJ_SET, 60,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_NOTSET, EMPTY_STR },

        {DAMOBJ_SET, "SQL_CONVERT_INTEGER", DAMOBJ_SET, 61,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_NOTSET, EMPTY_STR },

        {DAMOBJ_SET, "SQL_CONVERT_LONGVARCHAR", DAMOBJ_SET, 62,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_NOTSET, EMPTY_STR },

        {DAMOBJ_SET, "SQL_CONVERT_NUMERIC", DAMOBJ_SET, 63,
        DAMOBJ_NOTSET, EMPTY_VAL,       DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_NOTSET, EMPTY_STR },

        {DAMOBJ_SET, "SQL_CONVERT_REAL", DAMOBJ_SET, 64,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_NOTSET, EMPTY_STR },

        {DAMOBJ_SET, "SQL_CONVERT_SMALLINT", DAMOBJ_SET, 65,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_NOTSET, EMPTY_STR },

        {DAMOBJ_SET, "SQL_CONVERT_TIME", DAMOBJ_SET, 66,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_NOTSET, EMPTY_STR },

        {DAMOBJ_SET, "SQL_CONVERT_TIMESTAMP", DAMOBJ_SET, 67,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_NOTSET, EMPTY_STR },

        {DAMOBJ_SET, "SQL_CONVERT_TINYINT", DAMOBJ_SET, 68,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_NOTSET, EMPTY_STR },

        {DAMOBJ_SET, "SQL_CONVERT_VARBINARY", DAMOBJ_SET, 69,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_NOTSET, EMPTY_STR },

        {DAMOBJ_SET, "SQL_CONVERT_VARCHAR", DAMOBJ_SET, 70,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_NOTSET, EMPTY_STR },

        {DAMOBJ_SET, "SQL_CONVERT_LONGVARBINARY", DAMOBJ_SET, 71,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_NOTSET, EMPTY_STR },

        {DAMOBJ_SET, "SQL_ODBC_SQL_OPT_IEF", DAMOBJ_SET, 73,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_SET, "N", DAMOBJ_SET, "Specifies whether the data source supports Integrity Enhancement"},

        {DAMOBJ_SET, "SQL_TXN_ISOLATION_OPTION", DAMOBJ_SET, 72,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 7,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "bitmask enumarating transaction isolation levels."},

        {DAMOBJ_SET, "SQL_CORRELATION_NAME", DAMOBJ_SET, 74,
        DAMOBJ_SET, 2, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "16 bits integer indicating if correlation names are supported"},

        {DAMOBJ_SET, "SQL_NON_NULLABLE_COLUMNS", DAMOBJ_SET,     75,
        DAMOBJ_SET, 1, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "16 bit int specifying whether the data source supports non null"},

        {DAMOBJ_SET, "SQL_GETDATA_EXTENSIONS", DAMOBJ_SET, 81,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 3,
         DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "32 bit bitamask enumarating extensions to SQLGetData"},

        {DAMOBJ_SET, "SQL_NULL_COLLATION", DAMOBJ_SET, 85,
        DAMOBJ_SET, 1, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "Specifies where null are sorted in a list."},

        {DAMOBJ_SET, "SQL_ALTER_TABLE", DAMOBJ_SET,         86,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 37867,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "Bitmask enumerating supported ALTER TABLE clauses."},

        {DAMOBJ_SET, "SQL_COLUMN_ALIAS", DAMOBJ_SET,     87,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_SET, "Y", DAMOBJ_SET, "Y if driver supports column alias"},

        {DAMOBJ_SET, "SQL_GROUP_BY", DAMOBJ_SET, 88,
        DAMOBJ_SET, 2, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "16 bit int specifying the relationship between col in group by. 2=SQL_GB_GROUP_BY_CONTAINS_SELECT"},

        {DAMOBJ_SET, "SQL_KEYWORDS", DAMOBJ_SET, 89,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_SET, "HINT,IDENTIFIED", DAMOBJ_SET, "List of source specific Keywords"},

        {DAMOBJ_SET, "SQL_ORDER_BY_COLUMNS_IN_SELECT", DAMOBJ_SET, 90,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_SET, "N", DAMOBJ_SET, "Y if the columns in the ORDER BY stmt clause must be in the sel"},

        {DAMOBJ_SET, "SQL_OWNER_USAGE", DAMOBJ_SET, 91,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 15,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "Enumarates the statements in which owners can be used."},

        {DAMOBJ_SET, "SQL_QUALIFIER_USAGE", DAMOBJ_SET, 92,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 7,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "bitmask enumerating the statements in which a qualifier can be"},

        {DAMOBJ_SET, "SQL_QUOTED_IDENTIFIER_CASE", DAMOBJ_SET, 93,
        DAMOBJ_SET, 4, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_NOTSET, EMPTY_STR},

        {DAMOBJ_SET, "SQL_SPECIAL_CHARACTERS", DAMOBJ_SET, 94,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_SET, "*(){}|:;", DAMOBJ_SET, "List of special characters that can be used in an object name"},

        {DAMOBJ_SET, "SQL_SUBQUERIES", DAMOBJ_SET, 95,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 31,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "bitmask enumerating predicates that support subqueries"},

        {DAMOBJ_SET, "SQL_UNION", DAMOBJ_SET, 96,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 3,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "Bitmask enumarating the support for the union clause"},

        {DAMOBJ_SET, "SQL_MAX_COLUMNS_IN_GROUP_BY", DAMOBJ_SET, 97,
        DAMOBJ_SET, 0, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "Max number of columns in a GROUP BY stmt. 0 if unknown"},

        {DAMOBJ_SET, "SQL_MAX_COLUMNS_IN_INDEX", DAMOBJ_SET, 98,
        DAMOBJ_SET, 0, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "Max number of columns in an index. 0 if unknown"},

        {DAMOBJ_SET, "SQL_MAX_COLUMNS_IN_ORDER_BY", DAMOBJ_SET, 99,
        DAMOBJ_SET, 0, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "Max number of columns in an ORDER BY clause. 0 if unknown"},

        {DAMOBJ_SET, "SQL_MAX_COLUMNS_IN_SELECT", DAMOBJ_SET, 100,
        DAMOBJ_SET, 0, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "Max column in SELECT stmt. 0 if unknown"},

        {DAMOBJ_SET, "SQL_MAX_COLUMNS_IN_TABLE", DAMOBJ_SET, 101,
        DAMOBJ_SET, 0, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "Max number of tables in a table"},

        {DAMOBJ_SET, "SQL_MAX_INDEX_SIZE", DAMOBJ_SET, 102,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "Max number of bytes allowed in the combined field of an index."},

        {DAMOBJ_SET, "SQL_MAX_ROW_SIZE_INCLUDES_LONG", DAMOBJ_SET, 103,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_SET, "N", DAMOBJ_SET, "Y if MAX_ROW_SIZE includes the length of all long Data types."},

        {DAMOBJ_SET, "SQL_MAX_ROW_SIZE", DAMOBJ_SET, 104,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "Max size of a row in a datasource. This limitation comes from t"},

        {DAMOBJ_SET, "SQL_MAX_STATEMENT_LEN", DAMOBJ_SET, 105,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 32768,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "Max length of an SQL stmt"},

        {DAMOBJ_SET, "SQL_MAX_TABLES_IN_SELECT", DAMOBJ_SET, 106,
        DAMOBJ_SET, 0, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "Max table number in Select stmt. 0 if unknown"},

        {DAMOBJ_SET, "SQL_MAX_CHAR_LITERAL_LEN", DAMOBJ_SET, 108,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 8192,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "32 bits int specifying max length of a character literal in a S"},

        {DAMOBJ_SET, "SQL_TIMEDATE_ADD_INTERVALS", DAMOBJ_SET, 109,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "bitmask enumerating the timestamp intervals supported in TIMEST"},

        {DAMOBJ_SET, "SQL_TIMEDATE_DIFF_INTERVALS", DAMOBJ_SET, 110,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "bitmask enumerating the timestamp intervals supported in TIMEST"},

        {DAMOBJ_SET, "SQL_MAX_BINARY_LITERAL_LEN", DAMOBJ_SET, 112,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 8192,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "32 bits int specifying max length of a binary literal"},

        {DAMOBJ_SET, "SQL_LIKE_ESCAPE_CLAUSE", DAMOBJ_SET, 113,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_SET, "Y", DAMOBJ_SET, "Y if data source supports escape character in LIKE clause"},

        {DAMOBJ_SET, "SQL_QUALIFIER_LOCATION", DAMOBJ_SET, 114,
        DAMOBJ_SET, 1, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, "indicates the position of the qualifier in a qualified table name"},

        {DAMOBJ_SET, "SQL_OJ_CAPABILITIES", DAMOBJ_SET, 115,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0x49,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, " bitmask enumerating the types of outer joins supported by the driver "},

        {DAMOBJ_SET, "SQL_ALTER_DOMAIN", DAMOBJ_SET, 117,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_SQL_CONFORMANCE", DAMOBJ_SET, 118,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 1,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_DATETIME_LITERALS", DAMOBJ_SET, 119,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_BATCH_ROW_COUNT", DAMOBJ_SET, 120,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_BATCH_SUPPORT", DAMOBJ_SET, 121,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_CREATE_ASSERTION", DAMOBJ_SET, 127,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_CREATE_CHARACTER_SET", DAMOBJ_SET, 128,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_CREATE_COLLATION", DAMOBJ_SET, 129,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_CREATE_DOMAIN", DAMOBJ_SET, 130,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_CREATE_SCHEMA", DAMOBJ_SET, 131,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_CREATE_TABLE", DAMOBJ_SET, 132,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 1,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_CREATE_TRANSLATION", DAMOBJ_SET, 133,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_CREATE_VIEW", DAMOBJ_SET, 134,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 1,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_DROP_ASSERTION", DAMOBJ_SET, 136,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_DROP_CHARACTER_SET", DAMOBJ_SET, 137,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_DROP_COLLATION", DAMOBJ_SET, 138,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_DROP_DOMAIN", DAMOBJ_SET, 139,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_DROP_SCHEMA", DAMOBJ_SET, 140,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_DROP_TABLE", DAMOBJ_SET, 141,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 1,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_DROP_TRANSLATION", DAMOBJ_SET, 142,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_DROP_VIEW", DAMOBJ_SET, 143,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 1,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_INDEX_KEYWORDS", DAMOBJ_SET, 148,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 3,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_INFO_SCHEMA_VIEWS", DAMOBJ_SET, 149,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_SQL92_DATETIME_FUNCTIONS", DAMOBJ_SET, 155,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 7,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_SQL92_FOREIGN_KEY_DELETE_RULE", DAMOBJ_SET, 156,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 2,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_SQL92_FOREIGN_KEY_UPDATE_RULE", DAMOBJ_SET, 157,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 2,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_SQL92_GRANT", DAMOBJ_SET, 158,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 3184,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_SQL92_NUMERIC_VALUE_FUNCTIONS", DAMOBJ_SET, 159,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 63,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_SQL92_PREDICATES", DAMOBJ_SET, 160,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 16135,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_SQL92_RELATIONAL_JOIN_OPERATORS", DAMOBJ_SET, 161,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 592,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_SQL92_REVOKE", DAMOBJ_SET, 162,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 3184,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_SQL92_ROW_VALUE_CONSTRUCTOR", DAMOBJ_SET, 163,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 11,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_SQL92_STRING_FUNCTIONS", DAMOBJ_SET, 164,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 238,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_SQL92_VALUE_EXPRESSIONS", DAMOBJ_SET, 165,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_AGGREGATE_FUNCTIONS", DAMOBJ_SET, 169,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 127,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_DDL_INDEX", DAMOBJ_SET, 170,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 0,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_INSERT_STATEMENT", DAMOBJ_SET, 172,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_SET, 3,
        DAMOBJ_NOTSET, EMPTY_STR, DAMOBJ_SET, ""},

        {DAMOBJ_SET, "SQL_COLLATION_SEQ", DAMOBJ_SET, 10004,
        DAMOBJ_NOTSET, EMPTY_VAL, DAMOBJ_NOTSET, EMPTY_VAL,
        DAMOBJ_SET, "ISO 8859-1", DAMOBJ_SET, "The name of the collation sequence for the default character set (for example, 'ISO 8859-1' or EBCDIC). "},

};

#define COL_ARRAY_SIZE_INFO  (sizeof(gColArray_Info) / sizeof(damobj_col_info))

int     OAIP_getDSInfo(DAM_HDBC hdbc,
                       short int       iInfoType,
                       void            *pInfoValue,
                       short int       iInfoValueMax,
                       short int       *piInfoValueLen)
{
    unsigned int       iRowCount = 0;
    damobj_col_info    *pObj     = NULL;


    while (iRowCount < COL_ARRAY_SIZE_INFO) {

        pObj = &gColArray_Info[iRowCount];

        if(iInfoType == pObj->info_num) break;
        iRowCount++;
    }

    if(iRowCount >= COL_ARRAY_SIZE_INFO)  return DAM_NOT_AVAILABLE;

    switch (iInfoType) {
    case    SQL_ACTIVE_STATEMENTS:
    case    SQL_ODBC_SQL_CONFORMANCE:
    case    SQL_CONCAT_NULL_BEHAVIOR:
    case    SQL_IDENTIFIER_CASE:
    case    SQL_MAX_COLUMN_NAME_LEN:
    case    SQL_MAX_OWNER_NAME_LEN:
    case    SQL_MAX_PROCEDURE_NAME_LEN:
    case    SQL_MAX_QUALIFIER_NAME_LEN:
    case    SQL_MAX_TABLE_NAME_LEN:
    case    SQL_TXN_CAPABLE:
    case    SQL_CORRELATION_NAME:
    case    SQL_NON_NULLABLE_COLUMNS:
    case    SQL_NULL_COLLATION:
    case    SQL_GROUP_BY:
    case    SQL_QUOTED_IDENTIFIER_CASE:
    case    SQL_MAX_COLUMNS_IN_GROUP_BY:
    case    SQL_MAX_COLUMNS_IN_INDEX:
    case    SQL_MAX_COLUMNS_IN_ORDER_BY:
    case    SQL_MAX_COLUMNS_IN_SELECT:
    case    SQL_MAX_COLUMNS_IN_TABLE:
    case    SQL_MAX_TABLES_IN_SELECT:
    case    SQL_QUALIFIER_LOCATION: {
                if (iInfoValueMax < (short int)sizeof (short int)) {
                    return DAM_FAILURE;
                }
                *(short int *)pInfoValue = pObj->info_int;
                *piInfoValueLen = (short int)sizeof(short int);
                return DAM_SUCCESS;
            }
    case    SQL_CONVERT_FUNCTIONS:
    case    SQL_NUMERIC_FUNCTIONS:
    case    SQL_STRING_FUNCTIONS:
    case    SQL_SYSTEM_FUNCTIONS:
    case    SQL_TIMEDATE_FUNCTIONS:
    case    SQL_CONVERT_BIGINT:
    case    SQL_CONVERT_BINARY:
    case    SQL_CONVERT_BIT:
    case    SQL_CONVERT_CHAR:
    case    SQL_CONVERT_DATE:
    case    SQL_CONVERT_DECIMAL:
    case    SQL_CONVERT_DOUBLE:
    case    SQL_CONVERT_FLOAT:
    case    SQL_CONVERT_INTEGER:
    case    SQL_CONVERT_LONGVARCHAR:
    case    SQL_CONVERT_NUMERIC:
    case    SQL_CONVERT_REAL:
    case    SQL_CONVERT_SMALLINT:
    case    SQL_CONVERT_TIME:
    case    SQL_CONVERT_TIMESTAMP:
    case    SQL_CONVERT_TINYINT:
    case    SQL_CONVERT_VARBINARY:
    case    SQL_CONVERT_VARCHAR:
    case    SQL_CONVERT_LONGVARBINARY:
    case    SQL_TXN_ISOLATION_OPTION:
    case    SQL_GETDATA_EXTENSIONS:
    case    SQL_ALTER_TABLE:
    case    SQL_OWNER_USAGE:
    case    SQL_QUALIFIER_USAGE:
    case    SQL_SUBQUERIES:
    case    SQL_UNION:
    case    SQL_MAX_INDEX_SIZE:
    case    SQL_MAX_ROW_SIZE:
    case    SQL_MAX_STATEMENT_LEN:
    case    SQL_MAX_CHAR_LITERAL_LEN:
    case    SQL_TIMEDATE_ADD_INTERVALS:
    case    SQL_TIMEDATE_DIFF_INTERVALS:
    case    SQL_MAX_BINARY_LITERAL_LEN:
    case    SQL_OJ_CAPABILITIES:
    case    SQL_ALTER_DOMAIN:
    case    SQL_SQL_CONFORMANCE:
    case    SQL_DATETIME_LITERALS:
    case    SQL_BATCH_ROW_COUNT:
    case    SQL_BATCH_SUPPORT:
    case    SQL_CREATE_ASSERTION:
    case    SQL_CREATE_CHARACTER_SET:
    case    SQL_CREATE_COLLATION:
    case    SQL_CREATE_DOMAIN:
    case    SQL_CREATE_SCHEMA:
    case    SQL_CREATE_TABLE:
    case    SQL_CREATE_TRANSLATION:
    case    SQL_CREATE_VIEW:
    case    SQL_DROP_ASSERTION:
    case    SQL_DROP_CHARACTER_SET:
    case    SQL_DROP_COLLATION:
    case    SQL_DROP_DOMAIN:
    case    SQL_DROP_SCHEMA:
    case    SQL_DROP_TABLE:
    case    SQL_DROP_TRANSLATION:
    case    SQL_DROP_VIEW:
    case    SQL_INDEX_KEYWORDS:
    case    SQL_INFO_SCHEMA_VIEWS:
    case    SQL_SQL92_DATETIME_FUNCTIONS:
    case    SQL_SQL92_FOREIGN_KEY_DELETE_RULE:
    case    SQL_SQL92_FOREIGN_KEY_UPDATE_RULE:
    case    SQL_SQL92_GRANT:
    case    SQL_SQL92_NUMERIC_VALUE_FUNCTIONS:
    case    SQL_SQL92_PREDICATES:
    case    SQL_SQL92_RELATIONAL_JOIN_OPERATORS:
    case    SQL_SQL92_REVOKE:
    case    SQL_SQL92_ROW_VALUE_CONSTRUCTOR:
    case    SQL_SQL92_STRING_FUNCTIONS:
    case    SQL_SQL92_VALUE_EXPRESSIONS:
    case    SQL_AGGREGATE_FUNCTIONS:
    case    SQL_DDL_INDEX:
    case    SQL_INSERT_STATEMENT: {
                if (iInfoValueMax < (short int)sizeof (int)) {
                    return DAM_FAILURE;
                }
                *(int *)pInfoValue = pObj->info_bitmask;
                *piInfoValueLen = (short int)sizeof(int);
                return DAM_SUCCESS;
            }
    case    SQL_ROW_UPDATES:
    case    SQL_SEARCH_PATTERN_ESCAPE:
    case    SQL_DBMS_NAME:
    case    SQL_DBMS_VER:
    case    SQL_ACCESSIBLE_TABLES:
    case    SQL_ACCESSIBLE_PROCEDURES:
    case    SQL_PROCEDURES:
    case    SQL_DATA_SOURCE_READ_ONLY:
    case    SQL_EXPRESSIONS_IN_ORDERBY:
    case    SQL_IDENTIFIER_QUOTE_CHAR:
    case    SQL_MULT_RESULT_SETS:
    case    SQL_MULTIPLE_ACTIVE_TXN:
    case    SQL_OUTER_JOINS:
    case    SQL_PROCEDURE_TERM:
    case    SQL_QUALIFIER_NAME_SEPARATOR:
    case    SQL_TABLE_TERM:
    case    SQL_ODBC_SQL_OPT_IEF:
    case    SQL_COLUMN_ALIAS:
    case    SQL_KEYWORDS:
    case    SQL_ORDER_BY_COLUMNS_IN_SELECT:
    case    SQL_SPECIAL_CHARACTERS:
    case    SQL_MAX_ROW_SIZE_INCLUDES_LONG:
    case    SQL_LIKE_ESCAPE_CLAUSE:
    case    SQL_COLLATION_SEQ: {
                short int tmpvalue;
                /* change iInfoValueMax to indicate number of unicode characters instead of bytes */
                iInfoValueMax = iInfoValueMax/sizeof(OAWCHAR);
                if (iInfoValueMax < (short int)strlen(pObj->info_text)) {
                    return DAM_FAILURE;
                }
                /* return as WCHAR string */
                oambstowcs((OAWCHAR *)pInfoValue, pObj->info_text, iInfoValueMax);
                tmpvalue = (short int)strlen(pObj->info_text);
                *piInfoValueLen = tmpvalue * sizeof(wchar_t);
                return DAM_SUCCESS;
           }
    }

    return DAM_NOT_AVAILABLE;
}
