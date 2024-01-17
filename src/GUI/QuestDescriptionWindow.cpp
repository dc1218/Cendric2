#include "GUI/QuestDescriptionWindow.h"

const float QuestDescriptionWindow::WIDTH = 340.f;

QuestDescriptionWindow::QuestDescriptionWindow(const CharacterCore* core) : Window(
	sf::FloatRect(0.f, 0.f, WIDTH, WIDTH),
	GUIOrnamentStyle::LARGE,
	GUIConstants::MAIN_COLOR,
	GUIConstants::ORNAMENT_COLOR) {
	m_core = core;

	m_titleText.setFont(*g_resourceManager->getFont(GlobalResource::FONT_TTF_DIALOGUE));
	m_titleText.setCharacterSize(GUIConstants::CHARACTER_SIZE_M);
	m_titleText.setColor(COLOR_WHITE);

	m_descriptionText.setFont(*g_resourceManager->getFont(GlobalResource::FONT_TTF_DIALOGUE));
	m_descriptionText.setCharacterSize(GUIConstants::CHARACTER_SIZE_S);
	m_descriptionText.setColor(COLOR_LIGHT_GREY);

	m_stateText.setFont(*g_resourceManager->getFont(GlobalResource::FONT_TTF_DIALOGUE));
	m_stateText.setCharacterSize(GUIConstants::CHARACTER_SIZE_S);
	m_stateText.setColor(COLOR_LIGHT_PURPLE);

	setPosition(sf::Vector2f(GUIConstants::LEFT + GUIConstants::TEXT_OFFSET + QuestLog::WIDTH, GUIConstants::TOP));
}

void QuestDescriptionWindow::reload(const std::string& questID) {
	m_collectiblesTexts.clear();
	m_conditionTexts.clear();
	m_targetsTexts.clear();
	const QuestData* data = m_core->getQuestData(questID);
	if (data == nullptr) {
		std::string line = g_textProvider->getText("Unknown");
		m_titleText.setString(sf::String::fromUtf8(line.begin(),line.end()));
		m_descriptionText.setString(sf::String::fromUtf8(line.begin(),line.end()));
		setPosition(getPosition());
		return;
	}

	std::string title = g_textProvider->getCroppedText(
		questID, "quest",
		GUIConstants::CHARACTER_SIZE_M,
		static_cast<int>(WIDTH - 2 * GUIConstants::TEXT_OFFSET));
	m_titleText.setString(sf::String::fromUtf8(title.begin(),title.end()));
	

	std::string description = g_textProvider->getCroppedText(
		questID, "quest_desc",
		GUIConstants::CHARACTER_SIZE_S,
		static_cast<int>(WIDTH - 2 * GUIConstants::TEXT_OFFSET));
	
	if (contains(m_core->getData().questDescriptionProgress, questID)) {
		const std::set<int>& unlockedDescriptions = m_core->getData().questDescriptionProgress.at(questID);
		for (auto& it : unlockedDescriptions) {
			description.append("\n\n");
			description.append(g_textProvider->getCroppedText(
				questID, "quest_desc_" + std::to_string(it),
				GUIConstants::CHARACTER_SIZE_S,
				static_cast<int>(WIDTH - 2 * GUIConstants::TEXT_OFFSET)));
		}
	}

	m_descriptionText.setString(sf::String::fromUtf8(description.begin(),description.end()));

	QuestState currentState = m_core->getQuestState(questID);
	std::string state = g_textProvider->getCroppedText(
		EnumNames::getQuestStateName(currentState),
		GUIConstants::CHARACTER_SIZE_S,
		static_cast<int>(WIDTH - 2 * GUIConstants::TEXT_OFFSET));

	if (currentState == QuestState::Completed) {
		m_stateText.setString(sf::String::fromUtf8(state.begin(),state.end()));
		m_stateText.setColor(COLOR_GOOD);
	}
	else if (currentState == QuestState::Failed) {
		m_stateText.setString(sf::String::fromUtf8(state.begin(),state.end()));
		m_stateText.setColor(COLOR_BAD);
	}
	else {
		m_stateText.setString("");
	}

	for (auto& it : data->targets) {
		std::string target = "";
		target.append(g_textProvider->getText(it.first, "enemy"));
		target.append(": ");

		int goal = it.second;
		int progress = (m_core->getQuestState(questID) == QuestState::Completed) ?
		goal :
			 m_core->getNumberOfTargetsKilled(questID, it.first);

		target.append(std::to_string(progress) + "/" + std::to_string(goal));
		target = g_textProvider->getCroppedString(target, GUIConstants::CHARACTER_SIZE_S, static_cast<int>(WIDTH - 2 * GUIConstants::TEXT_OFFSET));

		sf::Text targetText;
		targetText.setFont(*g_resourceManager->getFont(GlobalResource::FONT_TTF_DIALOGUE));
		targetText.setCharacterSize(GUIConstants::CHARACTER_SIZE_S);
		targetText.setString(sf::String::fromUtf8(target.begin(),target.end()));
		targetText.setColor(progress == 0 ? COLOR_BAD : progress >= goal ? COLOR_GOOD : COLOR_NEUTRAL);
		
		m_targetsTexts.push_back(targetText);
	}

	for (auto& it : data->collectibles) {
		std::string collectible = "";
		collectible.append(g_textProvider->getText(it.first, "item"));
		collectible.append(": ");
		int progress = 0;
		int goal = it.second;
		if (m_core->getQuestState(questID) == QuestState::Completed) {
			progress = goal;
		}
		else if (contains(m_core->getData().items, it.first)) {
			progress = m_core->getData().items.at(it.first);
		}
		collectible.append(std::to_string(progress) + "/" + std::to_string(goal));
		collectible = g_textProvider->getCroppedString(collectible, GUIConstants::CHARACTER_SIZE_S, static_cast<int>(WIDTH - 2 * GUIConstants::TEXT_OFFSET));

		sf::Text collectibleText;
		collectibleText.setFont(*g_resourceManager->getFont(GlobalResource::FONT_TTF_DIALOGUE));
		collectibleText.setCharacterSize(GUIConstants::CHARACTER_SIZE_S);
		collectibleText.setString(sf::String::fromUtf8(collectible.begin(),collectible.end()));
		collectibleText.setColor(progress == 0 ? COLOR_BAD : progress >= goal ? COLOR_GOOD : COLOR_NEUTRAL);
		m_collectiblesTexts.push_back(collectibleText);
	}

	for (auto& it : data->conditions) {
		sf::Text conditionText;
		conditionText.setFont(*g_resourceManager->getFont(GlobalResource::FONT_TTF_DIALOGUE));

		std::string condition = "";
		condition.append(g_textProvider->getText(it, "quest_condition"));
		condition.append(": ");

		if ((m_core->getQuestState(questID) == QuestState::Completed) ||
			(contains(m_core->getData().questConditionProgress, questID) &&
			contains(m_core->getData().questConditionProgress.at(questID), it))) {
			condition.append(g_textProvider->getText("Done"));
			conditionText.setColor(COLOR_GOOD);
		}
		else {
			condition.append(g_textProvider->getText("Pending"));
			conditionText.setColor(COLOR_BAD);
		}

		condition = g_textProvider->getCroppedString(condition, GUIConstants::CHARACTER_SIZE_S, static_cast<int>(WIDTH - 2 * GUIConstants::TEXT_OFFSET));

		conditionText.setCharacterSize(GUIConstants::CHARACTER_SIZE_S);
		conditionText.setString(sf::String::fromUtf8(condition.begin(),condition.end()));
		m_conditionTexts.push_back(conditionText);
	}

	setPosition(getPosition());
}

void QuestDescriptionWindow::show() {
	m_isVisible = true;
}

void QuestDescriptionWindow::hide() {
	m_isVisible = false;
}

void QuestDescriptionWindow::setPosition(const sf::Vector2f& position) {
	Window::setPosition(position);
	sf::Vector2f pos(position);
	pos.y += GUIConstants::TEXT_OFFSET;
	pos.x += GUIConstants::TEXT_OFFSET;

	m_titleText.setPosition(position.x + ((WIDTH - m_titleText.getLocalBounds().width) / 2.f), pos.y);
	pos.y += m_titleText.getLocalBounds().height + GUIConstants::TEXT_OFFSET;

	m_descriptionText.setPosition(pos);

	pos.y += GUIConstants::TEXT_OFFSET + m_descriptionText.getLocalBounds().height;

	if (!m_stateText.getString().isEmpty()) {
		m_stateText.setPosition(pos.x, pos.y);
		pos.y += GUIConstants::TEXT_OFFSET + m_stateText.getLocalBounds().height;
	}

	for (auto& it : m_targetsTexts) {
		it.setPosition(pos.x, pos.y);
		pos.y += 2 * it.getLocalBounds().height;
	}
	for (auto& it : m_collectiblesTexts) {
		it.setPosition(pos.x, pos.y);
		pos.y += 2 * it.getLocalBounds().height;
	}
	for (auto& it : m_conditionTexts) {
		it.setPosition(pos.x, pos.y);
		pos.y += 2 * it.getLocalBounds().height;
	}
	pos.y += GUIConstants::TEXT_OFFSET;

	setHeight(pos.y - position.y);
}

void QuestDescriptionWindow::render(sf::RenderTarget& renderTarget) {
	if (!m_isVisible) return;
	Window::render(renderTarget);
	renderTarget.draw(m_titleText);
	renderTarget.draw(m_descriptionText);
	renderTarget.draw(m_stateText);
	for (auto& it : m_targetsTexts) {
		renderTarget.draw(it);
	}
	for (auto& it : m_collectiblesTexts) {
		renderTarget.draw(it);
	}
	for (auto& it : m_conditionTexts) {
		renderTarget.draw(it);
	}
}