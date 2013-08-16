set.user <- function(name, gid=NA) .Call(C_setuser, name, gid)
user.info <- function(users = NULL) .Call(C_userinfo, users)
set.tempdir <- function(path) invisible(.Call(C_setTempDir, path.expand(path)))
