#pragma once

#include "global.h"
#include "ResourceManager.h"
#include "Screen.h"

#include "GUI/Button.h"
#include "GUI/SaveGameWindow.h"
#include "GUI/YesOrNoForm.h"

class LoadGameScreen final : public Screen {
public:
	LoadGameScreen(CharacterCore* core);
	~LoadGameScreen();

	void execUpdate(const sf::Time& frameTime) override;
	void render(sf::RenderTarget& renderTarget) override;

	void execOnEnter() override;
	void execOnExit() override;

private:
	void setAllButtonsEnabled(bool value) override;
	Button* m_loadSaveGameButton = nullptr;
	Button* m_deleteSaveGameButton = nullptr;
	sf::Text* m_title = nullptr;
	SaveGameWindow* m_saveGameWindow = nullptr;

	YesOrNoForm* m_yesOrNoForm = nullptr;
	CharacterCore* m_newCharacterCore = nullptr;

	// agents for yes or no form
	void onNo();
	void onLoadGame();
	void onDeleteSaveGame();
	void onLoadSaveGame();
	void onYesDeleteSaveGame();
	void onBack();

	bool m_loadGame = false;
};