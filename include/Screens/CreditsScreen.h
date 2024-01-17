#pragma once

#include "global.h"
#include "ResourceManager.h"
#include "Screen.h"


class CreditsScreen final : public Screen {
public:
	CreditsScreen(CharacterCore* core);

	void execUpdate(const sf::Time& frameTime) override;
	void render(sf::RenderTarget& renderTarget) override;

	void execOnEnter() override;
	void execOnExit() override;

private:
	sf::Sprite m_screenSprite;
	sf::Text* m_title = nullptr;
	sf::Text* m_credits = nullptr;

	void onBack();
};