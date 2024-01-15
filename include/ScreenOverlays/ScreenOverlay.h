#pragma once

#include "global.h"
#include "GUI/BitmapText.h"
#include "World/GameObject.h"
#include "ResourceManager.h"
#include "Enums/FractionID.h"
#include "Enums/QuestState.h"
#include "Enums/SpellID.h"
#include "Structs/SpellModifier.h"

class Item;

/* A screen overlay that supports arbitrary text, split in title and subtitle */
class ScreenOverlay : public virtual GameObject {
public:
	ScreenOverlay(const sf::Time& activeTime, const sf::Time& fadeTime = sf::Time::Zero);
	virtual ~ScreenOverlay() {};

	virtual void update(const sf::Time& frameTime) override;
	virtual void render(sf::RenderTarget& renderTarget) override;

	void setTitle(const std::string& textKey, const std::string& textType = "core");
	void setTitleRaw(const std::string& text);
	void setTitleCharacterSize(int characterSize);
	void setTitleColor(const sf::Color& color);
	void setTextPosition(float percentage = 0.3f);
	void setSubtitle(const std::string& textKey, const std::string& textType = "core");
	void setSubtitleRaw(const std::string& text);
	void setSubtitleCharacterSize(int characterSize);
	void setSubtitleColor(const sf::Color& color);
	void setPermanent(bool permanent);

	GameObjectType getConfiguredType() const override;

	static ScreenOverlay* createQuestScreenOverlay(const std::string& questID, QuestState state);
	static ScreenOverlay* createLocationScreenOverlay(const std::string& locationKey, bool isBossLevel = false, bool isObserved = false);
	static ScreenOverlay* createHintScreenOverlay(const std::string& hintKey);
	static ScreenOverlay* createPermanentItemScreenOverlay(const Item* item);
	static ScreenOverlay* createSpellLearnedScreenOverlay(SpellID id);
	static ScreenOverlay* createModifierLearnedScreenOverlay(const SpellModifier& modifier);
	static ScreenOverlay* createGameOverScreenOverlay();
	static ScreenOverlay* createGamePausedScreenOverlay();
	static ScreenOverlay* createEnemyDefeatedScreenOverlay(std::map<std::string, int>& items, int gold);
	static ScreenOverlay* createArrestedScreenOverlay();
	static ScreenOverlay* createGuildJoinedScreenOverlay(FractionID id);

protected:
	float m_scale;
	float m_textPositionPercentage;
	bool m_isPermanent;

	sf::Text m_title;
	sf::Text m_subtitle;

	virtual void repositionText();

	sf::Time m_fadeTime;

private:
	sf::Time m_activeTime;
	sf::Time m_fadeInTimer;
	sf::Time m_fadeOutTimer;

	void load();
};
