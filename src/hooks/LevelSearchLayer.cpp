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
            return hook->setAutoEnable(incognitoMode), hook;
        }).mapErr([](const std::string& err) {
            return log::error("Failed to find LevelSearchLayer::onSearch hook: {}", err), err;
        }).unwrapOr(nullptr);
        auto onSearchUserHook = self.getHook("LevelSearchLayer::onSearchUser").map([incognitoMode](Hook* hook) {
            return hook->setAutoEnable(incognitoMode), hook;
        }).mapErr([](const std::string& err) {
            return log::error("Failed to find LevelSearchLayer::onSearchUser hook: {}", err), err;
        }).unwrapOr(nullptr);

        listenForSettingChangesV3<bool>("incognito-mode", [onSearchHook, onSearchUserHook](bool value) {
            if (onSearchHook) (void)(value ? onSearchHook->enable().mapErr([](const std::string& err) {
                return log::error("Failed to enable LevelSearchLayer::onSearch hook: {}", err), err;
            }) : onSearchHook->disable().mapErr([](const std::string& err) {
                return log::error("Failed to disable LevelSearchLayer::onSearch hook: {}", err), err;
            }));
            if (onSearchUserHook) (void)(value ? onSearchUserHook->enable().mapErr([](const std::string& err) {
                return log::error("Failed to enable LevelSearchLayer::onSearchUser hook: {}", err), err;
            }) : onSearchUserHook->disable().mapErr([](const std::string& err) {
                return log::error("Failed to disable LevelSearchLayer::onSearchUser hook: {}", err), err;
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
                glm->setBoolForKey(object.mythic, "legendary_filter"); // Mythic and legendary are swapped, nice job RobTop
                glm->setBoolForKey(object.legendary, "mythic_filter");
                glm->setBoolForKey(object.customSong, "customsong_filter");
                glm->setIntForKey(object.songID, "song_filter");
            }

            if (object.type == 0 || object.type == 1) {
                if (glm->getBoolForKey("star_filter") != object.star) toggleStar(nullptr);
                for (int i = 0; i < 8; i++) {
                    auto diff = i;
                    switch (i) {
                        case 0: diff = -1; break;
                        case 6: diff = -2; break;
                        case 7: diff = -3; break;
                    }
                    toggleDifficultyNum(i, std::find(object.difficulties.begin(), object.difficulties.end(), diff) != object.difficulties.end());
                    if (i != 6) continue;

                    auto demonToggled = m_difficultyDict->valueForKey(getDiffKey(i))->boolValue();
                    m_demonTypeButton->setEnabled(demonToggled);
                    m_demonTypeButton->setVisible(demonToggled);
                    if (auto dibFilter = getChildByIDRecursive("hiimjustin000.demons_in_between/quick-search-button")) dibFilter->setVisible(demonToggled);
                }
                if (object.type == 0) for (int i = 0; i < 6; i++) {
                    toggleTimeNum(i, std::find(object.lengths.begin(), object.lengths.end(), i) != object.lengths.end());
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
