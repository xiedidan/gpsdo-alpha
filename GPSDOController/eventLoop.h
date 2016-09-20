#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

typedef void (*callback)(void* error, void* param) EventCallback;

typedef struct _EVENT {
  char* eventName,
  EventCallback callback,
  void* next;
} EVENT;

typedef struct _PENDING_EVENT {
  char* eventName,
  void* error,
  void* param,
  void* next
} PENDING_EVENT;

void processEvent();

void on(char* eventName, EventCallback);
void trigger(char* eventName, void* error, void* param);

#endif
