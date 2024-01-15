#include "GUI/GUITabButton.h"

#include "GlobalResource.h"
#include "GUI/GUIConstants.h"
#include "ResourceManager.h"

const int GUITabButton::ICON_SIZE = 50;
const int GUITabButton::OFFSET = 4;
const int GUITabButton::SIZE = ICON_SIZE + 2 * OFFSET;

GUITabButton::GUITabButton() {
	m_border.setTexture(*g_resourceManager->getTexture(GlobalResource::TEX_GUI_BUTTON_ROUND));
	
	m_background.setRadius(0.5f * SIZE);
	m_background.setFillColor(m_backgroundColor);
	
	m_icon.setTextureRect(sf::IntRect(0, 0, ICON_SIZE, ICON_SIZE));

	GameObject::setDebugBoundingBox(sf::Color::Blue);
	setInputInDefaultView(true);
	setBoundingBox(sf::FloatRect(0.f, 0.f, SIZE - 2 * OFFSET, SIZE - 2 * OFFSET));
	m_inputKey.setFont(*g_resourceManager->getFont(GlobalResource::FONT_TTF_DIALOGUE));
}

void GUITabButton::setText(const std::string& text) {
	m_inputKey.setString(sf::String::fromUtf8(text.begin(),text.end()));

	m_inputKey.setCharacterSize((m_inputKey.getLocalBounds().width > SIZE - 10.f) ?
		GUIConstants::CHARACTER_SIZE_S :
		GUIConstants::CHARACTER_SIZE_L);

	const sf::Vector2f positionOffset(0.5f * (SIZE - m_inputKey.getLocalBounds().width), SIZE + 2.f);
	m_inputKey.setPosition(getPosition() + positionOffset);
}

void GUITabButton::click() {
	m_executeOnClick();
	m_isClicked = true;
}

void GUITabButton::setTexture(const sf::Texture* tex, int x) {
	if (tex == nullptr) return;
	m_icon.setTexture(*tex);
	auto rect = m_icon.getTextureRect();
	rect.left = x;
	m_icon.setTextureRect(rect);
}

void GUITabButton::setPosition(const sf::Vector2f& position) {
	auto const innerPosition = position + sf::Vector2f(static_cast<float>(OFFSET), static_cast<float>(OFFSET));
	m_border.setPosition(position);
	m_icon.setPosition(innerPosition);
	m_background.setPosition(position);

	m_boundingBox.left = innerPosition.x;
	m_boundingBox.top = innerPosition.y;

	const sf::Vector2f positionOffset(0.5f * (SIZE - m_inputKey.getLocalBounds().width), SIZE + 2.f);
	m_inputKey.setPosition(position + positionOffset);

	GameObject::setPosition(position);
}

void GUITabButton::update(const sf::Time& frameTime) {
	if (m_isMouseOver && !(g_inputController->isMouseOver(getBoundingBox(), true))) {
		m_isMouseOver = false;
		m_isPressed = false;
		if (!m_isSelected) {
			m_background.setFillColor(m_backgroundColor);
			auto rec = m_icon.getTextureRect();
			rec.top = 0;
			m_icon.setTextureRect(rec);
		}
	}
	m_isClicked = false;
	GameObject::update(frameTime);
	if (m_isClicked) {
		click();
	}
}

void GUITabButton::render(sf::RenderTarget& renderTarget) {
	renderTarget.draw(m_background);
	renderTarget.draw(m_icon);
	renderTarget.draw(m_border);
	renderTarget.draw(m_inputKey);
}

void GUITabButton::updateColor() {
	auto rec = m_icon.getTextureRect();
	if (m_isSelected) {
		m_background.setFillColor(m_selectedColor);
		m_border.setTexture(*g_resourceManager->getTexture(GlobalResource::TEX_GUI_BUTTON_ROUND_SELECTED));
		rec.top = ICON_SIZE;
	}
	else {
		m_background.setFillColor(m_backgroundColor);
		m_border.setTexture(*g_resourceManager->getTexture(GlobalResource::TEX_GUI_BUTTON_ROUND));
		rec.top = 0;
	}
	m_icon.setTextureRect(rec);
}

void GUITabButton::onLeftJustPressed() {
	m_isPressed = true;
	g_inputController->lockAction();
}

void GUITabButton::onLeftClick() {
	if (m_isPressed) {
		m_isClicked = true;
		m_isPressed = false;
		g_inputController->lockAction();
	}
	if (!m_isSelected) {
		m_background.setFillColor(m_backgroundColor);
	}
}

void GUITabButton::onMouseOver() {
	if (!m_isSelected && !m_isMouseOver) {
		m_background.setFillColor(m_highlightColor);
		auto rec = m_icon.getTextureRect();
		rec.top = ICON_SIZE;
		m_icon.setTextureRect(rec);
	}
	m_isMouseOver = true;
}

void GUITabButton::setOnClick(const std::function<void()>& agent) {
	m_executeOnClick = agent;
}