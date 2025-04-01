#include "SearchHistory.hpp"
#include <Geode/binding/GJSearchObject.hpp>
#include <Geode/loader/Mod.hpp>
#include <Geode/utils/ranges.hpp>

using namespace geode::prelude;

void SearchHistory::add(GJSearchObject* search, time_t time, int type) {
    auto history = Mod::get()->getSavedValue<std::vector<SearchHistoryObject>>("search-history");

    auto difficulties = ranges::reduce<std::vector<int>>(
        search->m_difficulty != "-" ? string::split(search->m_difficulty, ",") : std::vector<std::string>(),
        [](std::vector<int>& vec, const std::string& str) {
            if (auto num = numFromString<int>(str)) vec.push_back(num.unwrap());
        });

    auto lengths = ranges::reduce<std::vector<int>>(
        search->m_length != "-" ? string::split(search->m_length, ",") : std::vector<std::string>(),
        [](std::vector<int>& vec, const std::string& str) {
            if (auto num = numFromString<int>(str)) vec.push_back(num.unwrap());
        });

    SearchHistoryObject obj = {
        .time = time,
        .type = type,
        .query = search->m_searchQuery,
        .difficulties = difficulties,
        .lengths = lengths,
        .uncompleted = search->m_uncompletedFilter,
        .completed = search->m_completedFilter,
        .featured = search->m_featuredFilter,
        .original = search->m_originalFilter,
        .twoPlayer = search->m_twoPlayerFilter,
        .coins = search->m_coinsFilter,
        .epic = search->m_epicFilter,
        .legendary = search->m_legendaryFilter,
        .mythic = search->m_mythicFilter,
        .song = search->m_songFilter,
        .customSong = search->m_customSongFilter,
        .songID = search->m_songID,
        .demonFilter = (int)search->m_demonFilter,
        .noStar = search->m_noStarFilter,
        .star = search->m_starFilter
    };

    auto found = std::ranges::find_if(history, [&obj](const SearchHistoryObject& o) { return obj == o; });

    if (found != history.end()) history.erase(found);

    history.insert(history.begin(), obj);

    Mod::get()->setSavedValue("search-history", history);
}

void SearchHistory::clear() {
    Mod::get()->setSavedValue<std::vector<SearchHistoryObject>>("search-history", {});
}

std::vector<SearchHistoryObject> SearchHistory::get() {
    return Mod::get()->getSavedValue<std::vector<SearchHistoryObject>>("search-history");
}

void SearchHistory::remove(int index) {
    auto history = Mod::get()->getSavedValue<std::vector<SearchHistoryObject>>("search-history");
    history.erase(history.begin() + index);
    Mod::get()->setSavedValue("search-history", history);
}

Result<std::vector<SearchHistoryObject>> matjson::Serialize<std::vector<SearchHistoryObject>>::fromJson(const matjson::Value& value) {
    if (!value.isArray()) return Err("Expected array");

    return Ok(ranges::reduce<std::vector<SearchHistoryObject>>(value.asArray().unwrap(),
        [](std::vector<SearchHistoryObject>& vec, const matjson::Value& elem) {
            SearchHistoryObject obj = {
                .time = (int64_t)elem["time"].asInt().unwrapOr(0),
                .type = (int)elem["type"].asInt().unwrapOr(0),
                .query = elem["query"].asString().unwrapOr(""),
                .difficulties = ranges::map<std::vector<int>>(
                    elem["difficulties"].isArray() ? elem["difficulties"].asArray().unwrap() : std::vector<matjson::Value>(),
                    [](const matjson::Value& e) { return e.asInt().unwrapOr(0); }),
                .lengths = ranges::map<std::vector<int>>(
                    elem["lengths"].isArray() ? elem["lengths"].asArray().unwrap() : std::vector<matjson::Value>(),
                    [](const matjson::Value& e) { return e.asInt().unwrapOr(0); }),
                .uncompleted = elem["uncompleted"].asBool().unwrapOr(false),
                .completed = elem["completed"].asBool().unwrapOr(false),
                .featured = elem["featured"].asBool().unwrapOr(false),
                .original = elem["original"].asBool().unwrapOr(false),
                .twoPlayer = elem["two-player"].asBool().unwrapOr(false),
                .coins = elem["coins"].asBool().unwrapOr(false),
                .epic = elem["epic"].asBool().unwrapOr(false),
                .legendary = elem["legendary"].asBool().unwrapOr(false),
                .mythic = elem["mythic"].asBool().unwrapOr(false),
                .song = elem["song"].asBool().unwrapOr(false),
                .customSong = elem["custom-song"].asBool().unwrapOr(false),
                .songID = (int)elem["song-id"].asInt().unwrapOr(0),
                .demonFilter = (int)elem["demon-filter"].asInt().unwrapOr(0),
                .noStar = elem["no-star"].asBool().unwrapOr(false),
                .star = elem["star"].asBool().unwrapOr(false)
            };

            if (!std::ranges::any_of(vec, [&obj](const SearchHistoryObject& o) { return obj == o; })) vec.push_back(obj);
        }));
}

matjson::Value matjson::Serialize<std::vector<SearchHistoryObject>>::toJson(const std::vector<SearchHistoryObject>& vec) {
    return ranges::map<std::vector<matjson::Value>>(vec, [](const SearchHistoryObject& obj) {
        matjson::Value historyObject;
        historyObject["time"] = obj.time;
        historyObject["type"] = obj.type;
        if (!obj.query.empty()) historyObject["query"] = obj.query;
        if (!obj.difficulties.empty()) historyObject["difficulties"] = obj.difficulties;
        if (!obj.lengths.empty()) historyObject["lengths"] = obj.lengths;
        if (obj.uncompleted) historyObject["uncompleted"] = obj.uncompleted;
        if (obj.completed) historyObject["completed"] = obj.completed;
        if (obj.featured) historyObject["featured"] = obj.featured;
        if (obj.original) historyObject["original"] = obj.original;
        if (obj.twoPlayer) historyObject["two-player"] = obj.twoPlayer;
        if (obj.coins) historyObject["coins"] = obj.coins;
        if (obj.epic) historyObject["epic"] = obj.epic;
        if (obj.legendary) historyObject["legendary"] = obj.legendary;
        if (obj.mythic) historyObject["mythic"] = obj.mythic;
        if (obj.song) {
            historyObject["song"] = obj.song;
            if (obj.customSong) historyObject["custom-song"] = obj.customSong;
            historyObject["song-id"] = obj.songID;
        }
        if (obj.demonFilter != 0) historyObject["demon-filter"] = obj.demonFilter;
        if (obj.noStar) historyObject["no-star"] = obj.noStar;
        if (obj.star) historyObject["star"] = obj.star;

        return historyObject;
    });
}
