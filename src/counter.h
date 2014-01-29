#pragma once

struct Counter {
	Counter(int counter_stop_value);
	bool is_active() const;
	void start();
	int multiplied_value(int factor);
	void tick(int increase);
private:
	bool active;
	int current_value, stop_value;
};

