#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

typedef void (*EventCallback)(void* error, void* param);

typedef struct _EVENT {
  char* eventName;
  EventCallback callback;
  void* next;
} EVENT;

typedef struct _PENDING_EVENT {
  char* eventName;
  void* error;
  void* param;
  void* next;
} PENDING_EVENT;

typedef struct _TIMER {
  char* eventName;
  unsigned long start;
  unsigned long lifespan;
  EventCallback callback;
  void* next;
} TIMER;

void processEvent();

void on(char* eventName, EventCallback callback);
void off(char* eventName);
void trigger(char* eventName, void* error, void* param);
void sleep(int ms, EventCallback callback);

// helper
void checkTimer();

#endif
