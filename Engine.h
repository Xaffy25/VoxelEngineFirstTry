#pragma once

enum ENGINE_STATE
{
	STATE_STARTING = 0,
	STATE_RUNNING = 1,
	STATE_CLOSE = -1,
	STATE_ERROR = -2
};

class Engine {
private:
	ENGINE_STATE state = STATE_STARTING;

public:
	ENGINE_STATE GetEngineState();
	void NextFrame();
};