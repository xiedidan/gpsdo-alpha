# include "eventLoop.h"

EVENT* eventList = NULL;
PENDING_EVENT* pendingEventList = NULL;

void processEvent() {
  EVENT* event = eventList;
  if (pendingEventList != NULL) {
    while (event) {
      if (strcmp(pendingEventList->eventName, event->eventName) == 0) {
        // callback
        *(event->callback)(pendingEventList->error, pendingEventList->param);

        // move on to next event
        PENDING_EVENT* currEvent = pendingEventList;
        pendingEventList = pendingEventList->next;
        free(currEvent);

        break;
      }
      
      event = event->next;
    }
  }
}

void on(char* eventName, EventCallback callback) {
  if (eventList == NULL) {
    EVENT* newEvent = (EVENT*)malloc(sizeof(EVENT));
    newEvent->eventName = eventName;
    newEvent->callback = callback;
    newEvent->next = NULL;
    eventList = newEvent;
  }
  else {
    EVENT* event = eventList;
    while (event->next != NULL) {
      event = event->next;
    }

    event->next = (EVENT*)malloc(sizeof(EVENT));
    event = event->next;
    event->eventName = eventName;
    event->callback = callback;
    event->next = NULL;
  }
}

void trigger(char* eventName, void* error, void* param) {
  if (pendingEventList == NULL) {
    PENDING_EVENT* newPendingEvent = (PENDING_EVENT*)malloc(sizeof(PENDING_EVENT));
    newPendingEvent->eventName = eventName;
    newPendingEvent->error = error;
    newPendingEvent->param = param;
    newPendingEvent->next = NULL;
    pendingEventList = newPendingEvent;
  }
  else {
    PENDING_EVENT* pendingEvent = pendingEventList;
    while (pendingEvent->next != NULL) {
      pendingEvent = pendingEvent->next;
    }

    pendingEvent->next = (PENDING_EVENT*)malloc(sizeof(PENDING_EVENT));
    pendingEvent = pendingEvent->next;
    pendingEvent->eventName = eventName;
    pendingEvent->error = error;
    pendingEvent->param = param;
    pendingEvent->next = NULL;
  }
}

