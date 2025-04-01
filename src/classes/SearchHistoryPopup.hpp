#include "../SearchHistory.hpp"
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/ScrollLayer.hpp>
#include <Geode/ui/TextInput.hpp>

typedef std::function<void(const SearchHistoryObject&)> SearchHistoryCallback;

class SearchHistoryPopup : public geode::Popup<SearchHistoryCallback> {
protected:
    geode::ScrollLayer* m_scrollLayer;
    SearchHistoryCallback m_searchCallback;
    geode::TextInput* m_searchInput;
    CCMenuItemSpriteExtra* m_prevButton;
    CCMenuItemSpriteExtra* m_nextButton;
    cocos2d::CCLabelBMFont* m_countLabel;
    int m_page;

    bool setup(SearchHistoryCallback) override;
public:
    static SearchHistoryPopup* create(SearchHistoryCallback);

    void page(int);
};
