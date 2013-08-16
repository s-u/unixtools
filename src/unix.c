#include <sys/types.h>
#include <sys/stat.h>
#include <grp.h>
#include <pwd.h>
#include <unistd.h>

#include <Rinternals.h>

SEXP C_setuser(SEXP sName, SEXP sGID) {
    int gid = asInteger(sGID);
    struct passwd *p;
    if (TYPEOF(sName) == STRSXP) {
	if (LENGTH(sName) != 1)
	    Rf_error("user name must be a string");
	p = getpwnam(CHAR(STRING_ELT(sName, 0)));
	if (!p)
	    Rf_error("user '%s' not found", CHAR(STRING_ELT(sName, 0)));
	if (gid >= 0 && setgid(gid))
	    Rf_error("failed to set gid (%d)", gid);
	else {
	    if (setgid(p->pw_gid))
		Rf_error("failed to set gid (%d) from passwd for user %s", p->pw_gid, CHAR(STRING_ELT(sName, 0)));
	    initgroups(p->pw_name, p->pw_gid);
	}
	if (setuid(p->pw_uid))
	    Rf_error("failed to set uid (%d) from passwd for user %s", p->pw_uid, CHAR(STRING_ELT(sName, 0)));
	return ScalarInteger(p->pw_uid);
    } else {
	int uid = asInteger(sName);
	if (gid >= 0 && setgid(gid)) Rf_error("failed to set gid (%d)", gid);
	if (uid >= 0 && setuid(uid)) Rf_error("failed to set uid (%d)", uid);
    }
    return ScalarLogical(1);
}

SEXP C_userinfo(SEXP sName) {
    SEXP res;
    struct passwd *p;
    int pc = 1, n, i;
    int *uid, *gid;
    SEXP name, realname, home, shell;
    if (TYPEOF(sName) == REALSXP) {
	sName = PROTECT(coerceVector(sName, INTSXP));
	pc++;
    }
    if (sName == R_NilValue) {
	sName = PROTECT(ScalarInteger(getuid()));
	pc++;
    }
    if (TYPEOF(sName) != STRSXP && TYPEOF(sName) != INTSXP)
	Rf_error("user must be either a numeric vector, character vector or NULL");
    n = LENGTH(sName);
    res = PROTECT(mkNamed(VECSXP, (const char *[]) { "name", "uid", "gid", "realname", "home", "shell", "" }));
    name     = SET_VECTOR_ELT(res, 0, allocVector(STRSXP, n));
    uid      = INTEGER(SET_VECTOR_ELT(res, 1, allocVector(INTSXP, n)));
    gid      = INTEGER(SET_VECTOR_ELT(res, 2, allocVector(INTSXP, n)));
    realname = SET_VECTOR_ELT(res, 3, allocVector(STRSXP, n));
    home     = SET_VECTOR_ELT(res, 4, allocVector(STRSXP, n));
    shell    = SET_VECTOR_ELT(res, 5, allocVector(STRSXP, n));
    for (i = 0; i < n; i++) {
	if (TYPEOF(sName) == INTSXP && INTEGER(sName)[i] < 0)
	    Rf_error("invalid user element %d - uids may not be negative or NA", i + 1);
	if (TYPEOF(sName) == STRSXP && STRING_ELT(sName, i) == R_NaString)
	    Rf_error("invalid user element %d - user names may not be NA", i + 1);
	p = (TYPEOF(sName) == STRSXP) ? getpwnam(CHAR(STRING_ELT(sName, i))) : getpwuid(INTEGER(sName)[i]);
	SET_STRING_ELT(name, i, (p && p->pw_name) ? mkChar(p->pw_name) : R_NaString);
	uid[i] = p ? p->pw_uid : R_NaInt;
	gid[i] = p ? p->pw_gid : R_NaInt;
	SET_STRING_ELT(realname, i, (p && p->pw_gecos) ? mkChar(p->pw_gecos) : R_NaString);
	SET_STRING_ELT(home, i, (p && p->pw_dir) ? mkChar(p->pw_dir) : R_NaString);
	SET_STRING_ELT(shell, i, (p && p->pw_shell) ? mkChar(p->pw_shell) : R_NaString);
    }
    setAttrib(res, R_ClassSymbol, mkString("data.frame"));
    setAttrib(res, R_RowNamesSymbol, sName);
    UNPROTECT(pc);
    return res;
}

SEXP C_chown(SEXP sFn, SEXP sUid, SEXP sGid, SEXP sFollow) {
    SEXP res;
    int i, n, *ok, uid = -1, gid = -1, follow = asInteger(sFollow);
    if (TYPEOF(sFn) != STRSXP) Rf_error("invalid paths");
    n = LENGTH(sFn);

    if (TYPEOF(sUid) == STRSXP) {
	if (LENGTH(sUid) == 1) {
	    struct passwd *p = getpwnam(CHAR(STRING_ELT(sUid, 0)));
	    if (!p) Rf_error("User '%s' not found", CHAR(STRING_ELT(sUid, 0)));
	    uid = p->pw_uid;
	    if (sGid == R_NilValue)
		gid = p->pw_gid;
	} else Rf_error("Invalid user specification, must be a string or numeric scalar");
    } else if (sUid != R_NilValue)
	uid = asInteger(sUid);
    
    if (TYPEOF(sGid) == STRSXP) {
	if (LENGTH(sGid) == 1) {
	    struct group *g = getgrnam(CHAR(STRING_ELT(sGid, 0)));
	    if (!g) Rf_error("Group '%s' not found", CHAR(STRING_ELT(sGid, 0)));
	    gid = g->gr_gid;
	} else Rf_error("Invalid group specification, must be a string or numeric scalar");
    } else if (sGid != R_NilValue)
	gid = asInteger(sGid);

    if (gid < 0 && uid < 0)
	Rf_error("Either uid or gid must be set to valid values");

    ok = LOGICAL(res = allocVector(LGLSXP, n));
    for (i = 0; i < n; i++) {
	int this_gid = gid, this_uid = uid;

	/* if either uid or gid is not set, we have to get it from the existing file */
	if (gid < 0 || uid < 0) {
	    struct stat st;
	    if (follow ? stat(CHAR(STRING_ELT(sFn, i)), &st) : lstat(CHAR(STRING_ELT(sFn, i)), &st)) {
		ok[i] = FALSE;
		continue;
	    }
	    if (gid < 0) this_gid = st.st_gid;
	    if (uid < 0) this_uid = st.st_uid;
	}

	ok[i] = (follow ? chown(CHAR(STRING_ELT(sFn, i)), this_uid, this_gid) : lchown(CHAR(STRING_ELT(sFn, i)), this_uid, this_gid)) ? FALSE : TRUE;
    }
    return res;
}
