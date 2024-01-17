#include "GUI/TabButton.h"
#include "GlobalResource.h"

const float TabButton::BORDER_OFFSET = 5.f;
const float TabButton::BOUNDING_BOX_OFFSET = 9.f;
const float TabButton::ALIGNMENT_OFFSET = 14.f;

TabButton::TabButton(const sf::FloatRect& box) {
	// using default values for constructor.
	m_border = SlicedSprite(g_resourceManager->getTexture(GlobalResource::TEX_GUI_TAB_INACTIVE), COLOR_WHITE, box.width, box.height);

	m_outerRect = box;
	sf::FloatRect backgroundBox = sf::FloatRect(box.left + BORDER_OFFSET, box.top + BORDER_OFFSET, box.width - 2.f * BORDER_OFFSET, box.height - 2.f * BORDER_OFFSET);
	m_background.setSize(sf::Vector2f(backgroundBox.width, backgroundBox.height));
	m_background.setFillColor(m_backgroundColor);
	sf::FloatRect boundingBox = sf::FloatRect(box.left + BOUNDING_BOX_OFFSET, box.top + BORDER_OFFSET, box.width - 2.f * BOUNDING_BOX_OFFSET, box.height - 2.f * BORDER_OFFSET);
	setBoundingBox(boundingBox);
	
	setInputInDefaultView(true);
	setDebugBoundingBox(COLOR_BAD);

	m_isActive = false;
	m_text.setFont(*g_resourceManager->getFont(GlobalResource::FONT_TTF_DIALOGUE));
	setPosition(sf::Vector2f(box.left, box.top));
}

void TabButton::setPosition(const sf::Vector2f& position) {
	GameObject::setPosition(position + sf::Vector2f(BOUNDING_BOX_OFFSET, BORDER_OFFSET));
	m_border.setPosition(position);

	sf::Vector2f backgroundPos = sf::Vector2f(position.x + BORDER_OFFSET, position.y + BORDER_OFFSET);
	m_background.setPosition(backgroundPos);

	float width = m_text.getLocalBounds().width;
	float height = m_text.getLocalBounds().height;
	float x = getBoundingBox()->left + 0.5f * (getBoundingBox()->width - width);
	float y = getBoundingBox()->top + 0.5f * (getBoundingBox()->height - height);

	m_text.setPosition(sf::Vector2f(x, y));
}

void TabButton::setActive(bool active) {
	m_isActive = active;
	if (m_isActive) {
		m_background.setFillColor(m_activeColor);
	}
	else {
		m_background.setFillColor(m_backgroundColor);
	}
}

bool TabButton::isActive() const {
	return m_isActive;
}

void TabButton::onLeftClick() {
	if (m_isPressed) {
		m_isClicked = true;
		m_isPressed = false;
		g_inputController->lockAction();
	}
	if (!m_isActive) {
		m_background.setFillColor(m_backgroundColor);
	}
}

void TabButton::onLeftJustPressed() {
	m_isPressed = true;
	g_inputController->lockAction();
}

void TabButton::onMouseOver() {
	m_isMouseOver = true;
	if (!m_isActive) {
		m_background.setFillColor(m_highlightColor);
	}
}

void TabButton::render(sf::RenderTarget& renderTarget) {
	renderTarget.draw(m_background);
	renderTarget.draw(m_border);
	renderTarget.draw(m_text);
}

void TabButton::update(const sf::Time& frameTime) {
	if (m_isMouseOver && !(g_inputController->isMouseOver(getBoundingBox(), true))) {
		m_isMouseOver = false;
		m_isPressed = false;
		if (!m_isActive) {
			m_background.setFillColor(m_backgroundColor);
		}
	}
	m_isClicked = false;
	GameObject::update(frameTime);
	if (m_isClicked) {
		m_executeOnClick();
	}
}

void TabButton::setText(const std::string& text, const sf::Color& color, int charSize) {
	std::string line = g_textProvider->getText(text);
	m_text.setString(sf::String::fromUtf8(line.begin(),line.end()));

	setTextColor(color);
	setCharacterSize(charSize);
}

void TabButton::setText(const std::string& text) {
	setText(text, COLOR_WHITE, 16);
}

void TabButton::setText(const std::string& text, int charSize) {
	setText(text, COLOR_WHITE, charSize);
}

void TabButton::setTextRaw(const std::string& text, const sf::Color& color, int charSize) {
	m_text.setString(sf::String::fromUtf8(text.begin(),text.end()));

	setTextColor(color);
	setCharacterSize(charSize);
}

void TabButton::setTextRaw(const std::string& text, int charSize) {
	setTextRaw(text, COLOR_WHITE, charSize);
}

void TabButton::setTextRaw(const std::string& text) {
	setTextRaw(text, COLOR_WHITE, 16);
}

void TabButton::setCharacterSize(int size) {
	m_text.setCharacterSize(size);

	float width = m_text.getLocalBounds().width;
	float height = m_text.getLocalBounds().height;
	float x = getBoundingBox()->left + 0.5f * (getBoundingBox()->width - width);
	float y = getBoundingBox()->top + 0.5f * (getBoundingBox()->height - height);

	m_text.setPosition(sf::Vector2f(x, y));
}

void TabButton::setTextColor(const sf::Color& color) {
	m_text.setColor(color);
}

void TabButton::setBackgroundColor(const sf::Color& color) {
	m_backgroundColor = color;
}

void TabButton::setHighlightColor(const sf::Color& color) {
	m_highlightColor = color;
}

bool TabButton::isClicked() const {
	return m_isClicked;
}

GameObjectType TabButton::getConfiguredType() const {
	return _Button;
}

void TabButton::setOnClick(const std::function<void()>& agent) {
	m_executeOnClick = agent;
}