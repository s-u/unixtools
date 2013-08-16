\name{user.info}
\alias{user.info}
\title{
  Get unix user information
}
\description{
  \code{user.info} queries the user database to obtain information about
  a list of users.
}
\usage{
user.info(users = NULL)
}
\arguments{
  \item{users}{a character string of user names, numeric vectors of user
    IDs or \code{NULL} to obtain information about the current user}
}
\details{
  \code{user.info} queries the user database to find the specified
  users. Each element of \code{users} will result in one row in the
  resulting data frame. If the user cannot be found, all entries in the
  row will be \code{NA}.

  The database implementation is system-specific. It is not limited to
  the legacy \code{/etc/passwd} but insead uses \code{getpw*} system
  calls which may also query NIS, DomainServices, LDAP and other sources
  as configured in the system.
}
\value{
  data frame with \code{users} as row names and the columns \code{name},
  \code{uid}, \code{gid}, \code{realname}, \code{home} and \code{shell}.
  Columns \code{uid} and \code{gid} are integer vectors, all others are
  character vectors.
}
%\references{
%}
\author{
  Simon Urbanek
}
%\seealso{
%}
\examples{
user.info()
user.info(c("root", "admin", "daemon"))
user.info(1:10)
}
\keyword{IO}
