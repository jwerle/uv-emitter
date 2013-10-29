
#include <stdio.h>
#include <assert.h>
#include "emitter.h"

static emitter_t *emitter = NULL;

static void
on_foo1 (void *data);

static void
on_foo2 (void *data);

static void
on_bar1 (void *data);

static void
on_bar2 (void *data);

static void
emit_foo (async_work_data_t *work);

int
main (void) {
  emitter = emitter_new(uv_default_loop());
  emitter_event_cb_t **foo_listeners;
  emitter_event_cb_t **bar_listeners;

  assert(emitter);

  // printf("\n");
  // printf(" - on_foo1 %d\n - on_foo2 %d\n - on_bar1 %d\n - on_bar2 %d\n",
  //     (int) &on_foo1,
  //     (int) &on_foo2,
  //     (int) &on_bar1,
  //     (int) &on_bar2);
  // printf("\n");

  //
  // test emptyness of listeners for both events
  //
  assert(NULL == emitter_listeners(emitter, "foo"));
  assert(NULL == emitter_listeners(emitter, "bar"));

  //
  // assert that all events have been bound
  //
  assert(0 == emitter_on(emitter, "foo", on_foo1));
  assert(0 == emitter_on(emitter, "foo", on_foo2));
  assert(0 == emitter_on(emitter, "bar", on_bar1));
  assert(0 == emitter_once(emitter, "bar", on_bar2));

  //
  // assert that both events have listeners
  //
  assert(emitter_listeners(emitter, "foo"));
  assert(emitter_listeners(emitter, "bar"));

  //
  // get listeners for foo and bar
  //
  foo_listeners = emitter_listeners(emitter, "foo");
  bar_listeners = emitter_listeners(emitter, "bar");

  //
  // assert that we now have listeners
  //
  assert(foo_listeners);
  assert(bar_listeners);

  //
  // assert that the bound function pointers are set
  // on the listeners array
  //
  assert(on_foo1 == foo_listeners[0]->cb);
  assert(on_foo2 == foo_listeners[1]->cb);
  assert(on_bar1 == bar_listeners[0]->cb);
  assert(on_bar2 == bar_listeners[1]->cb);

  //
  // assert that the listeners ids are the
  // address value of the function pointers
  //
  assert((long) &on_foo1 == foo_listeners[0]->id);
  assert((long) &on_foo2 == foo_listeners[1]->id);
  assert((long) &on_bar1 == bar_listeners[0]->id);
  assert((long) &on_bar2 == bar_listeners[1]->id);

  async(env, uv_default_loop()) {
    queue(env, emit_foo);
  }

  uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  return 0;
}

static void
on_foo1 (void *data) {
  assert(42 == (unsigned long) data);
  printf("#1\n");
  usleep(5000); // this will block next event to be called
                // for 5ms
}

static void
on_foo2 (void *data) {
  if (NULL != data) {
    assert(0 == emitter_off(emitter, "foo", on_foo1));
    assert(42 == (unsigned long) data);
    printf("#2\n");
    assert(0 == emitter_emit(emitter, "foo", NULL));
  } else {
    printf("#3\n");
    assert(0 == emitter_off(emitter, "foo", on_foo2));
    assert(0 == emitter_emit(emitter, "bar", (void *)1));
  }
}

static void
on_bar1 (void *data) {
  printf("#4\n");
}

static void
on_bar2 (void *data) {
  printf("#5\n");
  assert(1 == emitter_off(emitter, "bar", on_bar2));
  emitter_free(emitter);
}

static void
emit_foo (async_work_data_t *work) {
  emitter_emit(emitter, "foo", (void *) 42);
}
