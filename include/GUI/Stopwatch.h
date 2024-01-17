#pragma once

#include "global.h"
#include "GUI/Window.h"
#include "TextProvider.h"
#include "ResourceManager.h"

class CharacterCore;

class Stopwatch final : public GameObject {
public:
	Stopwatch(const CharacterCore* core);
	~Stopwatch();

	void render(sf::RenderTarget& renderTarget) override;
	void update(const sf::Time& frameTime) override;
	
	GameObjectType getConfiguredType() const override;

private:
	void initBackgroundLayers();

private:
	sf::Text m_text;
	const CharacterCore* m_core;
	Window* m_window = nullptr;
};
