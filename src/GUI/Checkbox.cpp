#include "GUI/Checkbox.h"
#include "GlobalResource.h"

const float Checkbox::SIDE_LENGTH = 40.f;
const float Checkbox::CENTER_SIZE = 30.f;

Checkbox::Checkbox() :
	GameObject(),
	m_backgroundColor(COLOR_BLACK),
	m_ornamentColor(COLOR_WHITE),
	m_symbolColor(COLOR_WHITE),
	m_mouseOverColor(COLOR_PURPLE) {
	m_background = SlicedSprite(g_resourceManager->getTexture(GlobalResource::TEX_GUI_ROUNDED_RECTANGLE), m_backgroundColor, SIDE_LENGTH, SIDE_LENGTH);

	m_ornament = SlicedSprite(g_resourceManager->getTexture(GlobalResource::TEX_GUI_ORNAMENT_NONE), m_ornamentColor, SIDE_LENGTH, SIDE_LENGTH);

	m_checkedSymbol.setSize(sf::Vector2f(CENTER_SIZE, CENTER_SIZE));
	m_checkedSymbol.setTexture(g_resourceManager->getTexture(GlobalResource::TEX_GUI_CHECKBOX));

	setBoundingBox(m_background.getLocalBounds());
	setInputInDefaultView(true);
	m_text.setFont(*g_resourceManager->getFont(GlobalResource::FONT_TTF_DIALOGUE));
}

void Checkbox::onLeftClick() {
	if (m_isEnabled && m_isPressed) {
		m_isPressed = false;
		click();
		m_executeOnClick();
		g_inputController->lockAction();
	}
}

void Checkbox::click() {
	setChecked(!m_isChecked);
}

void Checkbox::setChecked(bool checked) {
	if (checked) {
		check();
	}
	else {
		uncheck();
	}
}

void Checkbox::check() {
	m_isChecked = true;
}

void Checkbox::uncheck() {
	m_isChecked = false;
}

void Checkbox::onLeftJustPressed() {
	if (m_isEnabled) {
		m_isPressed = true;
		g_inputController->lockAction();
	}
}

void Checkbox::onMouseOver() {
	m_isMouseOver = true;
	if (m_isEnabled && !m_isPressed) {
		m_background.setColor(m_mouseOverColor);
	}
}

void Checkbox::render(sf::RenderTarget& renderTarget) {
	if (!m_isVisible) return;
	renderTarget.draw(m_background);
	renderTarget.draw(m_ornament);
	if (m_isChecked) {
		renderTarget.draw(m_checkedSymbol);
	}
	renderTarget.draw(m_text);
}

void Checkbox::setPosition(const sf::Vector2f& pos) {
	GameObject::setPosition(pos);
	m_background.setPosition(pos);
	m_ornament.setPosition(pos);
	m_checkedSymbol.setPosition(pos + sf::Vector2f(
		0.5f * (SIDE_LENGTH - m_checkedSymbol.getSize().x),
		0.5f * (SIDE_LENGTH - m_checkedSymbol.getSize().y)));
}

void Checkbox::update(const sf::Time& frameTime) {
	if (!m_isVisible) return;

	if (m_isMouseOver && !(g_inputController->isMouseOver(getBoundingBox(), true))) {
		m_isMouseOver = false;
		m_isPressed = false;
		m_background.setColor(m_backgroundColor);
	}

	GameObject::update(frameTime);
}

void Checkbox::setSymbolColor(const sf::Color& color) {
	m_checkedSymbol.setFillColor(color);
	m_symbolColor = color;
}

void Checkbox::setMouseOverColor(const sf::Color& color) {
	m_mouseOverColor = color;
}

void Checkbox::setBackgroundColor(const sf::Color& color) {
	m_background.setColor(color);
	m_backgroundColor = color;
}

void Checkbox::setMarginColor(const sf::Color& color) {
	m_ornament.setColor(color);
	m_ornamentColor = color;
}

void Checkbox::setTextColor(const sf::Color& color) {
	m_text.setColor(color);
}

void Checkbox::setText(const std::string& text, const sf::Color& color, int charSize) {
	std::string line = g_textProvider->getText(text);
	m_text.setString(sf::String::fromUtf8(line.begin(),line.end()));//BitmapText(g_textProvider->getText(text));

	setTextColor(color);
	setCharacterSize(charSize);
}

void Checkbox::setText(const std::string& text) {
	setText(text, COLOR_WHITE, 12);
}

void Checkbox::setText(const std::string& text, int charSize) {
	setText(text, COLOR_WHITE, charSize);
}

void Checkbox::setTextRaw(const std::string& text, const sf::Color& color, int charSize) {
	m_text.setString(sf::String::fromUtf8(text.begin(),text.end()));//BitmapText(text);

	setTextColor(color);
	setCharacterSize(charSize);
}

void Checkbox::setTextRaw(const std::string& text, int charSize) {
	setTextRaw(text, COLOR_WHITE, charSize);
}

void Checkbox::setTextRaw(const std::string& text) {
	setTextRaw(text, COLOR_WHITE, 12);
}

void Checkbox::setCharacterSize(int size) {
	m_text.setCharacterSize(size);
	float xOffset = SIDE_LENGTH + 30.f;
	float yOffset = SIDE_LENGTH / 2.f - m_text.getLocalBounds().height / 2.f;
	m_textOffset = sf::Vector2f(xOffset, yOffset);
	m_text.setPosition(getPosition() + m_textOffset);
}

void Checkbox::setEnabled(bool enabled) {
	ButtonInterface::setEnabled(enabled);
	m_ornament.setColor(sf::Color(
		m_ornament.getColor().r,
		m_ornament.getColor().g,
		m_ornament.getColor().b,
		m_isEnabled ? 255 : 100));
	// m_text.setColorAlpha(m_isEnabled ? 255 : 100);
}

void Checkbox::updateColor() {
	m_text.setColor(m_isSelected ? COLOR_BRIGHT_PURPLE : COLOR_WHITE);
	// m_text.setColorAlpha(m_isEnabled ? 255 : 100);

	m_background.setColor(m_isSelected ? COLOR_MEDIUM_PURPLE : m_backgroundColor);
}

bool Checkbox::isChecked() const {
	return m_isChecked;
}

void Checkbox::setOnClick(const std::function<void()>& agent) {
	m_executeOnClick = agent;
}