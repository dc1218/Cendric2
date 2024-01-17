#include "Screens/ErrorScreen.h"
#include "Screens/ScreenManager.h"
#include "Logger.h"
#include "GlobalResource.h"

ErrorScreen::ErrorScreen(CharacterCore* core) : Screen(core) {
	ErrorID error = g_resourceManager->pollError()->first;
	switch (error) {
	case ErrorID::Error_fileNotFound:
		m_screenResource = "res/texture/screens/screen_error_filenotfound.png";
		break;
	case ErrorID::Error_dataCorrupted:
		m_screenResource = "res/texture/screens/screen_error_datacorrupted.png";
		break;
	default:
		// unexpected
		g_logger->logError("ErrorScreen", "Error screen has been set without an error occurring");
		m_screenResource = "res/texture/screens/screen_error_datacorrupted.png";
		break;
	}
	g_resourceManager->loadTexture(m_screenResource, ResourceType::Unique, this);
	m_screenSprite = sf::Sprite((*g_resourceManager->getTexture(m_screenResource)));
	std::string line = g_resourceManager->pollError()->second;
	m_errorText.setString(sf::String::fromUtf8(line.begin(),line.end()));
	m_errorText.setFont(*g_resourceManager->getFont(GlobalResource::FONT_TTF_DIALOGUE));
	m_errorText.setColor(COLOR_BAD);
	m_errorText.setPosition(sf::Vector2f(64, 518));
	m_errorText.setCharacterSize(12);
}

void ErrorScreen::render(sf::RenderTarget& renderTarget) {
	renderTarget.setView(renderTarget.getDefaultView());
	renderTarget.draw(m_screenSprite);
	renderTarget.draw(m_errorText);
}

void ErrorScreen::execUpdate(const sf::Time& frameTime) {
	if (g_inputController->isKeyActive(Key::Escape)) {
		m_screenManager->requestQuit();
	}
}

void ErrorScreen::execOnEnter() {
}

void ErrorScreen::execOnExit() {
	g_resourceManager->deleteUniqueResources(this);
}