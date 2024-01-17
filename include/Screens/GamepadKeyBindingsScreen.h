#pragma once

#include "global.h"
#include "Screen.h"

#include "GUI/ButtonGroup.h"
#include "GUI/SlicedSprite.h"
#include "GUI/ScrollBar.h"
#include "Enums/EnumNames.h"

class ScrollBar;
class ScrollHelper;
class Button;

class GamepadKeyBindingsScreen final : public Screen {
public:
	GamepadKeyBindingsScreen(CharacterCore* core);

	void execUpdate(const sf::Time& frameTime) override;
	void render(sf::RenderTarget& renderTarget) override;

	void execOnEnter() override;
	void execOnExit() override;

private:
	void reload();
	// returns true if succeeded, false if not allowed
	// it also reloads the key map if it was successful
	bool trySetKeyBinding(Key key, GamepadInput input);

	void calculateEntryPositions();

	// agents for the buttons
	void onBack();
	void onApply();
	void onUseDefault();
	void onReset();
	bool checkSet(Key key);

private:
	sf::Text* m_title = nullptr;

	Key m_selectedKey = Key::VOID;
	bool m_lockInput = false;

	ButtonGroup* m_keyButtonGroup;
	std::map<Key, std::pair<Button*, GamepadInput>> m_keyButtons;
	std::map<Key, sf::Text*> m_keyTexts;

	std::map<Key, GamepadInput> m_selectedKeys;

	// scrolling
	SlicedSprite m_scrollWindow;
	ScrollBar* m_scrollBar = nullptr;
	ScrollHelper* m_scrollHelper = nullptr;

	static const float WINDOW_MARGIN;

	static const float TOP;
	static const float LEFT;
	static const float WIDTH;
	static const float HEIGHT;
};