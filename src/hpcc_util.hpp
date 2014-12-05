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
#ifndef __HPCCUTIL_HPP
#define __HPCCUTIL_HPP

#include "ipdef.h"
#include "hpccdb.hpp"
#include "jstring.hpp"


/* Environment Descriptor Area */
typedef struct hpcc_env_struct
{
    void        *pMemTree;      /* Memory Tree for the global context */
} HPCC_ENV_DA;


/* Connection Descriptor Area */
typedef struct hpcc_connection_struct
{
    XM_Tree *       pMemTree;
    HPCC_ENV_DA *   pEnvDA;        /* handle to EnvDA */
    HPCCdb *        pHPCCdb;
    char            sQualifier[DAM_MAX_ID_LEN];
    char            sUserName[DAM_MAX_ID_LEN];
    DAM_HDBC        dam_hdbc;
} HPCC_CONN_DA;


/* Statement Descriptor Area */
typedef struct hpcc_statement_struct
{
    HPCC_CONN_DA        *       pConnDA;       /* handle to connection descriptor area */
    XM_Tree             *       pMemTree;
    DAM_HSTMT                   dam_hstmt;      /* DAM handle to the statement */
    int                         iType;          /* Type of the query */
    char                        sTableName[DAM_MAX_ID_LEN+1];   /* Name of the table being queried */
    long                        lCurrentItem;   /* points to the current cursor position */
} HPCC_STMT_DA;

extern TM_ModuleCB     hpcc_tm_Handle; /* declared in hpcc_drv.c */

int     hpcc_exec(const char * sqlQuery, const char * targetQuerySet, HPCC_STMT_DA *pStmtDA, int *piNumResRows);

#endif  /* __HPCCUTIL_HPP */
