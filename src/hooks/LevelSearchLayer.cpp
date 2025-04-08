#include "../classes/SearchHistoryPopup.hpp"
#include <Geode/binding/GameLevelManager.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>

using namespace geode::prelude;

class $modify(SHLevelSearchLayer, LevelSearchLayer) {
    static void onModify(ModifyBase<ModifyDerive<SHLevelSearchLayer, LevelSearchLayer>>& self) {
        auto mod = Mod::get();
        auto incognitoMode = mod->getSettingValue<bool>("incognito-mode");

        auto onSearchHook = self.getHook("LevelSearchLayer::onSearch").map([incognitoMode](Hook* hook) {
            return hook->setAutoEnable(!incognitoMode), hook;
        }).mapErr([](const std::string& err) {
            return log::error("Failed to find LevelSearchLayer::onSearch hook: {}", err), err;
        }).unwrapOr(nullptr);
        auto onSearchUserHook = self.getHook("LevelSearchLayer::onSearchUser").map([incognitoMode](Hook* hook) {
            return hook->setAutoEnable(!incognitoMode), hook;
        }).mapErr([](const std::string& err) {
            return log::error("Failed to find LevelSearchLayer::onSearchUser hook: {}", err), err;
        }).unwrapOr(nullptr);

        listenForSettingChangesV3<bool>("incognito-mode", [onSearchHook, onSearchUserHook](bool value) {
            if (onSearchHook) (void)(value ? onSearchHook->disable().mapErr([](const std::string& err) {
                return log::error("Failed to disable LevelSearchLayer::onSearch hook: {}", err), err;
            }) : onSearchHook->enable().mapErr([](const std::string& err) {
                return log::error("Failed to enable LevelSearchLayer::onSearch hook: {}", err), err;
            }));
            if (onSearchUserHook) (void)(value ? onSearchUserHook->disable().mapErr([](const std::string& err) {
                return log::error("Failed to disable LevelSearchLayer::onSearchUser hook: {}", err), err;
            }) : onSearchUserHook->enable().mapErr([](const std::string& err) {
                return log::error("Failed to enable LevelSearchLayer::onSearchUser hook: {}", err), err;
            }));
        }, mod);
    }

    bool init(int type) {
        if (!LevelSearchLayer::init(type)) return false;

        auto historyButtonSprite = CircleButtonSprite::createWithSprite("SH_historyBtn_001.png"_spr);
        historyButtonSprite->getTopNode()->setScale(1.0f);
        historyButtonSprite->setScale(0.8f);
        auto historyButton = CCMenuItemSpriteExtra::create(historyButtonSprite, this, menu_selector(SHLevelSearchLayer::onHistory));
        historyButton->setID("search-history-button"_spr);

        auto otherFilterMenu = getChildByID("other-filter-menu");
        otherFilterMenu->addChild(historyButton);
        otherFilterMenu->updateLayout();

        return true;
    }

    void onHistory(CCObject* sender) {
        SearchHistoryPopup::create([this](const SearchHistoryObject& object) {
            auto searchFilters = GameLevelManager::get()->m_searchFilters;

            if (object.type == 0) {
                searchFilters->setObject(CCString::create(object.uncompleted ? "1" : "0"), "uncompleted_filter");
                searchFilters->setObject(CCString::create(object.completed ? "1" : "0"), "completed_filter");
                searchFilters->setObject(CCString::create(object.original ? "1" : "0"), "original_filter");
                searchFilters->setObject(CCString::create(object.coins ? "1" : "0"), "coin_filter");
                searchFilters->setObject(CCString::create(object.twoPlayer ? "1" : "0"), "twoP_filter");
                searchFilters->setObject(CCString::create(object.song ? "1" : "0"), "enable_songFilter");
                searchFilters->setObject(CCString::create(object.noStar ? "1" : "0"), "nostar_filter");
                searchFilters->setObject(CCString::create(object.featured ? "1" : "0"), "featured_filter");
                searchFilters->setObject(CCString::create(object.epic ? "1" : "0"), "epic_filter");
                searchFilters->setObject(CCString::create(object.mythic ? "1" : "0"), "legendary_filter"); // Nice job RobTop
                searchFilters->setObject(CCString::create(object.legendary ? "1" : "0"), "mythic_filter"); // Nice job RobTop
                searchFilters->setObject(CCString::create(object.customSong ? "1" : "0"), "customsong_filter");
                searchFilters->setObject(CCString::create(std::to_string(object.songID)), "song_filter");
            }

            if (object.type == 0 || object.type == 1) {
                if (searchFilters->valueForKey("star_filter")->boolValue() != object.star) toggleStar(nullptr);

                for (int i = 0; i < 8; i++) {
                    toggleDifficultyNum(i, std::ranges::find(object.difficulties,
                        i == 0 ? -1 : i == 6 ? -2 : i == 7 ? -3 : i) != object.difficulties.end());

                    if (i != 6) continue;

                    auto demonToggled = m_difficultyDict->valueForKey("D6")->boolValue();
                    m_demonTypeButton->setEnabled(demonToggled);
                    m_demonTypeButton->setVisible(demonToggled);
                    if (auto dibFilter = getChildByIDRecursive("hiimjustin000.demons_in_between/quick-search-button"))
                        dibFilter->setVisible(demonToggled);
                }

                if (object.type == 0) for (int i = 0; i < 6; i++) {
                    toggleTimeNum(i, std::ranges::find(object.lengths, i) != object.lengths.end());
                }

                demonFilterSelectClosed(object.demonFilter);
            }

            m_searchInput->setString(object.query);
        })->show();
    }

    void onSearch(CCObject* sender) {
        LevelSearchLayer::onSearch(sender);
        SearchHistory::add(getSearchObject(SearchType::Search, m_searchInput->getString()), time(0), m_type);
    }

    void onSearchUser(CCObject* sender) {
        LevelSearchLayer::onSearchUser(sender);
        SearchHistory::add(getSearchObject(SearchType::Users, m_searchInput->getString()), time(0), 2);
    }
};
