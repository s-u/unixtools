#include <stdint.h>
#include <Rinternals.h>

extern uintptr_t R_CStackLimit;
extern uintptr_t R_CStackStart;

SEXP C_stackinfo() {
  int dummy;
  intptr_t usage = R_CStackStart - (uintptr_t)&dummy;
  SEXP res = mkNamed(REALSXP, (const char *[]) { "used", "limit", "" });
  if (usage < 0) usage = -usage;
  REAL(res)[0] = (double) usage;
  REAL(res)[1] = (R_CStackLimit == -1) ? NA_REAL : ((double) R_CStackLimit);
  return res;
}
