
#pragma once

// необходим для замера времени
// механизм - вызываем start() в начале
// вызываем stop() при завершении
// вызываем elapsedSeconds elapsedMilliseconds elapsedTime для получения желаемого результата

class ZxTimer {
public:
	enum TIMER_STATE {
		OFF = 0,
		ON = 1,
		HOLD = 2
	};
	// конструктор
	ZxTimer() : state(OFF), elapsed(0), startTime(0), stopTime(0), timeDelta(0) { setupTimerFrequency(); }

	// запуск
	void start() {
		startTime = samplePerformanceCounter();
		elapsed = 0;
		state = ON;
	}

	// прекращение
	void stop() {
		elapsed = elapsedCount();
		state = OFF;
	}

	// приостановить
	void suspend() {
		if(state == ON) {
			elapsed = elapsedCount();
			state = HOLD;
		}
	}

	// возобновление
	void resume() {
		if(state == HOLD) {
			startTime = samplePerformanceCounter();
			startTime -= timeDelta;
			elapsed = 0;
			state = ON;
		}
	}

	// прошло времени
	float elapsedTime() {
		if(state != ON) return (float)elapsed * invSecFrequency;

		stopTime = samplePerformanceCounter();
		timeDelta = stopTime - startTime;

		return (float)timeDelta * invSecFrequency;
	}

	// прошло секунд
	long elapsedSeconds() {
		if(state != ON) return elapsed / secondsFrequency;

		stopTime = samplePerformanceCounter();
		timeDelta = stopTime - startTime;

		return timeDelta / secondsFrequency;
	}

	// прошло миллисекунд
	long elapsedMilliseconds() {
		if(state != ON) return elapsed / millisecondsFrequency;

		stopTime = samplePerformanceCounter();
		timeDelta = stopTime - startTime;

		return timeDelta / millisecondsFrequency;
	}

	// прошло циклов процессора
	long elapsedCount() {
		if(state != ON) return elapsed;

		stopTime = samplePerformanceCounter();
		timeDelta = stopTime - startTime;

		return timeDelta;
	}

	long samplePerformanceCounter() {
		LARGE_INTEGER sample;
		QueryPerformanceCounter(&sample);
		return sample.LowPart;
	}
	static long millisecondsFrequency;
private:
	static long secondsFrequency;
	static float invSecFrequency;

	long startTime;
	long stopTime;
	long timeDelta;
	long elapsed;
	TIMER_STATE state;

	static void setupTimerFrequency() {
		if(!secondsFrequency) {
			LARGE_INTEGER frequency;
			QueryPerformanceFrequency(&frequency);

			secondsFrequency = frequency.LowPart;
			millisecondsFrequency = frequency.LowPart / 1000;

			invSecFrequency = 1.0f / (float)secondsFrequency;
		}
	}

	ZxTimer(const ZxTimer& Src);
	ZxTimer& operator = (const ZxTimer& Src);
};
