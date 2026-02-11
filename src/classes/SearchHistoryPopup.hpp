#include "../SearchHistory.hpp"
#include <Geode/binding/FLAlertLayerProtocol.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/ScrollLayer.hpp>
#include <Geode/ui/TextInput.hpp>

typedef geode::Function<void(int)> SearchHistoryCallback;

class SearchHistoryPopup : public geode::Popup, public FLAlertLayerProtocol, public TextInputDelegate {
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

    bool init(SearchHistoryCallback);
    void onPrevPage(cocos2d::CCObject*);
    void onNextPage(cocos2d::CCObject*);
    void onClear(cocos2d::CCObject*);
    void FLAlert_Clicked(FLAlertLayer*, bool) override;
    void onFilter(cocos2d::CCObject*);
    void textChanged(CCTextInputNode*) override;
public:
    static SearchHistoryPopup* create(SearchHistoryCallback);

    void page(int);
};
