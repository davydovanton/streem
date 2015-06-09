#include "strm.h"
#include "node.h"

struct seq_seeder {
  long n;
  long end;
  long inc;
};

static void
seq_seed(strm_task* task, strm_value data)
{
  struct seq_seeder *s = task->data;

  if (s->n > s->end) {
    strm_close(task);
    return;
  }
  strm_emit(task, strm_int_value(s->n), seq_seed);
  s->n += s->inc;
}

static int
exec_seq(strm_state* strm, int argc, strm_value* args, strm_value* ret)
{
  long start, end, inc=1;
  struct seq_seeder *s;

  switch (argc) {
  case 1:
    start = 1;
    end = strm_value_int(args[0]);
    break;
  case 2:
    start = strm_value_int(args[0]);
    end = strm_value_int(args[1]);
    break;
  case 3:
    start = strm_value_int(args[0]);
    inc = strm_value_int(args[1]);
    end = strm_value_int(args[2]);
    break;
  default:
    node_raise(strm, "wrong number of arguments");
    return 1;
  }
  s = malloc(sizeof(struct seq_seeder));
  s->n = start;
  s->inc = inc;
  s->end = end;
  *ret = strm_task_value(strm_alloc_stream(strm_task_prod, seq_seed, NULL, (void*)s));
  return 0;
}

void
strm_seq_init(strm_state* strm)
{
  strm_var_def("seq", strm_cfunc_value(exec_seq));
}
