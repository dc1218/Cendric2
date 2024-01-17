#pragma once

#include "global.h"
#include "ResourceManager.h"
#include "Screen.h"
#include "Screens/SplashScreen.h"
#include "CharacterCore.h"
#include "GUI/Button.h"
#include "GUI/YesOrNoForm.h"

namespace particles {
	class TextureParticleSystem;
}

class MenuScreen final : public Screen {
public:
	MenuScreen(CharacterCore* core);
	~MenuScreen();

	void execUpdate(const sf::Time& frameTime) override;
	void render(sf::RenderTarget& renderTarget) override;

	void execOnEnter() override;
	void execOnExit() override;

	void setFireParticles(particles::TextureParticleSystem* ps_left, particles::TextureParticleSystem* ps_right);

private:
	sf::Sprite m_screenSpriteBackground;
	sf::Sprite m_screenSpriteForeground;
	sf::Sprite m_logoSprite;
	sf::Text m_versionText;

	particles::TextureParticleSystem* m_ps_left = nullptr;
	particles::TextureParticleSystem* m_ps_right = nullptr;

	void setAllButtonsEnabled(bool value) override;

	YesOrNoForm* m_yesOrNoForm = nullptr;

	Button* m_saveGameButton = nullptr;

	void loadNewestSave();

	// agents for the yes or no form
	void onStartNewGame();
	void onNo();
	// agents for other buttons
	void onExit();
	void onResume();
	void onNewGame();
	void onLoadGame();
	void onSaveGame();
	void onOptions();
	void onKeybindings();
	void onCredits();
};