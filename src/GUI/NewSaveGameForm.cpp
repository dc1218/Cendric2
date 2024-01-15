#include "GUI/NewSaveGameForm.h"
#include "GUI/GUIConstants.h"

// determines the distance of the message text and the buttons from border
const float DIST_FROM_BORDER = 10.f;
const int MAX_NAME_LENGTH = 25;

NewSaveGameForm::NewSaveGameForm(const sf::FloatRect& box) : GameObject() {
	m_window = new Window(box, GUIOrnamentStyle::NONE);
	float buttonHeight = 50.f;
	float buttonGap = 20.f;
	float buttonWidth = (box.width - (buttonGap + 2.f * DIST_FROM_BORDER)) / 2.f;

	m_okButton = new Button(sf::FloatRect(box.left + DIST_FROM_BORDER, box.top + (box.height - (buttonHeight + DIST_FROM_BORDER)), buttonWidth, buttonHeight));
	m_cancelButton = new Button(sf::FloatRect(box.left + buttonGap + DIST_FROM_BORDER + buttonWidth, box.top + (box.height - (buttonHeight + DIST_FROM_BORDER)), buttonWidth, buttonHeight));

	m_okButton->setText("Okay");
	m_cancelButton->setText("Cancel");

	m_buttonGroup = new ButtonGroup(2);

	m_buttonGroup->addButton(m_okButton);
	m_buttonGroup->addButton(m_cancelButton);

	setBoundingBox(box);
	setPosition(sf::Vector2f(box.left, box.top));

	// message
	
	std::string line = g_textProvider->getCroppedText("MessageNewSaveGame", GUIConstants::CHARACTER_SIZE_L, static_cast<int>(m_window->getSize().x - (2 * DIST_FROM_BORDER)));
	m_message.setString(sf::String::fromUtf8(line.begin(),line.end()));
	m_message.setFont(*g_resourceManager->getFont(GlobalResource::FONT_TTF_DIALOGUE));
	m_message.setColor(COLOR_WHITE);
	m_message.setCharacterSize(GUIConstants::CHARACTER_SIZE_L);
	// calculate position
	m_message.setPosition(sf::Vector2f(DIST_FROM_BORDER, DIST_FROM_BORDER) + getPosition());

	m_savegameName = "";
	m_savegameNameText.setFont(*g_resourceManager->getFont(GlobalResource::FONT_TTF_DIALOGUE));
	m_savegameNameText.setString(m_savegameName);
	m_savegameNameText.setColor(COLOR_LIGHT_PURPLE);
	m_savegameNameText.setCharacterSize(16);
	m_savegameNameText.setPosition(sf::Vector2f(2 * DIST_FROM_BORDER, box.height - (buttonHeight + DIST_FROM_BORDER + 50)) + getPosition());

	g_inputController->startReadingText();
}

NewSaveGameForm::~NewSaveGameForm() {
	delete m_window;
	delete m_buttonGroup;
	g_inputController->stopReadingText();
}

void NewSaveGameForm::render(sf::RenderTarget& renderTarget) {
	m_window->render(renderTarget);
	renderTarget.draw(m_message);
	renderTarget.draw(m_savegameNameText);

	m_buttonGroup->render(renderTarget);
}

const std::string& NewSaveGameForm::getSavegameName() const {
	return m_savegameName;
}

void NewSaveGameForm::update(const sf::Time& frameTime) {
	g_inputController->cropReadText(MAX_NAME_LENGTH);
	m_savegameName = g_inputController->getReadText();
	m_savegameNameText.setString(sf::String::fromUtf8(m_savegameName.begin(),m_savegameName.end()));
	m_okButton->setEnabled(m_savegameName.size() < MAX_NAME_LENGTH && !m_savegameName.empty());
	m_buttonGroup->update(frameTime);

	if (m_cancelButton->isClicked() || m_okButton->isClicked())  {
		setDisposed();
	}
}

void NewSaveGameForm::setOnOkClicked(const std::function<void()>& agent) {
	m_executeOnOk = agent;
	m_okButton->setOnClick(agent);
}

void NewSaveGameForm::setOnCancelClicked(const std::function<void()>& agent) {
	m_cancelButton->setOnClick(agent);
}

GameObjectType NewSaveGameForm::getConfiguredType() const {
	return _Form;
}
