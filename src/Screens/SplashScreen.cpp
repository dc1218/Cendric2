#include "Screens/SplashScreen.h"
#include "Screens/ScreenManager.h"
#include "GUI/GUIConstants.h"

using namespace particles;

SplashScreen::SplashScreen() : Screen(nullptr) {
	m_screenSpriteBackground = sf::Sprite((*g_resourceManager->getTexture(GlobalResource::TEX_SPLASH_BG)));
	m_screenSpriteForeground = sf::Sprite((*g_resourceManager->getTexture(GlobalResource::TEX_SPLASH_FG)));

	float scale = 5.f;
	sf::Texture* tex = g_resourceManager->getTexture(GlobalResource::TEX_SPLASH_LOGO);
	m_logoSprite = sf::Sprite(*tex);
	m_logoSprite.setScale(sf::Vector2f(scale, scale));
	m_logoSprite.setPosition(0.5f * (WINDOW_WIDTH - scale * tex->getSize().x), 0.5f * (WINDOW_HEIGHT - scale * tex->getSize().y));

	g_resourceManager->getTexture(GlobalResource::TEX_PARTICLE_FLAME)->setSmooth(true);
	m_ps_right = new TextureParticleSystem(1000, g_resourceManager->getTexture(GlobalResource::TEX_PARTICLE_FLAME));
	m_ps_left = new TextureParticleSystem(1000, g_resourceManager->getTexture(GlobalResource::TEX_PARTICLE_FLAME));
	loadFireParticles(m_ps_left, sf::Vector2f(155.f, 330.f));
	loadFireParticles(m_ps_right, sf::Vector2f(1130.f, 330.f));
}

SplashScreen::~SplashScreen() {
	delete m_ps_left;
	delete m_ps_right;
}

void SplashScreen::execOnEnter() {
	// add version nr
	m_versionText.setFont(*g_resourceManager->getFont(GlobalResource::FONT_TTF_DIALOGUE));
	std::string line = "Cendric v" + std::string(CENDRIC_VERSION_NR);
	m_versionText.setString(sf::String::fromUtf8(line.begin(),line.end()));
	m_versionText.setCharacterSize(GUIConstants::CHARACTER_SIZE_S);
	m_versionText.setColor(COLOR_WHITE);
	m_versionText.setPosition(
		(WINDOW_WIDTH - m_versionText.getLocalBounds().width) / 2,
		WINDOW_HEIGHT - 18.f);

	g_resourceManager->playMusic(GlobalResource::MUSIC_MAIN, false);
}

void SplashScreen::execUpdate(const sf::Time& frameTime) {
	if (g_inputController->isSelected() || g_inputController->isMouseJustPressedLeft()) {
		MenuScreen* menuScreen = new MenuScreen(nullptr);
		menuScreen->setFireParticles(m_ps_left, m_ps_right);
		m_ps_left = nullptr;
		m_ps_right = nullptr;
		setNextScreen(menuScreen);
		return;
	}
	if (g_inputController->isKeyActive(Key::Escape)) {
		m_screenManager->requestQuit();
		return;
	}
	updateObjects(_Interface, frameTime);
	m_ps_left->update(frameTime);
	m_ps_right->update(frameTime);
}

void SplashScreen::render(sf::RenderTarget& renderTarget) {
	renderTarget.setView(renderTarget.getDefaultView());
	renderTarget.draw(m_screenSpriteBackground);
	m_ps_left->render(renderTarget);
	m_ps_right->render(renderTarget);
	renderTarget.draw(m_screenSpriteForeground);
	renderTarget.draw(m_logoSprite);
	renderObjects(_Interface, renderTarget);
	renderTarget.draw(m_versionText);
}

void SplashScreen::execOnExit() {
	g_resourceManager->deleteUniqueResources(this);
}

void SplashScreen::loadFireParticles(TextureParticleSystem* ps, const sf::Vector2f& center) {
	ps->additiveBlendMode = true;
	ps->emitRate = 70.f;

	// Generators
	auto spawner = ps->addSpawner<BoxSpawner>();
	spawner->center = center;
	spawner->size = sf::Vector2f(65.f, 0.f);

	auto sizeGen = ps->addGenerator<SizeGenerator>();
	sizeGen->minStartSize = 60.f;
	sizeGen->maxStartSize = 100.f;
	sizeGen->minEndSize = 40.f;
	sizeGen->maxEndSize = 80.f;

	auto colGen = ps->addGenerator<ColorGenerator>();
	colGen->minStartCol = sf::Color(171, 105, 243);
	colGen->maxStartCol = sf::Color(171, 105, 243);
	colGen->minEndCol = sf::Color(77, 54, 130, 200);
	colGen->maxEndCol = sf::Color(77, 54, 130, 200);

	auto velGen = ps->addGenerator<AimedVelocityGenerator>();
	velGen->goal = center - (sf::Vector2f(0.f, 200.f));
	velGen->minStartSpeed = 60.f;
	velGen->maxStartSpeed = 100.f;

	auto timeGen = ps->addGenerator<TimeGenerator>();
	timeGen->minTime = 0.5f;
	timeGen->maxTime = 1.8f;

	// Updaters
	ps->addUpdater<TimeUpdater>();
	ps->addUpdater<ColorUpdater>();
	ps->addUpdater<EulerUpdater>();
	ps->addUpdater<SizeUpdater>();
}