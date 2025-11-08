#pragma once
#include <Geode/GeneratedPredeclare.hpp>
#include <matjson.hpp>

struct SearchHistoryObject {
    int64_t time = 0;
    int type = 0;
    int index = 0;
    std::string query;
    std::vector<int> difficulties;
    std::vector<int> lengths;
    bool uncompleted = false;
    bool completed = false;
    bool featured = false;
    bool original = false;
    bool twoPlayer = false;
    bool coins = false;
    bool epic = false;
    bool legendary = false;
    bool mythic = false;
    bool song = false;
    bool customSong = false;
    int songID = 0;
    int demonFilter = 0;
    bool noStar = false;
    bool star = false;

    bool operator==(const SearchHistoryObject& other) const;
    bool contains(const SearchHistoryObject& other) const;
    bool empty() const;
};

class SearchHistory {
public:
    static std::vector<SearchHistoryObject> history;

    static void add(GJSearchObject*, std::vector<int>, std::vector<int>, int);
    static void remove(int);
    static void update();
};

template<>
struct matjson::Serialize<SearchHistoryObject> {
    static geode::Result<SearchHistoryObject> fromJson(const matjson::Value&);
    static matjson::Value toJson(const SearchHistoryObject&);
};
