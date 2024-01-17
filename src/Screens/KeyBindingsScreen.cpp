#include "Screens/KeyBindingsScreen.h"
#include "Screens/OptionsScreen.h"
#include "Screens/MenuScreen.h"
#include "FileIO/ConfigurationWriter.h"
#include "Screens/GamepadKeyBindingsScreen.h"
#include "Screens/KeyboardKeyBindingsScreen.h"
#include "GUI/ButtonGroup.h"

KeyBindingsScreen::KeyBindingsScreen(CharacterCore* core) : Screen(core) {
}

void KeyBindingsScreen::execUpdate(const sf::Time& frameTime) {
	updateObjects(_Button, frameTime);
}

void KeyBindingsScreen::render(sf::RenderTarget& renderTarget) {
	renderTarget.setView(renderTarget.getDefaultView());
	renderTarget.draw(*m_title);

	renderObjects(_Button, renderTarget);
}

void KeyBindingsScreen::execOnEnter() {

	const float buttonWidth = 200.f;
	const float buttonHeight = 50.f;
	const float marginX = 60.f;
	const float marginY = WINDOW_HEIGHT - 80.f;
	const float buttonSpaceWidth = WINDOW_WIDTH - 2 * marginX;
	const float buttonSpacing = (buttonSpaceWidth - 4 * buttonWidth) / 3.f;
	const float buttonX = (WINDOW_WIDTH - (2 * buttonWidth + buttonSpacing)) * 0.5f;
	const float buttonY = WINDOW_HEIGHT * 0.5f - buttonHeight;
	
	// title
	m_title = new sf::Text();
	m_title->setFont(*g_resourceManager->getFont(GlobalResource::FONT_TTF_DIALOGUE));
	std::string line = g_textProvider->getText("KeyBindings");
	m_title->setString(sf::String::fromUtf8(line.begin(),line.end()));
	m_title->setCharacterSize(24);
	m_title->setPosition(sf::Vector2f((WINDOW_WIDTH - m_title->getLocalBounds().width) / 2.f, 25.f));

	// back
	auto button = new Button(sf::FloatRect(marginX, marginY, buttonWidth, buttonHeight), GUIOrnamentStyle::SMALL);
	button->setText("Back");
	button->setGamepadKey(Key::Escape);
	button->setOnClick(std::bind(&KeyBindingsScreen::onBack, this));
	addObject(button);

	ButtonGroup* buttonGroup = new ButtonGroup(2);

	// keyboard keybindings
	button = new Button(sf::FloatRect(buttonX, buttonY, buttonWidth, buttonHeight), GUIOrnamentStyle::SMALL);
	button->setText("Keyboard");
	button->setOnClick(std::bind(&KeyBindingsScreen::onKeyboardKeyBindings, this));
	buttonGroup->addButton(button);
	
	// gamepad keybindings
	button = new Button(sf::FloatRect(buttonX + buttonSpacing + buttonWidth, buttonY, buttonWidth, buttonHeight), GUIOrnamentStyle::SMALL);
	button->setText("Gamepad");
	button->setOnClick(std::bind(&KeyBindingsScreen::onGamepadKeyBindings, this));
	buttonGroup->addButton(button);

	addObject(buttonGroup);
}

void KeyBindingsScreen::execOnExit() {
	delete m_title;
}

void KeyBindingsScreen::onBack() {
	setNextScreen(new MenuScreen(m_characterCore));
}

void KeyBindingsScreen::onGamepadKeyBindings() {
	setNextScreen(new GamepadKeyBindingsScreen(m_characterCore));
}

void KeyBindingsScreen::onKeyboardKeyBindings() {
	setNextScreen(new KeyboardKeyBindingsScreen(m_characterCore));
}
