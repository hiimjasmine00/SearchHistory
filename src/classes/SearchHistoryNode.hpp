#include "../SearchHistory.hpp"
#include <cocos2d.h>

typedef std::function<void(const SearchHistoryObject&)> SearchCallback;
typedef std::function<void(int)> RemoveCallback;

class SearchHistoryNode : public cocos2d::CCNode {
protected:
    inline static constexpr std::array lengths = { "Tiny", "Short", "Medium", "Long", "XL", "Plat." };

    int m_index;
    int m_count;
public:
    static SearchHistoryNode* create(const SearchHistoryObject&, int, int, SearchCallback, RemoveCallback, bool, bool, bool);

    bool init(const SearchHistoryObject&, int, int, SearchCallback, RemoveCallback, bool, bool, bool);
    void draw() override;
};
