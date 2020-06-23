#ifndef CAISHEN_MCCFR_MEM_INFORMATIONSET_STORE_H_
#define CAISHEN_MCCFR_MEM_INFORMATIONSET_STORE_H_

#include <map>
#include <list>
#include <fstream>
#include <experimental/filesystem>

#include "cfr_doudizhu_player.h"
#include "bucket_actions_nohistory_nopublic.h"


namespace caishen
{
namespace cfr
{

class LRU_MemoryInformationSetCache
{
public:
    static const std::string INDEX_FILE_NAME;
    static const std::string DATA_FILE_NAME;

public:
    static std::shared_ptr<InformationSet> loadInformationSetFromFile(std::fstream & fs, unsigned long long filePos);
    static void appendInformationSetToFile(std::fstream & fs, const InformationSet & is);

public:
    LRU_MemoryInformationSetCache(unsigned int cacheSize = 100000);
    ~LRU_MemoryInformationSetCache();

public:
    void clear();

    int getKeySize() { return _indexes.size(); }
    int getDataFileSize() { return std::experimental::filesystem::file_size(_dataFilePath); }

    void setupWorkingDir(const std::string &dir);
    bool loadIndexFile();
    bool loadDataFile();
    bool flushIndexFile();
    bool flushCacheDataToFile();
    
    bool contains(const BANHNPInformationSetKey & key) { return _indexes.find(key) != _indexes.end(); }
    void getInformationSet(const BANHNPInformationSetKey & key, InformationSet & is);
    void putInformationSet(const BANHNPInformationSetKey & key, const InformationSet & is);

    void touchCachedInformationSet(std::list<BANHNPInformationSet>::iterator & iter);
    std::list<BANHNPInformationSet>::iterator putInformationSetToCache(const BANHNPInformationSet & banhnpIs);

public:
    unsigned int _cacheSize;
    std::list<BANHNPInformationSet> _isCache;
    std::map<BANHNPInformationSetKeyWrapper, std::pair<unsigned long long, std::list<BANHNPInformationSet>::iterator>> _indexes;

    std::string _dataFilePath;
    std::string _indexFilePath;
    std::fstream _dataFileStream;
    unsigned long long _lastInformationSetPos;
};

class MemoryInformationSetStore
{
public:
    static const int ABSTRACT_ACTION_SIZE = 309;
    
public:
    static MemoryInformationSetStore * getInstance() {
        return &_instance;
    }

    static std::shared_ptr<BANHNPInformationSetKey> getInformationSetKey(const caishen::doudizhu::Game & game);

    int getKeySize() { return _lruCache.getKeySize(); }
    int getDataFileSize() { return _lruCache.getDataFileSize(); }

    bool isRunning() const;
    bool start(const std::string & path);
    bool stop();
    bool clean(const std::string & path);

    bool contains(const BANHNPInformationSetKey & key) { return _lruCache.contains(key); }
    void getInformationSet(const BANHNPInformationSetKey & key, InformationSet & is);
    void putInformationSet(const BANHNPInformationSetKey & key, const InformationSet & is);

private:
    MemoryInformationSetStore() :_isServiceRunning(false) {}
    ~MemoryInformationSetStore() {}
    MemoryInformationSetStore(const MemoryInformationSetStore&);
    MemoryInformationSetStore& operator=(const MemoryInformationSetStore&);

private:
    bool _isServiceRunning;

    LRU_MemoryInformationSetCache _lruCache;

private:
    static MemoryInformationSetStore _instance;
};

} // namespace cfr
} // namespace caishen

#endif