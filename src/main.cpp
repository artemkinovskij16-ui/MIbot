#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

// ========== БОТ МЕНЕДЖЕР ==========
struct BotFrame {
    int frame;
    bool pressing;
};

class BotManager {
public:
    static BotManager& get() {
        static BotManager instance;
        return instance;
    }

    enum State { Idle, Recording, Playing };
    
    State state = Idle;
    std::vector<BotFrame> frames;
    int playIndex = 0;

    void startRecord() {
        frames.clear();
        state = Recording;
    }

    void stopRecord() {
        state = Idle;
    }

    void startPlay() {
        playIndex = 0;
        state = Playing;
    }

    void stopPlay() {
        state = Idle;
    }

    void recordFrame(int frame, bool pressing) {
        if (state == Recording) {
            frames.push_back({frame, pressing});
        }
    }

    BotFrame* getFrame(int frame) {
        for (auto& f : frames) {
            if (f.frame == frame) return &f;
        }
        return nullptr;
    }
};

// ========== ХАКИ ==========
class HackManager {
public:
    static HackManager& get() {
        static HackManager instance;
        return instance;
    }

    bool noclip = false;
    bool speedhack = false;
    float speed = 1.0f;

    void applySpeed() {
        if (speedhack) {
            CCDirector::sharedDirector()->getScheduler()->setTimeScale(speed);
        } else {
            CCDirector::sharedDirector()->getScheduler()->setTimeScale(1.0f);
        }
    }
};

// ========== GUI СЛОЙ ==========
class MIBotLayer : public CCLayer {
public:
    CCLabelBMFont* statusLabel;
    CCLabelBMFont* frameLabel;

    static MIBotLayer* create() {
        auto ret = new MIBotLayer();
        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    bool init() {
        if (!CCLayer::init()) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        auto bg = CCLayerColor::create(ccc4(0, 0, 0, 180));
        this->addChild(bg);

        auto panel = CCScale9Sprite::create("GJ_square02.png");
        panel->setContentSize({280, 320});
        panel->setPosition({winSize.width / 2, winSize.height / 2});
        this->addChild(panel);

        auto menu = CCMenu::create();
        menu->setPosition({0, 0});
        this->addChild(menu);

        float cx = winSize.width / 2;
        float cy = winSize.height / 2;

        auto title = CCLabelBMFont::create("MIBot", "goldFont.fnt");
        title->setPosition({cx, cy + 130});
        title->setScale(0.8f);
        this->addChild(title);

        statusLabel = CCLabelBMFont::create("Status: Idle", "chatFont.fnt");
        statusLabel->setPosition({cx, cy + 100});
        statusLabel->setScale(0.5f);
        this->addChild(statusLabel);

        frameLabel = CCLabelBMFont::create("Frames: 0", "chatFont.fnt");
        frameLabel->setPosition({cx, cy + 85});
        frameLabel->setScale(0.45f);
        this->addChild(frameLabel);

        // Кнопки бота
        auto recBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("REC", 70, true, "bigFont.fnt", "GJ_button_01.png", 25, 0.6f),
            this, menu_selector(MIBotLayer::onRecord)
        );
        recBtn->setPosition({cx - 70, cy + 40});
        menu->addChild(recBtn);

        auto playBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("PLAY", 70, true, "bigFont.fnt", "GJ_button_02.png", 25, 0.6f),
            this, menu_selector(MIBotLayer::onPlay)
        );
        playBtn->setPosition({cx, cy + 40});
        menu->addChild(playBtn);

        auto stopBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("STOP", 70, true, "bigFont.fnt", "GJ_button_06.png", 25, 0.6f),
            this, menu_selector(MIBotLayer::onStop)
        );
        stopBtn->setPosition({cx + 70, cy + 40});
        menu->addChild(stopBtn);

        // Хаки
        auto noclipBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Noclip", 90, true, "bigFont.fnt", "GJ_button_04.png", 25, 0.5f),
            this, menu_selector(MIBotLayer::onNoclip)
        );
        noclipBtn->setPosition({cx, cy - 10});
        menu->addChild(noclipBtn);

        auto speedBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Speed 2x", 90, true, "bigFont.fnt", "GJ_button_04.png", 25, 0.5f),
            this, menu_selector(MIBotLayer::onSpeed)
        );
        speedBtn->setPosition({cx, cy - 50});
        menu->addChild(speedBtn);

        auto fpsBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("FPS 240", 90, true, "bigFont.fnt", "GJ_button_05.png", 25, 0.5f),
            this, menu_selector(MIBotLayer::onFPS)
        );
        fpsBtn->setPosition({cx, cy - 90});
        menu->addChild(fpsBtn);

        // Закрыть
        auto closeBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png"),
            this, menu_selector(MIBotLayer::onClose)
        );
        closeBtn->setPosition({cx - 125, cy + 145});
        closeBtn->setScale(0.7f);
        menu->addChild(closeBtn);

        this->setTouchEnabled(true);
        return true;
    }

    void updateLabels() {
        auto& bot = BotManager::get();
        
        if (bot.state == BotManager::Recording) {
            statusLabel->setString("Status: Recording");
            statusLabel->setColor(ccc3(255, 80, 80));
        } else if (bot.state == BotManager::Playing) {
            statusLabel->setString("Status: Playing");
            statusLabel->setColor(ccc3(80, 255, 80));
        } else {
            statusLabel->setString("Status: Idle");
            statusLabel->setColor(ccc3(200, 200, 200));
        }

        frameLabel->setString(fmt::format("Frames: {}", bot.frames.size()).c_str());
    }

    void onRecord(CCObject*) {
        BotManager::get().startRecord();
        updateLabels();
    }

    void onPlay(CCObject*) {
        BotManager::get().startPlay();
        updateLabels();
    }

    void onStop(CCObject*) {
        auto& bot = BotManager::get();
        if (bot.state == BotManager::Recording) {
            bot.stopRecord();
        } else {
            bot.stopPlay();
        }
        HackManager::get().speedhack = false;
        HackManager::get().applySpeed();
        updateLabels();
    }

    void onNoclip(CCObject*) {
        auto& hack = HackManager::get();
        hack.noclip = !hack.noclip;
        FLAlertLayer::create("MIBot", hack.noclip ? "Noclip ON" : "Noclip OFF", "OK")->show();
    }

    void onSpeed(CCObject*) {
        auto& hack = HackManager::get();
        hack.speedhack = !hack.speedhack;
        hack.speed = 2.0f;
        hack.applySpeed();
        FLAlertLayer::create("MIBot", hack.speedhack ? "Speed 2x ON" : "Speed OFF", "OK")->show();
    }

    void onFPS(CCObject*) {
        static bool enabled = false;
        enabled = !enabled;
        if (enabled) {
            CCDirector::sharedDirector()->setAnimationInterval(1.0 / 240.0);
        } else {
            CCDirector::sharedDirector()->setAnimationInterval(1.0 / 60.0);
        }
        FLAlertLayer::create("MIBot", enabled ? "FPS 240 ON" : "FPS 60", "OK")->show();
    }

    void onClose(CCObject*) {
        this->removeFromParentAndCleanup(true);
    }

    void show() {
        auto scene = CCDirector::sharedDirector()->getRunningScene();
        if (scene) {
            this->setZOrder(1000);
            scene->addChild(this);
        }
    }
};

// ========== КНОПКА ==========
class FloatingButton : public CCNode {
public:
    static FloatingButton* create() {
        auto ret = new FloatingButton();
        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    bool init() {
        if (!CCNode::init()) return false;

        auto menu = CCMenu::create();
        menu->setPosition({0, 0});
        this->addChild(menu);

        auto circle = CCSprite::createWithSpriteFrameName("GJ_button_01.png");
        circle->setScale(0.5f);

        auto btn = CCMenuItemSpriteExtra::create(circle, this, menu_selector(FloatingButton::onClick));
        
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        btn->setPosition({40, winSize.height / 2});
        menu->addChild(btn);

        auto label = CCLabelBMFont::create("MI", "bigFont.fnt");
        label->setScale(0.3f);
        label->setPosition(btn->getPosition());
        menu->addChild(label);

        this->setZOrder(999);
        return true;
    }

    void onClick(CCObject*) {
        auto scene = CCDirector::sharedDirector()->getRunningScene();
        if (!scene) return;

        if (auto old = scene->getChildByTag(7777)) {
            old->removeFromParentAndCleanup(true);
            return;
        }

        auto layer = MIBotLayer::create();
        if (layer) {
            layer->setTag(7777);
            layer->show();
        }
    }
};

// ========== ХУКИ ==========
class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        this->addChild(FloatingButton::create());
        return true;
    }
};

class $modify(PlayLayer) {
    struct Fields {
        int frameCounter = 0;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
        m_fields->frameCounter = 0;
        this->addChild(FloatingButton::create());
        return true;
    }

    void update(float dt) {
        PlayLayer::update(dt);
        m_fields->frameCounter++;

        auto& bot = BotManager::get();
        if (bot.state == BotManager::Playing) {
            auto frame = bot.getFrame(m_fields->frameCounter);
            if (frame) {
                if (frame->pressing) {
                    this->pushButton(1, true);
                } else {
                    this->releaseButton(1, true);
                }
            }
        }
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        m_fields->frameCounter = 0;
        if (BotManager::get().state == BotManager::Recording) {
            BotManager::get().frames.clear();
        }
    }

    void pushButton(int button, bool player1) {
        PlayLayer::pushButton(button, player1);
        BotManager::get().recordFrame(m_fields->frameCounter, true);
    }

    void releaseButton(int button, bool player1) {
        PlayLayer::releaseButton(button, player1);
        BotManager::get().recordFrame(m_fields->frameCounter, false);
    }

    void destroyPlayer(PlayerObject* player, GameObject* obj) {
        if (HackManager::get().noclip) return;
        PlayLayer::destroyPlayer(player, obj);
    }
};
