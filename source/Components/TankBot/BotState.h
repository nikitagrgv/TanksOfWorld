#pragma once

class TankBot;

class BotState
{
public:
	explicit BotState(TankBot *bot);

	virtual ~BotState() = default;

	virtual void update() = 0;
	virtual void onSwitchToState() = 0;

protected:
	TankBot *bot_;
};
