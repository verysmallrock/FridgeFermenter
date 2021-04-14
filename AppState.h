#ifndef APPSTATE_INCLUDE
#define APPSTATE_INCLUDE

struct AppState {
	bool valid;
	unsigned long lastInputTime;
	unsigned long maxIdleTime;
};
#define DEFAULT_STATE { true, 0, 15000 }

extern AppState state;

void loadAppState();
void saveAppState();

#endif
