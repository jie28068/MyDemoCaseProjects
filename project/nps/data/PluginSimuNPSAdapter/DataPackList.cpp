#include "DataPackList.h"

#include "server/DataDictionary/IElementBase.h"

DataPackList::DataPackList() : m_pDataBuff(NULL) { }

DataPackList::~DataPackList()
{
    m_pDataBuff = NULL;
}

unsigned char *DataPackList::GetItemDataBuff(unsigned int index)
{
    if (index >= m_itemCount)
        return NULL;
    return m_pDataBuff + index * m_ItemBuffSize;
}

DataInfo DataPackList::GetDataTimestamp(unsigned int index)
{
    if (index >= m_itemCount)
        return DataInfo();

    DataInfo dataInfo = m_firstDataTimeStamp;
    dataInfo.number += index * m_stepCount;
    return dataInfo;
}

bool DataPackList::SetVariableBase(PIVariableBase varible)
{
    m_variableBase = varible;
    m_varType = (VariableType)(m_variableBase->Element()->Attribute("VariantType").toUInt());
    return true;
}

bool DataPackList::GetValueList(QList<NumberUnit> &valueList)
{
    if (m_variableBase->ElementType() == ElementType_Number) {
        for (int i = 0; i < m_itemCount; ++i) {
            NumberUnit nu;
            if (getValue(GetItemDataBuff(i), GetItemBuffSize(), nu))
                ;
            valueList.push_back(nu);
        }
        return true;
    }
    return false;
}

void DataPackList::attachBuff(unsigned char *pBuff, unsigned int buffLen, unsigned int itemCount,
                              double firstDataTimeStamp, unsigned int stepCount)
{
    if (!pBuff)
        return;

    m_pDataBuff = pBuff;
    m_buffLength = buffLen;
    m_itemCount = itemCount;
    m_firstDataTimeStamp = DataInfo();
    m_firstDataTimeStamp.number = firstDataTimeStamp;
    m_stepCount = stepCount;
    m_ItemBuffSize = buffLen / itemCount;
}

//-----
bool DataPackList::getValue(unsigned char *pData, unsigned int len, NumberUnit &unit)
{
    if (!pData || !len) {
        return false;
    }

    unsigned int nStartByte = 0;
    switch (m_varType) {
    case DVariable_Double:
        unit = *(double *)(pData + nStartByte);
        return true;
    default:
        break;
    }
    return false;
}
