#include <iostream>
#include <fstream>
#include <bitset>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "mem_informationset_store.h"
#include "bucket_actions_nohistory_nopublic.h"

using namespace caishen::cfr;

using namespace std;

MemoryInformationSetStore MemoryInformationSetStore::_instance;

const std::string LRU_MemoryInformationSetCache::INDEX_FILE_NAME = "banhnp_informationset_store.index";
const std::string LRU_MemoryInformationSetCache::DATA_FILE_NAME = "banhnp_informationset_store.data";

LRU_MemoryInformationSetCache::LRU_MemoryInformationSetCache(unsigned int cacheSize)
    :_cacheSize(cacheSize)
{}

LRU_MemoryInformationSetCache::~LRU_MemoryInformationSetCache()
{}

void LRU_MemoryInformationSetCache::clear()
{
    _isCache.clear();
    _indexes.clear();
    _lastInformationSetPos = 0;
    _dataFilePath = "";
    _indexFilePath = "";
    _dataFileStream.close();
}

void LRU_MemoryInformationSetCache::setupWorkingDir(const std::string &dir)
{
    _dataFilePath = dir + "/" + DATA_FILE_NAME;
    _indexFilePath = dir + "/" + INDEX_FILE_NAME;
}

bool LRU_MemoryInformationSetCache::loadIndexFile()
{
    ifstream ifIndex;
    ifIndex.open(_indexFilePath, ios::in | ios::binary);
    if (!ifIndex.is_open())
    {
        fstream fIndex;
        fIndex.open(_indexFilePath, ios::out | ios::trunc | ios::binary);
        if (!fIndex.is_open())
        {
            return false;
        }
        fIndex.close();
    }
    else
    {
        ifIndex.seekg(0, ios::beg);
        while(ifIndex.peek() != EOF)
        {
            unsigned long long pos;
            BANHNPInformationSetKey::KeyDataType keyData;
            ifIndex.read(reinterpret_cast<char*>(keyData.data()), keyData.size() * sizeof(decltype(keyData[0])));
            ifIndex.read(reinterpret_cast<char*>(&pos), sizeof(decltype(pos)));

            _indexes.insert(std::make_pair(
                    BANHNPInformationSetKeyWrapper(keyData), 
                    std::make_pair(pos, _isCache.end()))
                );
        }
        ifIndex.close();
    }

    return true;
}

bool LRU_MemoryInformationSetCache::loadDataFile()
{
    _dataFileStream.open(_dataFilePath, ios::in | ios::out | ios::app | ios::binary);
    if (!_dataFileStream.is_open())
    {
        return false;
    }
    _lastInformationSetPos = _dataFileStream.tellp();

    return true;
}

void LRU_MemoryInformationSetCache::touchCachedInformationSet(std::list<BANHNPInformationSet>::iterator & iter)
{
    _isCache.splice(_isCache.begin(), _isCache, iter);
}

std::list<BANHNPInformationSet>::iterator LRU_MemoryInformationSetCache::putInformationSetToCache(const BANHNPInformationSet & banhnpIs)
{
    if (_isCache.size() >= _cacheSize)
    {
        auto & item = _isCache.back();
        // write item back to file
        const auto & iter = _indexes.find(item);
        if (iter == _indexes.end())
        {
            assert(false);
        }
        else
        {
            iter->second = std::make_pair(_lastInformationSetPos, _isCache.end());
            appendInformationSetToFile(_dataFileStream, *item._pIs);
            _lastInformationSetPos = _dataFileStream.tellp();
        }

        // free memory of InformationSet
        item._pIs = nullptr;

        item = banhnpIs;
        // move it to front
        _isCache.splice(_isCache.begin(), _isCache, --_isCache.end());
    }
    else
    {
        _isCache.push_front(banhnpIs);
    }
    
    return _isCache.begin();
}

constexpr int BIT_ARRAY_SIZE = (MemoryInformationSetStore::ABSTRACT_ACTION_SIZE / 8) + ((MemoryInformationSetStore::ABSTRACT_ACTION_SIZE % 8) ? 1 : 0);
std::shared_ptr<InformationSet> LRU_MemoryInformationSetCache::loadInformationSetFromFile(fstream & fs, unsigned long long filePos)
{
    fs.seekg(filePos);
    std::shared_ptr<InformationSet> pis(new InformationSet(MemoryInformationSetStore::ABSTRACT_ACTION_SIZE));

    std::array<char, BIT_ARRAY_SIZE> ba({0});
    fs.read(ba.data(), BIT_ARRAY_SIZE);

    for (int i = 0; i < MemoryInformationSetStore::ABSTRACT_ACTION_SIZE; ++i)
    {
        pis->legalActionMask[i] = ((ba[i/8] & (1 << i % 8)) != 0);
    }
    pis->uniformDistConstant = 1.0 / (pis->legalActionMask == true).count();

    fs.read(reinterpret_cast<char*>(pis->cumulativeRegrets.data()), MemoryInformationSetStore::ABSTRACT_ACTION_SIZE * sizeof(double));
    fs.read(reinterpret_cast<char*>(pis->cumulativePolicy.data()), MemoryInformationSetStore::ABSTRACT_ACTION_SIZE * sizeof(double));

    return pis;
}

void LRU_MemoryInformationSetCache::appendInformationSetToFile(fstream & fs, const InformationSet & is)
{
    std::array<char, BIT_ARRAY_SIZE> ba({0});
    for (int i = 0; i < MemoryInformationSetStore::ABSTRACT_ACTION_SIZE; ++i)
    {
        if (is.legalActionMask[i]) {
            ba[i/8] |= (1 << i % 8);
        }
    }
    fs.write(ba.data(), BIT_ARRAY_SIZE);
    fs.write(reinterpret_cast<const char*>(is.cumulativeRegrets.data()), MemoryInformationSetStore::ABSTRACT_ACTION_SIZE * sizeof(double));
    fs.write(reinterpret_cast<const char*>(is.cumulativePolicy.data()), MemoryInformationSetStore::ABSTRACT_ACTION_SIZE * sizeof(double));
}

bool LRU_MemoryInformationSetCache::flushCacheDataToFile()
{
    for (auto item = _isCache.begin(); item != _isCache.end(); ++item)
    {
        const auto & iter = _indexes.find(*item);
        if (iter == _indexes.end())
        {
            assert(false);
        }
        else
        {
            iter->second = std::make_pair(_lastInformationSetPos, _isCache.end());
            appendInformationSetToFile(_dataFileStream, *(*item)._pIs);
            _lastInformationSetPos = _dataFileStream.tellp();
        }
    }
    _isCache.clear();

    return true;
}

bool LRU_MemoryInformationSetCache::flushIndexFile()
{
    ofstream ofIndex;
    ofIndex.open(_indexFilePath, ios::out | ios::trunc | ios::binary);
    if (!ofIndex.is_open())
    {
        spdlog::error("Open Index file " + _indexFilePath + " failed!" );
        return false;
    }
    else
    {
        for (auto iter = _indexes.begin(); iter != _indexes.end(); ++iter)
        {
            const auto & data = iter->first._pKey->data();
            ofIndex.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(decltype(data[0])));
            ofIndex.write(reinterpret_cast<const char*>(&(iter->second.first)), sizeof(decltype(iter->second.first)));
        }
    }
    ofIndex.clear();
    
    return true;
}

void LRU_MemoryInformationSetCache::getInformationSet(const BANHNPInformationSetKey & key, InformationSet & is)
{
    // keyWrapper object is used to look for key in tree
    BANHNPInformationSetKeyWrapper keyWrapper(key);
    const auto & iter = _indexes.find(keyWrapper);
    if (iter == _indexes.end())
    {
        std::shared_ptr<InformationSet> pis(new InformationSet(MemoryInformationSetStore::ABSTRACT_ACTION_SIZE));
        pis->cumulativeRegrets.setZero();
        pis->cumulativePolicy.setZero();
        pis->applyRegretMatching();

        BANHNPInformationSet banhnpIs(keyWrapper, pis);
        const auto & iter = putInformationSetToCache(banhnpIs);

        _indexes.insert(std::make_pair(
                    banhnpIs, 
                    std::make_pair(0, iter))
                );

        is = *pis;
    }
    else
    {
        std::list<BANHNPInformationSet>::iterator & cacheIter = iter->second.second;
        if (cacheIter != _isCache.end())
        {
            is = *(cacheIter->_pIs);
            touchCachedInformationSet(cacheIter);
        }
        else
        {
            std::shared_ptr<InformationSet> pis = loadInformationSetFromFile(_dataFileStream, iter->second.first);
            pis->applyRegretMatching();

            // keyWrapper and *iter->first has the same keyData but in different memory address.
            // Should use *iter->first here and keyWrapper will be freed at then end of this function
            BANHNPInformationSet banhnpIs(iter->first, pis);
            iter->second = make_pair(0,  putInformationSetToCache(banhnpIs));
            is = *pis;
        }
    }
}

void LRU_MemoryInformationSetCache::putInformationSet(const BANHNPInformationSetKey & key, const InformationSet & is)
{
    // keyWrapper object is used to look for key in tree
    BANHNPInformationSetKeyWrapper keyWrapper(key);
    const auto & iter = _indexes.find(keyWrapper);
    if (iter == _indexes.end())
    {
        std::shared_ptr<InformationSet> pis(new InformationSet(is));

        BANHNPInformationSet banhnpIs(keyWrapper, pis);
        const auto & i = putInformationSetToCache(banhnpIs);
        _indexes.insert(std::make_pair(
                    banhnpIs, 
                    std::make_pair(0, i))
                );
    }
    else
    {
        std::list<BANHNPInformationSet>::iterator & cacheIter = iter->second.second;
        if (cacheIter != _isCache.end())
        {
            *(cacheIter->_pIs) = is;
            touchCachedInformationSet(cacheIter);
        }
        else
        {
            std::shared_ptr<InformationSet> pis(new InformationSet(is));

            // keyWrapper and *iter->first has the same keyData but in different memory address.
            // Should use *iter->first here and keyWrapper will be freed at then end of this function
            BANHNPInformationSet banhnpIs(iter->first, pis);
            iter->second = make_pair(0, putInformationSetToCache(banhnpIs));
        }
    }
}

bool MemoryInformationSetStore::isRunning() const
{
    return _isServiceRunning;
}

std::shared_ptr<BANHNPInformationSetKey> MemoryInformationSetStore::getInformationSetKey(const caishen::doudizhu::Game & game)
{
    return std::shared_ptr<BANHNPInformationSetKey>(new BANHNPInformationSetKey(game));
}

bool MemoryInformationSetStore::start(const std::string & path)
{
    if (isRunning())
    {
        spdlog::info("Memory InformationSet Store Service already running");
        return false;
    }

    spdlog::info("Starting Memory InformationSet Store Service ...");
    _lruCache.setupWorkingDir(path);

    if (!_lruCache.loadIndexFile())
    {
        spdlog::error("Failed loading index file!");
        spdlog::error("Starting Memory InformationSet Store Service Failed!");
        return false;
    }

    if (!_lruCache.loadDataFile())
    {
        spdlog::error("Failed loading data file!");
        spdlog::error("Starting Memory InformationSet Store Service Failed!");
        return false;
    }
    
    spdlog::info("Memory InformationSet Store Service started");
    _isServiceRunning = true;
    return true;
}

bool MemoryInformationSetStore::stop()
{
    if (!isRunning())
    {
        spdlog::info("Memory InformationSet Store Service is not running");
        return true;
    }

    spdlog::info("Stopping Memory InformationSet Store Service ...");

    if (!_lruCache.flushCacheDataToFile())
    {
        spdlog::error("Memory InformationSet Store Service flushing data file to disk failed!");
        spdlog::error("Stopping Memory InformationSet Store Service Failed!");
        return false;
    }

    if (!_lruCache.flushIndexFile())
    {
        spdlog::error("Memory InformationSet Store Service flushing index file to disk failed!");
        spdlog::error("Stopping Memory InformationSet Store Service Failed!");
        return false;
    }

    _lruCache.clear();
    _isServiceRunning = false;

    spdlog::info("Memory InformationSet Store Service stopped");
    return true;
}

bool MemoryInformationSetStore::clean(const std::string & path)
{
    spdlog::info("Cleanning Memory InformationSet Store data file ...");
    if (isRunning())
    {
        spdlog::error("Memory InformationSet Store service is running, CAN NOT clean!" );
        return false;
    }

    _lruCache.clear();
    _lruCache.setupWorkingDir(path);

    spdlog::info("Loading index file into memory ..." );
    if (!_lruCache.loadIndexFile())
    {
        spdlog::error("Failed loading index file!");
        spdlog::error("Clean Memory InformationSet Store data file failed!" );
        return false;
    }    
    spdlog::info("Loading index file into memory done" );

    fstream oldDataFileStream;
    oldDataFileStream.open(_lruCache._dataFilePath, ios::in | ios::binary);
    if (!oldDataFileStream.is_open())
    {
        spdlog::error("Open old data file " + _lruCache._dataFilePath + " failed!" );
        spdlog::error("Clean Memory InformationSet Store data file failed!" );
        _lruCache.clear();
        return false;
    }

    string newDataFilePath = _lruCache._dataFilePath + ".new";
    fstream newDataFileStream;
    newDataFileStream.open(newDataFilePath, ios::out | ios::trunc | ios::binary);
    if (!newDataFileStream.is_open())
    {
        spdlog::error("Open new data file " + newDataFilePath + " failed!" );
        spdlog::error("Clean Memory InformationSet Store data file failed!" );
        _lruCache.clear();
        return false;
    }

    for (auto iter = _lruCache._indexes.begin(); iter != _lruCache._indexes.end(); ++iter)
    {
        auto pis = _lruCache.loadInformationSetFromFile(oldDataFileStream, iter->second.first);
        iter->second.first = newDataFileStream.tellp();
        _lruCache.appendInformationSetToFile(newDataFileStream, *pis);
    }
    oldDataFileStream.close();
    newDataFileStream.close();
    _lruCache.flushIndexFile();

    if (remove(_lruCache._dataFilePath.c_str()) != 0)
    {
        spdlog::error("Remove old data file " + _lruCache._dataFilePath + " failed!" );
        spdlog::error("Clean Memory InformationSet Store data file failed!" );
        _lruCache.clear();
        return false;
    }
    if (rename(newDataFilePath.c_str(), _lruCache._dataFilePath.c_str()) != 0)
    {
        spdlog::error("Rename new data file " + newDataFilePath + " to " + _lruCache._dataFilePath + " failed!" );
        spdlog::error("Clean Memory InformationSet Store data file failed!" );
        _lruCache.clear();
        return false;
    }

    _lruCache.clear();
    spdlog::info("Cleanning Memory InformationSet Store data file done");
    return true;
}

void MemoryInformationSetStore::getInformationSet(const BANHNPInformationSetKey & key, InformationSet & is)
{
    _lruCache.getInformationSet(key, is);
}

void MemoryInformationSetStore::putInformationSet(const BANHNPInformationSetKey & key, const InformationSet & is)
{
    _lruCache.putInformationSet(key, is);
}

