#pragma once
#include <Geode/GeneratedPredeclare.hpp>
#include <matjson.hpp>

struct SearchHistoryObject {
    int64_t time;
    int type;
    std::string query;
    std::vector<int> difficulties;
    std::vector<int> lengths;
    bool uncompleted;
    bool completed;
    bool featured;
    bool original;
    bool twoPlayer;
    bool coins;
    bool epic;
    bool legendary;
    bool mythic;
    bool song;
    bool customSong;
    int songID;
    int demonFilter;
    bool noStar;
    bool star;

    bool operator==(const SearchHistoryObject& other) const {
        return floor(time / 86400.0) == floor(other.time / 86400.0) &&
            type == other.type &&
            query == other.query &&
            difficulties == other.difficulties &&
            lengths == other.lengths &&
            uncompleted == other.uncompleted &&
            completed == other.completed &&
            featured == other.featured &&
            original == other.original &&
            twoPlayer == other.twoPlayer &&
            coins == other.coins &&
            epic == other.epic &&
            legendary == other.legendary &&
            mythic == other.mythic &&
            song == other.song &&
            (!song || (customSong == other.customSong && songID == other.songID)) &&
            demonFilter == other.demonFilter &&
            noStar == other.noStar &&
            star == other.star;
    }
};

class SearchHistory {
public:
    static void add(GJSearchObject*, time_t, int);
    static void clear();
    static std::vector<SearchHistoryObject> get();
    static void remove(int);
};

template<>
struct matjson::Serialize<std::vector<SearchHistoryObject>> {
    static geode::Result<std::vector<SearchHistoryObject>> fromJson(const matjson::Value&);
    static matjson::Value toJson(const std::vector<SearchHistoryObject>&);
};
