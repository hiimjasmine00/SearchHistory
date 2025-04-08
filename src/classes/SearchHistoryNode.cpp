#include "SearchHistoryNode.hpp"
#include <Geode/binding/LevelTools.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/utils/cocos.hpp>

using namespace geode::prelude;

SearchHistoryNode* SearchHistoryNode::create(
    const SearchHistoryObject& object, int index, int count, SearchCallback search, RemoveCallback remove, bool h12, bool white, bool dark
) {
    auto ret = new SearchHistoryNode();
    if (ret->init(object, index, count, std::move(search), std::move(remove), h12, white, dark)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool SearchHistoryNode::init(
    const SearchHistoryObject& object, int index, int count, SearchCallback search, RemoveCallback remove, bool h12, bool white, bool dark
) {
    if (!CCNode::init()) return false;

    setID("SearchHistoryNode");
    setContentSize({ 400.0f, 50.0f });

    m_index = index;
    m_count = count;

    auto background = CCScale9Sprite::create(
        index % 10 == 0 ? GEODE_MOD_ID "/square-top.png" :
        index % 10 == 9 || index == count - 1 ? GEODE_MOD_ID "/square-bottom.png" :
        GEODE_MOD_ID "/square-middle.png",
        { 0, 0, 80, 80 }
    );
    if (dark) background->setColor(index % 2 == 0 ? ccColor3B { 48, 48, 48 } : ccColor3B { 80, 80, 80 });
    else background->setColor(index % 2 == 0 ? ccColor3B { 161, 88, 44 } : ccColor3B { 194, 114, 62 });
    background->setContentSize({ 400.0f, 50.0f });
    background->setPosition({ 200.0f, 25.0f });
    background->setID("background");
    addChild(background);

    auto queryLabel = CCLabelBMFont::create(object.query.empty() ? "(No Query)" : object.query.c_str(), "bigFont.fnt");
    queryLabel->setColor(object.query.empty() ? ccColor3B { 127, 127, 127 } : ccColor3B { 255, 255, 255 });
    queryLabel->setScale(0.375f);
    queryLabel->setAnchorPoint({ 0.0f, 0.5f });
    queryLabel->setPositionX(50.0f);
    queryLabel->limitLabelWidth(225.0f, 0.375f, 0.1f);
    queryLabel->setID("query-label");
    addChild(queryLabel);

    auto type = object.type;
    switch (type) {
        case 0: {
            queryLabel->setPositionY(42.5f);
            auto levelSprite = CCSprite::createWithSpriteFrameName("GJ_viewLevelsBtn_001.png");
            levelSprite->setScale(0.875f);
            levelSprite->setPosition({ 25.0f, 25.0f });
            levelSprite->setID("search-sprite");
            addChild(levelSprite);
            break;
        }
        case 1: {
            queryLabel->setPositionY(32.5f);
            auto listSprite = CCSprite::createWithSpriteFrameName("GJ_viewListsBtn_001.png");
            listSprite->setScale(0.875f);
            listSprite->setPosition({ 25.0f, 25.0f });
            listSprite->setID("search-sprite");
            addChild(listSprite);
            break;
        }
        case 2: {
            queryLabel->setPositionY(25.0f);
            auto userSprite = CCSprite::createWithSpriteFrameName("GJ_profileButton_001.png");
            userSprite->setScale(0.725f);
            userSprite->setPosition({ 25.0f, 25.0f });
            userSprite->setID("search-sprite");
            addChild(userSprite);
            break;
        }
    }

    auto buttonMenu = CCMenu::create();
    buttonMenu->setContentSize({ 400.0f, 50.0f });
    buttonMenu->setPosition({ 0.0f, 0.0f });
    buttonMenu->setID("button-menu");
    addChild(buttonMenu);

    auto removeButton = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_deleteBtn_001.png", 0.5f, [this, remove = std::move(remove)](auto) {
        createQuickPopup("Remove Search", "Are you sure you want to remove this search history entry?", "No", "Yes",
            [this, remove = std::move(remove)](auto, bool btn2) {
                if (btn2) remove(m_index);
            });
    });
    removeButton->setPosition({ 380.0f, 25.0f });
    removeButton->setID("remove-button");
    buttonMenu->addChild(removeButton);

    auto searchButton = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_undoBtn_001.png", 0.6f, [this, object, search = std::move(search)](auto) {
        search(object);
    });
    searchButton->setPosition({ 350.0f, 25.0f });
    searchButton->setID("search-button");
    buttonMenu->addChild(searchButton);

    if (type < 2) {
        if (type < 1) {
            auto filtersNode = CCNode::create();
            filtersNode->setAnchorPoint({ 0.0f, 0.5f });
            filtersNode->setContentSize({ 250.0f, 8.0f });
            filtersNode->setPosition({ 50.0f, 30.0f });
            filtersNode->setLayout(RowLayout::create()->setGap(1.5f)->setAxisAlignment(AxisAlignment::Start)->setAutoScale(false));
            filtersNode->setID("filters-node");
            addChild(filtersNode);

            if (object.uncompleted) {
                auto uncompletedSprite = CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png");
                uncompletedSprite->setID("uncompleted-sprite");
                filtersNode->addChild(uncompletedSprite);
            }

            if (object.completed) {
                auto completedSprite = CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png");
                completedSprite->setID("completed-sprite");
                filtersNode->addChild(completedSprite);
            }

            if (object.featured) {
                auto featuredSprite = CCSprite::createWithSpriteFrameName(GEODE_MOD_ID "/SH_featuredCoin_001.png");
                featuredSprite->setID("featured-sprite");
                filtersNode->addChild(featuredSprite);
            }

            if (object.original) {
                auto originalSprite = CCSprite::createWithSpriteFrameName(GEODE_MOD_ID "/SH_originalIcon_001.png");
                originalSprite->setID("original-sprite");
                filtersNode->addChild(originalSprite);
            }

            if (object.twoPlayer) {
                auto twoPlayerSprite = CCSprite::createWithSpriteFrameName(GEODE_MOD_ID "/SH_twoPlayerBtn_001.png");
                twoPlayerSprite->setID("two-player-sprite");
                filtersNode->addChild(twoPlayerSprite);
            }

            if (object.coins) {
                auto coinsSprite = CCSprite::createWithSpriteFrameName("GJ_coinsIcon2_001.png");
                coinsSprite->setID("coins-sprite");
                filtersNode->addChild(coinsSprite);
            }

            if (object.epic) {
                auto epicSprite = CCSprite::createWithSpriteFrameName(GEODE_MOD_ID "/SH_epicCoin_001.png");
                epicSprite->setID("epic-sprite");
                filtersNode->addChild(epicSprite);
            }

            if (object.mythic) {
                auto mythicSprite = CCSprite::createWithSpriteFrameName(GEODE_MOD_ID "/SH_epicCoin2_001.png");
                mythicSprite->setID("legendary-sprite");
                filtersNode->addChild(mythicSprite);
            }

            if (object.legendary) {
                auto legendarySprite = CCSprite::createWithSpriteFrameName(GEODE_MOD_ID "/SH_epicCoin3_001.png");
                legendarySprite->setID("mythic-sprite");
                filtersNode->addChild(legendarySprite);
            }

            if (object.noStar) {
                auto noStarSprite = CCSprite::createWithSpriteFrameName("GJ_starsIcon_gray_001.png");
                noStarSprite->setID("no-star-sprite");
                filtersNode->addChild(noStarSprite);
            }

            for (auto child : CCArrayExt<CCNode*>(filtersNode->getChildren())) {
                child->setScale(0.35f);
            }

            if (filtersNode->getChildrenCount() == 0) {
                filtersNode->removeFromParent();
                auto noFiltersLabel = CCLabelBMFont::create("(No Filters)", "bigFont.fnt");
                noFiltersLabel->setAnchorPoint({ 0.0f, 0.5f });
                noFiltersLabel->setColor({ 127, 127, 127 });
                noFiltersLabel->setScale(0.25f);
                noFiltersLabel->setPosition({ 50.0f, 30.0f });
                noFiltersLabel->setID("no-filters-label");
                addChild(noFiltersLabel);
            }
            else filtersNode->updateLayout();
        }

        auto difficultiesNode = CCNode::create();
        difficultiesNode->setAnchorPoint({ 0.0f, 0.5f });
        difficultiesNode->setContentSize({ 250.0f, 8.0f });
        difficultiesNode->setPosition({ 50.0f, 18.0f + (type > 0 ? 2.0f : 0.0f) });
        difficultiesNode->setLayout(RowLayout::create()->setGap(1.5f)->setAxisAlignment(AxisAlignment::Start)->setAutoScale(false));
        difficultiesNode->setID("difficulties-node");
        addChild(difficultiesNode);

        for (auto difficulty : object.difficulties) {
            auto diff = difficulty == -1 ? 0 : difficulty == -2 ? 6 : difficulty == -3 ? -1 : difficulty;
            auto difficultySprite = CCSprite::createWithSpriteFrameName(
                diff == -1 ? "diffIcon_auto_btn_001.png" : fmt::format("diffIcon_{:02d}_btn_001.png", diff).c_str());
            difficultySprite->setScale(0.25f);
            difficultySprite->setID(fmt::format("difficulty-{}-sprite", diff));
            difficultiesNode->addChild(difficultySprite);
        }

        if (std::ranges::find(object.difficulties, -2) != object.difficulties.end() && object.demonFilter > 0) {
            auto demonFilterSprite = CCSprite::createWithSpriteFrameName(fmt::format("diffIcon_{:02d}_btn_001.png", object.demonFilter).c_str());
            demonFilterSprite->setScale(0.25f);
            demonFilterSprite->setID("demon-filter-sprite");
            difficultiesNode->addChild(demonFilterSprite);
        }

        if (type < 1) for (auto time : object.lengths) {
            auto lengthLabel = CCLabelBMFont::create(time < lengths.size() ? lengths[time] : "Unknown", "bigFont.fnt");
            lengthLabel->setScale(0.25f);
            lengthLabel->setID(fmt::format("length-{}-label", time));
            difficultiesNode->addChild(lengthLabel);
        }

        if (object.star) {
            auto starSprite = CCSprite::createWithSpriteFrameName("GJ_starsIcon_001.png");
            starSprite->setScale(0.35f);
            starSprite->setID("star-sprite");
            difficultiesNode->addChild(starSprite);
        }

        if (difficultiesNode->getChildrenCount() == 0) {
            difficultiesNode->removeFromParent();
            auto noDifficultiesLabel = CCLabelBMFont::create(type < 1 ? "(No Difficulties/Lengths)" : "(No Difficulties)", "bigFont.fnt");
            noDifficultiesLabel->setAnchorPoint({ 0.0f, 0.5f });
            noDifficultiesLabel->setColor({ 127, 127, 127 });
            noDifficultiesLabel->setScale(0.25f);
            noDifficultiesLabel->setPosition({ 50.0f, 18.0f + (type > 0 ? 2.0f : 0.0f) });
            noDifficultiesLabel->setID("no-difficulties-label");
            addChild(noDifficultiesLabel);
        }
        else difficultiesNode->updateLayout();

        if (type < 1 && object.song) {
            auto songSprite = CCSprite::createWithSpriteFrameName("GJ_musicIcon_001.png");
            songSprite->setScale(0.325f);
            songSprite->setPosition({ 50.0f + (songSprite->getScaledContentSize().width / 2), 7.0f });
            songSprite->setID("song-sprite");
            addChild(songSprite);

            auto songLabel = CCLabelBMFont::create(
                object.customSong ? std::to_string(object.songID).c_str() : LevelTools::getAudioTitle(object.songID - 1).c_str(), "bigFont.fnt");
            songLabel->setScale(0.25f);
            songLabel->setAnchorPoint({ 0.0f, 0.5f });
            songLabel->setPosition({ 60.0f, 7.0f });
            songLabel->limitLabelWidth(280.0f, 0.25f, 0.1f);
            songLabel->setID("song-label");
            addChild(songLabel);
        } else if (type < 1) {
            auto noSongLabel = CCLabelBMFont::create("(No Song)", "bigFont.fnt");
            noSongLabel->setAnchorPoint({ 0.0f, 0.5f });
            noSongLabel->setColor({ 127, 127, 127 });
            noSongLabel->setScale(0.25f);
            noSongLabel->setPosition({ 50.0f, 7.0f });
            noSongLabel->setID("no-song-label");
            addChild(noSongLabel);
        }
    }

    std::stringstream ss;
    time_t time = object.time;
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdeprecated-declarations"
    ss << std::put_time(std::localtime(&time), h12 ? "%Y-%m-%d %I:%M:%S %p" : "%Y-%m-%d %H:%M:%S");
    #pragma clang diagnostic pop

    auto timeLabel = CCLabelBMFont::create(ss.str().c_str(), "chatFont.fnt");
    timeLabel->setColor(white ? ccColor3B { 255, 255, 255 } : ccColor3B { 51, 51, 51 });
    timeLabel->setOpacity(white ? 200 : 152);
    timeLabel->setScale(0.4f);
    timeLabel->setAnchorPoint({ 1.0f, 0.0f });
    timeLabel->setPosition({ 399.0f, 1.0f });
    timeLabel->setID("time-label");
    addChild(timeLabel);

    return true;
}

void SearchHistoryNode::draw() {
    ccDrawColor4B(0, 0, 0, 75);
    glLineWidth(2.0f);
    if (m_index % 10 < 9 && m_index < m_count - 1) ccDrawLine({ 0.0f, 0.0f }, { 400.0f, 0.0f });
    if (m_index % 10 > 0) ccDrawLine({ 0.0f, 50.0f }, { 400.0f, 50.0f });
}
