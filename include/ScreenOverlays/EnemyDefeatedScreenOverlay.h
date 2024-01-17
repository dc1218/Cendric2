#pragma once

#include "ScreenOverlays/TextureScreenOverlay.h"
#include "GUI/InventorySlot.h"

class EnemyDefeatedScreenOverlay final : public TextureScreenOverlay {
public:
	EnemyDefeatedScreenOverlay(const sf::Time& activeTime, const sf::Time& fadeTime = sf::Time::Zero);
	~EnemyDefeatedScreenOverlay();

	void update(const sf::Time& frameTime) override;
	void render(sf::RenderTarget& target) override;

	void setLoot(std::map<std::string, int>& items, int gold);

private:
	std::vector<InventorySlot*> m_items;
	std::vector<sf::Text*> m_texts;
};
