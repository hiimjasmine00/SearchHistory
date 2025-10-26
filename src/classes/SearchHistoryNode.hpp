#include "../SearchHistory.hpp"
#include <cocos2d.h>

typedef std::function<void()> SearchCallback;
typedef std::function<void()> RemoveCallback;

class SearchHistoryNode : public cocos2d::CCLayer {
protected:
    int m_index;
    int m_count;
public:
    static SearchHistoryNode* create(const SearchHistoryObject&, int, int, SearchCallback, RemoveCallback, bool, bool, bool);

    bool init(const SearchHistoryObject&, int, int, SearchCallback, RemoveCallback, bool, bool, bool);
    void draw() override;
};
