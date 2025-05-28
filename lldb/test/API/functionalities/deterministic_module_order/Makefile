C_SOURCES := main.c
include Makefile.rules

# Use full path to shared libraries to make test effective on linux.
LD_EXTRAS := $(BUILDDIR)/liba.so $(BUILDDIR)/libb.so $(BUILDDIR)/libc.so $(BUILDDIR)/libd.so $(BUILDDIR)/libe.so

a.out: liba libb libc libd libe



liba:
	"$(MAKE)" -f $(MAKEFILE_RULES) \
		DYLIB_ONLY=YES DYLIB_C_SOURCES=lib.c DYLIB_NAME=a

libb:
	"$(MAKE)" -f $(MAKEFILE_RULES) \
		DYLIB_ONLY=YES DYLIB_C_SOURCES=lib.c DYLIB_NAME=b

libc:
	"$(MAKE)" -f $(MAKEFILE_RULES) \
		DYLIB_ONLY=YES DYLIB_C_SOURCES=lib.c DYLIB_NAME=c

libd:
	"$(MAKE)" -f $(MAKEFILE_RULES) \
		DYLIB_ONLY=YES DYLIB_C_SOURCES=lib.c DYLIB_NAME=d

libe:
	"$(MAKE)" -f $(MAKEFILE_RULES) \
		DYLIB_ONLY=YES DYLIB_C_SOURCES=lib.c DYLIB_NAME=e
