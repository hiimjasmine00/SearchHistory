#include "../SearchHistory.hpp"
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/ScrollLayer.hpp>
#include <Geode/ui/TextInput.hpp>

typedef std::function<void(int)> SearchHistoryCallback;

class SearchHistoryPopup : public geode::Popup<SearchHistoryCallback> {
protected:
    SearchHistoryObject m_searchFilter;
    geode::ScrollLayer* m_scrollLayer;
    SearchHistoryCallback m_searchCallback;
    geode::TextInput* m_searchInput;
    CCMenuItemSpriteExtra* m_prevButton;
    CCMenuItemSpriteExtra* m_nextButton;
    CCMenuItemSpriteExtra* m_filterButton;
    cocos2d::CCLabelBMFont* m_countLabel;
    int m_page;

    bool setup(SearchHistoryCallback) override;
public:
    static SearchHistoryPopup* create(SearchHistoryCallback);

    void page(int);
};
