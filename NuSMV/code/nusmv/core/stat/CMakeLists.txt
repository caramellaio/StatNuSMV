set(SRCS
  stat.c
  statPkg.c
  StatTrace.c
  )

nusmv_add_pkg_lib(${SRCS})

nusmv_add_pkg_headers(
  stat.h
  StatTrace.h
  )
