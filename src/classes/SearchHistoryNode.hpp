#include "../SearchHistory.hpp"
#include <Geode/binding/FLAlertLayerProtocol.hpp>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>

typedef geode::Function<void()> SearchCallback;
typedef geode::Function<void()> RemoveCallback;

class SearchHistoryNode : public cocos2d::CCLayer, public FLAlertLayerProtocol {
protected:
    SearchCallback m_searchCallback;
    RemoveCallback m_removeCallback;
    int m_index;
    int m_count;
public:
    static SearchHistoryNode* create(const SearchHistoryObject&, int, int, SearchCallback, RemoveCallback, bool, bool, bool);

    bool init(const SearchHistoryObject&, int, int, SearchCallback, RemoveCallback, bool, bool, bool);
    void onRemove(cocos2d::CCObject*);
    void onSearch(cocos2d::CCObject*);
    void FLAlert_Clicked(FLAlertLayer*, bool) override;
    void draw() override;
};
