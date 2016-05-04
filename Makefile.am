ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4
AM_CFLAGS = -std=gnu11 -O3 -flto -fuse-linker-plugin
AM_LDFLAGS = -static

SOURCE_FILES = \
src/clo.c

HEADER_FILES = \
src/clo.h

AUTOMAKE_OPTIONS = subdir-objects
lib_LTLIBRARIES = libclo.la
libclo_la_SOURCES = $(SOURCE_FILES) $(HEADER_FILES)

headerfilesdir = $(includedir)
headerfiles_HEADERS = $(HEADER_FILES)

### example ###

bin_PROGRAMS = example/hello
example_hello_LDADD = -lclo

### unit tests ###

CHECK_CFLAGS = -std=gnu11 -O0 -g -ftest-coverage -fprofile-arcs
CHECK_LDADD = -L. -lclo_test -lcmocka

check_LIBRARIES = libclo_test.a
libclo_test_a_CFLAGS = $(CHECK_CFLAGS)
libclo_test_a_SOURCES = $(SOURCE_FILES) $(HEADER_FILES)

check_PROGRAMS = test/clo
test_clo_CFLAGS = $(CHECK_CFLAGS)
test_clo_LDADD = $(CHECK_LDADD)
test_clo_LDFLAGS = $(CHECK_LDFLAGS_EXTRA)
test_clo_SOURCES = test/clo.c

dist_noinst_SCRIPTS = test/valgrind.sh test/coverage.sh

TESTS = $(check_PROGRAMS) test/valgrind.sh test/coverage.sh
CLEANFILES = {.,src,test}/*.{gcno,gcda,gcov}

MAINTAINERCLEANFILES = aclocal.m4 config.h.in configure Makefile.in libclo-?.?.?.tar.gz clo.c.gcov
maintainer-clean-local:; rm -rf autotools m4 libclo-?.?.?
