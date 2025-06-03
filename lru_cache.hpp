#pragma once
#include <unordered_map>
#include <list>
#include <string>

template <typename Key, typename Value>
class LRUCache {
public:
    LRUCache(size_t maxSize) : maxSize_(maxSize) {}

    bool get(const Key& key, Value& value) {
        auto it = cacheItemsMap_.find(key);
        if (it == cacheItemsMap_.end())
            return false;
        cacheItemsList_.splice(cacheItemsList_.begin(), cacheItemsList_, it->second);
        value = it->second->second;
        return true;
    }

    void put(const Key& key, const Value& value) {
        auto it = cacheItemsMap_.find(key);
        if (it != cacheItemsMap_.end()) {
            cacheItemsList_.erase(it->second);
            cacheItemsMap_.erase(it);
        }
        cacheItemsList_.emplace_front(key, value);
        cacheItemsMap_[key] = cacheItemsList_.begin();

        if (cacheItemsMap_.size() > maxSize_) {
            auto last = cacheItemsList_.end();
            --last;
            cacheItemsMap_.erase(last->first);
            cacheItemsList_.pop_back();
        }
    }

private:
    std::list<std::pair<Key, Value>> cacheItemsList_;
    std::unordered_map<Key, decltype(cacheItemsList_.begin())> cacheItemsMap_;
    size_t maxSize_;
};

