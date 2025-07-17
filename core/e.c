#include "e.h"
#include "print.h"
#include <stdlib.h>

error_t unwrap(error_t err) {
  if (!err.null) {
    printfn("error: %s", err.src);
    exit(err.status);
  }
  return err;
}
