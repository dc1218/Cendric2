#include "Screens/LoadGameScreen.h"
#include "Screens/MenuScreen.h"
#include "Screens/LoadingScreen.h"

LoadGameScreen::LoadGameScreen(CharacterCore* core) : Screen(core) {
}

LoadGameScreen::~LoadGameScreen() {
	delete m_saveGameWindow;
}

void LoadGameScreen::execUpdate(const sf::Time& frameTime) {
	if (g_inputController->isKeyActive(Key::Escape)) {
		onBack();
		return;
	}
	if (m_loadGame) {
		setNextScreen(new LoadingScreen(m_characterCore));
		return;
	}
	updateObjects(_Button, frameTime);
	updateObjects(_Form, frameTime);
	updateTooltipText(frameTime);
	m_saveGameWindow->update(frameTime);
	if (!getObjects(_Form)->empty()) return;
	if (m_saveGameWindow->isChosen()) {
		onLoadSaveGame();
	}
}

void LoadGameScreen::render(sf::RenderTarget& renderTarget) {
	renderTarget.setView(renderTarget.getDefaultView());
	renderTarget.draw(*m_title);
	renderTooltipText(renderTarget);
	m_saveGameWindow->render(renderTarget);
	renderObjects(_Button, renderTarget);
	renderObjects(_Form, renderTarget);
}

void LoadGameScreen::execOnEnter() {
	// text
	m_title = new sf::Text();
	m_title->setFont(*g_resourceManager->getFont(GlobalResource::FONT_TTF_DIALOGUE));
	std::string line = g_textProvider->getText("LoadGame");
	m_title->setString(sf::String::fromUtf8(line.begin(),line.end()));
	m_title->setCharacterSize(24);
	m_title->setPosition(sf::Vector2f((WINDOW_WIDTH - m_title->getLocalBounds().width) / 2.f, 25.f));

	const auto buttonWidth = 200.f;
	const auto buttonHeight = 50.f;
	const auto marginX = 60.f;
	const auto marginY = WINDOW_HEIGHT - 80.f;
	const auto buttonSpaceWidth = WINDOW_WIDTH - 2 * marginX;
	const auto buttonSpacing = (buttonSpaceWidth - 3 * buttonWidth) / 2.f;

	// add buttons
	Button* button = new Button(sf::FloatRect(marginX, marginY, buttonWidth, buttonHeight), GUIOrnamentStyle::SMALL);
	button->setText("Back");
	button->setOnClick(std::bind(&LoadGameScreen::onBack, this));
	button->setGamepadKey(Key::Escape);
	addObject(button);

	m_deleteSaveGameButton = new Button(sf::FloatRect(marginX + buttonWidth + buttonSpacing, marginY, buttonWidth, buttonHeight), GUIOrnamentStyle::SMALL);
	m_deleteSaveGameButton->setText("Delete");
	m_deleteSaveGameButton->setOnClick(std::bind(&LoadGameScreen::onDeleteSaveGame, this));
	m_deleteSaveGameButton->setGamepadKey(Key::Attack);
	addObject(m_deleteSaveGameButton);

	m_loadSaveGameButton = new Button(sf::FloatRect(marginX + 2 * (buttonWidth + buttonSpacing), marginY, buttonWidth, buttonHeight), GUIOrnamentStyle::SMALL);
	m_loadSaveGameButton->setText("Load");
	m_loadSaveGameButton->setOnClick(std::bind(&LoadGameScreen::onLoadSaveGame, this));
	m_loadSaveGameButton->setGamepadKey(Key::Confirm);
	addObject(m_loadSaveGameButton);

	// savegame window
	m_saveGameWindow = new SaveGameWindow();
	setAllButtonsEnabled(true);
}

void LoadGameScreen::execOnExit() {
	delete m_title;
	delete m_newCharacterCore;
}

void LoadGameScreen::setAllButtonsEnabled(bool value) {
	Screen::setAllButtonsEnabled(value);
	bool empty = m_saveGameWindow->getChosenFilename().empty();
	m_loadSaveGameButton->setEnabled(value && !empty);
	m_deleteSaveGameButton->setEnabled(value && !empty);
	m_saveGameWindow->setEnabled(value);
}

// <<< agents for yes or no form >>>

void LoadGameScreen::onNo() {
	m_yesOrNoForm = nullptr;
	delete m_newCharacterCore;
	m_newCharacterCore = nullptr;
	setAllButtonsEnabled(true);
}

void LoadGameScreen::onLoadGame() {
	m_yesOrNoForm = nullptr;
	delete m_characterCore;
	m_characterCore = m_newCharacterCore;
	m_newCharacterCore = nullptr;
	m_loadGame = true;
}

void LoadGameScreen::onYesDeleteSaveGame() {
	m_yesOrNoForm = nullptr;
	if (remove(m_saveGameWindow->getChosenFilename().c_str()) == 0) {
		setTooltipText("SavegameDeleted", COLOR_LIGHT_PURPLE, true);
	}
	else {
		g_logger->logError("SaveGameScreen", "Savegame could not be deleted");
		setNegativeTooltip("OperationFailed");
	}
	m_saveGameWindow->reload();
	setAllButtonsEnabled(true);
}

void LoadGameScreen::onDeleteSaveGame() {
	float width = 450;
	float height = 200;
	m_yesOrNoForm = new YesOrNoForm(sf::FloatRect(0.5f * (WINDOW_WIDTH - width), 0.5f * (WINDOW_HEIGHT - height), width, height));
	m_yesOrNoForm->setMessage("QuestionDeleteSaveGame");
	m_yesOrNoForm->setOnNoClicked(std::bind(&LoadGameScreen::onNo, this));
	m_yesOrNoForm->setOnYesClicked(std::bind(&LoadGameScreen::onYesDeleteSaveGame, this));
	addObject(m_yesOrNoForm);
	setAllButtonsEnabled(false);
}

void LoadGameScreen::onLoadSaveGame() {
	if (m_characterCore == nullptr || m_characterCore->isAutosave()) {
		delete m_characterCore;
		// load a savegame
		m_characterCore = new CharacterCore();
		if (!(m_characterCore->load(m_saveGameWindow->getChosenFilename()))) {
			std::string errormsg = std::string(m_saveGameWindow->getChosenFilename()) + ": save file corrupted!";
			g_resourceManager->setError(ErrorID::Error_dataCorrupted, errormsg);
		}
		setNextScreen(new LoadingScreen(m_characterCore));
	}
	else {
		m_newCharacterCore = new CharacterCore();
		if (!(m_newCharacterCore->load(m_saveGameWindow->getChosenFilename()))) {
			std::string errormsg = std::string(m_saveGameWindow->getChosenFilename()) + ": save file corrupted!";
			g_resourceManager->setError(ErrorID::Error_dataCorrupted, errormsg);
		}
		float width = 450;
		float height = 200;
		m_yesOrNoForm = new YesOrNoForm(sf::FloatRect(0.5f * (WINDOW_WIDTH - width), 0.5f * (WINDOW_HEIGHT - height), width, height));
		m_yesOrNoForm->setMessage("QuestionLoadGame");
		m_yesOrNoForm->setOnNoClicked(std::bind(&LoadGameScreen::onNo, this));
		m_yesOrNoForm->setOnYesClicked(std::bind(&LoadGameScreen::onLoadGame, this));
		addObject(m_yesOrNoForm);
		setAllButtonsEnabled(false);
	}
}

void LoadGameScreen::onBack() {
	setNextScreen(new MenuScreen(m_characterCore));
}