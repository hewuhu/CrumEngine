export module crumb.core:time;
import sdl;

export namespace Crumb {

class Time {
public:
	Time();
	~Time() = default;

	Time(const Time&) = delete;
	Time& operator=(const Time&) = delete;

	Time(Time&&) = delete;
	Time& operator=(Time&&) = delete;

	/** 标记帧结束，返回 delta time（秒）并自动 sleep 到目标帧率 */
	float Tick();
	/** 设置目标 FPS，0 表示不限制 */
	void SetTargetFPS(int fps);
	/** 设置全局时间缩放，1.0 = 正常速度 */
	void SetTimeScale(double scale);

	double DeltaTime() const { return m_deltaTime; }
	double TimeScale() const { return m_timeScale; }
	Uint64 FrameCount() const { return m_frameCount; }
	float FPS() const;

private:
	Uint64 m_lastCounter = 0;
	Uint64 m_frequency = 0;

	double m_deltaTime = 0.0;
	double m_timeScale = 1.0;
	double m_targetFrameTime = 0.0;
	Uint64 m_frameCount = 0;

	float m_fps = 0.0F;

	void sleepForBudget();
};

} // namespace Crumb
