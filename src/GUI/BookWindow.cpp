#include "GUI/BookWindow.h"
#include "GUI/GUIConstants.h"
#include "World/Item.h"

const float BookWindow::WIDTH = WINDOW_WIDTH / 3.f;
const float BookWindow::HEIGHT = WINDOW_HEIGHT - 2 * GUIConstants::TOP;
const float BookWindow::MARGIN = 50.f;
const std::string BookWindow::SOUND_PATH = "res/sound/gui/page_turn.ogg";

BookWindow::BookWindow(const Item& item) : Window(
	sf::FloatRect(0.f, 0.f, WIDTH, HEIGHT),
	GUIOrnamentStyle::LARGE,
	COLOR_LIGHT_BROWN, // back
	COLOR_MEDIUM_BROWN), // ornament 
	m_item(item) {
	auto const pages = m_item.getBeans<ItemDocumentPageBean>();
	assert(!pages.empty() && "A document has to have at least one page!");

	// load resources
	g_resourceManager->loadSoundbuffer(SOUND_PATH, ResourceType::Unique, this);
	for (auto const page : pages) {
		if (page->texture_path.empty()) continue;
		g_resourceManager->loadTexture(page->texture_path, ResourceType::Unique, this);
	}

	m_leftArrow = new ArrowButton(false);
	m_leftArrow->setMainColor(COLOR_DARK_BROWN);
	m_leftArrow->setDisabledColor(COLOR_MEDIUM_BROWN);
	m_leftArrow->setMouseoverColor(COLOR_LIGHT_PURPLE);

	m_rightArrow = new ArrowButton(true);
	m_rightArrow->setMainColor(COLOR_DARK_BROWN);
	m_rightArrow->setDisabledColor(COLOR_MEDIUM_BROWN);
	m_rightArrow->setMouseoverColor(COLOR_LIGHT_PURPLE);

	m_title.setFont(*g_resourceManager->getFont(GlobalResource::FONT_TTF_DIALOGUE));
	m_title.setCharacterSize(GUIConstants::CHARACTER_SIZE_L);
	m_title.setColor(COLOR_DARK_BROWN);

	m_bookTitle.setFont(*g_resourceManager->getFont(GlobalResource::FONT_TTF_DIALOGUE));
	m_bookTitle.setCharacterSize(GUIConstants::CHARACTER_SIZE_XL);
	m_bookTitle.setColor(COLOR_MEDIUM_BROWN);
	// m_bookTitle.setTextStyle(TextStyle::Shadowed);

	m_content.setFont(*g_resourceManager->getFont(GlobalResource::FONT_TTF_DIALOGUE));
	m_content.setCharacterSize(GUIConstants::CHARACTER_SIZE_M);
	m_content.setColor(COLOR_BLACK);

	setPosition(sf::Vector2f(0.5f * (WINDOW_WIDTH - WIDTH), 0.5f * (WINDOW_HEIGHT - HEIGHT)));
	setPage(0);
}

BookWindow::~BookWindow() {
	delete m_leftArrow;
	delete m_rightArrow;
	g_resourceManager->deleteUniqueResources(this);
}

void BookWindow::render(sf::RenderTarget& renderTarget) {
	Window::render(renderTarget);

	if (m_showSprite) {
		renderTarget.draw(m_sprite);
	}
	renderTarget.draw(m_bookTitle);
	renderTarget.draw(m_title);
	renderTarget.draw(m_content);

	m_leftArrow->render(renderTarget);
	m_rightArrow->render(renderTarget);
}

bool BookWindow::updateWindow(const sf::Time frameTime) {
	m_leftArrow->update(frameTime);
	m_rightArrow->update(frameTime);

	Window::update(frameTime);
	if (g_inputController->isKeyJustPressed(Key::Escape)) {
		g_inputController->lockAction();
		return false;
	}

	if (g_inputController->isJustLeft() || m_leftArrow->isClicked()) {
		g_inputController->lockAction();
		setPage(m_currentPage - 1);
		setPosition(getPosition());
		g_inputController->lockAction();
	}
	else if (g_inputController->isJustRight()|| m_rightArrow->isClicked()) {
		g_inputController->lockAction();
		setPage(m_currentPage + 1);
		setPosition(getPosition());
		g_inputController->lockAction();
	}

	return true;
}

void BookWindow::setPage(int index) {
	g_resourceManager->playSound(SOUND_PATH);
	auto const pages = m_item.getBeans<ItemDocumentPageBean>();
	if (index < 0 || index > static_cast<int>(pages.size()) - 1) return;

	m_currentPage = index;
	auto const& page = pages.at(m_currentPage);
	if (!page->title.empty() && page->content.empty()) {
		// handle a title page
		std::string line = g_textProvider->getCroppedText(page->title, "document",
			GUIConstants::CHARACTER_SIZE_XL, static_cast<int>(WIDTH - 2 * MARGIN + 10));
		m_bookTitle.setString(sf::String::fromUtf8(line.begin(),line.end()));
		m_title.setString("");
		m_content.setString("");
	}
	else {
		// handle a normal page
		std::string line = g_textProvider->getCroppedText(page->title, "document",
			GUIConstants::CHARACTER_SIZE_L, static_cast<int>(WIDTH - 2 * MARGIN + 10));
		m_title.setString(sf::String::fromUtf8(line.begin(),line.end()));
		line = g_textProvider->getCroppedText(page->content, "document",
			GUIConstants::CHARACTER_SIZE_M, static_cast<int>(WIDTH - 2 * MARGIN + 10));
		m_content.setString(sf::String::fromUtf8(line.begin(),line.end()));
		m_bookTitle.setString("");
	}

	// handle texture
	if (sf::Texture* tex = g_resourceManager->getTexture(page->texture_path)) {
		m_sprite.setTexture(*tex, true);
		m_showSprite = true;
	}
	else {
		m_showSprite = false;
	}

	m_rightArrow->setEnabled(m_currentPage + 1 < static_cast<int>(pages.size()));
	m_leftArrow->setEnabled(m_currentPage > 0);
	setPosition(getPosition());
}

void BookWindow::setPosition(const sf::Vector2f& pos) {
	Window::setPosition(pos);

	m_leftArrow->setPosition(pos + sf::Vector2f(0.5f * WIDTH - 20.f - m_leftArrow->getBoundingBox()->width, HEIGHT - 50.f));
	m_rightArrow->setPosition(pos + sf::Vector2f(0.5f * WIDTH + 20.f, HEIGHT - 50.f));

	m_bookTitle.setPosition(pos + sf::Vector2f(0.5f * WIDTH - 0.5f * m_bookTitle.getLocalBounds().width, 0.3f * HEIGHT));
	m_title.setPosition(pos + sf::Vector2f(0.5f * WIDTH - 0.5f * m_title.getLocalBounds().width, GUIConstants::TOP));

	// m_content.setPosition(
	// 	pos + sf::Vector2f(contentX,
	// 		m_title.getString().empty() ? GUIConstants::TOP : GUIConstants::TOP + 3 * GUIConstants::CHARACTER_SIZE_L));

	if (m_showSprite) {
		m_sprite.setPosition(pos + sf::Vector2f(
			0.5f * (WIDTH - m_sprite.getTextureRect().width),
			0.5f * (HEIGHT - 50.f - m_sprite.getTextureRect().height)));
	}
}
