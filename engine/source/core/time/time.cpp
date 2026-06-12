module crumb.core;
import :time;
import sdl;

namespace Crumb {
Time::Time()
	: m_frequency(SDL_GetPerformanceFrequency())
	, m_lastCounter(SDL_GetPerformanceCounter())
{
}

float Time::Tick()
{
	Uint64 now = SDL_GetPerformanceCounter();
	m_deltaTime = static_cast<double>(now - m_lastCounter) / static_cast<double>(m_frequency);
	m_lastCounter = now;

	sleepForBudget();

	++m_frameCount;

	m_fps = (m_fps * 0.9F) + (static_cast<float>(1.0 / m_deltaTime) * 0.1F);

	return static_cast<float>(m_deltaTime * m_timeScale);
}

void Time::SetTargetFPS(int fps)
{
	if (fps > 0) {
		m_targetFrameTime = 1.0 / static_cast<double>(fps);
	} else {
		m_targetFrameTime = 0.0;
	}
}

void Time::SetTimeScale(double scale)
{
	m_timeScale = scale;
}

float Time::FPS() const
{
	return m_fps;
}

void Time::sleepForBudget()
{
	if (m_targetFrameTime <= 0.0) {
		return;
	}

	double const budgetMs = (m_targetFrameTime - m_deltaTime) * 1000.0;
	if (budgetMs <= 0.0) {
		return;
	}

	double const sleepMs = budgetMs - 1.5;
	if (sleepMs > 0.0) {
		SDL_Delay(static_cast<Uint32>(sleepMs));
	}

	Uint64 const targetCounter = m_lastCounter + static_cast<Uint64>(m_targetFrameTime * static_cast<double>(m_frequency));
	while (SDL_GetPerformanceCounter() < targetCounter) {
		// spin
	}

	Uint64 const now = SDL_GetPerformanceCounter();
	m_deltaTime = static_cast<double>(now - m_lastCounter) / static_cast<double>(m_frequency);
	m_lastCounter = now;
}

}  // namespace Crumb