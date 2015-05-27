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

#include <vector>
#include "damip.h" //for tm_ and other portable functions

//The following include of platform.h is necessary because HPCC and Progress both have a file of the same name. By
//including it here, the HPCC one will always be available and any other #includes of platform.h will open the Progress one
//Note that the HPCC header directory ..\HPCC-Platform\system\include must be listed before the Progress header
//directory in the #include path
#include "..\HPCC-Platform\system\include\platform.h"
#include "ws_sql.hpp"

#ifdef _DEBUG
    #define DUMPPTREE(pt)   {StringBuffer sb;toXML(pt,sb);OutputDebugString("\n");OutputDebugString(sb.str());OutputDebugString("\n");}
    #define DUMP(s)         {OutputDebugString("\n");OutputDebugString(s);OutputDebugString("\n");}
#endif

//---------------------------------------------
//Represents a column in a Table, or any named data element.
//Contains the HPCC type descriptors retrieved from WSSQL "GetDBMetaData,"
//and will later be mapped to OpenAccess data types (as needed)
//---------------------------------------------
class CColumn : public CInterface, public IInterface
{
public:
    IMPLEMENT_IINTERFACE;
#define UNINITIALIZED  -100
    CColumn(const char * _name) : m_name(_name), m_iXOType(UNINITIALIZED) {}
    virtual ~CColumn()  {}

    //HPCC attributes, queried from wssql
    StringAttr      m_name;             // Column/dataElement name
    StringAttr      m_alias;            // sumout1, etc
    StringAttr      m_hpccType;         // HPCC ECL types ("INTEGER", "REAL", "DECIMAL", "STRING", etc)

    //Open Access attributes, populated on demand
    short           m_iXOType;          // OpenAccess data type  (OpenAccess SDK Prog Guide, chap 5, Specifying Data Types)
    StringAttr      m_type_name;        // The X/Open column type.  (OpenAccess SQL Engine Programmers Reference, dam_add_damobj_column method)
    unsigned long   m_char_max_length;  // Length in bytes of data transferred to the client in its default format
    unsigned long   m_numeric_precision;// Maximum number of digits used by the data in the column.
    DAM_HCOL        m_hcol;             // column handle
};

//---------------------------------------------
//Represents a database table, which is a named collection of ordered CColumn
//---------------------------------------------
class CTable : public CInterface, public IInterface
{
    StringAttr          m_name;
    StringAttr          m_description;
    IArrayOf<CColumn>   m_columns;
    StringAttr          m_owner;

public:
    IMPLEMENT_IINTERFACE;
    CTable(const char * _name, const char * _description, const char * _owner) : m_name(_name), m_description(_description), m_owner(_owner) {}
    virtual ~CTable() {}

    const char * queryName()                    { return m_name.get(); }
    const char * queryDescription()             { return m_description.get(); }
    const char * queryOwner()                   { return m_owner.get(); }

    void        addColumn(CColumn * _column)    { m_columns.append(*_column); }
    aindex_t    queryNumColumns()               { return m_columns.ordinality(); }
    CColumn *   queryColumn(aindex_t colNum)    { return (CColumn *)&m_columns.item(colNum); }//zero based column indices
};



//---------------------------------------------
//Represents an HPCC published/deployed query, known as a
//Stored Procedure in SQL. Queries are contained in HPCC QuerySets
//---------------------------------------------
class CMyQuerySetQuery : public CInterface, public IInterface
{
private:
    StringAttr          m_name;
    StringAttr          m_aliasName;
    IArrayOf<CColumn>   m_inputs;//ordered array of input arguments
    IArrayOf<CTable>    m_outputDatasets;//ordered array of named output datasets

public:
    IMPLEMENT_IINTERFACE;

    CMyQuerySetQuery(const char * _aliasName, const char * _qsqname)
    {
        m_aliasName.set(_aliasName);
        m_name.set(_qsqname);
    }

    virtual ~CMyQuerySetQuery() {}

    const char * queryName()            { return m_name; }
    const char * queryAlias()           { return m_aliasName; }
    //Inputs
    void addInput(CColumn * _input)     { m_inputs.append(*_input); }
    aindex_t queryNumInputs()           { return m_inputs.ordinality(); }
    CColumn * queryInput(aindex_t idx)  { return (CColumn*)&m_inputs.item(idx); }//zero based index

    //Outputs
    void addOutputDatasetColumn(const char * datasetName, CColumn * _output)
    {
        if (0 == m_outputDatasets.ordinality())
            m_outputDatasets.append(*(new CTable(datasetName,NULL,NULL)));//add new output dataset
        else
        {
            CTable & dataset = m_outputDatasets.item(m_outputDatasets.ordinality() - 1);
            if (0 != strcmp(datasetName, dataset.queryName()))
                m_outputDatasets.append(*(new CTable(datasetName,NULL,NULL)));//add new output dataset
        }
        CTable & dataset = m_outputDatasets.item(m_outputDatasets.ordinality() - 1);
        dataset.addColumn(_output);
    }

    aindex_t queryNumOutputDatasets()           { return m_outputDatasets.ordinality(); }
    CTable * queryOutputDataset(aindex_t idx)   { return (CTable *)&m_outputDatasets.item(idx); }
};


//---------------------------------------------
//Represents an HPCC QuerySet, which contains  zero or more CMyQuerySetQuery.
//---------------------------------------------
class CMyQuerySet : public CInterface, public IInterface
{
private:
    StringBuffer                m_name;//thor, hthor, roxie...
    IArrayOf<CMyQuerySetQuery>  m_queries;

public:
    IMPLEMENT_IINTERFACE;

    CMyQuerySet(const char * _name) { m_name.append(_name); }
    virtual ~CMyQuerySet() {}

    const char * queryName()                    { return m_name.str(); }

    void addQuery(CMyQuerySetQuery * _query)    { m_queries.append(*_query); }
    IArrayOf<CMyQuerySetQuery> * queryQueries() { return &m_queries; }
    CMyQuerySetQuery * queryQuery(const char * queryName)
    {
        ForEachItemIn(Idx, m_queries)
        {
            CMyQuerySetQuery &qryItem = (CMyQuerySetQuery &)m_queries.item(Idx);
            if (0 == strcmpi(qryItem.queryName(), queryName))
                return &qryItem;
        }
        return NULL;
    }
};

//---------------------------------------------
// HPCCdb Class
//---------------------------------------------
class CResultDataset;
class HPCCdb
{
private:
    //attributes
    CriticalSection             crit;
    StringAttr                  m_protocol;//http, https
    StringAttr                  m_wsSqlPort;
    StringAttr                  m_userName;
    StringAttr                  m_password;
    StringAttr                  m_wssqlIP;
    StringAttr                  m_targetCluster;//"thor", "hthor", etc
    StringAttr                  m_defaultQuerySet;//thor, roxie, thor_roxie, etc
    StringAttr                  m_FullHPCCVersion;
    aindex_t                    m_maxFetchRowCount;//max num rows to fetch in a single retrieval
    aindex_t                    m_cacheTimeout;//in minutes

    Owned<IClientwssql>         m_clientWs_sql;//ws_sql client

    vector<CColumn*>            m_hpccColPtrs;  //Ordered array of pointers to output CColumns that reside in m_schemaCache

    //Query results
    IArrayOf<CResultDataset>    m_ResultsDatasets;
    CResultDataset *            m_pCurDataset;

    //Stored procedures
    bool                        m_bGotDeployedQueries;
    IArrayOf<CMyQuerySet>       m_arrQuerySets;//QuerySets "thor", "hthor", "roxie", etc
    StringAttr                  m_currentQuerySet;

    //methods
    void        convertHPCCType(const char * hpccType, CColumn * pColumn);
    void        getDeployedQueries();//populates m_arrQuerySets
    void        killResultsDatasets()   { m_ResultsDatasets.kill(); }
    bool        checkForTopLevelErrors(const IMultiException & _exc, StringBuffer & _sbErrors);
    bool        checkForErrors(const IMultiException & _exc, IConstECLWorkunit & _wu, StringBuffer & _sbErrors);

public:
    HPCCdb(TM_ModuleCB tmHandle, const char * _protocol, const char * _wsSqlPort, const char * _user, const char * _pwd, const char * _wssqlIP, const char * _targetCluster, const char * _defaultQuerySet, aindex_t _maxFetchRowCount, aindex_t _cacheTimeout);
    virtual ~HPCCdb();

    const char * queryUserName()                            { return m_userName; }

    void        addOutputColumnDescriptor(CColumn * pCol)   { m_hpccColPtrs.push_back(pCol); }
    CColumn *   queryOutputColumnDescriptor(aindex_t idx)   { return m_hpccColPtrs.at(idx); }
    aindex_t    queryOutputColumnDescriptorCount()          { return m_hpccColPtrs.size(); }
    void        clearOutputColumnDescriptors()              { m_hpccColPtrs.clear(); }

    const char *queryTargetCluster()                        { return m_targetCluster; }
    const char *queryDefaultQuerySet()                      { return m_defaultQuerySet; }
    aindex_t    queryMaxFetchRowCount()                     { return m_maxFetchRowCount; }
    aindex_t    queryCacheTimeout()                         { return m_cacheTimeout; }

    const char *queryCurrentQuerySet()                      { return m_currentQuerySet.get(); }
    void        setCurrentQuerySet(const char * qs)         { m_currentQuerySet.set(qs); }



    //stored procedure support
    //An SQL stored procedure is loosly the same as an HPCC deployed query
    CMyQuerySetQuery * queryStoredProcedure(const char * querySetName, const char * name)
    {
        if ( querySetName && *querySetName && name && *name)
        {
            if (!m_bGotDeployedQueries)
                getDeployedQueries();//populate cache

            ForEachItemIn(Idx, m_arrQuerySets)//thor,hthor,roxie
            {
                CMyQuerySet &qrySet = (CMyQuerySet&)m_arrQuerySets.item(Idx);
                if (0 == strcmp(qrySet.queryName(), querySetName))
                {
                    CMyQuerySetQuery * pQuery = (CMyQuerySetQuery*)qrySet.queryQuery(name);
                    if (pQuery)
                        return pQuery;
                    else
                        break;
                }
            }
        }
        return NULL;
    }

    const IArrayOf<CMyQuerySet> * queryQuerySets()
    {
        if (!m_bGotDeployedQueries)
            getDeployedQueries();//populate cache
        return &m_arrQuerySets;
    }


    //Result row access
    bool        ReadFirstRow(IPropertyTree ** _row);
    bool        ReadNextRow(IPropertyTree ** _row);

    //ws_sql calls
    bool        getHPCCDBSystemInfo();
    bool        getTableSchema(const char * _tableFilter, IArrayOf<CTable> &_tables);
    const char *xformSQL(const char * pSQL, StringBuffer & xformedSQL);
    bool        executeSQL(const char * sql, const char * targetQuerySet, StringBuffer & sbErrors);
    bool        getMoreResults(const char * _wuid, const char * dsName, aindex_t _start, aindex_t _count, IPropertyTree ** _ppResultsTree, StringBuffer & _sbErrors);
    bool        executeStoredProcedure(const char * procName, const char * querySet);

    //attributes
    StringBuffer hpccErrors;
};

//------------------------------------------------
// Contains a property tree reference (returned from WSSQL) to an HPCC query result dataset
// Handles scrolling through blocks by calling WSSQL to get additional rows when current
// block is finished
//------------------------------------------------
class CResultDataset : public CInterface, public IInterface
{
    StringAttr                      m_name;
    StringAttr                      m_wuid;

    Owned<IPropertyTree>            m_DSresultsTree;
    Owned<IPropertyTreeIterator>    m_DSresultsTreeIterator;

    HPCCdb *                        m_pHPCCdb;
    aindex_t                        m_TotRecs;

public:
    IMPLEMENT_IINTERFACE;
    CResultDataset(HPCCdb * _pHPCCdb, const char * _wuid, const char * _name, IPropertyTree & _resultsTree)
        : m_pHPCCdb(_pHPCCdb), m_wuid(_wuid), m_name(_name), m_TotRecs(0)
    {
        m_DSresultsTree.setown(&_resultsTree);
        LINK(m_DSresultsTree);//@@
        m_DSresultsTreeIterator.setown(m_DSresultsTree->getElements("Row"));//allocate iterator
        moveFirst();
    }
    virtual ~CResultDataset()       {}

    const char *    queryName()     { return m_name.get(); }
    void            moveFirst()     { m_DSresultsTreeIterator->first(); }

    //retrieve pointer to next row in dataset (may have to call WSSQL to get more)
    IPropertyTree * queryNextRow()
    {
        if (!m_DSresultsTreeIterator->isValid())
        {
            if (!loadNextBlock() || !m_DSresultsTreeIterator->isValid())//call wssql to get more rows
                return NULL;
        }
        IPropertyTree *_row = &m_DSresultsTreeIterator->get();//iterator is already positioned at next row
        ++m_TotRecs;
        m_DSresultsTreeIterator->next();//position iterator for next element
        return _row;
    }

    //Call WSSQL to retrieve more results
    bool loadNextBlock()
    {
        if (m_pHPCCdb->queryMaxFetchRowCount() == -1)//no limit on initial fetch
            return false;
        m_DSresultsTree.clear();
        m_DSresultsTreeIterator.clear();

        StringBuffer sbErrors;
        IPropertyTree * resultsTree;
        if (!m_pHPCCdb->getMoreResults(m_wuid.get(), m_name.get(), m_TotRecs, m_pHPCCdb->queryMaxFetchRowCount(), &resultsTree, sbErrors))
            return false;

        m_DSresultsTree.setown(resultsTree);
        m_DSresultsTreeIterator.setown(m_DSresultsTree->getElements("Row"));//allocate iterator
        moveFirst();
        return true;
    }
};
