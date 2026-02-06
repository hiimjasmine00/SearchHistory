#include "../SearchHistory.hpp"
#include <Geode/binding/DemonFilterDelegate.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/TextInput.hpp>

typedef geode::Function<void(SearchHistoryObject)> SearchFilterCallback;

class SearchFilterPopup : public geode::Popup, public DemonFilterDelegate {
protected:
    SearchHistoryObject m_searchFilter;
    SearchFilterCallback m_searchCallback;
    CCMenuItemSpriteExtra* m_demonFilterButton;
    std::vector<CCMenuItemSpriteExtra*> m_typeButtons;
    std::vector<CCMenuItemSpriteExtra*> m_difficultyButtons;
    std::vector<CCMenuItemSpriteExtra*> m_lengthButtons;
    CCMenuItemSpriteExtra* m_normalButton;
    CCMenuItemSpriteExtra* m_customButton;
    CCMenuItemSpriteExtra* m_prevButton;
    CCMenuItemSpriteExtra* m_nextButton;
    cocos2d::CCLabelBMFont* m_songLabel;
    geode::TextInput* m_songInput;

    bool init(const SearchHistoryObject&, SearchFilterCallback);
    void updateTypes();
    void updateDifficulties();
    void updateLengths();
public:
    static SearchFilterPopup* create(const SearchHistoryObject&, SearchFilterCallback);

    void demonFilterSelectClosed(int) override;
};
