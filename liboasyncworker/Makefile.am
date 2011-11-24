AM_CPPFLAGS =                                                   \
	-I$(top_srcdir)                                         \
	$(LIBOASYNCWORKER_CFLAGS) 				\
	$(NULL)

lib_LTLIBRARIES = liboasyncworker-1.0.la

liboasyncworker_1_0_la_LIBADD = 	\
	$(LIBOASYNCWORKER_LIBS)	\
	$(NULL)

liboasyncworker_1_0_la_SOURCES = \
	shared-priv.h		\
	oasyncworkertask-priv.h oasyncworker-priv.h \
	oasyncworker.c	oasyncworker.h	 \
	oasyncworkertask.c oasyncworkertask.h \
	$(NULL)

