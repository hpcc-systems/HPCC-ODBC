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

#pragma warning (disable : 4005)    //warning C4005: 'strdup' : macro redefinition

#include "jstring.hpp"
#include "ws_sql_esp.ipp"
#include "hpccdb.hpp"

//Include generated files containing CPP code
#include "ws_workunits.esp"
#include "ws_topology.esp"
#include "common.esp"

static TM_ModuleCB  driver_tm_Hdle;//for logging


//*******************************************************************
//Shared schema cache singleton, persists even when connections close
//*******************************************************************
class CTable;
class CTableSchemaCache : public CInterface, public IInterface
{
public:
    IMPLEMENT_IINTERFACE;
    static CTableSchemaCache * queryInstance();
    virtual ~CTableSchemaCache() {}

    void clearCache()
    {
        CriticalBlock b(m_crit);
        m_tableCache.kill();
        m_metaDataCacheComplete = false;
    }

    aindex_t getMatchingTables(const char * tblFilter, IArrayOf<CTable> &_tables, bool bExact)
    {
        CriticalBlock b(m_crit);
        HashIterator iter(m_tableCache);
        if (bExact)
        {
            CTable *tbl = m_tableCache.getValue(tblFilter);
            _tables.append(*(LINK(tbl)));
        }
        else
        {
            unsigned filterLen = strlen(tblFilter);
            ForEach(iter)
            {
                CTable *tbl = m_tableCache.mapToValue(&iter.query());
                if (0 == strnicmp(tblFilter, tbl->queryName(), filterLen))
                    _tables.append(*(LINK(tbl)));
            }
        }
        return _tables.ordinality();
    }

    aindex_t getAllCachedTables(IArrayOf<CTable> &_tables)
    {
        CriticalBlock b(m_crit);
        HashIterator iter(m_tableCache);
        ForEach(iter)
        {
            CTable *tbl = m_tableCache.mapToValue(&iter.query());
            _tables.append(*(LINK(tbl)));
        }
        return _tables.ordinality();
    }

    void addTable(const char * tblName, Owned<CTable> *tblEntry)
    {
        CriticalBlock b(m_crit);
        m_tableCache.setValue(tblName, *tblEntry);
    }

    void setMetaDataCacheComplete(bool complete)//called when cache has the entire HPCC metadata, to ensure no more calls to WsSQL
    {
        CriticalBlock b(m_crit);
        m_metaDataCacheComplete = complete;
    }

    bool queryMetaDataCacheComplete()
    {
        CriticalBlock b(m_crit);
        return m_metaDataCacheComplete;
    }

protected:
    CTableSchemaCache() { m_metaDataCacheComplete = false; }

private:
    //attributes
    MapStringToMyClass<CTable>  m_tableCache;//Table Schema Cache
    CriticalSection             m_crit;
    bool                        m_metaDataCacheComplete;
};

CTableSchemaCache *CTableSchemaCache::queryInstance()
{
    static CTableSchemaCache Instance;
    return &Instance;
}


/************************************************************************
Class:  HPCCdb
*************************************************************************/

HPCCdb::HPCCdb(TM_ModuleCB tmHandle, const char * _protocol, const char * _wsSqlPort, const char * _user, const char * _pwd, const char * _wssqlIP, const char * _targetCluster, const char * _defaultQuerySet, aindex_t _maxFetchRowCount, aindex_t _cacheTimeout)
{
    tm_trace(driver_tm_Hdle, UL_TM_INFO, "HPCC_Conn:call to HPCCdb::HPCCdb()\n", ());
    driver_tm_Hdle = tmHandle;

    m_protocol.set(_protocol);
    m_wsSqlPort.set(_wsSqlPort);
    m_userName.set(_user);
    m_password.set(_pwd);

    m_wssqlIP.set(_wssqlIP);//TODO URLdecode

    m_targetCluster.set(_targetCluster);
    m_defaultQuerySet.set(_defaultQuerySet);
    m_maxFetchRowCount = _maxFetchRowCount;
    m_cacheTimeout = _cacheTimeout;

    tm_trace(driver_tm_Hdle, UL_TM_INFO, "HPCC_Conn: protocol='%s', wsSqlPort='%s', user='%s', wssqlIP='%s', targetCluster='%s', defaultQuerySet='%s', maxFetchRowCount='%d', cacheTimeout=%d\n", (_protocol,_wsSqlPort,_user,_wssqlIP,_targetCluster,_defaultQuerySet,_maxFetchRowCount,_cacheTimeout));

    //Create connection object
    m_clientWs_sql.setown(createwssqlClient());
    StringBuffer wssql;
    wssql.appendf("%s://%s:%s/wssql", m_protocol, m_wssqlIP, m_wsSqlPort);
    m_clientWs_sql->addServiceUrl(wssql.str());
    m_clientWs_sql->setUsernameToken(m_userName, m_password, NULL);
    m_bGotDeployedQueries = false;
}

HPCCdb::~HPCCdb()
{
    tm_trace(driver_tm_Hdle, UL_TM_INFO, "HPCC_Conn:call to HPCCdb::~HPCCdb()\n", ());
}


/************************************************************************
Function:       HPCCdb::getHPCCDBSystemInfo
Description:    Call esp "ws_sql" service to see it it is available
Return:         true    on Success
                false   unsuccessful
************************************************************************/
bool HPCCdb::getHPCCDBSystemInfo()
{
    tm_trace(driver_tm_Hdle, UL_TM_INFO, "HPCC_Conn:call to HPCCdb::getHPCCDBSystemInfo\n", ());

    Owned<IClientGetDBSystemInfoRequest> req;
    Owned<IClientGetDBSystemInfoResponse> resp;
    req.setown(createClientGetDBSystemInfoRequest());
    req->setIncludeAll(true);

    CriticalBlock b(crit);
    try
    {
        tm_trace(driver_tm_Hdle, UL_TM_INFO, "HPCC_Conn:calling ws_sql.getHPCCDBSystemInfo...\n", ());
        resp.setown(m_clientWs_sql->GetDBSystemInfo(req));//calls ws_sql
        tm_trace(driver_tm_Hdle, UL_TM_INFO, "HPCC_Conn:complete\n", ());
    }
    catch (IException* e)
    {
        StringBuffer sb;
        e->errorMessage(sb);

        hpccErrors.setf("HPCC_Conn:Error calling ws_sql : '%s'\n", (sb.str()));
        tm_trace(driver_tm_Hdle, UL_TM_ERRORS, "%s", (sb.str()));
        return false;
    }
    catch (...)
    {
        hpccErrors.setf("Exception thrown calling ws_sql");
        tm_trace(driver_tm_Hdle, UL_TM_ERRORS, "HPCC_Conn:Error calling ws_sql\n", ());
        return false;
    }

    StringBuffer sbErrors;
    if (checkForTopLevelErrors(resp->getExceptions(), sbErrors))
        return false;

    m_FullHPCCVersion.set(resp->getFullVersion());
    tm_trace(driver_tm_Hdle, UL_TM_INFO, "HPCC_Conn:Connected to '%s'\n", (m_FullHPCCVersion.get()));

    return true;
}

/************************************************************************
Function:       HPCCdb::getDeployedQueries
Description:
Return:         true    on Success
                false   unsuccessful
************************************************************************/
void HPCCdb::getDeployedQueries()
{
    tm_trace(driver_tm_Hdle, UL_TM_INFO, "HPCC_Conn:call to HPCCdb::getDeployedQueries\n", ());
    if (!m_bGotDeployedQueries)
    {
        Owned<IClientGetDBMetaDataRequest> req;
        Owned<IClientGetDBMetaDataResponse> resp;

        req.setown( createClientGetDBMetaDataRequest());
        req->setIncludeStoredProcedures(true);
        CriticalBlock b(crit);
        try
        {
            tm_trace(driver_tm_Hdle, UL_TM_INFO, "HPCC_Conn:calling ws_sql.GetDBMetaData()...\n", ());
            resp.setown(m_clientWs_sql->GetDBMetaData(req));//calls ws_sql
            tm_trace(driver_tm_Hdle, UL_TM_INFO, "HPCC_Conn:complete\n", ());
        }

        catch (IException* e)
        {
            StringBuffer sb;
            e->errorMessage(sb);

            hpccErrors.setf("HPCC_Conn:Error calling ws_sql : '%s'\n", (sb.str()));
            tm_trace(driver_tm_Hdle, UL_TM_ERRORS, "%s", (sb.str()));
            return;
        }

        catch (...)
        {
            hpccErrors.setf("Exception thrown calling ws_sql");
            tm_trace(driver_tm_Hdle, UL_TM_ERRORS, "HPCC_Conn:Error calling ws_sql\n", ());
            return;
        }

        StringBuffer sbErrors;
        if (checkForTopLevelErrors(resp->getExceptions(), sbErrors))
            return;

        //---------------------------
        //Process each QuerySet
        //---------------------------
        IArrayOf<IConstHPCCQuerySet> &qSets = resp->getQuerySets();
        ForEachItemIn(QSIdx, qSets)//thor, hthor, roxie
        {
            CHPCCQuerySet &hpccQuerySetItem = (CHPCCQuerySet &)qSets.item(QSIdx);
            tm_trace(driver_tm_Hdle, UL_TM_F_TRACE, "HPCC_Conn: Searching QuerySet '%s'...\n", (hpccQuerySetItem.getName()));
#ifdef _WIN32
            StringBuffer sb;
            sb.appendf("QuerySet %s\n",hpccQuerySetItem.getName());//thor, hthor, roxie
            OutputDebugString(sb.str());
#endif
            Owned<CMyQuerySet> querySet;
            //---------------------------
            //Get array of published queries within QuerySet item.
            //We will look up a matching query in this array
            //for each QuerySetAlias in this QuerySet
            //---------------------------
            IArrayOf<IConstPublishedQuery> & publishedQueries = hpccQuerySetItem.getQuerySetQueries();

            //---------------------------
            //Process each QuerySetQuery
            // Find the published query set query for each alias
            //---------------------------
            IArrayOf<IConstQuerySetAliasMap> & qsAliaseMap = hpccQuerySetItem.getQuerySetAliases();
            ForEachItemIn(QSAliasMapIdx, qsAliaseMap)
            {
                CQuerySetAliasMap &qsAliaseMapItem = (CQuerySetAliasMap &)qsAliaseMap.item(QSAliasMapIdx);
#ifdef _WIN32
                StringBuffer sb;
                sb.appendf("\tQuerySetQuery (alias) '%s', id '%s'\n",qsAliaseMapItem.getName(), qsAliaseMapItem.getId());
                OutputDebugString(sb.str());
#endif
                Owned<CMyQuerySetQuery> querySetQuery;
                querySetQuery.setown(new CMyQuerySetQuery(qsAliaseMapItem.getId(), qsAliaseMapItem.getName()));
                //Find this aliased QuerySetQuery in the published queries array
                ForEachItemIn(pqIdx, publishedQueries)
                {
                    //----------------------------------
                    // Found a query set query. Extract
                    // signature, inputs, outputs, etc
                    //----------------------------------
                    CPublishedQuery &publishedQueryItem = (CPublishedQuery &)publishedQueries.item(pqIdx);
                    if (0 == strcmp(qsAliaseMapItem.getId(), publishedQueryItem.getId()))
                    {
#ifdef _WIN32
                        StringBuffer sb;
                        sb.appendf("\t\tFound matching QuerySetQuery %s, id = %s\n",publishedQueryItem.getName(),publishedQueryItem.getId());
                        OutputDebugString(sb.str());
#endif

                        IConstQuerySignature &qSig = publishedQueryItem.getSignature();
                        tm_trace(driver_tm_Hdle, UL_TM_F_TRACE, "   HPCC:Found '%s', id = %s\n", (publishedQueryItem.getName(),publishedQueryItem.getId()));

                        //Process this query set query's inputs
                        IArrayOf<IConstHPCCColumn> &columns = qSig.getInParams();
                        ForEachItemIn(columnsIdx, columns)//process all columns in given table
                        {
                            CHPCCColumn &columnItem = (CHPCCColumn &)columns.item(columnsIdx);
                            Owned<CColumn> columnEntry;
                            tm_trace(driver_tm_Hdle, UL_TM_F_TRACE, "   HPCC:Found '%s' input '%s'('%s') \n", (publishedQueryItem.getName(),columnItem.getName(),columnItem.getType()));
                            columnEntry.setown(new CColumn(columnItem.getName()));
                            columnEntry->m_hpccType.set(columnItem.getType());
                            querySetQuery->addInput(LINK(columnEntry));
                        }

                        //Process this query set query's output dataset(s)
                        IArrayOf<IConstOutputDataset> &dataset = qSig.getResultSets();
                        ForEachItemIn(datasetsIdx, dataset)//process each output dataset
                        {
                            IConstOutputDataset & datasetItem = (IConstOutputDataset &)dataset.item(datasetsIdx);
                            IArrayOf<IConstHPCCColumn> &columns = datasetItem.getOutParams();
                            ForEachItemIn(columnsIdx, columns)//process all columns in given table
                            {
                                CHPCCColumn &columnItem = (CHPCCColumn &)columns.item(columnsIdx);
                                Owned<CColumn> columnEntry;
                                tm_trace(driver_tm_Hdle, UL_TM_F_TRACE, "   HPCC:Found '%s' output '%s'('%s') \n", (publishedQueryItem.getName(),columnItem.getName(),columnItem.getType()));
                                columnEntry.setown(new CColumn(columnItem.getName()));
                                columnEntry->m_hpccType.set(columnItem.getType());
                                querySetQuery->addOutputDatasetColumn(datasetItem.getName(), LINK(columnEntry));
                            }
                        }

                        //Add query to query set
                        if (!querySet)
                            querySet.setown(new CMyQuerySet(hpccQuerySetItem.getName()));//thor, hthor, roxie
                        querySet->addQuery(LINK(querySetQuery));
                        break;
                    }
                }
            }
            if (querySet)
            {
                LINK(querySet);
                m_arrQuerySets.append(*querySet.get());//thor, hthor, roxie
            }
        }
        m_bGotDeployedQueries = true;
    }
    else
        tm_trace(driver_tm_Hdle, UL_TM_INFO, "HPCC_Conn:using cached deployed query info\n", ());
}

/************************************************************************
Function:       HPCCdb::getTableSchema
Description:
Return:         true    on Success
                false   unsuccessful
************************************************************************/
bool HPCCdb::getTableSchema(const char * _tableFilter, IArrayOf<CTable> &_tables)
{
    tm_trace(driver_tm_Hdle, UL_TM_INFO, "HPCC_Conn:call to HPCCdb::getTableSchema('%s')\n", (_tableFilter));

    //Cache management
    CTableSchemaCache * pCache = CTableSchemaCache::queryInstance();
    static time_t m_lastCacheClear = 0;
    if ((msTick() - m_lastCacheClear) >= (queryCacheTimeout() * 60 * 1000))
    {
        pCache->clearCache();
        m_lastCacheClear = msTick();
    }
    StringBuffer sbTableFilter;
    if (_tableFilter && !*_tableFilter)
        _tableFilter = NULL;
    else
        sbTableFilter.set(_tableFilter);

    //Allow backend to handle wildcards
    bool bIsWildcard = false;
    for (char * p = (char *)sbTableFilter.str(); *p; p++)
    {
        if (*p == '*' || *p == '%')
        {
            if (!bIsWildcard)
            {
                bIsWildcard = true;
                pCache->clearCache();
                m_lastCacheClear = msTick();
            }
            if (*p == '%')
                *p = '*';//wssql only recognizes * as wildcard
        }
    }

    //First look in the schemaCache
    if (_tableFilter)
    {
        if (pCache->getMatchingTables(sbTableFilter.str(), _tables, true))
        {
            tm_trace(driver_tm_Hdle, UL_TM_INFO, "HPCC_Conn:using cached table schema info\n", ());
            return true;
        }
        else if (pCache->queryMetaDataCacheComplete())//we have everything, and it's not here
            return false;
    }

    if (!pCache->queryMetaDataCacheComplete())//
    {
        //Call out to ws_sql to get schema
        Owned<IClientGetDBMetaDataRequest> req;
        Owned<IClientGetDBMetaDataResponse> resp;

        req.setown( createClientGetDBMetaDataRequest());
        req->setIncludeTables(true);
        req->setTableFilter(sbTableFilter.str());
        req->setIncludeStoredProcedures(false);
        CriticalBlock b(crit);
        try
        {
            tm_trace(driver_tm_Hdle, UL_TM_INFO, "HPCC_Conn:calling ws_sql.GetDBMetaData()...\n", ());
            resp.setown(m_clientWs_sql->GetDBMetaData(req));//calls ws_sql
            tm_trace(driver_tm_Hdle, UL_TM_INFO, "HPCC_Conn:complete\n", ());
        }

        catch (IException* e)
        {
            StringBuffer sb;
            e->errorMessage(sb);

            hpccErrors.setf("HPCC_Conn:Error calling ws_sql : '%s'\n", (sb.str()));
            tm_trace(driver_tm_Hdle, UL_TM_ERRORS, "%s", (sb.str()));
            return false;
        }

        catch (...)
        {
            hpccErrors.setf("Exception thrown calling ws_sql");
            tm_trace(driver_tm_Hdle, UL_TM_ERRORS, "HPCC_Conn:Error calling ws_sql\n", ());
            return false;
        }

        StringBuffer sbErrors;
        if (checkForTopLevelErrors(resp->getExceptions(), sbErrors))
            return false;

        if (!_tableFilter)
        {
            pCache->clearCache();//empty cache
            pCache->setMetaDataCacheComplete(true);
            m_lastCacheClear = msTick();//reset cache expiration
        }

        //--------------------
        //Tables
        //--------------------
        IArrayOf<IConstHPCCTable> &tables = resp->getTables();
        ForEachItemIn(tableIdx, tables)//process all tables
        {
            //Add all found tables and their columns to the schemaCache
            CHPCCTable &tableItem = (CHPCCTable &)tables.item(tableIdx);
            tm_trace(driver_tm_Hdle, UL_TM_F_TRACE, "HPCC_Conn:Found table '%s'\n", (tableItem.getName()));
            Owned<CTable> tblEntry;
            tblEntry.setown(new CTable(tableItem.getName(), tableItem.getDescription(), tableItem.getOwner()));
#ifdef _WIN32
            OutputDebugString(tableItem.getName());OutputDebugString("\n");
#endif
            IArrayOf<IConstHPCCColumn> &columns = tableItem.getColumns();
            ForEachItemIn(columnsIdx, columns)//process all columns in given table
            {
                CHPCCColumn &columnItem = (CHPCCColumn &)columns.item(columnsIdx);
                Owned<CColumn> columnEntry;
                tm_trace(driver_tm_Hdle, UL_TM_F_TRACE, "   HPCC_Conn:Found column '%s'(%s)\n", (columnItem.getName(),columnItem.getType()));
                columnEntry.setown(new CColumn(columnItem.getName()));
#ifdef _WIN32
                StringBuffer sb;
                sb.appendf("\t%s (%s)\n",columnItem.getName(), columnItem.getType());
                OutputDebugString(sb.str());
#endif
                columnEntry->m_hpccType.set(columnItem.getType());

                //Add to column array
                tblEntry->addColumn(LINK(columnEntry));
            }
            pCache->addTable(tableItem.getName(), &tblEntry);//save in the schemaCache
        }
    }

    if (_tableFilter && !bIsWildcard)
    {
        if (pCache->getMatchingTables(sbTableFilter.str(), _tables, true))
            return true;
        else
            tm_trace(driver_tm_Hdle, UL_TM_ERRORS, "HPCC_Conn:Could not find table matching '%s'\n", (sbTableFilter.str()));
    }
    else
    {
        if (pCache->getAllCachedTables(_tables))
            return true;
        else
            tm_trace(driver_tm_Hdle, UL_TM_ERRORS, "HPCC_Conn:Could not find any tables\n", ());
    }

    return false;
}


/************************************************************************
Function:       HPCCdb::checkForTopLevelErrors
Description:
Return:         true :errors
                false: no errors
************************************************************************/
bool HPCCdb::checkForTopLevelErrors(const IMultiException & _exc, StringBuffer & _sbErrors)
{
    //Check for top level exceptions
    if (_exc.ordinality() > 0)
    {
        //log exceptions
        StringBuffer errxml;
        _exc.serialize(errxml, 0, true, false);
        Owned<IPropertyTree> exceptions;
        exceptions.setown(createPTreeFromXMLString(errxml));
        const char * msg = exceptions->queryProp("Message");
        _sbErrors.appendf("HPCC_Conn:WSSQL EXCEPTION:%s\n",msg);
        tm_trace(driver_tm_Hdle, UL_TM_ERRORS, (char*)_sbErrors.str(),());
        return true;//errors
    }
    return false;//no errors
}

/************************************************************************
Function:       HPCCdb::checkForErrors
Description:
Return:         true :errors
                false: no errors
************************************************************************/
bool HPCCdb::checkForErrors(const IMultiException & _exc, IConstECLWorkunit & _wu, StringBuffer & _sbErrors)
{
    if (checkForTopLevelErrors(_exc, _sbErrors))
        return true;//errors

    //Check for workunit errors
    IArrayOf<IConstECLException> &exceptions = _wu.getExceptions();
    ForEachItemIn(i, exceptions)
    {
        IConstECLException &e = exceptions.item(i);
        _sbErrors.appendf("\nHPCC_Conn: Workunit exception : %s - %s\n", _wu.getWuid(), e.getMessage());
        tm_trace(driver_tm_Hdle, UL_TM_ERRORS, (char*)_sbErrors.str(),());
    }
    if (!exceptions.empty())
        return true;//errors

    return false;//no errors
}

/************************************************************************
Function:       HPCCdb::executeSQL
Description:    Call esp "ws_sql" service to execute the given SQL "SELECT" or "CALL" statement.
Return:         true    on Success
                false   unsuccessful
************************************************************************/
bool HPCCdb::executeSQL(const char * sql, const char * targetQuerySet, StringBuffer & sbErrors)
{
    tm_trace(driver_tm_Hdle, UL_TM_INFO, "HPCC_Conn:call to HPCCdb::executeSQL with query '%s'\n", (sql));
    killResultsDatasets();

    Owned<IClientExecuteSQLRequest> req;
    Owned<IClientExecuteSQLResponse> resp;
    req.setown( createClientExecuteSQLRequest());
    req->setSqlText(sql);
    req->setSuppressXmlSchema(true);
    req->setResultWindowStart(0);
    if (m_maxFetchRowCount != -1)
        req->setResultWindowCount(m_maxFetchRowCount);

    if (targetQuerySet && *targetQuerySet)
        req->setTargetQuerySet(targetQuerySet);
    if (strnicmp(sql, "call", 4))
        req->setTargetCluster(m_targetCluster);//not allowed on CALL

    CriticalBlock b(crit);
    try
    {
        tm_trace(driver_tm_Hdle, UL_TM_INFO, "HPCC_Conn:calling ws_sql.ExecuteSQL('%s')...\n", (sql));
        resp.setown(m_clientWs_sql->ExecuteSQL(req));//calls ws_sql
        tm_trace(driver_tm_Hdle, UL_TM_INFO, "HPCC_Conn:complete\n", ());
    }

    catch (IException* e)
    {
        StringBuffer sb;
        e->errorMessage(sb);

        hpccErrors.setf("HPCC_Conn:Error calling ws_sql : '%s'\n", (sb.str()));
        tm_trace(driver_tm_Hdle, UL_TM_ERRORS, "%s", (sb.str()));
        return false;
    }

    catch (...)
    {
        sbErrors.append("Exception thrown calling ws_sql");
        tm_trace(driver_tm_Hdle, UL_TM_ERRORS, "HPCC_Conn:Error calling ws_sql.ExecuteSQL\n", ());
        return false;
    }

    if (checkForErrors(resp->getExceptions(), resp->getWorkunit(), sbErrors))
        return false;

    //-----------------
    //Process results
    //-----------------
    const char * resultXML = resp->getResult();

#ifdef _DEBUG
    DUMP(resultXML);
#endif
    Owned<IPropertyTree> resultsTree;
    {
        Owned<IPTreeMaker> maker = createRootLessPTreeMaker();
        resultsTree.setown(createPTreeFromXMLString(resultXML, ipt_none, (PTreeReaderOptions)(ptr_noRoot | ptr_ignoreWhiteSpace), maker));
    }
#ifdef _DEBUG
    DUMPPTREE(resultsTree);
#endif
    Owned<IPropertyTreeIterator> datasetIterator;
    datasetIterator.setown(resultsTree->getElements("Dataset"));
    ForEach(*datasetIterator)
    {
        IPropertyTree & dataSet = datasetIterator->query();//isolate dataset branch
        Owned<IAttributeIterator> attributes = dataSet.getAttributes();
        ForEach(*attributes)
        {
            if (strieq(attributes->queryName(),"@name"))//find dataset name attribute
            {
                Owned<CResultDataset> resultsDS;
                tm_trace(driver_tm_Hdle, UL_TM_F_TRACE, "HPCC_Conn:Found dataset '%s'\n", (attributes->queryValue()));
                resultsDS.setown(new CResultDataset(this, resp->getWorkunit().getWuid(), attributes->queryValue(), dataSet));
                LINK(resultsDS);
                m_ResultsDatasets.append(*resultsDS);
                break;
            }
        }
    }
    return true;
}

/************************************************************************
Function:       HPCCdb::getMoreResults()
Description:    Call esp "ws_sql" service to get a range of results
Return:         true    on Success
                false   unsuccessful
************************************************************************/
bool HPCCdb::getMoreResults(const char * _wuid, const char * _dsName, aindex_t _start, aindex_t _count, IPropertyTree * *_ppResultsTree, StringBuffer & _sbErrors)
{
    tm_trace(driver_tm_Hdle, UL_TM_INFO, "HPCC_Conn:call to HPCCdb::getMoreResults(%s,%s,from %ld,limit %ld)\n",(_wuid, _dsName, _start, _count));

    Owned<IClientGetResultsRequest> req;
    Owned<IClientGetResultsResponse> resp;
    req.setown( createClientGetResultsRequest());

    req->setResultWindowStart(_start);
    req->setResultWindowCount(_count);
    req->setSuppressXmlSchema(true);
    req->setWuId(_wuid);

    CriticalBlock b(crit);
    try
    {
        tm_trace(driver_tm_Hdle, UL_TM_INFO, "HPCC_Conn:calling ws_sql.GetResults('%s')...\n", (_wuid));
        resp.setown(m_clientWs_sql->GetResults(req));//calls ws_sql
        tm_trace(driver_tm_Hdle, UL_TM_INFO, "HPCC_Conn:complete\n", ());
    }

    catch (IException* e)
    {
        StringBuffer sb;
        e->errorMessage(sb);

        hpccErrors.setf("HPCC_Conn:Error calling ws_sql : '%s'\n", (sb.str()));
        tm_trace(driver_tm_Hdle, UL_TM_ERRORS, "%s", (sb.str()));
        return false;
    }

    catch (...)
    {
        _sbErrors.append("Exception thrown calling ws_sql");
        tm_trace(driver_tm_Hdle, UL_TM_ERRORS, "HPCC_Conn:Error calling ws_sql.GetResults\n", ());
        return false;
    }
    StringBuffer sbErrors;
    if (checkForErrors(resp->getExceptions(), resp->getWorkunit(), sbErrors))
        return false;

    //-----------------
    //Process results
    //-----------------
    const char * resultXML = resp->getResult();

#ifdef _DEBUG
    DUMP(resultXML);
#endif
    Owned<IPropertyTree> resultsTree;
    {
        Owned<IPTreeMaker> maker = createRootLessPTreeMaker();
        resultsTree.setown(createPTreeFromXMLString(resultXML, ipt_none, (PTreeReaderOptions)(ptr_noRoot | ptr_ignoreWhiteSpace), maker));
    }
#ifdef _DEBUG
    DUMPPTREE(resultsTree);
#endif
    Owned<IPropertyTreeIterator> datasetIterator;
    datasetIterator.setown(resultsTree->getElements("Dataset"));
    ForEach(*datasetIterator)
    {
        IPropertyTree & dataSet = datasetIterator->query();//isolate dataset branch
        Owned<IAttributeIterator> attributes = dataSet.getAttributes();
        ForEach(*attributes)
        {
            if (strieq(attributes->queryName(),"@name"))//find dataset name attribute
            {
                if (0 == strcmpi(_dsName,attributes->queryValue() ))
                {
                    tm_trace(driver_tm_Hdle, UL_TM_F_TRACE, "HPCC_Conn:Adding records from '%s' to '%s'\n", (_wuid, _dsName));
                    dataSet.Link();
                    *_ppResultsTree = &dataSet;
                    return true;
                }
            }
        }
    }
    return false;
}

/************************************************************************
Function:       HPCCdb::ReadFirstRow()
Description:    Get data for the first row.
                Values are not returned for null pointers
                Use with HPCCdb::ReadNextRow() to get all the rows

Return:         true    on Success
                false   if no data read ( end of database )
************************************************************************/
bool            HPCCdb::ReadFirstRow(IPropertyTree * * _row)
{
    if (0 == m_ResultsDatasets.ordinality())
        return false;
    m_pCurDataset = &m_ResultsDatasets.item(0);//BUGBUG Only supports 1 dataset at this time
    m_pCurDataset->moveFirst();
    tm_trace(driver_tm_Hdle, UL_TM_F_TRACE, "HPCC_Conn:Iterating rows for dataset '%s'\n", (m_pCurDataset->queryName()));
    return ReadNextRow(_row);
}

/************************************************************************
Function:       HPCCdb::ReadNextRow()
Description:    Get data for the next row.
                Values are not returned for null pointers
                Can only be called after calling HPCCdb::ReadFirstRow()

Return:         true    on Success
                false   if no data read ( end of database )
************************************************************************/
bool            HPCCdb::ReadNextRow(IPropertyTree * * _row)
{
    CriticalBlock b(crit);
    *_row = m_pCurDataset->queryNextRow();
    return *_row ? true : false;
}
