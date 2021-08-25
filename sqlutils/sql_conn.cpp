/*************************************************************************
    > File Name: sql_conn.cpp
    > Author: hsz
    > Mail:
    > Created Time: Fri 20 Aug 2021 09:12:37 AM CST
 ************************************************************************/

#include "sql_conn.h"
#include <log/log.h>
#include <utils/Errors.h>
#include <utils/utils.h>

#define LOG_TAG __FILE__
static const char *INSERT_SQL_FMT = "insert into %s value(%s);";
static const char *SELECT_SQL_FMT = "select %s from %s where(%s);";
static const char *UPDATE_SQL_FMT = "update %s set %s where(%s);";
static const char *DELETE_SQL_FMT = "delete from %s where(%s)";

#define SQL_BUF_LEN (1024U)

namespace Alias
{
MySqlConn::MySqlConn() :
    mSqlHandle(nullptr),
    mSqlInit(false)
{
    mSqlHandle = mysql_init(mSqlHandle);
}

MySqlConn::MySqlConn(const char *sqlUser, const char *passwd, const char *db,
            const char *ip, uint16_t port) :
    mSqlHandle(nullptr),
    mSqlInit(false)
{
    mSqlHandle = mysql_init(mSqlHandle);
    if (mSqlHandle == nullptr) {
        LOGE("mysql_init error: maybe not enough memory");
        return;
    }
    if (sqlUser == nullptr || passwd == nullptr || db == nullptr) {
        return;
    }
    MYSQL *pSql = mysql_real_connect(mSqlHandle, ip, sqlUser, passwd, db, port, nullptr, 0);
    if (pSql == nullptr) {
        LOGE("mysql_real_connect error: %s", mysql_error(mSqlHandle));
        return;
    }
    mSqlInit = true;
}

MySqlConn::~MySqlConn()
{
    mysql_close(mSqlHandle);
}

/**
 * @description: 
 * @param sqlUser {const char *}
 * @param passwd  {const char *}
 * @param db      {const char *}
 * @param ip      {const char *}
 * @param port    {uint16_t} 
 * @return {enum}
 */
int MySqlConn::ConnectSql(const char *sqlUser, const char *passwd, const char *db,
                        const char *ip, uint16_t port)
{
    if (sqlUser == nullptr || passwd == nullptr || db == nullptr) {
        return INVALID_PARAM;
    }
    if (mSqlInit) {
        return INVALID_OPERATION;
    }
    size_t index = 0;
    if (mSqlHandle == nullptr) {    // 在构造函数已执行过一次，如果那时分配内存失败，则在此处需再次执行
        mSqlHandle = mysql_init(mSqlHandle);
        if (mSqlHandle == nullptr) {
            return NO_MEMORY;
        }
    }
    MYSQL *pSql = mysql_real_connect(mSqlHandle, ip, sqlUser, passwd, db, port, nullptr, 0);
    if (pSql == nullptr) {
        LOGE("mysql_real_connect error: %s", mysql_error(mSqlHandle));
        return UNKNOWN_ERROR;
    }
    mSqlInit = true;
    return OK;
}

/**
 * @description: 
 * @param table {const char *} 数据库表名
 * @param value {const char *} 字段名，为空则是表中所有字段
 * @param cond  {const char *} 查询条件，where 后面的值
 * @return {int}
 */
int  MySqlConn::SelectSql(const char *table, const char *value, const char *cond)
{
    if (table == nullptr) {
        return INVALID_PARAM;
    }
    if (mSqlInit == false) {
        return NO_INIT;
    }

    // len 比实际长度大6个字节
    uint32_t len = strlen(SELECT_SQL_FMT) + strlen(table) + 
        (value ? strlen(value) : 1) + (cond ? strlen(cond) : 4);
    LOGD("sql len = %u\n", len);
    if (len > SQL_BUF_LEN) {
        return NO_MEMORY;
    }
    static char buf[SQL_BUF_LEN] = {0};
    memset(buf, 0, SQL_BUF_LEN);
    snprintf(buf, SQL_BUF_LEN - 1, SELECT_SQL_FMT, (value ? value : "*"), table, (cond ? cond : "true"));
    LOGD("%s() buf = %s\n", __func__, buf);
    if (mysql_query(mSqlHandle, buf)) {
        LOGE("mysql query error. errno: %u, errmsg: %s\n",
            mysql_errno(mSqlHandle), mysql_error(mSqlHandle));
        return UNKNOWN_ERROR;
    }

    return OK;
}

/**
 * @description: 
 * @param table {const char *} 要插入的表名，允许包含字段
 * @param value {const char *} 要插入的值，如果表名不包含字段，则需补充所有的值
 * @return {int}
 */
int  MySqlConn::InsertSql(const char *table, const char *value)
{
    if (!table || !value) {
        return INVALID_PARAM;
    }
    uint32_t len = strlen(INSERT_SQL_FMT) + strlen(table) + 
        (value ? strlen(value) : 1);
    LOGD("sql len = %u\n", len);
    if (len >= SQL_BUF_LEN) {
        return NO_MEMORY;
    }
    static char buf[SQL_BUF_LEN] = {0};
    memset(buf, 0, SQL_BUF_LEN);
    snprintf(buf, SQL_BUF_LEN - 1, INSERT_SQL_FMT, table, value);
    LOGD("%s() buf = %s\n", __func__, buf);
    int nRetCode = mysql_query(mSqlHandle, buf);
    if (nRetCode) {
        LOGE("%s() in mysql_query error. [errno: %u, errmsg: %s]",
            __func__, mysql_errno(mSqlHandle), mysql_error(mSqlHandle));
        return UNKNOWN_ERROR;
    }
    return OK;
}

int  MySqlConn::UpdateSql(const char *table, const char *value, const char *cond)
{
    if (table == nullptr || value == nullptr || cond == nullptr) {
        return INVALID_PARAM;
    }
    uint32_t len = strlen(UPDATE_SQL_FMT) + strlen(table) +
        strlen(value) + strlen(cond);
    LOGD("sql len = %u\n", len);
    if (len >= SQL_BUF_LEN) {
        return NO_MEMORY;
    }
    static char buf[SQL_BUF_LEN] = {0};
    memset(buf, 0, SQL_BUF_LEN);
    snprintf(buf, SQL_BUF_LEN - 1, UPDATE_SQL_FMT, table, value, cond);
    LOGD("%s() buf = %s\n", __func__, buf);
    int nRetCode = mysql_query(mSqlHandle, buf);
    if (nRetCode) {
        LOGE("%s() mysql_query error. [errno: %u, errmsg: %s]",
            __func__, mysql_errno(mSqlHandle), mysql_error(mSqlHandle));
        return UNKNOWN_ERROR;
    }
    return OK;
}

int  MySqlConn::DeleteSql(const char *table, const char *cond)
{
    if (table == nullptr || cond == nullptr) {
        return INVALID_PARAM;
    }
    uint32_t len = strlen(DELETE_SQL_FMT) + strlen(table) + strlen(cond);
    LOGD("sql len = %u\n", len);
    if (len >= SQL_BUF_LEN) {
        return NO_MEMORY;
    }
    static char buf[SQL_BUF_LEN];
    memset(buf, 0, SQL_BUF_LEN);
    snprintf(buf, SQL_BUF_LEN - 1, DELETE_SQL_FMT, table, cond);
    LOGD("%s() buf = %s\n", __func__, buf);
    if (mysql_query(mSqlHandle, buf)) {
        LOGE("%s() mysql_query error. [errno: %u, errmsg: %s]",
            __func__, mysql_errno(mSqlHandle), mysql_error(mSqlHandle));
        return UNKNOWN_ERROR;
    }
    return OK;
}

/**
 * @description: 非select状态下获取受影响的行数
 * @param void
 * @return int64_t {< 0 error; > 0 no error}
 */
int64_t MySqlConn::getAffectedRows() const
{
    if (mysql_field_count(mSqlHandle) == 0) {
        return mysql_affected_rows(mSqlHandle);
    } else {
        LOGE("mysql_store_result() should have returned data, but occer errors");
        return UNKNOWN_ERROR;
    }
}

SqlResBase::sp MySqlConn::getSqlRes()
{
    MYSQL_RES *sqlRes = mysql_store_result(mSqlHandle);
    return SqlResBase::sp(new MySqlRes(sqlRes, mysql_errno(mSqlHandle), mysql_error(mSqlHandle)));
}

void MySqlConn::CloseConn()
{
    mysql_close(mSqlHandle);
    mSqlHandle = nullptr;
    mSqlInit = false;
}

MySqlRes::MySqlRes(MYSQL_RES* res, int eno, const char* estr) :
    mErrno(eno),
    mErrStr(estr),
    mRow(nullptr),
    mRowLength(nullptr),
    mIsVaild(false)
{
    if(res) {
        mIsVaild = true;
        mResData.reset(res, mysql_free_result);
    }
}

bool MySqlRes::next()
{
    mRow = mysql_fetch_row(mResData.get());
    mRowLength = mysql_fetch_lengths(mResData.get());
    return !!mRow;
}

int MySqlRes::getDataCount()
{
    return mysql_num_rows(mResData.get());
}

int MySqlRes::getColumnCount()
{
    return mysql_num_fields(mResData.get());
}

int MySqlRes::getColumnBytes(int idx)
{
    return mRowLength[idx];
}

int MySqlRes::getColumnType(int idx)
{
    return 0;
}

std::string MySqlRes::getColumnName(int idx)
{
    return "";
}

bool MySqlRes::isValid(int idx)
{
    if (mRow[idx] != nullptr) {
        return true;
    }
    return false;
}

int8_t MySqlRes::getInt8(int idx)
{
    return getInt64(idx);
}
uint8_t MySqlRes::getUint8(int idx)
{
    return getInt64(idx);
}
int16_t MySqlRes::getInt16(int idx)
{
    return getInt64(idx);
}
uint16_t MySqlRes::getUint16(int idx)
{
    return getInt64(idx);
}
int32_t MySqlRes::getInt32(int idx)
{
    return getInt64(idx);
}
uint32_t MySqlRes::getUint32(int idx)
{
    return getInt64(idx);
}
int64_t MySqlRes::getInt64(int idx)
{
    return TypeUtil::Atoi(mRow[idx]);
}
uint64_t MySqlRes::getUint64(int idx)
{
    return getInt64(idx);
}
float MySqlRes::getFloat(int idx)
{
    return getDouble(idx);
}
double MySqlRes::getDouble(int idx)
{
    return TypeUtil::Atof(mRow[idx]);
}
std::string MySqlRes::getString(int idx)
{
    return mRow[idx];
}
time_t MySqlRes::getTime(int idx)
{
    if(!mRow[idx]) {
        return 0;
    }
    return Str2Time(mRow[idx]);
}

} // namespace Alias
