#ifndef DATAPACKLIST_H
#define DATAPACKLIST_H

#include "server/DataDictionary/IVariableBase.h"

using namespace Kcc::DataDictionary;

class DataPackList : public IDataPackList
{
public:
    DataPackList();
    ~DataPackList();

    // IDataPackList
    virtual unsigned char *GetAllDataBuff() override { return m_pDataBuff; }
    virtual unsigned int GetAllDataBuffLength() override { return m_buffLength; }
    virtual unsigned int GetItemCount() override { return m_itemCount; }
    virtual unsigned int GetItemBuffSize() { return m_ItemBuffSize; }
    virtual unsigned char *GetItemDataBuff(unsigned int index) override;
    virtual DataInfo GetDataTimestamp(unsigned int index = 0) override;
    virtual unsigned int GetDataSampleRate() override { return m_stepCount; }
    virtual bool SetVariableBase(PIVariableBase varible) override;
    virtual bool GetValueList(QList<NumberUnit> &valueList) override;
    virtual bool GetArrayValueList(QList<QList<NumberUnit>> &valueArraryList) override { return false; }
    virtual bool GetArrayIndex(int &index) override { return true; }
    virtual bool GetBytePos(unsigned int &bytePos) override
    {
        bytePos = 0;
        return true;
    }

    void attachBuff(unsigned char *pBuff, unsigned int buffLen, unsigned int itemCount, double firstDataTimeStamp,
                    unsigned int stepCount);

    bool getValue(unsigned char *pData, unsigned int len, NumberUnit &unit);

private:
    unsigned char *m_pDataBuff;
    unsigned int m_buffLength;
    unsigned int m_itemCount;
    unsigned int m_ItemBuffSize;

    DataInfo m_firstDataTimeStamp;
    unsigned int m_stepCount;

    PIVariableBase m_variableBase;
    VariableType m_varType;
};

#endif
