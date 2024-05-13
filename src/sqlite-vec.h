#ifdef __cplusplus
extern "C" {
#endif

#include "sqlite3ext.h"

#define SQLITE_VEC_VERSION "v0.0.1-alpha.9"
#define SQLITE_VEC_DATE "2024-05-13T14:12:41Z+0800"
#define SQLITE_VEC_SOURCE "f532692f7ba476795d9f5ed2f46a9d2269fbab42"


int sqlite3_vec_init(sqlite3 *db, char **pzErrMsg,
                     const sqlite3_api_routines *pApi);
int sqlite3_vec_fs_read_init(sqlite3 *db, char **pzErrMsg,
                             const sqlite3_api_routines *pApi);

#ifdef __cplusplus
}  /* end of the 'extern "C"' block */
#endif