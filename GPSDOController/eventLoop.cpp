#include <Arduino.h>
#include "eventLoop.h"

EVENT* eventList = NULL;
PENDING_EVENT* pendingEventList = NULL;
TIMER* timerList = NULL;

void processEvent() {
  checkTimer();
  
  EVENT* event = eventList;
  if (pendingEventList != NULL && event != NULL) {
      if (strcmp(pendingEventList->eventName, event->eventName) == 0) {
        // callback
        EventCallback callback = event->callback;
        callback(pendingEventList->error, pendingEventList->param);

        // move on to next event
        PENDING_EVENT* currEvent = pendingEventList;
        pendingEventList = (PENDING_EVENT*)pendingEventList->next;

        // remove current event
        free(currEvent->eventName);
        if (currEvent->error)
          free(currEvent->error);
        if (currEvent->param)
          free(currEvent->param);
        free(currEvent);
      }
  }
}

void on(char* eventName, EventCallback callback) {
  if (eventList == NULL) {
    EVENT* newEvent = (EVENT*)malloc(sizeof(EVENT));
    
    int len = strlen(eventName);
    newEvent->eventName = (char*)malloc(len + 1);
    newEvent->eventName[len] = '\0';
    memcpy(newEvent->eventName, eventName, len);
    
    newEvent->callback = callback;
    newEvent->next = NULL;
    eventList = newEvent;
  }
  else {
    EVENT* event = eventList;

    // attach to the tail
    while (event->next != NULL) {
      event = (EVENT*)event->next;
    }

    event->next = (EVENT*)malloc(sizeof(EVENT));
    event = (EVENT*)event->next;

    int len = strlen(eventName);
    event->eventName = (char*)malloc(len + 1);
    event->eventName[len] = '\0';
    memcpy(event->eventName, eventName, len);

    event->callback = callback;
    event->next = NULL;
  }
}

void off(char* eventName) {
  EVENT* event = eventList;
  EVENT* prevEvent = NULL;
  
  while (event != NULL) {
    if (strcmp(eventName, event->eventName) == 0) {
      if (prevEvent == NULL)
        eventList = (EVENT*)event->next;
      else
        prevEvent->next = event->next;
        
      EVENT* currEvent = event;
      event = (EVENT*)event->next;

      free(currEvent->eventName);
      free(currEvent);

      break;
    }
    else {
      prevEvent = event;
      event = (EVENT*)event->next;
    }
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
    
    // attach to the tail of pendingEventList
    while (pendingEvent->next != NULL) {
      pendingEvent = (PENDING_EVENT*)pendingEvent->next;
    }

    pendingEvent->next = (PENDING_EVENT*)malloc(sizeof(PENDING_EVENT));
    pendingEvent = (PENDING_EVENT*)pendingEvent->next;
    pendingEvent->eventName = eventName;
    pendingEvent->error = error;
    pendingEvent->param = param;
    pendingEvent->next = NULL;
  }
}

void sleep(unsigned long ms, EventCallback callback) {
  // create a name
  unsigned long now = millis();
  char* eventName = (char*)malloc(8);
  sprintf(eventName, "%x", now);
  
  on(eventName, callback);
}

void checkTimer() {
  unsigned long now = millis();
  TIMER* timer = timerList;
  TIMER* prevTimer = NULL;
  
  while (timer != NULL) {
    if (timer->lifespan <= now - timer->start) {
      // alarm
      trigger(timer->eventName, NULL, NULL);
      
      // remove this timer
      if (prevTimer == NULL)
        timerList = (TIMER*)timer->next;
      else
        prevTimer->next = timer->next;
        
      TIMER* currTimer = timer;
      timer = (TIMER*)timer->next;

      // clear event
      off(timer->eventName);
      
      free(currTimer->eventName);
      free(currTimer);
    }
    else {
      prevTimer = timer;
      timer = (TIMER*)timer->next; 
    }
  }
}

