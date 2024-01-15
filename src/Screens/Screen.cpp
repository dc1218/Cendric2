#include "Screens/Screen.h"
#include "GUI/Button.h"
#include "GUI/ButtonGroup.h"
#include "Screens/ScreenManager.h"
#include "GUI/GUIConstants.h"

#define VIEW_MARGIN 20.f;

inline bool isInsideView(const sf::View& targetView, const sf::FloatRect& boundingBox) {
	sf::FloatRect view(targetView.getCenter().x - targetView.getSize().x / 2.f,
		targetView.getCenter().y - targetView.getSize().y / 2.f,
		targetView.getSize().x,
		targetView.getSize().y);

	view.left -= VIEW_MARGIN;
	view.top -= VIEW_MARGIN;
	view.width += 2 * VIEW_MARGIN;
	view.height += 2 * VIEW_MARGIN;
	return fastIntersect(view, boundingBox);
}

Screen::Screen(CharacterCore* core) {
	m_characterCore = core;

	m_objects = std::vector<std::vector<GameObject*>>();
	for (GameObjectType t = _Undefined; t < _MAX; t = GameObjectType(t + 1)) {
		std::vector<GameObject*> newVector;
		m_objects.push_back(newVector);
	}
}

Screen::~Screen() {
}

void Screen::addObject(GameObject* object) {
	m_toAdd.push_back(object);
	object->setScreen(this);
}

void Screen::update(const sf::Time& frameTime) {
	execUpdate(frameTime);
	deleteDisposedObjects();
	for (auto& obj : m_toAdd) {
		if (obj->isDisposed()) {
			delete obj;
		}
		else {
			m_objects[obj->getConfiguredType()].push_back(obj);
		}
	}
	m_toAdd.clear();
}

std::vector<GameObject*>* Screen::getObjects(GameObjectType type) {
	return &m_objects[type];
}

std::vector<GameObject*>& Screen::getToAddObjects() {
	return m_toAdd;
}

void Screen::onEnter() {
	execOnEnter();
	for (auto& obj : m_toAdd) {
		m_objects[obj->getConfiguredType()].push_back(obj);
	}
	m_toAdd.clear();
}

void Screen::execOnEnter() {
	// nop
}

void Screen::onExit() {
	deleteAllObjects();
	execOnExit();
}

void Screen::execOnExit() {
	// nop
}

void Screen::deleteDisposedObjects() {
	for (GameObjectType t = _Undefined; t < _MAX; t = GameObjectType(t + 1)) {
		for (std::vector<GameObject*>::iterator it = m_objects[t].begin(); it != m_objects[t].end(); /*don't increment here*/) {
			if ((*it)->isDisposed()) {
				delete (*it);
				it = m_objects[t].erase(it);
			}
			else {
				++it;
			}
		}
	}
}

void Screen::setAllButtonsEnabled(bool value) {
	std::vector<GameObject*>* buttons = getObjects(_Button);
	for (auto it : *buttons) {
		Button* button = dynamic_cast<Button*>(it);
		if (button != nullptr) {
			button->setEnabled(value);
			continue;
		}
		ButtonGroup* buttonGroup = dynamic_cast<ButtonGroup*>(it);
		if (buttonGroup != nullptr) {
			buttonGroup->setEnabled(value);
		}
	}
}

void Screen::deleteAllObjects() {
	for (GameObjectType t = _Undefined; t < _MAX; t = GameObjectType(t + 1)) {
		deleteObjects(t);
	}
}

void Screen::deleteObjects(GameObjectType type) {
	for (std::vector<GameObject*>::iterator it = m_objects[type].begin(); it != m_objects[type].end(); /*don't increment here*/) {
		delete (*it);
		it = m_objects[type].erase(it);
	}
}

void Screen::updateObjectsFirst(GameObjectType type, const sf::Time& frameTime) {
	for (auto& it : m_objects[type]) {
		it->updateFirst(frameTime);
	}
}

void Screen::updateObjects(GameObjectType type, const sf::Time& frameTime) {
	for (auto& it : m_objects[type]) {
		if (it->isUpdatable())
			it->update(frameTime);
	}
}

inline bool compareYCoordAsc(const GameObject* go1, const GameObject* go2) { 
	return (go1->getPosition().y + go1->getBoundingBox()->height < 
		go2->getPosition().y + go2->getBoundingBox()->height);
}

inline bool compareYCoordDesc(const GameObject* go1, const GameObject* go2) {
	return (go1->getPosition().y + go1->getBoundingBox()->height >
		go2->getPosition().y + go2->getBoundingBox()->height);
}

void Screen::depthSortObjects(GameObjectType type, bool asc) {
	if (asc)
		std::sort(m_objects[type].begin(), m_objects[type].end(), compareYCoordAsc);
	else
		std::sort(m_objects[type].begin(), m_objects[type].end(), compareYCoordDesc);
}

void Screen::renderObjects(GameObjectType type, sf::RenderTarget& renderTarget) {
	for (auto& obj : m_objects[type]) {
		if (obj->isDisposed()) continue;
		obj->setViewable(isInsideView(renderTarget.getView(), *(obj->getBoundingBox())));
		if (obj->isViewable())
			obj->render(renderTarget);
	}
}

void Screen::renderObjectsAfterForeground(GameObjectType type, sf::RenderTarget& renderTarget) {
	for (auto& obj : m_objects[type]) {
		if (obj->isViewable())
			obj->renderAfterForeground(renderTarget);
	}
}

const sf::Text* Screen::getTooltipText() const {
	return &m_tooltipText;
}

void Screen::setTooltipTextRaw(const std::string& text, const sf::Color& color, bool isOverride) {
	if (m_tooltipTime > sf::Time::Zero && !isOverride) {
		// another text is still displaying
		return;
	}
	
	// m_tooltipText = BitmapText(g_textProvider->getCroppedString(text, GUIConstants::CHARACTER_SIZE_M, static_cast<int>((m_isTooltipTop ? 0.4f : 0.6f) * WINDOW_WIDTH)));
	// m_tooltipText.setTextStyle(TextStyle::Shadowed);
	// m_tooltipText.setTextAlignment(TextAlignment::Center);
	std::string line = g_textProvider->getCroppedString(text, GUIConstants::CHARACTER_SIZE_M, static_cast<int>((m_isTooltipTop ? 0.4f : 0.6f) * WINDOW_WIDTH));
	m_tooltipText.setString(sf::String::fromUtf8(line.begin(),line.end()));
	m_tooltipText.setFont(*g_resourceManager->getFont(GlobalResource::FONT_TTF_DIALOGUE));
	m_tooltipText.setCharacterSize(GUIConstants::CHARACTER_SIZE_M);
	m_tooltipText.setPosition(std::max(0.f, (WINDOW_WIDTH - m_tooltipText.getLocalBounds().width) / 2.f), m_isTooltipTop ? 12.f : WINDOW_HEIGHT - m_tooltipText.getLocalBounds().height - 12.f);
	m_tooltipText.setColor(color);
	m_tooltipTime = sf::seconds(1.f + 0.06f * static_cast<float>(text.length()));
}

void Screen::setTooltipText(const std::string& textKey, const sf::Color& color, bool isOverride) {
	setTooltipTextRaw(g_textProvider->getText(textKey), color, isOverride);
}

void Screen::setNegativeTooltip(const std::string& textKey) {
	g_resourceManager->playSound(GlobalResource::SOUND_GUI_NEGATIVE);
	setTooltipText(textKey, COLOR_BAD, true);
}

void Screen::renderTooltipText(sf::RenderTarget& target) const {
	sf::View oldView = target.getView();
	target.setView(target.getDefaultView());
	target.draw(m_tooltipText);
	target.setView(oldView);
}

void Screen::setTooltipPositionTop(bool top) {
	m_isTooltipTop = top;
}

CharacterCore* Screen::getCharacterCore() const {
	return m_characterCore;
}

void Screen::updateTooltipText(const sf::Time& frameTime) {
	if (m_tooltipTime > sf::Time::Zero) {
		m_tooltipTime -= frameTime;
		if (m_tooltipTime <= sf::Time::Zero) // yes, sf::Time can be negative.
		{
			// reset tooltip text
			m_tooltipText.setString("");
			m_tooltipTime = sf::Time::Zero;
		}
	}
}

void Screen::setNextScreen(Screen* nextScreen, bool backup) {
	if (nextScreen == this) return;
	m_screenManager->setNextScreen(nextScreen, backup);
}

void Screen::setScreenManager(ScreenManager* screenManager) {
	m_screenManager = screenManager;
}
