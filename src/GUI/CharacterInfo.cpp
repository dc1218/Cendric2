#include "GUI/CharacterInfo.h"
#include "GUI/HintDescriptionWindow.h"
#include "GUI/ScrollBar.h"
#include "GUI/ScrollHelper.h"
#include "GlobalResource.h"
#include "Screens/WorldScreen.h"

static const std::string GODMODE_CHEAT = "god";

const float CharacterInfo::TOP = GUIConstants::TOP;
const float CharacterInfo::LEFT = GUIConstants::LEFT;
const float CharacterInfo::WIDTH = 500.f;
const float CharacterInfo::HEIGHT = GUIConstants::GUI_WINDOW_HEIGHT;

const sf::Vector2f CharacterInfo::BUTTON_SIZE = sf::Vector2f(120.f, 40.f);

const int CharacterInfo::ENTRY_COUNT = 16;
const int CharacterInfo::MAX_ENTRY_LENGTH_CHARACTERS = 34;
const float CharacterInfo::MAX_ENTRY_LENGTH = static_cast<float>(MAX_ENTRY_LENGTH_CHARACTERS) * GUIConstants::CHARACTER_SIZE_M;

const float CharacterInfo::WINDOW_MARGIN = 6.f;

const float CharacterInfo::SCROLL_WINDOW_WIDTH = MAX_ENTRY_LENGTH + 4 * WINDOW_MARGIN + ScrollBar::WIDTH;
const float CharacterInfo::SCROLL_WINDOW_HEIGHT = ENTRY_COUNT * GUIConstants::CHARACTER_SIZE_M + (ENTRY_COUNT - 1) * GUIConstants::CHARACTER_SIZE_M + 2 * GUIConstants::CHARACTER_SIZE_M + 2 * WINDOW_MARGIN;

const float CharacterInfo::SCROLL_WINDOW_TOP = GUIConstants::GUI_TABS_TOP + 2 * WINDOW_MARGIN + BUTTON_SIZE.y;
const float CharacterInfo::SCROLL_WINDOW_LEFT = 0.5f * (WIDTH - SCROLL_WINDOW_WIDTH);

const std::vector<std::string> CharacterInfo::LABELS = {
	"Health",
	"HealthRegeneration",
	"CriticalHitChance",
	"Haste",
	"Heal",
	"",
	"PhysicalDamage",
	"FireDamage",
	"IceDamage",
	"ShadowDamage",
	"LightDamage",
	"",
	"Armor",
	"FireResistance",
	"IceResistance",
	"ShadowResistance",
	"LightResistance",
};

CharacterInfo::CharacterInfo(WorldScreen* screen, const AttributeData* attributes) {
	m_screen = screen;
	m_core = screen->getCharacterCore();
	m_attributes = attributes;
	m_entries = &m_hintEntries;

	sf::Text name;
	name.setFont(*g_resourceManager->getFont(GlobalResource::FONT_TTF_DIALOGUE));
	name.setCharacterSize(GUIConstants::CHARACTER_SIZE_M);
	name.setColor(COLOR_WHITE);
	name.setString("Placeholder");

	sf::Sprite icon;
	icon.setTexture(*g_resourceManager->getTexture(GlobalResource::TEX_GUI_CHARACTERINFO_ICONS));

	float yOffset = GUIConstants::TOP + GUIConstants::GUI_TABS_TOP + 2 * WINDOW_MARGIN + BUTTON_SIZE.y + GUIConstants::CHARACTER_SIZE_M;
	float textHeight = name.getLocalBounds().height;
	float dy = textHeight + GUIConstants::CHARACTER_SIZE_M;

	for (size_t i = 0; i < LABELS.size(); ++i) {
		if (LABELS[i].empty()) {
			yOffset += dy * 0.8f;
			continue;
		}

		name.setPosition(LEFT + 2 * GUIConstants::TEXT_OFFSET, yOffset);
		std::string line = g_textProvider->getText(LABELS[i]) + ":";
		name.setString(sf::String::fromUtf8(line.begin(),line.end()));
		m_nameTexts.push_back(name);

		icon.setTextureRect(sf::IntRect(0, static_cast<int>(i) * 18, 18, 18));
		icon.setPosition(sf::Vector2f(LEFT + WIDTH - 160.f, yOffset + 0.5f * (textHeight - 18)));
		m_statIcons.push_back(icon);

		yOffset += dy;
	}

	// init window
	sf::FloatRect box(LEFT, TOP, WIDTH, HEIGHT);
	m_window = new Window(box,
		GUIOrnamentStyle::LARGE,
		GUIConstants::MAIN_COLOR,
		GUIConstants::ORNAMENT_COLOR);

	m_window->addCloseButton(std::bind(&CharacterInfo::hide, this));

	delete m_descriptionWindow;
	m_descriptionWindow = new HintDescriptionWindow(m_core);

	// init hint tab section
	m_scrollWindow = SlicedSprite(g_resourceManager->getTexture(GlobalResource::TEX_GUI_ORNAMENT_NONE), COLOR_WHITE, SCROLL_WINDOW_WIDTH, SCROLL_WINDOW_HEIGHT);
	m_scrollWindow.setPosition(sf::Vector2f(LEFT + SCROLL_WINDOW_LEFT, TOP + SCROLL_WINDOW_TOP));

	m_scrollBar = new ScrollBar(SCROLL_WINDOW_HEIGHT, m_window);
	m_scrollBar->setPosition(sf::Vector2f(LEFT + SCROLL_WINDOW_LEFT + SCROLL_WINDOW_WIDTH - ScrollBar::WIDTH, TOP + SCROLL_WINDOW_TOP));

	sf::FloatRect scrollBox(LEFT + SCROLL_WINDOW_LEFT, TOP + SCROLL_WINDOW_TOP, SCROLL_WINDOW_WIDTH, SCROLL_WINDOW_HEIGHT);
	m_scrollHelper = new ScrollHelper(scrollBox);

	reload();

	// init tab bar
	int nTabs = 3;
	float barWidth = nTabs * BUTTON_SIZE.x;
	float barHeight = BUTTON_SIZE.y;
	float x = GUIConstants::LEFT + 0.5f * (m_window->getSize().x - barWidth);
	float y = GUIConstants::TOP + GUIConstants::GUI_TABS_TOP;

	m_tabBar = new TabBar();
	m_tabBar->init(sf::FloatRect(x, y, barWidth, barHeight), nTabs);
	m_tabBar->getTabButton(0)->setText("Stats");
	m_tabBar->getTabButton(1)->setText("Reputation");
	m_tabBar->getTabButton(2)->setText("Hints");
	for (int i = 0; i < nTabs; ++i) {
		m_tabBar->getTabButton(i)->setCharacterSize(GUIConstants::CHARACTER_SIZE_S);
	}

	// init title
	m_title.setFont(*g_resourceManager->getFont(GlobalResource::FONT_TTF_DIALOGUE));
	m_title.setPosition(sf::Vector2f(GUIConstants::LEFT + GUIConstants::TEXT_OFFSET, GUIConstants::TOP + GUIConstants::TEXT_OFFSET));
	m_title.setColor(COLOR_WHITE);
	m_title.setCharacterSize(GUIConstants::CHARACTER_SIZE_M);
	std::string line = g_textProvider->getText("CharacterInfo");
	m_title.setString(sf::String::fromUtf8(line.begin(),line.end()));
	m_title.setPosition(
		m_window->getPosition().x +
		m_window->getSize().x / 2 -
		m_title.getLocalBounds().width / 2, m_title.getPosition().y);
}

CharacterInfo::~CharacterInfo() {
	delete m_window;
	delete m_tabBar;
	delete m_descriptionWindow;
	delete m_scrollBar;
	delete m_scrollHelper;
	CLEAR_VECTOR(m_hintEntries);
}

bool CharacterInfo::isVisible() const {
	return m_isVisible;
}

void CharacterInfo::update(const sf::Time& frameTime) {
	if (!m_isVisible) return;

	m_scrollBar->update(frameTime);
	updateTabBar(frameTime);
	if (m_tabBar->getActiveTabIndex() == 2) {
		updateSelection(frameTime);
	}
	updateSelectableWindow();
	m_window->update(frameTime);
	calculateEntryPositions();
	checkReload();
}

void CharacterInfo::updateSelectableWindow() {
	if (!isWindowSelected() || g_inputController->isActionLocked()) {
		return;
	}

	if (g_inputController->isJustLeft()) {
		setLeftWindowSelected();
	}
}

bool CharacterInfo::isEntryInvisible(const ScrollEntry* entry) const {
	auto const pos = entry->getPosition();
	return pos.y < TOP + SCROLL_WINDOW_TOP ||
		pos.y + GUIConstants::CHARACTER_SIZE_M > TOP + SCROLL_WINDOW_TOP + SCROLL_WINDOW_HEIGHT;
}

void CharacterInfo::updateTabBar(const sf::Time& frameTime) {
	int lastIndex = m_tabBar->getActiveTabIndex();
	m_tabBar->update(frameTime);
	int newIndex = m_tabBar->getActiveTabIndex();
	if (lastIndex != newIndex) {
		if (newIndex == 2) {
			showDescription(m_selectedHintKey);
		}
		else {
			hideDescription();
		}
	}
}

void CharacterInfo::checkReload() {
	if (!m_isReloadNeeded) {
		return;
	}

	reload();
	m_isReloadNeeded = false;
}

void CharacterInfo::showDescription(const std::string& hintKey) {
	m_descriptionWindow->reload(hintKey);
	m_descriptionWindow->show();
}

void CharacterInfo::hideDescription() {
	m_descriptionWindow->hide();
}

void CharacterInfo::updateWindowSelected() {
	m_tabBar->setGamepadEnabled(isWindowSelected());
}

void CharacterInfo::render(sf::RenderTarget& target) {
	if (!m_isVisible) return;

	m_window->render(target);
	target.draw(m_title);
	m_tabBar->render(target);

	if (m_tabBar->getActiveTabIndex() == 0) {
		for (auto& text : m_nameTexts) {
			target.draw(text);
		}
		for (auto& text : m_attributeTexts) {
			target.draw(text);
		}
		for (auto& icon : m_statIcons) {
			target.draw(icon);
		}
	}
	else if (m_tabBar->getActiveTabIndex() == 1) {
		target.draw(m_guild);
		target.draw(m_guildSprite);
		for (auto& text : m_reputationTexts) {
			target.draw(text);
		}
	}
	else {
		for (auto entry : m_hintEntries) {
			entry->render(m_scrollHelper->texture);
		}
		m_scrollHelper->render(target);

		m_descriptionWindow->render(target);

		target.draw(m_scrollWindow);
		m_scrollBar->render(target);
	}
}

void CharacterInfo::notifyChange() {
	m_isReloadNeeded = true;
}

void CharacterInfo::reload() {
	updateAttributes();
	updateReputation();
	updateHints();
}

void CharacterInfo::updateAttributes() {
	auto currentAttributes = m_attributes == nullptr ? m_core->getTotalAttributes() : *m_attributes;

	m_attributeTexts.clear();

	std::vector<std::string> attributes;

	// health
	std::string health = std::to_string(currentAttributes.currentHealthPoints) +
		"/" +
		std::to_string(currentAttributes.maxHealthPoints);
	attributes.push_back(health);

	// health regeneration
	std::string healthRegen = std::to_string(currentAttributes.healthRegenerationPerS)
		+ "/s";
	attributes.push_back(healthRegen);

	// crit
	std::string crit = std::to_string(currentAttributes.criticalHitChance) +
		"%";
	attributes.push_back(crit);

	// cooldown reduction
	attributes.push_back(std::to_string(currentAttributes.haste));

	// healing power
	attributes.push_back(std::to_string(currentAttributes.heal));
	attributes.push_back("");

	// dmg 
	attributes.push_back(std::to_string(currentAttributes.damagePhysical));
	attributes.push_back(std::to_string(currentAttributes.damageFire));
	attributes.push_back(std::to_string(currentAttributes.damageIce));
	attributes.push_back(std::to_string(currentAttributes.damageShadow));
	attributes.push_back(std::to_string(currentAttributes.damageLight));

	attributes.emplace_back("");

	// resistance
	attributes.push_back(std::to_string(currentAttributes.resistancePhysical));
	attributes.push_back(std::to_string(currentAttributes.resistanceFire));
	attributes.push_back(std::to_string(currentAttributes.resistanceIce));
	attributes.push_back(std::to_string(currentAttributes.resistanceShadow));
	attributes.push_back(std::to_string(currentAttributes.resistanceLight));

	sf::Text attributeText;
	attributeText.setFont(*g_resourceManager->getFont(GlobalResource::FONT_TTF_DIALOGUE));
	attributeText.setCharacterSize(GUIConstants::CHARACTER_SIZE_M);
	attributeText.setColor(COLOR_WHITE);
	attributeText.setString("Placeholder");

	float yOffset = GUIConstants::TOP + GUIConstants::GUI_TABS_TOP + 2 * WINDOW_MARGIN + BUTTON_SIZE.y + GUIConstants::CHARACTER_SIZE_M;
	float textHeight = attributeText.getLocalBounds().height;
	float dy = textHeight + GUIConstants::CHARACTER_SIZE_M;

	for (size_t i = 0; i < attributes.size(); ++i) {
		if (attributes[i].empty()) {
			yOffset += dy * 0.8f;
			continue;
		}

		attributeText.setPosition(LEFT + WIDTH - 136.f, yOffset);
		std::string line = attributes[i];
		attributeText.setString(sf::String::fromUtf8(line.begin(),line.end()));
		m_attributeTexts.push_back(attributeText);

		yOffset += dy;
	}
}

void CharacterInfo::updateReputation() {
	m_reputationTexts.clear();

	float yOffset = GUIConstants::TOP + GUIConstants::GUI_TABS_TOP + 2 * WINDOW_MARGIN + BUTTON_SIZE.y + GUIConstants::CHARACTER_SIZE_M;
	float xOffset = 3 * GUIConstants::TEXT_OFFSET + GUIConstants::LEFT;

	std::string currentGuild;
	currentGuild.append(g_textProvider->getText("Guild"));
	currentGuild.append(": ");
	currentGuild.append(g_textProvider->getText(EnumNames::getFractionIDName(m_core->getData().guild)));
	m_guild.setString(currentGuild);
	m_guild.setPosition(LEFT + (WIDTH - m_guild.getLocalBounds().width) * 0.5f, yOffset);
	m_guild.setCharacterSize(GUIConstants::CHARACTER_SIZE_M);

	yOffset += 2 * GUIConstants::CHARACTER_SIZE_M;

	auto* tex = g_resourceManager->getTexture(GlobalResource::TEX_GUILD_ICONS);
	int texHeight = 100;
	if (tex != nullptr)
		m_guildSprite.setTexture(*tex);
	m_guildSprite.setPosition(LEFT + (WIDTH - 100) * 0.5f, yOffset);
	if (m_core->getData().guild == FractionID::VOID) {
		m_guildSprite.setTextureRect(sf::IntRect());
	}
	else {
		m_guildSprite.setTextureRect(sf::IntRect(texHeight * (static_cast<int>(m_core->getData().guild) - 1), 0, texHeight, texHeight));
		yOffset += texHeight;
	}

	yOffset += 2 * GUIConstants::CHARACTER_SIZE_M;

	if (m_core->getData().reputationProgress.empty()) {
		sf::Text noRep;
		noRep.setFont(*g_resourceManager->getFont(GlobalResource::FONT_TTF_DIALOGUE));
		noRep.setCharacterSize(GUIConstants::CHARACTER_SIZE_M);
		noRep.setPosition(xOffset, yOffset);
		noRep.setColor(COLOR_LIGHT_PURPLE);
		std::string line = g_textProvider->getCroppedText("NoReputation", GUIConstants::CHARACTER_SIZE_M, static_cast<int>(m_window->getSize().x - 6 * GUIConstants::TEXT_OFFSET));
		noRep.setString(sf::String::fromUtf8(line.begin(),line.end()));
		m_reputationTexts.push_back(noRep);
		return;
	}

	for (auto const& rep : m_core->getData().reputationProgress) {
		sf::Text title;
		title.setFont(*g_resourceManager->getFont(GlobalResource::FONT_TTF_DIALOGUE));
		std::string line = g_textProvider->getText(EnumNames::getFractionIDName(rep.first)) + ": " + std::to_string(rep.second);
		title.setString(sf::String::fromUtf8(line.begin(),line.end()));
		title.setCharacterSize(GUIConstants::CHARACTER_SIZE_M);
		title.setPosition(xOffset, yOffset);
		title.setColor(COLOR_WHITE);
		m_reputationTexts.push_back(title);

		yOffset += title.getLocalBounds().height * (3 / 2.f);

		sf::Text subtitle;
		subtitle.setFont(*g_resourceManager->getFont(GlobalResource::FONT_TTF_DIALOGUE));
		std::string key = EnumNames::getFractionIDName(rep.first) + "_" +
			(rep.second >= 75 ? "100" : rep.second >= 50 ? "75" : rep.second >= 25 ? "50" : "25");
		line = g_textProvider->getCroppedText(key, GUIConstants::CHARACTER_SIZE_M, static_cast<int>(m_window->getSize().x - 6 * GUIConstants::TEXT_OFFSET));
		subtitle.setString(sf::String::fromUtf8(line.begin(),line.end()));
		subtitle.setCharacterSize(GUIConstants::CHARACTER_SIZE_M);
		subtitle.setPosition(xOffset, yOffset);
		subtitle.setColor(COLOR_LIGHT_PURPLE);
		m_reputationTexts.push_back(subtitle);

		yOffset += subtitle.getLocalBounds().height + 2 * GUIConstants::CHARACTER_SIZE_M;
	}
}

void CharacterInfo::updateHints() {
	CLEAR_VECTOR(m_hintEntries);
	m_selectedEntryId = -1;

	const auto& hints = m_core->getData().hintsLearned;

	for (const auto& hint : hints) {
		const auto newEntry = new HintEntry(hint);
		newEntry->deselect();
		m_hintEntries.push_back(newEntry);
		if (newEntry->getHintKey() == m_selectedHintKey) {
			selectEntry(static_cast<int>(hints.size()) - 1);
		}
	}

	selectEntry(0);
}

void CharacterInfo::execEntrySelected(const ScrollEntry* entry) {
	const auto hintEntry = dynamic_cast<const HintEntry*>(entry);
	showDescription(hintEntry->getHintKey());
	m_selectedHintKey = hintEntry->getHintKey();
}

void CharacterInfo::show() {
	m_isVisible = true;
	g_inputController->startReadingText();

	if (m_tabBar->getActiveTabIndex() == 2) {
		showDescription(m_selectedHintKey);
	}
}

void CharacterInfo::hide() {
	m_isVisible = false;
	m_descriptionWindow->hide();

	// handle GODMODE
	std::string read = g_inputController->getReadText();
	if (!read.empty()) {
		read.pop_back();
		if (GODMODE_CHEAT == read) {
			m_screen->toggleGodmode();
		}
	}

	g_inputController->stopReadingText();
}

// <<< HINT ENTRY >>>

HintEntry::HintEntry(const std::string& hintKey) {
	m_hintKey = hintKey;
	m_name.setFont(*g_resourceManager->getFont(GlobalResource::FONT_TTF_DIALOGUE));
	m_name.setCharacterSize(GUIConstants::CHARACTER_SIZE_M);

	std::string hintTitle = "> " + g_textProvider->getText(hintKey, "hint");
	if (hintTitle.size() > CharacterInfo::MAX_ENTRY_LENGTH_CHARACTERS) {
		hintTitle = hintTitle.substr(0, CharacterInfo::MAX_ENTRY_LENGTH_CHARACTERS - 3) + "...";
	}
	m_name.setString(sf::String::fromUtf8(hintTitle.begin(),hintTitle.end()));

	setBoundingBox(sf::FloatRect(0.f, 0.f, m_name.getLocalBounds().width, m_name.getLocalBounds().height));
	setInputInDefaultView(true);
}

const std::string& HintEntry::getHintKey() const {
	return m_hintKey;
}

void HintEntry::setPosition(const sf::Vector2f& pos) {
	GameObject::setPosition(pos);
	m_name.setPosition(pos);
}

void HintEntry::render(sf::RenderTarget& renderTarget) {
	renderTarget.draw(m_name);
}

void HintEntry::setColor(const sf::Color& color) {
	m_name.setColor(color);
}

void HintEntry::updateColor() {
	m_name.setColor(isSelected() ? COLOR_WHITE :
		isMouseover() ? COLOR_LIGHT_PURPLE :
		COLOR_MEDIUM_GREY);
}
