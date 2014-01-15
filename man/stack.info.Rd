\name{stack.info}
\alias{stack.info}
\title{
  Get C stack usage information
}
\description{
  \code{stack.info} return the current C stack usage and the current limit.
}
\usage{
stack.info()
}
\value{
A named numeric vector with elements
\item{usage}{Number of bytes used in the C stack so far}
\item{limit}{Current C stack size limit or \code{NA} if disabled}
}
\author{
  Simon Urbanek
}
\examples{
stack.info()
}
\keyword{debugging}
