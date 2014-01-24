uv-emitter
=========

Event emitter in C with an API modeled after Node's EventEmitter using libuv

## install

```sh
$ clib install jwerle/uv-emitter
```

## usage

Using an the `getch()` function which [getch.h](https://github.com/jwerle/getch.h) implements you can get the keys pressed in stdin
```c
#include <assert.h>
#include <emitter.h>
#include "getch.h"

static uv_loop_t *loop = NULL;
static emitter_t *emitter = NULL;

static void
get_key (async_work_data_t *work);

static void
on_keypress (void *ch);

int
main (void) {
  loop = uv_default_loop();
  assert(loop);

  emitter = emitter_new(loop);
  assert(emitter);

  assert(0 == emitter_on(emitter, "keypress", on_keypress));

  async(env, loop) {
    assert(env);
    assert(loop);
    wait(env, 500, get_key);
  }

  uv_run(loop, UV_RUN_DEFAULT);
  return 0;
}

static void
get_key (async_work_data_t *work) {
  int ch = 0;
  while (EOF != (ch = getch())) {
    emitter_emit(emitter, "keypress", (void *) ch);
  }
}
  
static void
on_keypress (void *ch) {
  printf("key = %c (%d)\n", (int) ch, (int) ch);
}
```

## api

### emitter_new(loop)

Allocates a new `emitter_t *` pointer with a `uv_loop_t *` pointer bound
to it.

#### example

```c
emitter_t *emitter = emitter_new(uv_default_loop());
```

## types

### emitter_event_cb

A function pointer that is a callback bound to an event.

```c
typedef void (emitter_event_cb)(void *data);
```

### emitter_event_t

A structure that represents an event and its associated callbacks.

```c
async_env_t         *env;     // async environment that the emitter is executed in
struct emitter      *emitter; // emitter struct that the event belongs to
char                *name;    // the name of the event
void                *data;    // data, if any, to pass to the associated callbacks
int                 cb_count; // the amount of callbacks associated with the event
emitter_event_cb_t  *cbs[EMITTER_EVENT_MAX_CALLBACKS; // callbacks associated with this event
```

###

## license

MIT
