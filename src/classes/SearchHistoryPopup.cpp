#include "SearchHistoryPopup.hpp"
#include "SearchFilterPopup.hpp"
#include "SearchHistoryNode.hpp"
#include <Geode/binding/ButtonSprite.hpp>
#include <jasmine/setting.hpp>

using namespace geode::prelude;

SearchHistoryPopup* SearchHistoryPopup::create(SearchHistoryCallback callback) {
    auto ret = new SearchHistoryPopup();
    if (ret->init(std::move(callback))) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool SearchHistoryPopup::init(SearchHistoryCallback callback) {
    if (!Popup::init(440.0f, 290.0f, "GJ_square02.png")) return false;

    setID("SearchHistoryPopup");
    setTitle("Search History", "bigFont.fnt", 0.53f);
    m_title->setID("search-history-title");
    m_mainLayer->setID("main-layer");
    m_buttonMenu->setID("button-menu");
    m_bgSprite->setID("background");
    m_closeBtn->setID("close-button");

    m_searchCallback = std::move(callback);
    m_searchFilter.type = -1;

    auto background = NineSlice::create("square02_001.png", { 0, 0, 80, 80 });
    background->setContentSize({ 400.0f, 195.0f });
    background->setPosition({ 220.0f, 117.5f });
    background->setOpacity(127);
    background->setID("scroll-background");
    m_mainLayer->addChild(background);

    m_scrollLayer = ScrollLayer::create({ 400.0f, 195.0f });
    m_scrollLayer->setPosition({ 20.0f, 20.0f });
    m_scrollLayer->m_contentLayer->setLayout(
        ColumnLayout::create()
            ->setAxisReverse(true)
            ->setAxisAlignment(AxisAlignment::End)
            ->setAutoGrowAxis(195.0f)
            ->setGap(0.0f)
    );
    m_scrollLayer->setID("scroll-layer");
    m_mainLayer->addChild(m_scrollLayer);

    auto prevButtonSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    m_prevButton = CCMenuItemSpriteExtra::create(prevButtonSprite, this, menu_selector(SearchHistoryPopup::onPrevPage));
    m_prevButton->setPosition({ -34.5f, 145.0f });
    m_prevButton->setID("prev-button");
    m_buttonMenu->addChild(m_prevButton);

    auto nextButtonSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    nextButtonSprite->setFlipX(true);
    m_nextButton = CCMenuItemSpriteExtra::create(nextButtonSprite, this, menu_selector(SearchHistoryPopup::onNextPage));
    m_nextButton->setPosition({ 474.5f, 145.0f });
    m_nextButton->setID("next-button");
    m_buttonMenu->addChild(m_nextButton);

    auto clearSprite = CCSprite::createWithSpriteFrameName("GJ_deleteBtn_001.png");
    clearSprite->setScale(0.6f);
    auto clearButton = CCMenuItemSpriteExtra::create(clearSprite, this, menu_selector(SearchHistoryPopup::onClear));
    clearButton->setPosition({ 420.0f, 270.0f });
    clearButton->setID("clear-button");
    m_buttonMenu->addChild(clearButton);

    m_countLabel = CCLabelBMFont::create("", "goldFont.fnt");
    m_countLabel->setAnchorPoint({ 1.0f, 0.0f });
    m_countLabel->setScale(0.5f);
    m_countLabel->setPosition({ 435.0f, 7.0f });
    m_countLabel->setID("count-label");
    m_mainLayer->addChild(m_countLabel);

    m_searchInput = TextInput::create(360.0f, "Search History...");
    m_searchInput->setCommonFilter(CommonFilter::Any);
    m_searchInput->setTextAlign(TextInputAlign::Left);
    m_searchInput->setPosition({ 200.0f, 235.0f });
    m_searchInput->setCallback([this](auto) {
        page(0);
    });
    m_searchInput->setID("search-input");
    m_mainLayer->addChild(m_searchInput);

    auto filterSprite = ButtonSprite::create(
        CCSprite::createWithSpriteFrameName("GJ_filterIcon_001.png"), 32, false, 32.0f, "GJ_button_01.png", 1.0f);
    filterSprite->setScale(0.7f);
    m_filterButton = CCMenuItemSpriteExtra::create(filterSprite, this, menu_selector(SearchHistoryPopup::onFilter));
    m_filterButton->setPosition({ 405.0f, 235.0f });
    m_filterButton->setID("filter-button");
    m_buttonMenu->addChild(m_filterButton);

    page(0);

    return true;
}

void SearchHistoryPopup::onPrevPage(CCObject* sender) {
    page(m_page - 1);
}

void SearchHistoryPopup::onNextPage(CCObject* sender) {
    page(m_page + 1);
}

void SearchHistoryPopup::onClear(CCObject* sender) {
    FLAlertLayer::create(
        this,
        "Clear History",
        "Are you sure you want to clear your search history?",
        "No",
        "Yes",
        350.0f
    )->show();
}

void SearchHistoryPopup::FLAlert_Clicked(FLAlertLayer* layer, bool btn2) {
    if (btn2) {
        SearchHistory::history.clear();
        page(0);
    }
}

void SearchHistoryPopup::onFilter(CCObject* sender) {
    SearchFilterPopup::create(m_searchFilter, [this](SearchHistoryObject filter) {
        m_searchFilter = std::move(filter);
        static_cast<ButtonSprite*>(m_filterButton->getNormalImage())->updateBGImage(
            m_searchFilter.empty() ? "GJ_button_01.png" : "GJ_button_02.png");
        page(0);
    })->show();
}

void SearchHistoryPopup::page(int p) {
    m_scrollLayer->m_contentLayer->removeAllChildren();

    m_searchFilter.query = string::toLower(m_searchInput->getString());
    std::vector<SearchHistoryObject> history;
    for (auto& object : SearchHistory::history) {
        if (object.contains(m_searchFilter)) history.push_back(object);
    }

    auto count = history.size();
    m_prevButton->setVisible(p > 0);
    m_nextButton->setVisible(p < (count > 0 ? (count - 1) / 10 : 0));

    auto h12 = jasmine::setting::getValue<bool>("12-hour-time");
    auto white = jasmine::setting::getValue<bool>("white-time");
    auto dark = Loader::get()->isModLoaded("bitz.darkmode_v4");
    for (int i = p * 10; i < (p + 1) * 10 && i < count; i++) {
        auto& object = SearchHistory::history[history[i].index];
        auto index = object.index;
        m_scrollLayer->m_contentLayer->addChild(SearchHistoryNode::create(object, i, count, [this, index] {
            m_searchCallback(index);
            onClose(nullptr);
        }, [this, index] {
            SearchHistory::remove(index);
            page(m_page);
        }, h12, white, dark));
    }

    m_scrollLayer->m_contentLayer->updateLayout();
    m_scrollLayer->scrollToTop();

    m_countLabel->setString(fmt::format("{} to {} of {}", count > 0 ? p * 10 + 1 : 0, std::min<int>((p + 1) * 10, count), count).c_str());

    m_page = p;
}
