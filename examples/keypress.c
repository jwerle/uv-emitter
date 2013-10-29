
#include <assert.h>
#include "../emitter.h"
#include "getch.h"

static uv_loop_t *loop = NULL;
static emitter_t *emitter = NULL;

static void
get_key (async_work_data_t *work);

static void
pulse (async_work_data_t *work);

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
    interval(env, 250, pulse);
  }

  uv_run(loop, UV_RUN_DEFAULT);
  return 0;
}

static void
get_key (async_work_data_t *work) {
  int ch = 0;
  while (EOF != (ch = getch())) {
    emitter_emit(emitter, "keypress", (void *) &ch);
  }
}

static void
pulse (async_work_data_t *work) {
  printf("\r** pulse **\n");
}

static void
on_keypress (void *ch) {
  int c = *(int *) ch;

  printf("key = %c (%d)\n", c, c);
}
