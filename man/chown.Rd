\name{chown}
\alias{chown}
\title{
  Change the ownership of files or other file system objects.
}
\description{
  \code{chown} attepts to change the user of group ownership of items
  specified by paths.
}
\usage{
chown(paths, uid, gid = NA, follow.symlinks = TRUE)
}
\arguments{
  \item{paths}{a character vectors of paths to items (files,
    directories, ..) to change ownership of. \code{path.expand} is
    called implicitly before use.}
  \item{uid}{string specifying a username or numeric scalar specifying
    the \code{uid} or \code{NA} if the owning user is not to be
    changed.}
  \item{gid}{string specifying a groupname or numeric scalar specifying
    the \code{gid} or \code{NA} if the group is not to be changed. If
    \code{uid} is a username then \code{gid} can also be set to
    \code{NULL} in which case the \code{gid} is set to the primary group
    of the user.}
  \item{follow.symlinks}{if \code{FALSE} then only the ownership of the
    item specified by the path is changed. If \code{TRUE} and the target
    is a symlink then the ownership of the final target is changed, not
    the symlink itself.}
}
\details{
  Typically, regular users can only change \code{gid} and only to a
  group of which they are a member. Only the superuser (\code{root}) can
  change the owner \code{uid}.

  Both \code{uid} and \code{gid} cannot be \code{NA}, that is currently
  treated as an error.
}
\value{
  Logical vector of the same length as \code{paths} with \code{TRUE} for
  successful ownership change and \code{FALSE} if the action could not
  be performed on that item.
}
%\references{
%}
\author{
  Simon Urbanek
}
%\seealso{
%}
%\examples{
%}
\keyword{IO}
