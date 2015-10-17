//
// SidreAllocatable.cpp - Routines used by Fortran interface
// Uses cog to insert some generated code into this file.
//
#include <cstddef>
#include "common/CommonTypes.hpp"
#include "common/FC.h"
#include "SidreWrapperHelpers.hpp"

#include "sidre/DataGroup.hpp"
#include "sidre/SidreAllocatable.hpp"
#include "sidre/MetaBuffer.hpp"

// import cog once
//[[[cog import cog;import genfsidresplicer as gen ]]]
//[[[end]]]

namespace asctoolkit {
namespace sidre {
class DataView;

extern "C" {
//[[[cog
//gen.print_lines(cog.outl, gen.print_atk_size_allocatable_header)
//]]]
#define SIZE_ALLOCATABLE_INT_SCALAR FC_GLOBAL(atk_size_allocatable_int_scalar,ATK_SIZE_ALLOCATABLE_INT_SCALAR)
size_t SIZE_ALLOCATABLE_INT_SCALAR(void * array);

#define SIZE_ALLOCATABLE_INT_1D FC_GLOBAL(atk_size_allocatable_int_1d,ATK_SIZE_ALLOCATABLE_INT_1D)
size_t SIZE_ALLOCATABLE_INT_1D(void * array);

#define SIZE_ALLOCATABLE_LONG_SCALAR FC_GLOBAL(atk_size_allocatable_long_scalar,ATK_SIZE_ALLOCATABLE_LONG_SCALAR)
size_t SIZE_ALLOCATABLE_LONG_SCALAR(void * array);

#define SIZE_ALLOCATABLE_LONG_1D FC_GLOBAL(atk_size_allocatable_long_1d,ATK_SIZE_ALLOCATABLE_LONG_1D)
size_t SIZE_ALLOCATABLE_LONG_1D(void * array);

#define SIZE_ALLOCATABLE_FLOAT_SCALAR FC_GLOBAL(atk_size_allocatable_float_scalar,ATK_SIZE_ALLOCATABLE_FLOAT_SCALAR)
size_t SIZE_ALLOCATABLE_FLOAT_SCALAR(void * array);

#define SIZE_ALLOCATABLE_FLOAT_1D FC_GLOBAL(atk_size_allocatable_float_1d,ATK_SIZE_ALLOCATABLE_FLOAT_1D)
size_t SIZE_ALLOCATABLE_FLOAT_1D(void * array);

#define SIZE_ALLOCATABLE_DOUBLE_SCALAR FC_GLOBAL(atk_size_allocatable_double_scalar,ATK_SIZE_ALLOCATABLE_DOUBLE_SCALAR)
size_t SIZE_ALLOCATABLE_DOUBLE_SCALAR(void * array);

#define SIZE_ALLOCATABLE_DOUBLE_1D FC_GLOBAL(atk_size_allocatable_double_1d,ATK_SIZE_ALLOCATABLE_DOUBLE_1D)
size_t SIZE_ALLOCATABLE_DOUBLE_1D(void * array);

//[[[end]]]

//[[[cog
//gen.print_lines(cog.outl, gen.print_atk_address_allocatable_header)
//]]]
#define ADDRESS_ALLOCATABLE_INT_SCALAR FC_GLOBAL(atk_address_allocatable_int_scalar,ATK_ADDRESS_ALLOCATABLE_INT_SCALAR)
void * ADDRESS_ALLOCATABLE_INT_SCALAR(void * array);

#define ADDRESS_ALLOCATABLE_INT_1D FC_GLOBAL(atk_address_allocatable_int_1d,ATK_ADDRESS_ALLOCATABLE_INT_1D)
void * ADDRESS_ALLOCATABLE_INT_1D(void * array);

#define ADDRESS_ALLOCATABLE_LONG_SCALAR FC_GLOBAL(atk_address_allocatable_long_scalar,ATK_ADDRESS_ALLOCATABLE_LONG_SCALAR)
void * ADDRESS_ALLOCATABLE_LONG_SCALAR(void * array);

#define ADDRESS_ALLOCATABLE_LONG_1D FC_GLOBAL(atk_address_allocatable_long_1d,ATK_ADDRESS_ALLOCATABLE_LONG_1D)
void * ADDRESS_ALLOCATABLE_LONG_1D(void * array);

#define ADDRESS_ALLOCATABLE_FLOAT_SCALAR FC_GLOBAL(atk_address_allocatable_float_scalar,ATK_ADDRESS_ALLOCATABLE_FLOAT_SCALAR)
void * ADDRESS_ALLOCATABLE_FLOAT_SCALAR(void * array);

#define ADDRESS_ALLOCATABLE_FLOAT_1D FC_GLOBAL(atk_address_allocatable_float_1d,ATK_ADDRESS_ALLOCATABLE_FLOAT_1D)
void * ADDRESS_ALLOCATABLE_FLOAT_1D(void * array);

#define ADDRESS_ALLOCATABLE_DOUBLE_SCALAR FC_GLOBAL(atk_address_allocatable_double_scalar,ATK_ADDRESS_ALLOCATABLE_DOUBLE_SCALAR)
void * ADDRESS_ALLOCATABLE_DOUBLE_SCALAR(void * array);

#define ADDRESS_ALLOCATABLE_DOUBLE_1D FC_GLOBAL(atk_address_allocatable_double_1d,ATK_ADDRESS_ALLOCATABLE_DOUBLE_1D)
void * ADDRESS_ALLOCATABLE_DOUBLE_1D(void * array);

//[[[end]]]

//[[[cog
//gen.print_lines(cog.outl, gen.print_atk_allocate_allocatable_header)
//]]]
#define ALLOCATE_ALLOCATABLE_INT_SCALAR FC_GLOBAL(atk_allocate_allocatable_int_scalar,ATK_ALLOCATE_ALLOCATABLE_INT_SCALAR)
void ALLOCATE_ALLOCATABLE_INT_SCALAR(void * array, long nitems);

#define ALLOCATE_ALLOCATABLE_INT_1D FC_GLOBAL(atk_allocate_allocatable_int_1d,ATK_ALLOCATE_ALLOCATABLE_INT_1D)
void ALLOCATE_ALLOCATABLE_INT_1D(void * array, long nitems);

#define ALLOCATE_ALLOCATABLE_LONG_SCALAR FC_GLOBAL(atk_allocate_allocatable_long_scalar,ATK_ALLOCATE_ALLOCATABLE_LONG_SCALAR)
void ALLOCATE_ALLOCATABLE_LONG_SCALAR(void * array, long nitems);

#define ALLOCATE_ALLOCATABLE_LONG_1D FC_GLOBAL(atk_allocate_allocatable_long_1d,ATK_ALLOCATE_ALLOCATABLE_LONG_1D)
void ALLOCATE_ALLOCATABLE_LONG_1D(void * array, long nitems);

#define ALLOCATE_ALLOCATABLE_FLOAT_SCALAR FC_GLOBAL(atk_allocate_allocatable_float_scalar,ATK_ALLOCATE_ALLOCATABLE_FLOAT_SCALAR)
void ALLOCATE_ALLOCATABLE_FLOAT_SCALAR(void * array, long nitems);

#define ALLOCATE_ALLOCATABLE_FLOAT_1D FC_GLOBAL(atk_allocate_allocatable_float_1d,ATK_ALLOCATE_ALLOCATABLE_FLOAT_1D)
void ALLOCATE_ALLOCATABLE_FLOAT_1D(void * array, long nitems);

#define ALLOCATE_ALLOCATABLE_DOUBLE_SCALAR FC_GLOBAL(atk_allocate_allocatable_double_scalar,ATK_ALLOCATE_ALLOCATABLE_DOUBLE_SCALAR)
void ALLOCATE_ALLOCATABLE_DOUBLE_SCALAR(void * array, long nitems);

#define ALLOCATE_ALLOCATABLE_DOUBLE_1D FC_GLOBAL(atk_allocate_allocatable_double_1d,ATK_ALLOCATE_ALLOCATABLE_DOUBLE_1D)
void ALLOCATE_ALLOCATABLE_DOUBLE_1D(void * array, long nitems);

//[[[end]]]

//[[[cog
//gen.print_lines(cog.outl, gen.print_atk_deallocate_allocatable_header)
//]]]
#define DEALLOCATE_ALLOCATABLE_INT_SCALAR FC_GLOBAL(atk_deallocate_allocatable_int_scalar,ATK_DEALLOCATE_ALLOCATABLE_INT_SCALAR)
void DEALLOCATE_ALLOCATABLE_INT_SCALAR(void * array);

#define DEALLOCATE_ALLOCATABLE_INT_1D FC_GLOBAL(atk_deallocate_allocatable_int_1d,ATK_DEALLOCATE_ALLOCATABLE_INT_1D)
void DEALLOCATE_ALLOCATABLE_INT_1D(void * array);

#define DEALLOCATE_ALLOCATABLE_LONG_SCALAR FC_GLOBAL(atk_deallocate_allocatable_long_scalar,ATK_DEALLOCATE_ALLOCATABLE_LONG_SCALAR)
void DEALLOCATE_ALLOCATABLE_LONG_SCALAR(void * array);

#define DEALLOCATE_ALLOCATABLE_LONG_1D FC_GLOBAL(atk_deallocate_allocatable_long_1d,ATK_DEALLOCATE_ALLOCATABLE_LONG_1D)
void DEALLOCATE_ALLOCATABLE_LONG_1D(void * array);

#define DEALLOCATE_ALLOCATABLE_FLOAT_SCALAR FC_GLOBAL(atk_deallocate_allocatable_float_scalar,ATK_DEALLOCATE_ALLOCATABLE_FLOAT_SCALAR)
void DEALLOCATE_ALLOCATABLE_FLOAT_SCALAR(void * array);

#define DEALLOCATE_ALLOCATABLE_FLOAT_1D FC_GLOBAL(atk_deallocate_allocatable_float_1d,ATK_DEALLOCATE_ALLOCATABLE_FLOAT_1D)
void DEALLOCATE_ALLOCATABLE_FLOAT_1D(void * array);

#define DEALLOCATE_ALLOCATABLE_DOUBLE_SCALAR FC_GLOBAL(atk_deallocate_allocatable_double_scalar,ATK_DEALLOCATE_ALLOCATABLE_DOUBLE_SCALAR)
void DEALLOCATE_ALLOCATABLE_DOUBLE_SCALAR(void * array);

#define DEALLOCATE_ALLOCATABLE_DOUBLE_1D FC_GLOBAL(atk_deallocate_allocatable_double_1d,ATK_DEALLOCATE_ALLOCATABLE_DOUBLE_1D)
void DEALLOCATE_ALLOCATABLE_DOUBLE_1D(void * array);

//[[[end]]]

//[[[cog
//gen.print_lines(cog.outl, gen.print_atk_reallocate_allocatable_header)
//]]]
#define REALLOCATE_ALLOCATABLE_INT_SCALAR FC_GLOBAL(atk_reallocate_allocatable_int_scalar,ATK_REALLOCATE_ALLOCATABLE_INT_SCALAR)
void REALLOCATE_ALLOCATABLE_INT_SCALAR(void * array, long nitems);

#define REALLOCATE_ALLOCATABLE_INT_1D FC_GLOBAL(atk_reallocate_allocatable_int_1d,ATK_REALLOCATE_ALLOCATABLE_INT_1D)
void REALLOCATE_ALLOCATABLE_INT_1D(void * array, long nitems);

#define REALLOCATE_ALLOCATABLE_LONG_SCALAR FC_GLOBAL(atk_reallocate_allocatable_long_scalar,ATK_REALLOCATE_ALLOCATABLE_LONG_SCALAR)
void REALLOCATE_ALLOCATABLE_LONG_SCALAR(void * array, long nitems);

#define REALLOCATE_ALLOCATABLE_LONG_1D FC_GLOBAL(atk_reallocate_allocatable_long_1d,ATK_REALLOCATE_ALLOCATABLE_LONG_1D)
void REALLOCATE_ALLOCATABLE_LONG_1D(void * array, long nitems);

#define REALLOCATE_ALLOCATABLE_FLOAT_SCALAR FC_GLOBAL(atk_reallocate_allocatable_float_scalar,ATK_REALLOCATE_ALLOCATABLE_FLOAT_SCALAR)
void REALLOCATE_ALLOCATABLE_FLOAT_SCALAR(void * array, long nitems);

#define REALLOCATE_ALLOCATABLE_FLOAT_1D FC_GLOBAL(atk_reallocate_allocatable_float_1d,ATK_REALLOCATE_ALLOCATABLE_FLOAT_1D)
void REALLOCATE_ALLOCATABLE_FLOAT_1D(void * array, long nitems);

#define REALLOCATE_ALLOCATABLE_DOUBLE_SCALAR FC_GLOBAL(atk_reallocate_allocatable_double_scalar,ATK_REALLOCATE_ALLOCATABLE_DOUBLE_SCALAR)
void REALLOCATE_ALLOCATABLE_DOUBLE_SCALAR(void * array, long nitems);

#define REALLOCATE_ALLOCATABLE_DOUBLE_1D FC_GLOBAL(atk_reallocate_allocatable_double_1d,ATK_REALLOCATE_ALLOCATABLE_DOUBLE_1D)
void REALLOCATE_ALLOCATABLE_DOUBLE_1D(void * array, long nitems);

//[[[end]]]

}

//----------------------------------------------------------------------
#if 0
// Holds pointers to Fortran functions since they cannot be in the
// Class AllocatableMetaBuffer directly.
struct Fptrs {
    int rank;
    int type;
    size_t (*getNumberOfElements)(void *array);
    void *(*getDataPointer)(void *array);
    void (*allocate)(void *array, long nitems);
    void (*deallocate)(void *array);
    void (*reallocate)(void *array, long nitems);
};

//[[[cog cog.outl('static struct Fptrs fptrs_cache[%s] = {' % gen.num_metabuffers()) ]]]
static struct Fptrs fptrs_cache[8] = {
//[[[end]]]

//[[[cog
//gen.print_lines(cog.out, gen.print_fptrs_cache)
//]]]

{
  0,   // rank
  ATK_C_INT_T,
  atk_size_allocatable_int_scalar_,
  atk_address_allocatable_int_scalar_,
  atk_allocate_allocatable_int_scalar_,
  atk_deallocate_allocatable_int_scalar_,
  atk_reallocate_allocatable_int_scalar_
},

{
  1,   // rank
  ATK_C_INT_T,
  atk_size_allocatable_int_1d_,
  atk_address_allocatable_int_1d_,
  atk_allocate_allocatable_int_1d_,
  atk_deallocate_allocatable_int_1d_,
  atk_reallocate_allocatable_int_1d_
},

{
  0,   // rank
  ATK_C_LONG_T,
  atk_size_allocatable_long_scalar_,
  atk_address_allocatable_long_scalar_,
  atk_allocate_allocatable_long_scalar_,
  atk_deallocate_allocatable_long_scalar_,
  atk_reallocate_allocatable_long_scalar_
},

{
  1,   // rank
  ATK_C_LONG_T,
  atk_size_allocatable_long_1d_,
  atk_address_allocatable_long_1d_,
  atk_allocate_allocatable_long_1d_,
  atk_deallocate_allocatable_long_1d_,
  atk_reallocate_allocatable_long_1d_
},

{
  0,   // rank
  ATK_C_FLOAT_T,
  atk_size_allocatable_float_scalar_,
  atk_address_allocatable_float_scalar_,
  atk_allocate_allocatable_float_scalar_,
  atk_deallocate_allocatable_float_scalar_,
  atk_reallocate_allocatable_float_scalar_
},

{
  1,   // rank
  ATK_C_FLOAT_T,
  atk_size_allocatable_float_1d_,
  atk_address_allocatable_float_1d_,
  atk_allocate_allocatable_float_1d_,
  atk_deallocate_allocatable_float_1d_,
  atk_reallocate_allocatable_float_1d_
},

{
  0,   // rank
  ATK_C_DOUBLE_T,
  atk_size_allocatable_double_scalar_,
  atk_address_allocatable_double_scalar_,
  atk_allocate_allocatable_double_scalar_,
  atk_deallocate_allocatable_double_scalar_,
  atk_reallocate_allocatable_double_scalar_
},

{
  1,   // rank
  ATK_C_DOUBLE_T,
  atk_size_allocatable_double_1d_,
  atk_address_allocatable_double_1d_,
  atk_allocate_allocatable_double_1d_,
  atk_deallocate_allocatable_double_1d_,
  atk_reallocate_allocatable_double_1d_
},
//[[[end]]]
};

//----------------------------------------------------------------------

class AllocatableMetaBuffer : public MetaBuffer
{
public:
  virtual void *getDataPointer() const
    {
	return m_callbacks->getDataPointer(m_context);
    }

  virtual size_t getNumberOfElements() const
    {
	return m_callbacks->getNumberOfElements(m_context);
    }

  virtual TypeID getTypeID() const
  {
      return static_cast<TypeID>(m_callbacks->type);
  }

    virtual void * allocate(TypeID type, SidreLength nitems) const
    {
    // XXX - type is fixed in the context, unused
    // XXX - check requested type vs context type
	m_callbacks->allocate(m_context, nitems);
	return m_callbacks->getDataPointer(m_context);
    }

    virtual void release() const
    {
	m_callbacks->deallocate(m_context);
    }

    virtual void * reallocate(TypeID type, SidreLength nitems) const
    {
    // XXX - type is fixed in the context, unused
    // XXX - check requested type vs context type
	m_callbacks->reallocate(m_context, nitems);
	return m_callbacks->getDataPointer(m_context);
    }

  AllocatableMetaBuffer(void *context, const Fptrs * callbacks) :
    m_context(context),
    m_callbacks(callbacks)
  { }

private:
  void * m_context;   // pointer to Fortran allocatable
  const Fptrs * m_callbacks;
};
#endif



/*
 * Call a Fortran function to find the size of an allocatable.
 */
size_t SizeAllocatable(void * array, TypeID type, int rank)
{
  size_t nitems = 0;
//[[[cog
//gen.SizeAllocatable(cog.outl)
//]]]
switch(type)
{
case CONDUIT_NATIVE_INT_DATATYPE_ID:
  switch(rank)
  {
  case 0:
    nitems = SIZE_ALLOCATABLE_INT_SCALAR(array);
    break;
  case 1:
    nitems = SIZE_ALLOCATABLE_INT_1D(array);
    break;
  default:
    break;
  }
  break;
case CONDUIT_NATIVE_LONG_DATATYPE_ID:
  switch(rank)
  {
  case 0:
    nitems = SIZE_ALLOCATABLE_LONG_SCALAR(array);
    break;
  case 1:
    nitems = SIZE_ALLOCATABLE_LONG_1D(array);
    break;
  default:
    break;
  }
  break;
case CONDUIT_NATIVE_FLOAT_DATATYPE_ID:
  switch(rank)
  {
  case 0:
    nitems = SIZE_ALLOCATABLE_FLOAT_SCALAR(array);
    break;
  case 1:
    nitems = SIZE_ALLOCATABLE_FLOAT_1D(array);
    break;
  default:
    break;
  }
  break;
case CONDUIT_NATIVE_DOUBLE_DATATYPE_ID:
  switch(rank)
  {
  case 0:
    nitems = SIZE_ALLOCATABLE_DOUBLE_SCALAR(array);
    break;
  case 1:
    nitems = SIZE_ALLOCATABLE_DOUBLE_1D(array);
    break;
  default:
    break;
  }
  break;
default:
  break;
}
//[[[end]]]
  return nitems;
}

/*
 * Call a Fortran function to find the address of an allocatable.
 */
void * AddressAllocatable(void * array, TypeID type, int rank)
{
  void * addr = ATK_NULLPTR;
//[[[cog
//gen.AddressAllocatable(cog.outl)
//]]]
switch(type)
{
case CONDUIT_NATIVE_INT_DATATYPE_ID:
  switch(rank)
  {
  case 0:
    addr = ADDRESS_ALLOCATABLE_INT_SCALAR(array);
    break;
  case 1:
    addr = ADDRESS_ALLOCATABLE_INT_1D(array);
    break;
  default:
    break;
  }
  break;
case CONDUIT_NATIVE_LONG_DATATYPE_ID:
  switch(rank)
  {
  case 0:
    addr = ADDRESS_ALLOCATABLE_LONG_SCALAR(array);
    break;
  case 1:
    addr = ADDRESS_ALLOCATABLE_LONG_1D(array);
    break;
  default:
    break;
  }
  break;
case CONDUIT_NATIVE_FLOAT_DATATYPE_ID:
  switch(rank)
  {
  case 0:
    addr = ADDRESS_ALLOCATABLE_FLOAT_SCALAR(array);
    break;
  case 1:
    addr = ADDRESS_ALLOCATABLE_FLOAT_1D(array);
    break;
  default:
    break;
  }
  break;
case CONDUIT_NATIVE_DOUBLE_DATATYPE_ID:
  switch(rank)
  {
  case 0:
    addr = ADDRESS_ALLOCATABLE_DOUBLE_SCALAR(array);
    break;
  case 1:
    addr = ADDRESS_ALLOCATABLE_DOUBLE_1D(array);
    break;
  default:
    break;
  }
  break;
default:
  break;
}
//[[[end]]]
  return addr;
}

/*
 * Call a Fortran function to allocate an allocatable.
 */
void * AllocateAllocatable(void * array, TypeID type, int rank, SidreLength nitems)
{
  void * addr = ATK_NULLPTR;
//[[[cog
//gen.AllocateAllocatable(cog.outl)
//]]]
switch(type)
{
case CONDUIT_NATIVE_INT_DATATYPE_ID:
  switch(rank)
  {
  case 0:
    ALLOCATE_ALLOCATABLE_INT_SCALAR(array, nitems);
    addr = ADDRESS_ALLOCATABLE_INT_SCALAR(array);
    break;
  case 1:
    ALLOCATE_ALLOCATABLE_INT_1D(array, nitems);
    addr = ADDRESS_ALLOCATABLE_INT_1D(array);
    break;
  default:
    break;
  }
  break;
case CONDUIT_NATIVE_LONG_DATATYPE_ID:
  switch(rank)
  {
  case 0:
    ALLOCATE_ALLOCATABLE_LONG_SCALAR(array, nitems);
    addr = ADDRESS_ALLOCATABLE_LONG_SCALAR(array);
    break;
  case 1:
    ALLOCATE_ALLOCATABLE_LONG_1D(array, nitems);
    addr = ADDRESS_ALLOCATABLE_LONG_1D(array);
    break;
  default:
    break;
  }
  break;
case CONDUIT_NATIVE_FLOAT_DATATYPE_ID:
  switch(rank)
  {
  case 0:
    ALLOCATE_ALLOCATABLE_FLOAT_SCALAR(array, nitems);
    addr = ADDRESS_ALLOCATABLE_FLOAT_SCALAR(array);
    break;
  case 1:
    ALLOCATE_ALLOCATABLE_FLOAT_1D(array, nitems);
    addr = ADDRESS_ALLOCATABLE_FLOAT_1D(array);
    break;
  default:
    break;
  }
  break;
case CONDUIT_NATIVE_DOUBLE_DATATYPE_ID:
  switch(rank)
  {
  case 0:
    ALLOCATE_ALLOCATABLE_DOUBLE_SCALAR(array, nitems);
    addr = ADDRESS_ALLOCATABLE_DOUBLE_SCALAR(array);
    break;
  case 1:
    ALLOCATE_ALLOCATABLE_DOUBLE_1D(array, nitems);
    addr = ADDRESS_ALLOCATABLE_DOUBLE_1D(array);
    break;
  default:
    break;
  }
  break;
default:
  break;
}
//[[[end]]]
  return addr;
}


/*!
 * \brief Return DataView for a Fortran allocatable.
 *
 * The Fortran allocatable array is the buffer for the DataView.
 */
#if 0
static void * register_allocatable(DataGroup *group,
				  const std::string &name,
				  void *context, int imetabuffer)
{
  AllocatableMetaBuffer * metabuffer = new AllocatableMetaBuffer(context, fptrs_cache + imetabuffer);
  return group->createMetaBufferView(name, metabuffer);
}
#endif

extern "C" {

static void *ATK_create_fortran_allocatable_view(DataGroup *group,
						 char *name, int lname,
						 void *array, int type, int rank)
{
  DataView *view = group->createFortranAllocatableView(std::string(name, lname),
						       array,
						       static_cast<TypeID>(type),
						       rank);
  return view;
}


// called from Fortran
// return pointer to a DataView
void * ATK_register_static(void * group, char * name, int lname,
			   void * addr, int type, long nitems)
{
  DataGroup * grp = static_cast<DataGroup *>(group);
  DataView * view = grp->createExternalView( std::string(name, lname),
					     addr,
					     static_cast<TypeID>(type),
					     static_cast<SidreLength>(nitems));
  return view;
}

// called from Fortran
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53945
// Work around a problem with gfortran 4.7 where C_LOC does not work
// with assumed shape array.  Passing the first element of the
// array to a function without an interface will force the compiler
// to use f77 semantics and pass the address of the data, essentially
// the same as C_LOC.
// XXX Pass the first element, not the entire array, to avoid getting
// XXX a copy of the array.
void *atk_c_loc_(void *addr)
{
    return addr;
}

/*
 * These routines are used from Fortran with an interface
 * since they need the address of the allocatable array,
 * not the address of the contents of the allocatable array.
 */
//[[[cog
//gen.print_lines(cog.outl, gen.print_atk_create_allocatable_view)
//]]]

// Fortran callable routine.
// Needed for each type-kind-rank to get address of allocatable array.
// array is address of allocatable, not the result of C_LOC(array)
void *atk_create_allocatable_view_int_scalar_(
    DataGroup *group,
    char *name, int lname,
    void *array, int itype, int rank)
{
    return ATK_create_fortran_allocatable_view(group, name, lname, array, itype, rank);
}

// Fortran callable routine.
// Needed for each type-kind-rank to get address of allocatable array.
// array is address of allocatable, not the result of C_LOC(array)
void *atk_create_allocatable_view_int_1d_(
    DataGroup *group,
    char *name, int lname,
    void *array, int itype, int rank)
{
    return ATK_create_fortran_allocatable_view(group, name, lname, array, itype, rank);
}

// Fortran callable routine.
// Needed for each type-kind-rank to get address of allocatable array.
// array is address of allocatable, not the result of C_LOC(array)
void *atk_create_allocatable_view_long_scalar_(
    DataGroup *group,
    char *name, int lname,
    void *array, int itype, int rank)
{
    return ATK_create_fortran_allocatable_view(group, name, lname, array, itype, rank);
}

// Fortran callable routine.
// Needed for each type-kind-rank to get address of allocatable array.
// array is address of allocatable, not the result of C_LOC(array)
void *atk_create_allocatable_view_long_1d_(
    DataGroup *group,
    char *name, int lname,
    void *array, int itype, int rank)
{
    return ATK_create_fortran_allocatable_view(group, name, lname, array, itype, rank);
}

// Fortran callable routine.
// Needed for each type-kind-rank to get address of allocatable array.
// array is address of allocatable, not the result of C_LOC(array)
void *atk_create_allocatable_view_float_scalar_(
    DataGroup *group,
    char *name, int lname,
    void *array, int itype, int rank)
{
    return ATK_create_fortran_allocatable_view(group, name, lname, array, itype, rank);
}

// Fortran callable routine.
// Needed for each type-kind-rank to get address of allocatable array.
// array is address of allocatable, not the result of C_LOC(array)
void *atk_create_allocatable_view_float_1d_(
    DataGroup *group,
    char *name, int lname,
    void *array, int itype, int rank)
{
    return ATK_create_fortran_allocatable_view(group, name, lname, array, itype, rank);
}

// Fortran callable routine.
// Needed for each type-kind-rank to get address of allocatable array.
// array is address of allocatable, not the result of C_LOC(array)
void *atk_create_allocatable_view_double_scalar_(
    DataGroup *group,
    char *name, int lname,
    void *array, int itype, int rank)
{
    return ATK_create_fortran_allocatable_view(group, name, lname, array, itype, rank);
}

// Fortran callable routine.
// Needed for each type-kind-rank to get address of allocatable array.
// array is address of allocatable, not the result of C_LOC(array)
void *atk_create_allocatable_view_double_1d_(
    DataGroup *group,
    char *name, int lname,
    void *array, int itype, int rank)
{
    return ATK_create_fortran_allocatable_view(group, name, lname, array, itype, rank);
}
//[[[end]]]

}  // extern "C"


}  // namespace asctoolkit
}  // namespace sidre

