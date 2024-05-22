#ifndef SQLITE_VEC_H
#define SQLITE_VEC_H

#include "sqlite3ext.h"

#define SQLITE_VEC_VERSION "v0.0.1-alpha.10"
#define SQLITE_VEC_DATE "2024-05-22T11:15:27Z+0800"
#define SQLITE_VEC_SOURCE "0a5dcae4c81e0342d3488004a69b3d8e36181031"

#ifdef __cplusplus
extern "C" {
#endif

int sqlite3_vec_init(sqlite3 *db, char **pzErrMsg,
                  const sqlite3_api_routines *pApi);
int sqlite3_vec_fs_read_init(sqlite3 *db, char **pzErrMsg,
                          const sqlite3_api_routines *pApi);


#ifdef __cplusplus
}  /* end of the 'extern "C"' block */
#endif

#endif /* ifndef SQLITE_VEC_H */
