set(SRCS
  stat.c
  statPkg.c
  StatTrace.c
  StatProblemsGenerator.c
  StatProblemsGeneratorBmc.c
  statSexpProblem.c
  )

nusmv_add_pkg_lib(${SRCS})

nusmv_add_pkg_headers(
  stat.h
  statInt.h
  StatTrace.h
  StatProblemsGenerator.h
  StatProblemsGeneratorBmc.h
  )
