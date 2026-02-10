#include "SearchFilterPopup.hpp"
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/DemonFilterSelectLayer.hpp>
#include <Geode/binding/LevelTools.hpp>
#include <Geode/loader/Mod.hpp>

using namespace geode::prelude;

SearchFilterPopup* SearchFilterPopup::create(const SearchHistoryObject& filter, SearchFilterCallback callback) {
    auto ret = new SearchFilterPopup();
    if (ret->init(filter, std::move(callback))) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void updateButton(CCMenuItemSpriteExtra* button, bool enabled, bool clickable) {
    static_cast<CCNodeRGBA*>(button->getNormalImage())->setColor(enabled ? ccColor3B { 255, 255, 255 } : ccColor3B { 125, 125, 125 });
    button->setEnabled(clickable || enabled);
}

bool SearchFilterPopup::init(const SearchHistoryObject& filter, SearchFilterCallback callback) {
    if (!Popup::init(300.0f, 250.0f, "GJ_square02.png")) return false;

    setID("SearchFilterPopup");
    setTitle("Search Filter", "bigFont.fnt", 0.53f, 15.0f);
    m_title->setID("search-filter-title");
    m_mainLayer->setID("main-layer");
    m_buttonMenu->setID("button-menu");
    m_bgSprite->setID("background");
    m_closeBtn->setID("close-button");
    m_noElasticity = true;

    m_searchFilter = filter;
    m_searchCallback = std::move(callback);

    auto typesMenu = CCMenu::create();
    typesMenu->setPosition({ 150.0f, 200.0f });
    typesMenu->setContentSize({ 300.0f, 40.0f });
    typesMenu->setLayout(RowLayout::create()->setGap(20.0f)->setAxisAlignment(AxisAlignment::Center));
    typesMenu->setID("types-menu");
    m_mainLayer->addChild(typesMenu);

    constexpr std::array types = {
        std::make_tuple(0, "GJ_viewLevelsBtn_001.png", "levels-button"),
        std::make_tuple(1, "GJ_viewListsBtn_001.png", "lists-button"),
        std::make_tuple(2, "GJ_profileButton_001.png", "users-button")
    };

    for (auto [type, frameName, id] : types) {
        auto typeSprite = CCSprite::createWithSpriteFrameName(frameName);
        typeSprite->setScale(type == 2 ? 0.65f : 0.8f);
        auto typeButton = CCMenuItemSpriteExtra::create(typeSprite, this, menu_selector(SearchFilterPopup::onType));
        typeButton->setTag(type);
        typeButton->setID(id);
        typesMenu->addChild(typeButton);
        m_typeButtons.push_back(typeButton);
    }

    typesMenu->updateLayout();

    auto filtersMenu = CCMenu::create();
    filtersMenu->setPosition({ 150.0f, 165.0f });
    filtersMenu->setContentSize({ 300.0f, 20.0f });
    filtersMenu->setLayout(RowLayout::create()->setGap(7.0f)->setAxisAlignment(AxisAlignment::Center));
    filtersMenu->setID("filters-menu");
    m_mainLayer->addChild(filtersMenu);

    constexpr std::array filters = {
        std::make_tuple(offsetof(SearchHistoryObject, uncompleted), "GJ_deleteIcon_001.png", "uncompleted-button"),
        std::make_tuple(offsetof(SearchHistoryObject, completed), "GJ_completesIcon_001.png", "completed-button"),
        std::make_tuple(offsetof(SearchHistoryObject, featured), "SH_featuredCoin_001.png"_spr, "featured-button"),
        std::make_tuple(offsetof(SearchHistoryObject, original), "SH_originalIcon_001.png"_spr, "original-button"),
        std::make_tuple(offsetof(SearchHistoryObject, twoPlayer), "SH_twoPlayerBtn_001.png"_spr, "two-player-button"),
        std::make_tuple(offsetof(SearchHistoryObject, coins), "GJ_coinsIcon2_001.png", "coins-button"),
        std::make_tuple(offsetof(SearchHistoryObject, epic), "SH_epicCoin_001.png"_spr, "epic-button"),
        std::make_tuple(offsetof(SearchHistoryObject, mythic), "SH_epicCoin2_001.png"_spr, "mythic-button"),
        std::make_tuple(offsetof(SearchHistoryObject, legendary), "SH_epicCoin3_001.png"_spr, "legendary-button"),
        std::make_tuple(offsetof(SearchHistoryObject, noStar), "GJ_starsIcon_gray_001.png", "no-star-button"),
        std::make_tuple(offsetof(SearchHistoryObject, song), "GJ_musicIcon_001.png", "song-button")
    };

    for (auto [offset, frameName, id] : filters) {
        auto filterSprite = CCSprite::createWithSpriteFrameName(frameName);
        filterSprite->setScale(0.7f);
        CCMenuItemSpriteExtra* filterButton = nullptr;
        if (offset == offsetof(SearchHistoryObject, song)) {
            filterButton = CCMenuItemSpriteExtra::create(filterSprite, this, menu_selector(SearchFilterPopup::onSongFilter));
        }
        else {
            filterButton = CCMenuItemSpriteExtra::create(filterSprite, this, menu_selector(SearchFilterPopup::onFilter));
        }
        updateButton(filterButton, *reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(&m_searchFilter) + offset), true);
        filterButton->setTag(offset);
        filterButton->setID(id);
        filtersMenu->addChild(filterButton);
    }

    filtersMenu->updateLayout();

    auto normalSprite = ButtonSprite::create("Normal", 40, false, "bigFont.fnt",
        m_searchFilter.customSong ? "GJ_button_04.png" : "GJ_button_01.png", 20.0f, 0.4f);
    normalSprite->setScale(0.8f);
    normalSprite->setCascadeColorEnabled(true);
    auto customSprite = ButtonSprite::create("Custom", 40, false, "bigFont.fnt",
        m_searchFilter.customSong ? "GJ_button_01.png" : "GJ_button_04.png", 20.0f, 0.4f);
    customSprite->setScale(0.8f);
    customSprite->setCascadeColorEnabled(true);

    m_normalButton = CCMenuItemSpriteExtra::create(normalSprite, this, menu_selector(SearchFilterPopup::onNormalSong));
    m_normalButton->setPosition({ 120.0f, 140.0f });
    updateButton(m_normalButton, m_searchFilter.song, false);
    m_normalButton->setID("normal-song-button");
    m_buttonMenu->addChild(m_normalButton);

    m_customButton = CCMenuItemSpriteExtra::create(customSprite, this, menu_selector(SearchFilterPopup::onCustomSong));
    m_customButton->setPosition({ 180.0f, 140.0f });
    updateButton(m_customButton, m_searchFilter.song, false);
    m_customButton->setID("custom-song-button");
    m_buttonMenu->addChild(m_customButton);

    m_prevButton = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png"), this, menu_selector(SearchFilterPopup::onPrevSong)
    );
    m_prevButton->setPosition({ 55.0f, 120.0f });
    m_prevButton->setVisible(!m_searchFilter.customSong);
    updateButton(m_prevButton, m_searchFilter.song, false);
    m_prevButton->setID("prev-song-button");
    m_buttonMenu->addChild(m_prevButton);

    m_nextButton = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png"), this, menu_selector(SearchFilterPopup::onNextSong)
    );
    m_nextButton->setPosition({ 245.0f, 120.0f });
    m_nextButton->setVisible(!m_searchFilter.customSong);
    updateButton(m_nextButton, m_searchFilter.song, false);
    m_nextButton->setID("next-song-button");
    m_buttonMenu->addChild(m_nextButton);

    m_songLabel = CCLabelBMFont::create("Stereo Madness", "bigFont.fnt");
    m_songLabel->setPosition({ 150.0f, 120.0f });
    m_songLabel->setScale(0.6f);
    m_songLabel->setVisible(!m_searchFilter.customSong);
    if (m_searchFilter.song && !m_searchFilter.customSong) {
        m_songLabel->setString(LevelTools::getAudioTitle(m_searchFilter.songID - 1).c_str());
        m_songLabel->limitLabelWidth(170.0f, 0.6f, 0.0f);
    }
    m_songLabel->setColor(m_searchFilter.song ? ccColor3B { 255, 255, 255 } : ccColor3B { 125, 125, 125 });
    m_songLabel->setID("song-label");
    m_mainLayer->addChild(m_songLabel);

    m_songInput = TextInput::create(150.0f, "Song ID...");
    m_songInput->setPosition({ 150.0f, 115.0f });
    m_songInput->setScale(0.7f);
    m_songInput->setVisible(m_searchFilter.customSong);
    m_songInput->setEnabled(m_searchFilter.song);
    m_songInput->setCommonFilter(CommonFilter::Uint);
    m_songInput->setTextAlign(TextInputAlign::Center);
    m_songInput->setDelegate(this);
    if (m_searchFilter.song && m_searchFilter.customSong) {
        m_songInput->setString(m_searchFilter.songID > 0 ? fmt::to_string(m_searchFilter.songID) : "");
    }
    m_songInput->setID("song-input");
    m_mainLayer->addChild(m_songInput);

    auto demonFilterSprite = CCSprite::createWithSpriteFrameName("GJ_plus2Btn_001.png");
    demonFilterSprite->setScale(0.7f);
    m_demonFilterButton = CCMenuItemSpriteExtra::create(demonFilterSprite, this, menu_selector(SearchFilterPopup::onDemonFilter));
    m_demonFilterButton->setPosition({ 285.0f, 85.0f });
    m_demonFilterButton->setID("demon-filter-button");
    m_buttonMenu->addChild(m_demonFilterButton);

    auto difficultesMenu = CCMenu::create();
    difficultesMenu->setPosition({ 140.0f, 85.0f });
    difficultesMenu->setContentSize({ 280.0f, 40.0f });
    difficultesMenu->setLayout(RowLayout::create()->setGap(10.0f)->setAxisAlignment(AxisAlignment::Center));
    difficultesMenu->setID("difficulties-menu");
    m_mainLayer->addChild(difficultesMenu);

    constexpr std::array difficulties = {
        std::make_tuple(-1, "difficulty_00_btn_001.png", "na-button"),
        std::make_tuple(1, "difficulty_01_btn_001.png", "easy-button"),
        std::make_tuple(2, "difficulty_02_btn_001.png", "normal-button"),
        std::make_tuple(3, "difficulty_03_btn_001.png", "hard-button"),
        std::make_tuple(4, "difficulty_04_btn_001.png", "harder-button"),
        std::make_tuple(5, "difficulty_05_btn_001.png", "insane-button"),
        std::make_tuple(-2, "difficulty_06_btn_001.png", "demon-button"),
        std::make_tuple(-3, "difficulty_auto_btn_001.png", "auto-button")
    };

    for (auto [difficulty, frameName, id] : difficulties) {
        auto diffSprite = CCSprite::createWithSpriteFrameName(frameName);
        diffSprite->setScale(0.7f);
        CCMenuItemSpriteExtra* diffButton = nullptr;
        if (difficulty > 0) {
            diffButton = CCMenuItemSpriteExtra::create(diffSprite, this, menu_selector(SearchFilterPopup::onDifficulty));
        }
        else {
            diffButton = CCMenuItemSpriteExtra::create(diffSprite, this, menu_selector(SearchFilterPopup::onSpecialDifficulty));
        }
        diffButton->setTag(difficulty);
        diffButton->setID(id);
        difficultesMenu->addChild(diffButton);
        m_difficultyButtons.push_back(diffButton);
    }

    difficultesMenu->updateLayout();

    auto lengthsMenu = CCMenu::create();
    lengthsMenu->setPosition({ 150.0f, 55.0f });
    lengthsMenu->setContentSize({ 300.0f, 20.0f });
    lengthsMenu->setLayout(RowLayout::create()->setGap(5.0f)->setAxisAlignment(AxisAlignment::Center));
    lengthsMenu->setID("lengths-menu");
    m_mainLayer->addChild(lengthsMenu);

    constexpr std::array lengths = {
        std::make_pair("Tiny", "tiny-button"),
        std::make_pair("Short", "short-button"),
        std::make_pair("Medium", "medium-button"),
        std::make_pair("Long", "long-button"),
        std::make_pair("XL", "xl-button"),
        std::make_pair("Plat.", "plat-button")
    };

    for (int i = 0; i < lengths.size(); i++) {
        auto [labelText, buttonID] = lengths[i];
        auto lengthLabel = CCLabelBMFont::create(labelText, "bigFont.fnt");
        lengthLabel->setScale(0.4f);
        auto lengthButton = CCMenuItemSpriteExtra::create(lengthLabel, this, menu_selector(SearchFilterPopup::onLength));
        lengthButton->setTag(i);
        lengthButton->setID(buttonID);
        lengthsMenu->addChild(lengthButton);
        m_lengthButtons.push_back(lengthButton);
    }

    auto starSprite = CCSprite::createWithSpriteFrameName("GJ_starsIcon_001.png");
    starSprite->setScale(0.7f);
    auto starButton = CCMenuItemSpriteExtra::create(starSprite, this, menu_selector(SearchFilterPopup::onStar));
    updateButton(starButton, m_searchFilter.star, true);
    starButton->setID("star-button");
    lengthsMenu->addChild(starButton);

    lengthsMenu->updateLayout();

    updateTypes();
    updateDifficulties();
    updateLengths();

    auto confirmButton = CCMenuItemSpriteExtra::create(ButtonSprite::create("Confirm", 0.8f), this, menu_selector(SearchFilterPopup::onConfirm));
    confirmButton->setPosition({ 150.0f, 25.0f });
    confirmButton->setID("confirm-button");
    m_buttonMenu->addChild(confirmButton);

    return true;
}

void SearchFilterPopup::onType(CCObject* sender) {
    auto type = sender->getTag();
    if (m_searchFilter.type == type) m_searchFilter.type = -1;
    else m_searchFilter.type = type;
    updateTypes();
}

void SearchFilterPopup::onSongFilter(CCObject* sender) {
    m_searchFilter.song = !m_searchFilter.song;
    if (m_searchFilter.song && !m_searchFilter.customSong && m_searchFilter.songID == 0) m_searchFilter.songID = 1;
    updateButton(static_cast<CCMenuItemSpriteExtra*>(sender), m_searchFilter.song, true);
    updateButton(m_normalButton, m_searchFilter.song, false);
    updateButton(m_customButton, m_searchFilter.song, false);
    updateButton(m_prevButton, m_searchFilter.song, false);
    updateButton(m_nextButton, m_searchFilter.song, false);
    m_songLabel->setColor(m_searchFilter.song ? ccColor3B { 255, 255, 255 } : ccColor3B { 125, 125, 125 });
    m_songInput->setEnabled(m_searchFilter.song);
}

void SearchFilterPopup::onFilter(CCObject* sender) {
    auto& field = *reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(&m_searchFilter) + sender->getTag());
    field = !field;
    updateButton(static_cast<CCMenuItemSpriteExtra*>(sender), field, true);
}

void SearchFilterPopup::onNormalSong(CCObject* sender) {
    if (m_searchFilter.customSong) {
        m_searchFilter.customSong = false;
        m_searchFilter.songID = 1;
        static_cast<ButtonSprite*>(m_normalButton->getNormalImage())->updateBGImage("GJ_button_01.png");
        static_cast<ButtonSprite*>(m_customButton->getNormalImage())->updateBGImage("GJ_button_04.png");
        m_prevButton->setVisible(true);
        m_nextButton->setVisible(true);
        m_songLabel->setVisible(true);
        m_songInput->setVisible(false);
        m_songLabel->setString(LevelTools::getAudioTitle(m_searchFilter.songID - 1).c_str());
        m_songLabel->limitLabelWidth(170.0f, 0.6f, 0.0f);
    }
}

void SearchFilterPopup::onCustomSong(CCObject* sender) {
    if (!m_searchFilter.customSong) {
        m_searchFilter.customSong = true;
        m_searchFilter.songID = 0;
        static_cast<ButtonSprite*>(m_normalButton->getNormalImage())->updateBGImage("GJ_button_04.png");
        static_cast<ButtonSprite*>(m_customButton->getNormalImage())->updateBGImage("GJ_button_01.png");
        m_prevButton->setVisible(false);
        m_nextButton->setVisible(false);
        m_songLabel->setVisible(false);
        m_songInput->setVisible(true);
        m_songInput->setString("");
    }
}

void SearchFilterPopup::onPrevSong(CCObject* sender) {
    if (m_searchFilter.songID > 1) {
        m_searchFilter.songID--;
        m_songLabel->setString(LevelTools::getAudioTitle(m_searchFilter.songID - 1).c_str());
        m_songLabel->limitLabelWidth(170.0f, 0.6f, 0.0f);
    }
}

void SearchFilterPopup::onNextSong(CCObject* sender) {
    if (m_searchFilter.songID < 40) {
        m_searchFilter.songID++;
        m_songLabel->setString(LevelTools::getAudioTitle(m_searchFilter.songID - 1).c_str());
        m_songLabel->limitLabelWidth(170.0f, 0.6f, 0.0f);
    }
}

void SearchFilterPopup::textChanged(CCTextInputNode* input) {
    if (auto id = numFromString<int>(input->getString())) {
        m_searchFilter.songID = id.unwrap();
    }
    m_searchFilter.songID = std::max(m_searchFilter.songID, 0);
}

void SearchFilterPopup::onDemonFilter(CCObject* sender) {
    auto layer = DemonFilterSelectLayer::create();
    if (auto child = layer->m_buttonMenu->getChildByTag(m_searchFilter.demonFilter)) {
        layer->selectRating(child);
    }
    layer->m_noElasticity = true;
    layer->m_delegate = this;
    layer->show();
}

void SearchFilterPopup::onDifficulty(CCObject* sender) {
    auto difficulty = sender->getTag();
    if (auto it = std::ranges::find(m_searchFilter.difficulties, difficulty); it != m_searchFilter.difficulties.end()) {
        m_searchFilter.difficulties.erase(it);
    }
    else {
        if (!m_searchFilter.difficulties.empty()) {
            auto firstDiff = m_searchFilter.difficulties[0];
            if (m_searchFilter.difficulties[0] < 0) m_searchFilter.difficulties.clear();
        }
        m_searchFilter.difficulties.push_back(difficulty);
    }
    updateDifficulties();
}

void SearchFilterPopup::onSpecialDifficulty(CCObject* sender) {
    auto difficulty = sender->getTag();
    if (auto it = std::ranges::find(m_searchFilter.difficulties, difficulty); it != m_searchFilter.difficulties.end()) {
        m_searchFilter.difficulties.erase(it);
    }
    else {
        m_searchFilter.difficulties = { difficulty };
    }
    updateDifficulties();
}

void SearchFilterPopup::onLength(CCObject* sender) {
    auto length = sender->getTag();
    if (auto it = std::ranges::find(m_searchFilter.lengths, length); it != m_searchFilter.lengths.end()) {
        m_searchFilter.lengths.erase(it);
    }
    else {
        m_searchFilter.lengths.push_back(length);
    }
    updateLengths();
}

void SearchFilterPopup::onStar(CCObject* sender) {
    m_searchFilter.star = !m_searchFilter.star;
    updateButton(static_cast<CCMenuItemSpriteExtra*>(sender), m_searchFilter.star, true);
}

void SearchFilterPopup::onConfirm(CCObject* sender) {
    m_searchCallback(std::move(m_searchFilter));
    onClose(nullptr);
}

void SearchFilterPopup::updateTypes() {
    for (auto button : m_typeButtons) {
        updateButton(button, button->getTag() == m_searchFilter.type, true);
    }
}

void SearchFilterPopup::updateDifficulties() {
    auto spriteFrameCache = CCSpriteFrameCache::get();
    for (auto button : m_difficultyButtons) {
        auto sprite = static_cast<CCSprite*>(button->getNormalImage());
        auto tag = button->getTag();
        auto enabled = std::ranges::contains(m_searchFilter.difficulties, tag);
        if (tag == -2) {
            m_demonFilterButton->setEnabled(enabled);
            updateButton(m_demonFilterButton, enabled, false);
            sprite->setDisplayFrame(spriteFrameCache->spriteFrameByName(m_searchFilter.demonFilter > 0
                ? fmt::format("difficulty_{:02}_btn2_001.png", m_searchFilter.demonFilter).c_str() : "difficulty_06_btn_001.png"));
        }
        updateButton(button, enabled, true);
    }
}

void SearchFilterPopup::updateLengths() {
    for (auto button : m_lengthButtons) {
        static_cast<CCLabelBMFont*>(button->getNormalImage())->setColor(
            std::ranges::contains(m_searchFilter.lengths, button->getTag()) ? ccColor3B { 255, 255, 255 } : ccColor3B { 125, 125, 125 });
    }
}

void SearchFilterPopup::demonFilterSelectClosed(int filter) {
    m_searchFilter.demonFilter = filter;
    updateDifficulties();
}
