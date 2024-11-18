#ifndef SIMUNPSDATADICTIONARYCONTENTLOADER_H
#define SIMUNPSDATADICTIONARYCONTENTLOADER_H

#include "SimuNPSDataDictionaryServer.h"
#include "server/DataDictionary/IDataDictionaryBuilder.h"
class SimuNPSDataDictionaryContentLoader : public IDataDictionaryContentLoader
{
public:
    SimuNPSDataDictionaryContentLoader(void);
    ~SimuNPSDataDictionaryContentLoader(void);

    // IDataDictionaryContentLoader
    virtual bool Load(const PIDataDictionaryBuilder &dictionaryBuilder, const DictionaryParamMap &dictionaryParam);

    virtual bool Save(const PIDataDictionaryBuilder &dictionaryBuilder, const DictionaryParamMap &dictionaryParam);

    virtual bool LoadSystemStorage(const PIDataDictionaryBuilder &dictionaryBuilder, PIElementStructBuilder &systemRoot)
    {
        return true;
    }

    virtual bool LoadUserStorage(const PIDataDictionaryBuilder &dictionaryBuilder, const DictionaryParamMap &paramMap,
                                 PIElementStructBuilder &userRoot)
    {
        return true;
    }

    virtual bool SaveUserStorage(const PIDataDictionaryBuilder &dictionaryBuilder, const DictionaryParamMap &paramMap,
                                 PIElementStructBuilder &userRoot)
    {
        return true;
    }

private:
    PIElementStructBuilder createBuilderElement(const PIDataDictionaryBuilder &dictionaryBuilder,
                                                const PIElementStructBuilder &parentStruct, std::string str);
    void createDoubleElement(const PIDataDictionaryBuilder &dictionaryBuilder,
                             const PIElementStructBuilder &parentStruct, std::string str);

private:
    // int _beginElementChannelId;
};

#endif
