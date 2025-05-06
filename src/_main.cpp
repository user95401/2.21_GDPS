#include <Geode/Geode.hpp>
#include <Geode/ui/GeodeUI.hpp>
using namespace geode::prelude;
namespace fs = std::filesystem;

//lol
#define SETTING(type, key_name) Mod::get()->getSettingValue<type>(key_name)

#ifdef GEODE_IS_ANDROID
#define debug error
#endif // GEODE_IS_ANDROID

inline auto repo = std::string("user95401/2.21_GDPS");
inline auto repobranch = std::string("user95401/2.21_GDPS/main");
inline auto repo_lnk = std::string("https://github.com/" + repo);
inline auto raw_content_repo_lnk = std::string("https://raw.githubusercontent.com/" + repobranch);

#include <regex>

#include <_ImGui.hpp>

#include "download_mods.hpp"
#include "links.hpp"
#include "resources.hpp"
#include "hackpro.hpp"
#include "globed.hpp"

#if 1 //some basic fixes

#include <Geode/modify/CCSprite.hpp>
class $modify(CCSpriteExt, CCSprite) {
    $override virtual bool initWithTexture(CCTexture2D * pTexture) {
        //log::debug("{}({})", __FUNCTION__, pTexture);
        //if (pTexture) log::debug("pTexture Name: {}", pTexture->getName());
        return pTexture != nullptr ? CCSprite::initWithTexture(pTexture) : init();
    };
};

#include <Geode/modify/CCString.hpp>
class $modify(CCString) {
    const char* getCString() {
        //log::debug("{}(int:{})->{}", this, (int)this, __func__);
        if ((int)(size_t)this == 0) log::error("{}(int:{})->{}", this, (int)(size_t)this, __func__);
        return (int)(size_t)this != 0 ? CCString::getCString() : CCString::createWithFormat("")->getCString();
    }
};

#include <Geode/modify/GauntletSelectLayer.hpp>
class $modify(GauntletSelectLayerFix, GauntletSelectLayer) {
    $override void setupGauntlets() {
        GauntletSelectLayer::setupGauntlets();
        findFirstChildRecursive<ExtendedLayer>(this,
            [](ExtendedLayer* gauntlet_pages) {
                if (gauntlet_pages->getChildrenCount() > 1) return false;
                gauntlet_pages->setAnchorPoint(CCPointMake(1.f, 0.f));
                gauntlet_pages->ignoreAnchorPointForPosition(false);
                return true;
            }
        );
    };
};

#include <Geode/modify/CCNode.hpp>
class $modify(UpdateSceneScaleByScreenView, CCNode) {
    $override void visit() {
        CCNode::visit();
        if (auto game = GameManager::get()->m_gameLayer) if (game->isRunning()) return;
        if (auto gameplay = GameManager::get()->m_playLayer) if (gameplay->isRunning() or gameplay->m_isPaused) return;
        if (auto casted = typeinfo_cast<CCScene*>(this)) {
            this->setScaleX(CCDirector::get()->getScreenRight() / this->getContentWidth());
            this->setScaleY(CCDirector::get()->getScreenTop() / this->getContentHeight());
            this->setAnchorPoint(CCPointZero);
        };
    }
};

#endif

