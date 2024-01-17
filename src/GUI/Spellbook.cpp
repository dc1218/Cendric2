#include "GUI/Spellbook.h"
#include "GUI/WeaponWindow.h"
#include "GUI/WorldInterface.h"
#include "GUI/SlotClone.h"
#include "GlobalResource.h"
#include "Controller/InputController.h"

float Spellbook::WIDTH = 420.f;
float Spellbook::SPELL_OFFSET = 115.f;
const sf::Vector2f Spellbook::BUTTON_SIZE = sf::Vector2f(70.f, 57.f);

Spellbook::Spellbook(WorldInterface* _interface, bool modifiable) {
	m_interface = _interface;
	m_core = m_interface->getCore();
	m_isModifiable = modifiable;

	init();
}

void Spellbook::init() {
	// init window
	sf::FloatRect box(GUIConstants::LEFT, GUIConstants::TOP, WIDTH, GUIConstants::GUI_WINDOW_HEIGHT);
	m_window = new Window(box,
		GUIOrnamentStyle::LARGE,
		GUIConstants::MAIN_COLOR,
		GUIConstants::ORNAMENT_COLOR);

	// init text
	m_selectedTabText.setFont(*g_resourceManager->getFont(GlobalResource::FONT_TTF_DIALOGUE));
	m_selectedTabText.setPosition(sf::Vector2f(GUIConstants::LEFT + GUIConstants::TEXT_OFFSET, GUIConstants::TOP + GUIConstants::TEXT_OFFSET));
	m_selectedTabText.setColor(COLOR_WHITE);
	m_selectedTabText.setCharacterSize(GUIConstants::CHARACTER_SIZE_M);

	// fill the helper map
	m_typeMap.insert({
		{ SpellType::Twilight, &m_twilightSlots },
		{ SpellType::Divine, &m_divineSlots },
		{ SpellType::Elemental, &m_elementalSlots },
		{ SpellType::Necromancy, &m_necromancySlots }
		});

	delete m_weaponWindow;
	m_weaponWindow = new WeaponWindow(m_interface, this, m_isModifiable);
	m_weaponWindow->addCloseButton(std::bind(&Spellbook::hide, this));

	// init empty text
	m_emptyText.setFont(*g_resourceManager->getFont(GlobalResource::FONT_TTF_DIALOGUE));
	m_emptyText.setCharacterSize(GUIConstants::CHARACTER_SIZE_M);
	std::string line = g_textProvider->getText("NoSpells");
	m_emptyText.setString(sf::String::fromUtf8(line.begin(),line.end()));
	const sf::FloatRect bounds = m_emptyText.getLocalBounds();
	m_emptyText.setPosition(box.left + 0.5f * (box.width - bounds.width), box.top + 0.5f * (box.height - bounds.height));

	reload();
}

Spellbook::~Spellbook() {
	m_typeMap.clear();
	clearAllSlots();
	delete m_window;
	delete m_weaponWindow;
	delete m_currentModifierClone;
	delete m_currentSpellClone;
	delete m_tabBar;
	delete m_buttonGroup;
}

void Spellbook::clearAllSlots() {
	CLEAR_VECTOR(m_modifierSlots);
	m_modifierTexts.clear();

	for (auto it : m_necromancySlots) {
		delete it.first;
	}
	m_necromancySlots.clear();

	for (auto it : m_elementalSlots) {
		delete it.first;
	}
	m_elementalSlots.clear();

	for (auto it : m_divineSlots) {
		delete it.first;
	}
	m_divineSlots.clear();

	for (auto it : m_twilightSlots) {
		delete it.first;
	}
	m_twilightSlots.clear();
	m_selectedModifierSlot = nullptr;
	m_weaponWindow->m_selectedSpellSlot = nullptr;

	delete m_buttonGroup;
	m_buttonGroup = nullptr;
}

void Spellbook::update(const sf::Time& frameTime) {
	if (!m_isVisible) return;
	if (m_isGamepadSelection) {
		m_weaponWindow->update(frameTime);
		if (!g_inputController->isGamepadConnected()) {
			stopGamepadSelection();
		}
		return;
	}

	if (m_tabBar) {
		m_tabBar->update(frameTime);
		int activeIndex = m_tabBar->getActiveTabIndex();
		SpellType type = m_tabTypes[activeIndex];

		if (m_tabBar->getTabButton(activeIndex)->isClicked() && m_currentTab != type) {
			selectTab(type);
		}

		m_buttonGroup->update(frameTime);

		int xIndex = 0;
		int yIndex = 0;
		if (m_currentTab == SpellType::VOID) {
			// handle gems
			SpellModifierType currentType = SpellModifierType::VOID;
			yIndex = -1;
			xIndex = -1;
			for (auto it : m_modifierSlots) {
				if (currentType != it->getModifier().type) {
					yIndex++;
					xIndex = 0;
				}
				else {
					xIndex++;
				}
				currentType = it->getModifier().type;
				if (it->isClicked()) {
					m_buttonGroup->selectButton(yIndex, xIndex);
					selectModifierSlot(it);
					return;
				}
			}
		}
		else {
			// handle spells
			for (auto& it : *(m_typeMap[m_currentTab])) {
				if (it.first->isSelected() || (it.first->isMousedOver() && !m_hasDraggingStarted)) {
					selectSpellSlot(it.first);
					m_buttonGroup->selectButton(yIndex, xIndex);
					if (it.first->isDoubleClicked() && m_isModifiable) {
						equipSpell(it.first);
						return;
					}
				}
				yIndex++;
			}
		}
	}
	else if (isWindowSelected() && !g_inputController->isActionLocked()) {
		if (g_inputController->isJustLeft()) {
			setLeftWindowSelected();
			g_inputController->lockAction();
		}
		if (g_inputController->isJustRight()) {
			setRightWindowSelected();
			g_inputController->lockAction();
		}
	}

	// update weapon part
	m_weaponWindow->update(frameTime);

	if (!m_isModifiable) return;

	handleDragAndDrop();
	updateButtonActions();
}

bool Spellbook::isGamepadSelection() const {
	return m_isGamepadSelection;
}

void Spellbook::updateWindowSelected() {
	if (m_tabBar) m_tabBar->setGamepadEnabled(isWindowSelected());
	if (m_buttonGroup) m_buttonGroup->setGamepadEnabled(isWindowSelected());
}

void Spellbook::updateButtonActions() {
	if (!g_inputController->isGamepadConnected() || !isWindowSelected() || g_inputController->isActionLocked()) {
		return;
	}

	if (!m_buttonGroup) {
		return;
	}

	auto selectedSlot = m_buttonGroup->getSelectedButton();
	if (!selectedSlot) return;

	if (g_inputController->isKeyJustPressed(Key::Interact)) {
		g_inputController->lockAction();

		startGamepadSelection();
	}
}

void Spellbook::notifyGamepadSelected() {
	if (!m_isGamepadSelection) {
		return;
	}

	if (m_currentSpellClone != nullptr) {
		m_weaponWindow->notifySpellDrop(m_currentSpellClone, true);
	}
	else if (m_currentModifierClone != nullptr) {
		m_weaponWindow->notifyModifierDrop(m_currentModifierClone, true);
	}
}

void Spellbook::stopGamepadSelection() {
	if (m_currentSpellClone != nullptr) {
		const_cast<Slot*>(m_currentSpellClone->getOriginalSlot())->activate();
	}
	if (m_currentModifierClone != nullptr) {
		const_cast<Slot*>(m_currentModifierClone->getOriginalSlot())->activate();
	}
	m_weaponWindow->highlightSpellSlots(SpellType::VOID, false);
	m_weaponWindow->highlightModifierSlots(SpellModifierType::VOID, false);
	m_weaponWindow->activateAll();
	m_weaponWindow->setWindowLock(false);
	delete m_currentModifierClone;
	delete m_currentSpellClone;
	m_currentModifierClone = nullptr;
	m_currentSpellClone = nullptr;
	m_hasDraggingStarted = false;
	m_isDragging = false;
	m_isGamepadSelection = false;
}

void Spellbook::startGamepadSelection() {
	m_isGamepadSelection = true;

	delete m_currentModifierClone;
	m_currentModifierClone = nullptr;
	delete m_currentSpellClone;
	m_currentSpellClone = nullptr;

	auto selectedSlot = m_buttonGroup->getSelectedButton();
	auto modifierSlot = dynamic_cast<ModifierSlot*>(selectedSlot);

	if (modifierSlot != nullptr) {
		m_currentModifierClone = new SlotClone(modifierSlot);
		m_currentModifierClone->setPosition(modifierSlot->getPosition() - sf::Vector2f(InventorySlot::SIZE / 5.f, InventorySlot::SIZE / 5.f));
		modifierSlot->deactivate();
		m_weaponWindow->highlightModifierSlots(modifierSlot->getModifier().type, true);
		m_weaponWindow->notifyModifierSlotActivation(modifierSlot->getModifier().type);
	}

	auto spellSlot = dynamic_cast<SpellSlot*>(selectedSlot);
	if (spellSlot != nullptr) {
		m_currentSpellClone = new SlotClone(spellSlot);
		m_currentSpellClone->setPosition(spellSlot->getPosition() - sf::Vector2f(SpellSlot::SIZE / 3.f, SpellSlot::SIZE / 3.f));
		spellSlot->deactivate();
		m_weaponWindow->highlightSpellSlots(spellSlot->getSpellType(), true);
		m_weaponWindow->notifySpellSlotActivation(spellSlot->getSpellType());
	}

	setRightWindowSelected();
	m_weaponWindow->setWindowLock(true);
}

void Spellbook::selectModifierSlot(ModifierSlot* selectedSlot) {
	if (selectedSlot == nullptr) return;

	if (g_inputController->isMouseJustPressedLeftRaw()) {
		m_hasDraggingStarted = true;
		m_startMousePosition = g_inputController->getDefaultViewMousePosition();
	}
	if (selectedSlot == m_selectedModifierSlot) return;
	if (m_selectedModifierSlot != nullptr) {
		m_selectedModifierSlot->deselect();
	}
	m_selectedModifierSlot = selectedSlot;
	m_selectedModifierSlot->select();
}

void Spellbook::selectSpellSlot(SpellSlot* selectedSlot) {
	if (selectedSlot == nullptr) return;

	if (g_inputController->isMouseJustPressedLeftRaw()) {
		m_hasDraggingStarted = true;
		m_startMousePosition = g_inputController->getDefaultViewMousePosition();
	}

	if (selectedSlot == m_weaponWindow->m_selectedSpellSlot) return;
	deselectCurrentSlot();

	m_weaponWindow->m_selectedSpellSlot = selectedSlot;
	m_weaponWindow->m_selectedSpellSlot->select();

	m_weaponWindow->reloadSpellDesc();
}

void Spellbook::deselectCurrentSlot() {
	if (m_weaponWindow->m_selectedSpellSlot != nullptr) {
		m_weaponWindow->m_selectedSpellSlot->deselect();
	}
}

void Spellbook::equipSpell(SpellSlot* selectedSlot) {
	if (selectedSlot == nullptr) return;

	m_weaponWindow->equipSpell(selectedSlot);
}

void Spellbook::stopDragging() {
	if (m_selectedModifierSlot != nullptr) {
		m_selectedModifierSlot->activate();
	}
	if (m_currentSpellClone != nullptr) {
		const_cast<Slot*>(m_currentSpellClone->getOriginalSlot())->activate();
	}
	if (m_weaponWindow->m_selectedSpellSlot != nullptr) {
		m_weaponWindow->m_selectedSpellSlot->activate();
	}
	m_weaponWindow->notifyModifierDrop(m_currentModifierClone);
	m_weaponWindow->notifySpellDrop(m_currentSpellClone);
	m_weaponWindow->highlightSpellSlots(SpellType::VOID, false);
	m_weaponWindow->highlightModifierSlots(SpellModifierType::VOID, false);
	delete m_currentModifierClone;
	delete m_currentSpellClone;
	m_currentModifierClone = nullptr;
	m_currentSpellClone = nullptr;
	m_hasDraggingStarted = false;
	m_isDragging = false;
}

void Spellbook::handleDragAndDrop() {
	if (m_isGamepadSelection) return;
	if (!m_hasDraggingStarted) return;
	if (!(g_inputController->isMousePressedLeft())) {
		stopDragging();
		return;
	}
	sf::Vector2f mousePos = g_inputController->getDefaultViewMousePosition();
	if (!m_isDragging) {
		if (DRAG_DISTANCE < std::sqrt(
			(mousePos.x - m_startMousePosition.x) * (mousePos.x - m_startMousePosition.x) +
			(mousePos.y - m_startMousePosition.y) * (mousePos.y - m_startMousePosition.y))) {
			m_isDragging = true;
			delete m_currentModifierClone;
			m_currentModifierClone = nullptr;
			delete m_currentSpellClone;
			m_currentSpellClone = nullptr;
			if (m_selectedModifierSlot != nullptr) {
				m_currentModifierClone = new SlotClone(m_selectedModifierSlot);
				m_currentModifierClone->setPosition(mousePos - sf::Vector2f(InventorySlot::SIZE / 2.f, InventorySlot::SIZE / 2.f));
				m_selectedModifierSlot->deactivate();
				m_weaponWindow->highlightModifierSlots(m_selectedModifierSlot->getModifier().type, true);
			}
			else if (m_weaponWindow->m_selectedSpellSlot != nullptr) {
				m_currentSpellClone = new SlotClone(m_weaponWindow->m_selectedSpellSlot);
				m_currentSpellClone->setPosition(mousePos - sf::Vector2f(SpellSlot::SIZE / 2.f, SpellSlot::SIZE / 2.f));
				m_weaponWindow->m_selectedSpellSlot->deactivate();
				m_weaponWindow->highlightSpellSlots(m_weaponWindow->m_selectedSpellSlot->getSpellType(), true);
			}
		}
	}
	else {
		if (m_currentModifierClone != nullptr)
			m_currentModifierClone->setPosition(mousePos - sf::Vector2f(InventorySlot::SIZE / 2.f, InventorySlot::SIZE / 2.f));
		if (m_currentSpellClone != nullptr)
			m_currentSpellClone->setPosition(mousePos - sf::Vector2f(SpellSlot::SIZE / 2.f, SpellSlot::SIZE / 2.f));
	}
}

bool Spellbook::isVisible() const {
	return m_isVisible;
}

void Spellbook::render(sf::RenderTarget& target) {
	if (!m_isVisible) return;

	m_window->render(target);

	if (m_tabBar) {
		target.draw(m_selectedTabText);

		m_tabBar->render(target);

		if (m_currentTab == SpellType::VOID) {
			for (auto it : m_modifierSlots) {
				it->render(target);
			}
			for (auto& it : m_modifierTexts) {
				target.draw(it);
			}
		}
		else {
			for (auto& it : *m_typeMap[m_currentTab]) {
				it.first->render(target);
				it.first->renderAfterForeground(target);
				target.draw(it.second.first);
				target.draw(it.second.second);
			}
		}
	}
	else {
		target.draw(m_emptyText);
	}

	m_weaponWindow->render(target);

	if (m_currentModifierClone != nullptr) {
		m_currentModifierClone->render(target);
	}
	if (m_currentSpellClone != nullptr) {
		m_currentSpellClone->render(target);
	}

	if (m_currentTab == SpellType::VOID) {
		for (auto it : m_modifierSlots) {
			it->renderAfterForeground(target);
		}
	}
}

void Spellbook::selectTab(SpellType type) {
	bool reload = false;
	if (m_selectedModifierSlot != nullptr) {
		m_selectedModifierSlot->deselect();
		m_selectedModifierSlot = nullptr;
	}
	if (m_weaponWindow->m_selectedSpellSlot != nullptr && m_weaponWindow->m_selectedSpellSlot->getNr() == -1) {
		m_weaponWindow->m_selectedSpellSlot->deselect();
		m_weaponWindow->m_selectedSpellSlot = nullptr;
		reload = true;
	}
	m_currentTab = type;
	std::string line;
	switch (type) {
	case SpellType::VOID:
		line = g_textProvider->getText("Gems");
		break;
	case SpellType::Twilight:
	case SpellType::Necromancy:
	case SpellType::Divine:
	case SpellType::Elemental:
		line = g_textProvider->getText(EnumNames::getSpellTypeName(type));
		break;
	default:
		break;
	}
	if(line.size() > 0)
		m_selectedTabText.setString(sf::String::fromUtf8(line.begin(),line.end()));
	// center text
	m_selectedTabText.setPosition(
		m_window->getPosition().x +
		WIDTH / 2 -
		m_selectedTabText.getLocalBounds().width / 2, m_selectedTabText.getPosition().y);

	if (reload) {
		m_weaponWindow->reload();
	}

	reloadButtonGroup();
}

WeaponWindow* Spellbook::getWeaponWindow() const {
	return m_weaponWindow;
}

void Spellbook::reload() {
	// reload slots
	clearAllSlots();
	calculateModifierSlots();
	calculateSpellSlots();

	// reload weapon
	m_weaponWindow->reload();

	// Setup tabs
	m_tabTypes.clear();
	if (!m_core->getData().modfiersLearned.empty()) {
		m_tabTypes.push_back(SpellType::VOID);	// SpellType::VOID for modifier tab
	}
	for (int i = static_cast<int>(SpellType::VOID) + 1; i < static_cast<int>(SpellType::Meta); ++i) {
		SpellType spellType = static_cast<SpellType>(i);
		if (m_core->getData().spellsLearned.count(spellType) > 0) {
			m_tabTypes.push_back(spellType);
		}
	}

	int nTabs = static_cast<int>(m_tabTypes.size());
	float width = nTabs * BUTTON_SIZE.x;
	float height = BUTTON_SIZE.y;
	float x = GUIConstants::LEFT + 0.5f * (WIDTH - width);
	float y = GUIConstants::TOP + GUIConstants::GUI_TABS_TOP;

	if (m_tabBar) {
		delete m_tabBar;
		m_tabBar = nullptr;
	}

	if (nTabs > 0) {
		m_tabBar = new TexturedTabBar();
		m_tabBar->init(sf::FloatRect(x, y, width, height), nTabs);

		for (int i = 0; i < nTabs; ++i) {
			if (m_tabTypes[i] == SpellType::VOID) {
				m_tabBar->getTabButton(i)->setTexture(g_resourceManager->getTexture(GlobalResource::TEX_SPELLBOOKTABS), sf::IntRect(0, 0, 35, 35));
			}
			else if (m_tabTypes[i] == SpellType::Elemental) {
				m_tabBar->getTabButton(i)->setTexture(g_resourceManager->getTexture(GlobalResource::TEX_SPELLBOOKTABS), sf::IntRect(35, 0, 35, 35));
			}
			else if (m_tabTypes[i] == SpellType::Divine) {
				m_tabBar->getTabButton(i)->setTexture(g_resourceManager->getTexture(GlobalResource::TEX_SPELLBOOKTABS), sf::IntRect(140, 0, 35, 35));
			}
			else if (m_tabTypes[i] == SpellType::Necromancy) {
				m_tabBar->getTabButton(i)->setTexture(g_resourceManager->getTexture(GlobalResource::TEX_SPELLBOOKTABS), sf::IntRect(105, 0, 35, 35));
			}
			else if (m_tabTypes[i] == SpellType::Twilight) {
				m_tabBar->getTabButton(i)->setTexture(g_resourceManager->getTexture(GlobalResource::TEX_SPELLBOOKTABS), sf::IntRect(70, 0, 35, 35));
			}
		}
	}

	if (!m_tabTypes.empty()) {
		selectTab(m_tabTypes[0]);
	}
}

void Spellbook::calculateModifierSlots() {
	float yOffset = GUIConstants::TOP + SPELL_OFFSET;
	float xOffset = GUIConstants::LEFT + 2 * GUIConstants::TEXT_OFFSET;
	float modifierXOffset = GUIConstants::LEFT + WIDTH - 3 * ModifierSlot::SIZE - 2 * MARGIN - 2 * GUIConstants::TEXT_OFFSET;
	float textYOffset = SpellSlot::ICON_SIZE / 2.f - GUIConstants::CHARACTER_SIZE_S / 2.f;
	for (auto& it : m_core->getData().modfiersLearned) {
		sf::Text text;
		text.setFont(*g_resourceManager->getFont(GlobalResource::FONT_TTF_DIALOGUE));
		text.setCharacterSize(GUIConstants::CHARACTER_SIZE_S);
		text.setColor(COLOR_WHITE);
		text.setPosition(sf::Vector2f(xOffset, yOffset + textYOffset));
		std::string line = g_textProvider->getText(EnumNames::getSpellModifierTypeName(it.first));
		text.setString(sf::String::fromUtf8(line.begin(),line.end()));
		m_modifierTexts.push_back(text);
		for (int i = 0; i < it.second; i++) {
			SpellModifier modifier;
			modifier.level = i + 1;
			modifier.type = it.first;
			ModifierSlot* slot = new ModifierSlot(modifier);
			slot->setPosition(sf::Vector2f(modifierXOffset + (i * (ModifierSlot::SIZE + MARGIN)), yOffset));
			m_modifierSlots.push_back(slot);
		}
		yOffset += ModifierSlot::SIZE + 6.f;
	}
}

void Spellbook::calculateSpellSlots() {
	float yOffset = GUIConstants::TOP + SPELL_OFFSET;
	float xOffset = GUIConstants::LEFT + GUIConstants::TEXT_OFFSET / 2.f + SpellSlot::ICON_OFFSET;

	for (auto& it : m_core->getData().spellsLearned) {
		for (auto& it2 : it.second) {
			SpellSlot* slot = new SpellSlot(it2);
			slot->setPosition(sf::Vector2f(xOffset, yOffset));

			sf::Text text;
			text.setFont(*g_resourceManager->getFont(GlobalResource::FONT_TTF_DIALOGUE));
			text.setCharacterSize(GUIConstants::CHARACTER_SIZE_M);
			text.setColor(COLOR_WHITE);
			std::string line = g_textProvider->getText(EnumNames::getSpellIDName(it2));
			text.setString(sf::String::fromUtf8(line.begin(),line.end()));
			text.setPosition(sf::Vector2f(xOffset + SpellSlot::ICON_SIZE + SpellSlot::ICON_OFFSET + MARGIN, yOffset));

			sf::Text textDesc;
			textDesc.setFont(*g_resourceManager->getFont(GlobalResource::FONT_TTF_DIALOGUE));
			textDesc.setCharacterSize(GUIConstants::CHARACTER_SIZE_S);
			textDesc.setColor(COLOR_LIGHT_GREY);
			line = g_textProvider->getCroppedText(
				EnumNames::getSpellIDName(it2) + "Desc", GUIConstants::CHARACTER_SIZE_S,
				static_cast<int>(WIDTH - (SpellSlot::SIZE + 4 * MARGIN)));
			textDesc.setString(sf::String::fromUtf8(line.begin(),line.end()));
			textDesc.setPosition(sf::Vector2f(xOffset + SpellSlot::ICON_SIZE + SpellSlot::ICON_OFFSET + MARGIN, yOffset + GUIConstants::CHARACTER_SIZE_M + 4.f));

			std::pair<sf::Text, sf::Text> texts = std::pair<sf::Text, sf::Text>(text, textDesc);
			m_typeMap.at(it.first)->push_back(std::pair<SpellSlot*, std::pair<sf::Text, sf::Text>>(slot, texts));
			yOffset += SpellSlot::SIZE + MARGIN;
		}

		yOffset = GUIConstants::TOP + SPELL_OFFSET;
	}
}

void Spellbook::reloadButtonGroup() {
	delete m_buttonGroup;
	m_buttonGroup = new SpellButtonGroup();
	m_buttonGroup->setSelectableWindow(this);
	m_buttonGroup->setGamepadEnabled(isWindowSelected());

	switch (m_currentTab)
	{
	case SpellType::VOID:
	default:
	{
		int yIndex = -1;
		SpellModifierType currentType = SpellModifierType::VOID;
		for (auto modifierSlot : m_modifierSlots) {
			if (currentType != modifierSlot->getModifier().type) {
				++yIndex;
			}
			currentType = modifierSlot->getModifier().type;
			m_buttonGroup->addButton(modifierSlot, yIndex);
		}
		break;
	}
	case SpellType::Necromancy:
	case SpellType::Elemental:
	case SpellType::Divine:
	case SpellType::Twilight:
	{
		int yIndex = -1;
		for (auto slot : *m_typeMap[m_currentTab]) {
			++yIndex;
			m_buttonGroup->addButton(slot.first, yIndex);
		}
		break;
	}
	}
}

void Spellbook::show() {
	m_isVisible = true;
	m_weaponWindow->show();
}

void Spellbook::hide() {
	m_isVisible = false;
	stopGamepadSelection();
	stopDragging();
}