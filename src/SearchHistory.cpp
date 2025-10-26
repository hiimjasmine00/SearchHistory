#include "SearchHistory.hpp"
#include <Geode/binding/GJSearchObject.hpp>
#include <Geode/loader/Mod.hpp>

using namespace geode::prelude;

std::vector<SearchHistoryObject> SearchHistory::history;

$on_mod(Loaded) {
    for (auto& object : Mod::get()->getSavedValue<std::vector<SearchHistoryObject>>("search-history")) {
        if (!std::ranges::contains(SearchHistory::history, object)) {
            SearchHistory::history.push_back(std::move(object));
        }
    }
    SearchHistory::update();
}

$on_mod(DataSaved) {
    Mod::get()->setSavedValue("search-history", SearchHistory::history);
}

bool SearchHistoryObject::operator==(const SearchHistoryObject& other) const {
    return (time - time % 86400) == (other.time - other.time % 86400) &&
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

bool SearchHistoryObject::contains(const SearchHistoryObject& other) const {
    if (other.type > -1 && type != other.type) return false;
    if (!other.query.empty() && !string::toLower(query).contains(string::toLower(other.query))) return false;
    for (auto diff : other.difficulties) {
        if (!std::ranges::contains(difficulties, diff)) return false;
    }
    for (auto len : other.lengths) {
        if (!std::ranges::contains(lengths, len)) return false;
    }
    if (other.uncompleted && !uncompleted) return false;
    if (other.completed && !completed) return false;
    if (other.featured && !featured) return false;
    if (other.original && !original) return false;
    if (other.twoPlayer && !twoPlayer) return false;
    if (other.coins && !coins) return false;
    if (other.epic && !epic) return false;
    if (other.legendary && !legendary) return false;
    if (other.mythic && !mythic) return false;
    if (other.song) {
        if (!song) return false;
        if (customSong != other.customSong) return false;
        if (other.songID > 0 && songID != other.songID) return false;
    }
    if (other.demonFilter > 0 && demonFilter != other.demonFilter) return false;
    if (other.noStar && !noStar) return false;
    if (other.star && !star) return false;
    return true;
}

bool SearchHistoryObject::empty() const {
    return type < 0 &&
        query.empty() &&
        difficulties.empty() &&
        lengths.empty() &&
        !uncompleted &&
        !completed &&
        !featured &&
        !original &&
        !twoPlayer &&
        !coins &&
        !epic &&
        !legendary &&
        !mythic &&
        !song &&
        !noStar &&
        !star;
}

void SearchHistory::add(GJSearchObject* search, time_t time, int type) {
    std::vector<int> difficulties;
    if (search->m_difficulty != "-") {
        for (auto& diff : string::split(search->m_difficulty, ",")) {
            int num;
            if (auto res = std::from_chars(diff.data(), diff.data() + diff.size(), num); res.ec == std::errc()) {
                difficulties.push_back(num);
            }
        }
    }

    std::vector<int> lengths;
    if (search->m_length != "-") {
        for (auto& len : string::split(search->m_length, ",")) {
            int num;
            if (auto res = std::from_chars(len.data(), len.data() + len.size(), num); res.ec == std::errc()) {
                lengths.push_back(num);
            }
        }
    }

    SearchHistoryObject obj;
    obj.time = time;
    obj.type = type;
    obj.query = search->m_searchQuery;
    obj.difficulties = std::move(difficulties);
    obj.lengths = std::move(lengths);
    obj.uncompleted = search->m_uncompletedFilter;
    obj.completed = search->m_completedFilter;
    obj.featured = search->m_featuredFilter;
    obj.original = search->m_originalFilter;
    obj.twoPlayer = search->m_twoPlayerFilter;
    obj.coins = search->m_coinsFilter;
    obj.epic = search->m_epicFilter;
    obj.legendary = search->m_legendaryFilter;
    obj.mythic = search->m_mythicFilter;
    obj.song = search->m_songFilter;
    obj.customSong = search->m_customSongFilter;
    obj.songID = search->m_songID;
    obj.demonFilter = (int)search->m_demonFilter;
    obj.noStar = search->m_noStarFilter;
    obj.star = search->m_starFilter;

    auto found = std::ranges::find_if(history, [&obj](const SearchHistoryObject& o) {
        return obj == o;
    });
    if (found != history.end()) {
        history.erase(found);
    }

    history.insert(history.begin(), std::move(obj));
    update();
}

void SearchHistory::remove(int index) {
    auto it = history.begin() + index;
    if (it < history.end()) {
        history.erase(it);
        update();
    }
}

void SearchHistory::update() {
    for (auto it = history.begin(); it != history.end(); ++it) {
        it->index = it - history.begin();
    }
}

Result<SearchHistoryObject> matjson::Serialize<SearchHistoryObject>::fromJson(const matjson::Value& value) {
    if (!value.isObject()) return Err("Expected object");

    SearchHistoryObject obj;
    if (auto time = value.get<int64_t>("time").ok()) obj.time = *time;
    if (auto type = value.get<int>("type").ok()) obj.type = *type;
    if (auto query = value.get<std::string>("query").ok()) obj.query = *std::move(query);
    if (auto difficulties = value.get<std::vector<int>>("difficulties").ok()) obj.difficulties = *std::move(difficulties);
    if (auto lengths = value.get<std::vector<int>>("lengths").ok()) obj.lengths = *std::move(lengths);
    if (auto uncompleted = value.get<bool>("uncompleted").ok()) obj.uncompleted = *uncompleted;
    if (auto completed = value.get<bool>("completed").ok()) obj.completed = *completed;
    if (auto featured = value.get<bool>("featured").ok()) obj.featured = *featured;
    if (auto original = value.get<bool>("original").ok()) obj.original = *original;
    if (auto twoPlayer = value.get<bool>("two-player").ok()) obj.twoPlayer = *twoPlayer;
    if (auto coins = value.get<bool>("coins").ok()) obj.coins = *coins;
    if (auto epic = value.get<bool>("epic").ok()) obj.epic = *epic;
    if (auto legendary = value.get<bool>("legendary").ok()) obj.legendary = *legendary;
    if (auto mythic = value.get<bool>("mythic").ok()) obj.mythic = *mythic;
    if (auto song = value.get<bool>("song").ok()) obj.song = *song;
    if (auto customSong = value.get<bool>("custom-song").ok()) obj.customSong = *customSong;
    if (auto songID = value.get<int>("song-id").ok()) obj.songID = *songID;
    if (auto demonFilter = value.get<int>("demon-filter").ok()) obj.demonFilter = *demonFilter;
    if (auto noStar = value.get<bool>("no-star").ok()) obj.noStar = *noStar;
    if (auto star = value.get<bool>("star").ok()) obj.star = *star;
    return Ok(std::move(obj));
}

matjson::Value matjson::Serialize<SearchHistoryObject>::toJson(const SearchHistoryObject& obj) {
    matjson::Value historyObject;

    historyObject.set("time", obj.time);
    historyObject.set("type", obj.type);
    if (!obj.query.empty()) historyObject.set("query", obj.query);
    if (!obj.difficulties.empty()) historyObject.set("difficulties", obj.difficulties);
    if (!obj.lengths.empty()) historyObject.set("lengths", obj.lengths);
    if (obj.uncompleted) historyObject.set("uncompleted", obj.uncompleted);
    if (obj.completed) historyObject.set("completed", obj.completed);
    if (obj.featured) historyObject.set("featured", obj.featured);
    if (obj.original) historyObject.set("original", obj.original);
    if (obj.twoPlayer) historyObject.set("two-player", obj.twoPlayer);
    if (obj.coins) historyObject.set("coins", obj.coins);
    if (obj.epic) historyObject.set("epic", obj.epic);
    if (obj.legendary) historyObject.set("legendary", obj.legendary);
    if (obj.mythic) historyObject.set("mythic", obj.mythic);
    if (obj.song) {
        historyObject.set("song", obj.song);
        if (obj.customSong) historyObject.set("custom-song", obj.customSong);
        historyObject.set("song-id", obj.songID);
    }
    if (obj.demonFilter != 0) historyObject.set("demon-filter", obj.demonFilter);
    if (obj.noStar) historyObject.set("no-star", obj.noStar);
    if (obj.star) historyObject.set("star", obj.star);

    return historyObject;
}
