#include "../SearchHistory.hpp"
#include <Geode/binding/DemonFilterDelegate.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/TextInput.hpp>

typedef std::function<void(SearchHistoryObject)> SearchFilterCallback;

class SearchFilterPopup : public geode::Popup<const SearchHistoryObject&, SearchFilterCallback>, public DemonFilterDelegate {
protected:
    SearchHistoryObject m_searchFilter;
    SearchFilterCallback m_searchCallback;
    CCMenuItemSpriteExtra* m_demonFilterButton;
    geode::Ref<cocos2d::CCArray> m_typeButtons;
    geode::Ref<cocos2d::CCArray> m_difficultyButtons;
    geode::Ref<cocos2d::CCArray> m_lengthButtons;
    CCMenuItemSpriteExtra* m_normalButton;
    CCMenuItemSpriteExtra* m_customButton;
    CCMenuItemSpriteExtra* m_prevButton;
    CCMenuItemSpriteExtra* m_nextButton;
    cocos2d::CCLabelBMFont* m_songLabel;
    geode::TextInput* m_songInput;

    bool setup(const SearchHistoryObject&, SearchFilterCallback) override;
    void updateTypes();
    void updateDifficulties();
    void updateLengths();
public:
    static SearchFilterPopup* create(const SearchHistoryObject&, SearchFilterCallback);

    void demonFilterSelectClosed(int) override;
};
