#include "../classes/SearchHistoryPopup.hpp"
#include <Geode/binding/GameLevelManager.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <jasmine/hook.hpp>
#include <jasmine/setting.hpp>

using namespace geode::prelude;

class $modify(SHLevelSearchLayer, LevelSearchLayer) {
    static void onModify(ModifyBase<ModifyDerive<SHLevelSearchLayer, LevelSearchLayer>>& self) {
        auto incognitoMode = jasmine::setting::getValue<bool>("incognito-mode");
        auto onSearchHook = jasmine::hook::get(self.m_hooks, "LevelSearchLayer::onSearch", !incognitoMode);
        auto onSearchUserHook = jasmine::hook::get(self.m_hooks, "LevelSearchLayer::onSearchUser", !incognitoMode);

        SettingChangedEventV3(GEODE_MOD_ID, "incognito-mode").listen([onSearchHook, onSearchUserHook](std::shared_ptr<SettingV3> setting) {
            auto value = !std::static_pointer_cast<BoolSettingV3>(std::move(setting))->getValue();
            jasmine::hook::toggle(onSearchHook, value);
            jasmine::hook::toggle(onSearchUserHook, value);
        }).leak();
    }

    bool init(int type) {
        if (!LevelSearchLayer::init(type)) return false;

        if (auto otherFilterMenu = getChildByID("other-filter-menu")) {
            auto historyButtonSprite = CircleButtonSprite::createWithSprite("SH_historyBtn_001.png"_spr);
            historyButtonSprite->getTopNode()->setScale(1.0f);
            historyButtonSprite->setScale(0.8f);
            auto historyButton = CCMenuItemSpriteExtra::create(historyButtonSprite, this, menu_selector(SHLevelSearchLayer::onHistory));
            historyButton->setID("search-history-button"_spr);

            otherFilterMenu->addChild(historyButton);
            otherFilterMenu->updateLayout();
        }

        return true;
    }

    void onHistory(CCObject* sender) {
        SearchHistoryPopup::create([this](int index) {
            auto& object = SearchHistory::history[index];
            auto glm = GameLevelManager::get();

            if (object.type == 0) {
                glm->setBoolForKey(object.uncompleted, "uncompleted_filter");
                glm->setBoolForKey(object.completed, "completed_filter");
                glm->setBoolForKey(object.original, "original_filter");
                glm->setBoolForKey(object.coins, "coin_filter");
                glm->setBoolForKey(object.twoPlayer, "twoP_filter");
                glm->setBoolForKey(object.song, "enable_songFilter");
                glm->setBoolForKey(object.noStar, "nostar_filter");
                glm->setBoolForKey(object.featured, "featured_filter");
                glm->setBoolForKey(object.epic, "epic_filter");
                glm->setBoolForKey(object.mythic, "legendary_filter");
                glm->setBoolForKey(object.legendary, "mythic_filter");
                glm->setBoolForKey(object.customSong, "customsong_filter");
                glm->setIntForKey(object.songID, "song_filter");
            }

            if (object.type == 0 || object.type == 1) {
                if (glm->getBoolForKey("star_filter") != object.star) toggleStar(nullptr);

                for (auto diff : object.difficulties) {
                    if (diff > -4 && diff < 6 && diff != 0) {
                        toggleDifficultyNum(diff == -1 ? 0 : diff == -2 ? 6 : diff == -3 ? -1 : diff, true);
                    }
                }

                auto demonToggled = checkDiff(6);
                m_demonTypeButton->setEnabled(demonToggled);
                m_demonTypeButton->setVisible(demonToggled);
                if (auto dibFilter = getChildByIDRecursive("hiimjustin000.demons_in_between/quick-search-button")) {
                    dibFilter->setVisible(demonToggled);
                }

                if (object.type == 0) {
                    for (auto len : object.lengths) {
                        if (len > -1 && len < 6) toggleTimeNum(len, true);
                    }
                }

                demonFilterSelectClosed(object.demonFilter);
            }

            m_searchInput->setString(object.query);
        })->show();
    }

    void onSearch(CCObject* sender) {
        LevelSearchLayer::onSearch(sender);

        std::vector<int> difficulties;
        if (checkDiff(0)) difficulties.push_back(-1);
        else if (checkDiff(6)) difficulties.push_back(-2);
        else if (checkDiff(7)) difficulties.push_back(-3);
        else {
            for (int i = 1; i < 6; ++i) {
                if (checkDiff(i)) difficulties.push_back(i);
            }
        }

        std::vector<int> lengths;
        for (int i = 0; i < 6; ++i) {
            if (checkTime(i)) lengths.push_back(i);
        }

        SearchHistory::add(getSearchObject(SearchType::Search, m_searchInput->getString()), std::move(difficulties), std::move(lengths), m_type);
    }

    void onSearchUser(CCObject* sender) {
        LevelSearchLayer::onSearchUser(sender);

        SearchHistory::add(getSearchObject(SearchType::Users, m_searchInput->getString()), {}, {}, 2);
    }
};
