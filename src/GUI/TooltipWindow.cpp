#include "GUI/TooltipWindow.h"
#include "GUI/GUIConstants.h"

TooltipWindow::TooltipWindow() : Window(
	sf::FloatRect(), GUIOrnamentStyle::NONE, 
	sf::Color(0, 0, 0, 200),
	COLOR_WHITE) 
{
	m_text.setFont(*g_resourceManager->getFont(GlobalResource::FONT_TTF_DIALOGUE));
	m_text.setCharacterSize(GUIConstants::CHARACTER_SIZE_S);
	m_text.setPosition(getPosition() + m_textOffset);
	m_textOffset = sf::Vector2f(5.f, 10.f);
	m_maxWidth = 150;
}

void TooltipWindow::setText(const std::string& text) {
	std::string cropped = g_textProvider->getCroppedString(text, GUIConstants::CHARACTER_SIZE_S, m_maxWidth);
	std::basic_string<sf::Uint32> utf32line;
	sf::Utf8::toUtf32(cropped.begin(), cropped.end(), std::back_inserter(utf32line));
	m_text.setString(utf32line);
	setHeight(m_text.getLocalBounds().height + 2 * m_textOffset.y);
	setWidth(m_text.getLocalBounds().width + 2 * m_textOffset.x);
}

void TooltipWindow::setPosition(const sf::Vector2f& position) {
	Window::setPosition(position);
	m_text.setPosition(position + m_textOffset);
}

void TooltipWindow::render(sf::RenderTarget& renderTarget) {
	Window::render(renderTarget);
	renderTarget.draw(m_text);
}

void TooltipWindow::setTextOffset(const sf::Vector2f& offset) {
	m_textOffset = offset;
	setHeight(m_text.getLocalBounds().height + 2 * m_textOffset.y);
	setWidth(m_text.getLocalBounds().width + 2 * m_textOffset.x);
}

void TooltipWindow::setTextAlignment(TextAlignment alignment) {
	// m_text.setTextAlignment(alignment);
}

void TooltipWindow::setMaxWidth(int maxWidth) {
	m_maxWidth = maxWidth;
}