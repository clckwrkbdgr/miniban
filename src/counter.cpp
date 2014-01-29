#include "counter.h"

Counter::Counter(int counter_stop_value)
	: active(false), current_value(0), stop_value(counter_stop_value)
{
}

bool Counter::is_active() const
{
	return active;
}

void Counter::start()
{
	active = true;
	current_value = 0;
}

int Counter::multiplied_value(int factor)
{
	return factor * current_value / stop_value;
}

void Counter::tick(int increase)
{
	if(active) {
		current_value += increase;
		if(current_value >= stop_value) {
			active = false;
		}
	}
}

