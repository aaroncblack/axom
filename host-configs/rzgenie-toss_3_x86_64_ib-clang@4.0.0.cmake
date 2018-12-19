##################################
# !!!! This is a generated file, edit at own risk !!!!
##################################

##################################
#
# Copyright (c) 2017-2018, Lawrence Livermore National Security, LLC.
#
# Produced at the Lawrence Livermore National Laboratory.
#
# LLNL-CODE-741217
#
# All rights reserved.
#
# This file is part of Axom.
#
# For details about use and distribution, please read axom/LICENSE.
#
##################################

##################################

# SYS_TYPE: toss_3_x86_64_ib
# Compiler Spec: clang@4.0.0
##################################

# CMake executable path: /usr/WS1/axom/thirdparty_libs/builds/2018_12_18_13_23_11/clang-4.0.0/cmake-3.9.6/bin/cmake

##############
# Compilers
##############

# C compiler used by spack
set(CMAKE_C_COMPILER "/usr/tce/packages/clang/clang-4.0.0/bin/clang" CACHE PATH "")

# C++ compiler used by spack
set(CMAKE_CXX_COMPILER "/usr/tce/packages/clang/clang-4.0.0/bin/clang++" CACHE PATH "")

# Fortran compiler used by spack
set(ENABLE_FORTRAN ON CACHE BOOL "")

set(CMAKE_Fortran_COMPILER "/usr/tce/packages/gcc/gcc-4.9.3/bin/gfortran" CACHE PATH "")

##############
# TPLs
##############

# Root directory for generated TPLs
set(TPL_ROOT "/usr/WS1/axom/thirdparty_libs/builds/2018_12_18_13_23_11/clang-4.0.0" CACHE PATH "")

# hdf5 from uberenv
set(HDF5_DIR "${TPL_ROOT}/hdf5-1.8.19" CACHE PATH "")

# scr not built by uberenv

# conduit from uberenv
set(CONDUIT_DIR "${TPL_ROOT}/conduit-0.3.1" CACHE PATH "")

# mfem from uberenv
set(MFEM_DIR "${TPL_ROOT}/mfem-3.4.0" CACHE PATH "")

# python from uberenv
set(PYTHON_EXECUTABLE "${TPL_ROOT}/python-2.7.15/bin/python" CACHE PATH "")

set(ENABLE_DOCS ON CACHE BOOL "")

# doxygen from uberenv
set(DOXYGEN_EXECUTABLE "${TPL_ROOT}/doxygen-1.8.12/bin/doxygen" CACHE PATH "")

# sphinx 1.7.4 from uberenv
set(SPHINX_EXECUTABLE "${TPL_ROOT}/python-2.7.15/bin/sphinx-build" CACHE PATH "")

# shroud 0.10.1 from uberenv
set(SHROUD_EXECUTABLE "${TPL_ROOT}/python-2.7.15/bin/shroud" CACHE PATH "")

# uncrustify from uberenv
set(UNCRUSTIFY_EXECUTABLE "${TPL_ROOT}/uncrustify-0.61/bin/uncrustify" CACHE PATH "")

# lcov and genhtml not built by uberenv

# cppcheck from uberenv
set(CPPCHECK_EXECUTABLE "${TPL_ROOT}/cppcheck-1.81/bin/cppcheck" CACHE PATH "")

##############
# MPI
##############

set(ENABLE_MPI ON CACHE BOOL "")

set(MPI_C_COMPILER "/usr/tce/packages/mvapich2/mvapich2-2.2-clang-4.0.0/bin/mpicc" CACHE PATH "")

set(MPI_CXX_COMPILER "/usr/tce/packages/mvapich2/mvapich2-2.2-clang-4.0.0/bin/mpicxx" CACHE PATH "")

set(MPI_Fortran_COMPILER "/usr/tce/packages/mvapich2/mvapich2-2.2-clang-4.0.0/bin/mpif90" CACHE PATH "")

set(MPIEXEC "/usr/bin/srun" CACHE PATH "")

set(MPIEXEC_NUMPROC_FLAG "-n" CACHE PATH "")

##############
# Other machine specifics
##############

set(ENABLE_GTEST_DEATH_TESTS ON CACHE BOOL "")


