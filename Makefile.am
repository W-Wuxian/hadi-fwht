# ============================================================================
# Makefile.am - Automake input for the FWHT library
# ============================================================================

ACLOCAL_AMFLAGS = -I m4

AM_CFLAGS = -std=c99 -O3 -Wall -Wextra -pedantic -pthread -I$(top_srcdir)/include $(OPENMP_CFLAGS) $(SIMD_CFLAGS)
AM_CXXFLAGS = -O3 -Wall -Wextra -pedantic -pthread -I$(top_srcdir)/include $(OPENMP_CFLAGS) $(SIMD_CFLAGS)
AM_LDFLAGS = $(OPENMP_CFLAGS)

# ----------------------------------------------------------------------
# Bibliotheque (statique + partagee gere automatiquement par Libtool)
# ----------------------------------------------------------------------
lib_LTLIBRARIES = libfwht.la

libfwht_la_SOURCES = \
    src/fwht_backend.c \
    src/fwht_batch.c \
    src/fwht_core.c \
    src/fwht_sbox.c \
    src/fwht_simd_avx2.c

# IMPORTANT : comme libfwht_la_LDFLAGS est defini specifiquement pour
# cette cible, Automake N'APPLIQUE PLUS AM_LDFLAGS automatiquement ici.
# Il faut donc reprendre explicitement $(OPENMP_CFLAGS) et, si CUDA est
# actif, $(CUDA_LDFLAGS) -- sinon libfwht.so se construit sans erreur
# (un .so peut avoir des symboles non resolus) mais tout programme qui
# s'y lie echoue avec "undefined reference to 'cudaMalloc'" etc.
libfwht_la_LDFLAGS = -version-info 1:0:0 $(OPENMP_CFLAGS)

include_HEADERS = include/fwht.h

# ----------------------------------------------------------------------
# Sources CUDA (compilees seulement si HAVE_CUDA).
# ----------------------------------------------------------------------
if HAVE_CUDA
AM_CFLAGS += -DUSE_CUDA $(CUDA_INCFLAGS)
AM_CXXFLAGS += -DUSE_CUDA $(CUDA_INCFLAGS)
AM_LDFLAGS += $(CUDA_LDFLAGS)
libfwht_la_LDFLAGS += $(CUDA_LDFLAGS)

NVCCFLAGS = -O3 -I$(top_srcdir)/include --compiler-options -fPIC -std=c++17 -DUSE_CUDA $(NVCC_ARCH_FLAGS)

src/fwht_cuda.o: $(top_srcdir)/src/fwht_cuda.cu
	$(MKDIR_P) src
	$(NVCC) $(NVCCFLAGS) -c $(top_srcdir)/src/fwht_cuda.cu -o $@

src/fwht_cuda_lat.o: $(top_srcdir)/src/fwht_cuda_lat.cu
	$(MKDIR_P) src
	$(NVCC) $(NVCCFLAGS) -c $(top_srcdir)/src/fwht_cuda_lat.cu -o $@

libfwht_la_LIBADD = src/fwht_cuda.o src/fwht_cuda_lat.o -lstdc++
libfwht_la_DEPENDENCIES = src/fwht_cuda.o src/fwht_cuda_lat.o
endif

# ----------------------------------------------------------------------
# Outils CLI (tools/)
# ----------------------------------------------------------------------
bin_PROGRAMS = fwht_cli backend_tuner

fwht_cli_SOURCES = tools/fwht_cli.c
fwht_cli_LDADD = libfwht.la -lm

backend_tuner_SOURCES = tools/backend_tuner.c
backend_tuner_LDADD = libfwht.la -lm

# ----------------------------------------------------------------------
# Tests (make check, seulement si --enable-tests)
# ----------------------------------------------------------------------
if RUN_TESTS
check_PROGRAMS = test_correctness
TESTS = $(check_PROGRAMS)

test_correctness_SOURCES = tests/test_correctness.c
test_correctness_LDADD = libfwht.la -lm

if HAVE_CUDA
check_PROGRAMS += test_gpu
test_gpu_SOURCES = tests/test_gpu.c
test_gpu_LDADD = libfwht.la -lm
TESTS += test_gpu
endif
endif

# ----------------------------------------------------------------------
# Exemples (make examples, pas inclus dans make all)
# ----------------------------------------------------------------------
EXTRA_PROGRAMS = example_basic example_boolean_packed example_batch

example_basic_SOURCES = examples/example_basic.c
example_basic_LDADD = libfwht.la -lm

example_boolean_packed_SOURCES = examples/example_boolean_packed.c
example_boolean_packed_LDADD = libfwht.la -lm

example_batch_SOURCES = examples/example_batch.c
example_batch_LDADD = libfwht.la -lm

# Exemples GPU (.cu)
if HAVE_CUDA
EXTRA_PROGRAMS += example_gpu_multi_precision example_gpu_callbacks

examples/example_gpu_multi_precision.o: $(top_srcdir)/examples/example_gpu_multi_precision.cu
	$(MKDIR_P) examples
	$(NVCC) $(NVCCFLAGS) -c $(top_srcdir)/examples/example_gpu_multi_precision.cu -o $@

example_gpu_multi_precision$(EXEEXT): examples/example_gpu_multi_precision.o libfwht.la
	$(NVCC) $(NVCCFLAGS) examples/example_gpu_multi_precision.o -L.libs -lfwht $(CUDA_LDFLAGS) -o $@ -Xlinker -rpath -Xlinker $(abs_builddir)/.libs

examples/example_gpu_callbacks.o: $(top_srcdir)/examples/example_gpu_callbacks.cu
	$(MKDIR_P) examples
	$(NVCC) $(NVCCFLAGS) -c $(top_srcdir)/examples/example_gpu_callbacks.cu -o $@

example_gpu_callbacks$(EXEEXT): examples/example_gpu_callbacks.o libfwht.la
	$(NVCC) $(NVCCFLAGS) examples/example_gpu_callbacks.o -L.libs -lfwht $(CUDA_LDFLAGS) -o $@ -Xlinker -rpath -Xlinker $(abs_builddir)/.libs
endif

.PHONY: examples
examples: $(EXTRA_PROGRAMS)
