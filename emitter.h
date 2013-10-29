
#ifndef __EMITTER_H__
#define __EMITTER_H__ 1

#include <stdlib.h>
#include <uv.h>
#include "async.h"

#define EMITTER_MAX_EVENTS 50
#define EMITTER_EVENT_MAX_CALLBACKS 50

typedef void (emitter_event_cb)(void *data);

struct emitter;

typedef struct {
  long id;
  emitter_event_cb *cb;
  int once;
} emitter_event_cb_t;

typedef struct emitter_event {
  async_env_t *env;
  struct emitter *emitter;
  char *name;
  void *data;
  int cb_count;
  emitter_event_cb_t *cbs[EMITTER_EVENT_MAX_CALLBACKS];
} emitter_event_t;

typedef struct emitter {
  uv_loop_t *loop;
  int event_count;
  emitter_event_t *events[EMITTER_MAX_EVENTS];
} emitter_t;

// PROTOTYPE

emitter_t *
emitter_new (uv_loop_t *loop);

int
emitter_on (emitter_t *emitter, char *name, emitter_event_cb *cb);

int
emitter_once (emitter_t *emitter, char *name, emitter_event_cb *cb);

int
emitter_off (emitter_t *emitter, char *name, emitter_event_cb *cb);

emitter_event_cb_t **
emitter_listeners (emitter_t *emitter, char *name);

void
emitter_free (emitter_t *emitter);

int
emitter_emit (emitter_t *emitter, char *name, void *data);

static void
_handle_on_event (async_work_data_t *work);

static emitter_event_t *
_get_emitter_event (emitter_t *emitter, char *name);

// IMPLEMENTATION

static emitter_event_t *
_get_emitter_event (emitter_t *emitter, char *name) {
  int i = 0;
  int count = emitter->event_count;
  for (; i < count; ++i) {
    emitter_event_t *event = emitter->events[i];
    if (0 == strcmp(event->name, name)) {
      return event;
    }
  }

  return NULL;
}

static void
_handle_on_event (async_work_data_t *work) {
  emitter_event_t *event = (emitter_event_t *) work->env->data;
  int i = 0;
  int count = event->cb_count;

  for (; i < count; ++i) {
    emitter_event_cb_t *cbh = event->cbs[i];

    if (1 == cbh->once) {
      emitter_off(event->emitter, event->name, cbh->cb);
    }

    cbh->cb(event->data);
  }
}

static int
_create_event_handle (emitter_t *emitter, char *name, emitter_event_cb *cb, int once) {
  int new_event = 0;
  emitter_event_t *event;

  if (NULL == (event = _get_emitter_event(emitter, name))) {
    if (EMITTER_MAX_EVENTS == emitter->event_count) {
      return 1;
    }

    event = emitter->events[emitter->event_count++] = malloc(sizeof(emitter_event_t));
    event->cb_count = 0;
    new_event = 1;
  }

  event->name = name;

  emitter_event_cb_t *cbh = malloc(sizeof(emitter_event_cb_t));
  cbh->cb = cb;
  cbh->once = once;
  cbh->id = (long) cb;

  event->cbs[event->cb_count++] = cbh;

  return 0;
}

emitter_t *
emitter_new (uv_loop_t *loop) {
  emitter_t *emitter = malloc(sizeof(emitter_t));
  if (NULL == emitter) return NULL;
  emitter->loop = loop;
  emitter->event_count = 0;
  return emitter;
}

int
emitter_on (emitter_t *emitter, char *name, emitter_event_cb *cb) {
  return _create_event_handle(emitter, name, cb, 0);
}

int
emitter_once (emitter_t *emitter, char *name, emitter_event_cb *cb) {
  return _create_event_handle(emitter, name, cb, 1);
}

int
emitter_off (emitter_t *emitter, char *name, emitter_event_cb *cb) {
  emitter_event_t *event = _get_emitter_event(emitter, name);
  int i = 0;
  int count = event->cb_count;
  int idx = 0;
  int found = 0;

  if (NULL == event) {
    return 1;
  }

  for (; i < count; ++i) {
    emitter_event_cb_t *cbh = event->cbs[i];
    if (*cb == cbh->cb) {
      idx = i;
      found = 1;
      free(cbh);
      break;
    }
  }

  if (1 == found) {
    i = 0;
    while (i <= count) {
      if (i < idx) {
        i++; continue;
      } else if (i == idx) {
        event->cbs[idx] = event->cbs[++i];
      } else {
        event->cbs[i++] = event->cbs[i];
      }
    }

    event->cb_count--;
    return 0;
  }

  return 1;
}

emitter_event_cb_t **
emitter_listeners (emitter_t *emitter, char *name) {
  emitter_event_t *event = _get_emitter_event(emitter, name);

  if (NULL == event) {
    return NULL;
  }

  return event->cbs;
}

int
emitter_emit (emitter_t *emitter, char *name, void *data) {
  emitter_event_t *event = _get_emitter_event(emitter, name);

  if (NULL == event) {
    return 1;
  }

  async(env, emitter->loop) {
    event->env = env;
    event->data = data;
    event->emitter = emitter;
    env->data = (void *) event;
    queue(env, _handle_on_event);
  }

  return 0;
}

void
emitter_free (emitter_t *emitter) {
  int i = 0;
  int l = 0;
  int n = 0;
  int c = 0;

  l = emitter->event_count;

  //
  // free events
  // complexity: O(log n)
  for (; i < l; ++i) {
    c = emitter->events[i]->cb_count;
    // free event callbacks
    for (; n < c; ++n) {
      free(emitter->events[i]->cbs[n]);
    }

    free(emitter->events[i]);
  }

  // finally free emitter
  free(emitter);
}


#endif
