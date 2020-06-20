/** \file ev3_array.h
 * \brief Dynamically-allocated resizable arrays
 *
 * License:
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/
 *
 * The Initial Developer of this code is Jakub Vanek.
 * Portions created by Jakub Vanek are Copyright (C) 2020 Jakub Vanek.
 * All Rights Reserved.
 *
 * ----------------------------------------------------------------------------
 *
 * \author Jakub Vanek (linuxtardis_at_gmail.com)
 * \date 2020-03-03
 * \version 1
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ev3_array_h
#define ev3_array_h

////////////////////////////////////////////////////////////////
// INCLUDES

#include <stdlib.h>
#include <stdbool.h>

////////////////////////////////////////////////////////////////
// MACROS

/*!
 * \brief Initialize an empty resizable array.
 * \param size Size of each element (sizeof(x)).
 * \param alignment Alignment requirement of each element (alignof(x)).
 */
#define DYNAMIC_CARRAY_INITIALIZER(size, alignment) \
    ((DynamicCArray) {                             \
        .elements = NULL,                         \
        .count    =  0,                           \
        .capacity =  0,                           \
        .element_stride = (((size) + (alignment) - 1) / (alignment) * (alignment) ), \
    })

/*!
 * \brief Initialize an empty resizable array suitable for storing values of the given type.
 * \param type Type which will be stored in the array.
 */
#define DYNAMIC_CARRAY_INITIALIZER_FOR(type) DYNAMIC_CARRAY_INITIALIZER(sizeof(type), __alignof__(type))

/*!
 * \brief Iterate over all array elements (use just like a for cycle).
 * \param name Name of the iteration variable.
 * \param array CArray to iterate over.
 */
#define DYNAMIC_CARRAY_FOREACH(ptrname, array) \
    for (void *ptrname = (array)->elements; \
         (array)->elements != NULL && ptrname < (array)->elements + (array)->element_stride * (array)->count; \
         ptrname += (array)->element_stride)

////////////////////////////////////////////////////////////////
// DATA STRUCTURES

//! Simple resizable array.
typedef struct
{
    //! Memory backing this array.
    void *elements;
    //! Number of active array elements, in element count.
    size_t count;
    //! Current capacity of the resizable array, in bytes (= size of elements).
    size_t capacity;
    //! CArray stride (i.e. how large the area occupied by one element is while keeping proper alignment)
    size_t element_stride;
} DynamicCArray;


////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS

/*!
 * \brief Initialize and preallocate a resizable array.
 * \param array Pointer to the array control structure.
 * \param elem_size Size of each element, i.e. sizeof(type).
 * \param elem_alignment Alignment requirement of each element, i.e. alignof(type)
 * \param initial_capacity How many elements to preallocate.
 * \return True if the allocation succeeded, false otherwise.
 */
bool CArrayInit(DynamicCArray *array, size_t elem_size, size_t elem_alignment, size_t initial_capacity);

/*!
 * \brief Deallocate memory of a given resizable array.
 * \param array Pointer to the array control structure.
 */
void CArrayFree(DynamicCArray *array);

/*!
 * \brief Get a pointer to an array element.
 * \param array Pointer to the array control structure.
 * \param index Index of the element in the array.
 * \return Pointer to the element or NULL if not in [0,size) range
 */
void *CArrayAccess(DynamicCArray *array, size_t index);

/*!
 * \brief Ensure that there will be at least given amount of free unused elements.
 * \param array Pointer to the array control structure.
 * \param minReserve Minimum number of free items.
 * \return True if all allocations succeeded, false otherwise.
 */
bool CArrayEnsureFreeCapacity(DynamicCArray *array, size_t minReserve);

/*!
 * \brief Ensure that there will be at least given amount of elements.
 * \param array Pointer to the array control structure.
 * \param minCapacity Minimum capacity of the array.
 * \return True if all allocations succeeded, false otherwise.
 */
bool CArrayEnsureCapacity(DynamicCArray *array, size_t minCapacity);

/*!
 * \brief Reallocate the array so that it has the specified capacity.
 * \param array Pointer to the array control structure.
 * \param newCapacity New capacity of the array.
 * \return True if all allocations succeeded, false otherwise.
 */
bool CArrayChangeCapacity(DynamicCArray *array, size_t newCapacity);

/*!
 * \brief Insert an empty active element(s) into a given place.
 * \param array Pointer to the array control structure.
 * \param index Where to put the new element. Elements with index equal to or higher than this will be moved.
 * \param count How many elements to insert.
 * \return Pointer to the new element or NULL if an allocation failed or invalid arguments were provided.
 */
void *CArrayInsert(DynamicCArray *array, size_t index, size_t count);

/*!
 * \brief Remove a given active element(s) from the given array.
 * \param array Pointer to the array control structure.
 * \param index Where to remove the element from. Elements with index higher that this will be moved.
 * \param count How many elements to remove.
 * \return True if all arguments were valid, false otherwise.
 */
bool CArrayRemove(DynamicCArray *array, size_t index, size_t count);

/*!
 * \brief Add a new active empty element to the end of the array.
 * \param array Pointer to the array control structure.
 * \return Pointer to the new element or NULL if an allocation failed or invalid arguments were provided.
 */
void *CArrayStackPush(DynamicCArray *array);

/*!
 * \brief Remove the last active element from the array.
 * \param array Pointer to the array control structure.
 * @return True if it was possible to remove the element, false otherwise (there were already no elements).
 */
bool CArrayStackPop(DynamicCArray *array);

/*!
 * \brief Mirror data from one array to another.
 *
 * The destination array will be memory-independent from the source one.
 * It will receive a new memory allocation instead of sharing it with the source.
 * This way, it is necessary for example to perform locking only around this copy operation.
 *
 * \param destination Destination array.
 * \param source Source array.
 * \return Whether all allocations succeeded (true) or not (false).
 */
bool CArrayMirror(DynamicCArray *destination, DynamicCArray *source);

#ifdef __cplusplus
}
#endif
#endif // ev3_array_h
