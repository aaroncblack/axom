# Copyright (c) 2017-2023, Lawrence Livermore National Security, LLC and
# other Axom Project Developers. See the top-level LICENSE file for details.
#
# SPDX-License-Identifier: (BSD-3-Clause)
#------------------------------------------------------------------------------
# Axom's Core component
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Specify all headers/sources
#------------------------------------------------------------------------------

# This file contains metadata that often changes (ie. git sha).
# Hide these changes in the source file so we do not force
# a full rebuild.
axom_configure_file(
    utilities/About.cpp.in
    ${PROJECT_BINARY_DIR}/axom/core/utilities/About.cpp
)

set(core_headers

    ## utilities
    utilities/AnnotationMacros.hpp
    utilities/BitUtilities.hpp
    utilities/FileUtilities.hpp
    utilities/StringUtilities.hpp
    utilities/System.hpp
    utilities/Timer.hpp
    utilities/Utilities.hpp
    utilities/About.hpp

    utilities/nvtx/interface.hpp
    utilities/nvtx/Macros.hpp
    utilities/nvtx/Range.hpp

    ## numerics
    numerics/internal/matrix_norms.hpp

    numerics/Determinants.hpp
    numerics/LU.hpp
    numerics/Matrix.hpp
    numerics/eigen_solve.hpp
    numerics/eigen_sort.hpp
    numerics/floating_point_limits.hpp
    numerics/jacobi_eigensolve.hpp
    numerics/linear_solve.hpp
    numerics/matvecops.hpp
    numerics/polynomial_solvers.hpp

    ## core
    Array.hpp
    ArrayBase.hpp
    ArrayIteratorBase.hpp
    ArrayView.hpp
    IteratorBase.hpp
    Macros.hpp
    Map.hpp
    Path.hpp
    StackArray.hpp
    Types.hpp
    memory_management.hpp

    ## execution
    execution/execution_space.hpp
    execution/for_all.hpp
    execution/synchronize.hpp

    execution/internal/seq_exec.hpp
    execution/internal/omp_exec.hpp
    execution/internal/cuda_exec.hpp
    execution/internal/hip_exec.hpp

    )

set(core_sources
    utilities/FileUtilities.cpp
    utilities/StringUtilities.cpp
    utilities/System.cpp
    utilities/Utilities.cpp
    ${PROJECT_BINARY_DIR}/axom/core/utilities/About.cpp

    utilities/nvtx/interface.cpp
    utilities/nvtx/Range.cpp

    numerics/polynomial_solvers.cpp

    Path.cpp
    Types.cpp
    )

#------------------------------------------------------------------------------
# Set library dependencies
#------------------------------------------------------------------------------

set( core_depends fmt )
blt_list_append( TO core_depends ELEMENTS camp IF CAMP_FOUND )
blt_list_append( TO core_depends ELEMENTS umpire IF UMPIRE_FOUND )
blt_list_append( TO core_depends ELEMENTS RAJA IF RAJA_FOUND )
list(APPEND core_depends ${axom_device_depends})
blt_list_append( TO core_depends ELEMENTS nvToolsExt IF AXOM_ENABLE_CUDA )

blt_list_append( TO core_depends ELEMENTS openmp IF AXOM_ENABLE_OPENMP)
blt_list_append( TO core_depends ELEMENTS mpi IF AXOM_ENABLE_MPI )

# HACK: RAJA's dependencies are not getting added to core due to a bug in
# dependency propagation in blt_register_library. Explicitly add it in the short term.
if(TARGET RAJA::roctx)
    list(APPEND core_depends RAJA::roctx)
endif()

#------------------------------------------------------------------------------
# Make/Install the library
#------------------------------------------------------------------------------

blt_add_library( NAME        core
                 SOURCES     ${core_sources}
                 HEADERS     ${core_headers}
                 DEPENDS_ON  ${core_depends}
                 FOLDER      axom/core
                 OBJECT      TRUE
                 )

# Basic includes that should be inherited to all Axom targets
target_include_directories(core PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
    $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

# Add the AXOM_DEBUG compile definition, if non-empty
if(AXOM_DEBUG_DEFINE_STRING)
  blt_add_target_definitions(TO core SCOPE PUBLIC TARGET_DEFINITIONS "${AXOM_DEBUG_DEFINE_STRING}")
endif()

axom_write_unified_header( NAME    core
                           HEADERS ${core_headers}
                           )

axom_install_component( NAME      core
                        HEADERS   ${core_headers}
                        )


#------------------------------------------------------------------------------
# Add tests
#------------------------------------------------------------------------------
if (AXOM_ENABLE_TESTS)
  add_subdirectory(tests)
endif()

if(AXOM_ENABLE_EXAMPLES)
  add_subdirectory(examples)
endif()

#------------------------------------------------------------------------------
# Add code checks
#------------------------------------------------------------------------------
axom_add_code_checks(PREFIX core)
