#pragma once

#include "global.h"
#include "GUI/Window.h"
#include "TextProvider.h"
#include "Structs/QuestData.h"
#include "CharacterCore.h"
#include "GUI/GUIConstants.h"
#include "GUI/QuestLog.h"

class QuestDescriptionWindow final : public Window {
public:
	QuestDescriptionWindow(const CharacterCore* core);

	void reload(const std::string& questID);
	void render(sf::RenderTarget& renderTarget) override;
	void setPosition(const sf::Vector2f& position) override;

	void show();
	void hide();

	static const float WIDTH;

private:
	const CharacterCore* m_core;

	bool m_isVisible = false;

	sf::Text m_titleText;
	sf::Text m_descriptionText;
	sf::Text m_stateText;

	std::vector<sf::Text> m_conditionTexts;
	std::vector<sf::Text> m_targetsTexts;
	std::vector<sf::Text> m_collectiblesTexts;
};