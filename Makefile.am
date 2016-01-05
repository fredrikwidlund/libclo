ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4
AM_CFLAGS = -std=gnu11 -O3 -flto -fuse-linker-plugin
AM_LDFLAGS = -static

SOURCE_FILES = \
src/clo.c

HEADER_FILES = \
src/clo.h

AUTOMAKE_OPTIONS = subdir-objects
lib_LTLIBRARIES= libclo.la
libclo_la_SOURCES = $(SOURCE_FILES) $(HEADER_FILES)

headerfilesdir = $(includedir)
headerfiles_HEADERS = $(HEADER_FILES)

MAINTAINERCLEANFILES = aclocal.m4 config.h.in configure Makefile.in libclo-?.?.?.tar.gz
maintainer-clean-local:; rm -rf autotools m4 libclo-?.?.?
