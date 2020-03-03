/** \file ev3_array.c
 * \brief Implementation of resizable arrays
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

////////////////////////////////////////////////////////////////
// INCLUDES

#include "ev3_array.h"
#include <string.h>

////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS

/*!
 * \brief Round the given number up to a nearest 32bit power of two.
 * \param v Number to round up.
 * \return Rounded number.
 * \remark see https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
 */
static
size_t roundUpToNearestPowerOfTwo(size_t v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}


////////////////////////////////////////////////////////////////
// IMPLEMENTATION OF PUBLIC API

bool CArrayInit(DynamicCArray *array, size_t elem_size, size_t elem_alignment, size_t initial_capacity)
{
    *array = DYNAMIC_CARRAY_INITIALIZER(elem_size, elem_alignment);

    return CArrayEnsureCapacity(array, initial_capacity);
}

void CArrayFree(DynamicCArray *array)
{
    free(array->elements);
    array->elements = NULL;
    array->capacity = 0;
    array->count = 0;
}

void *CArrayAccess(DynamicCArray *array, size_t index)
{
    if (index >= array->count)
        return NULL;

    return array->elements + (index * array->element_stride);
}

bool CArrayEnsureFreeCapacity(DynamicCArray *array, size_t minReserve)
{
    return CArrayEnsureCapacity(array, array->count + minReserve);
}

bool CArrayEnsureCapacity(DynamicCArray *array, size_t minCapacity)
{
    if (array->capacity < minCapacity)
        return CArrayChangeCapacity(array, roundUpToNearestPowerOfTwo(minCapacity));
    return true;
}

bool CArrayChangeCapacity(DynamicCArray *array, size_t newCapacity)
{
    if (newCapacity == 0)
    {
        CArrayFree(array);
        return true;
    }

    size_t totalSize = array->element_stride * newCapacity;
    void *memory = realloc(array->elements, totalSize);
    if (memory == NULL)
        return false;

    array->elements = memory;
    array->capacity = newCapacity;
    if (array->count > array->capacity)
        array->count = array->capacity;

    return true;
}

void *CArrayStackPush(DynamicCArray *array)
{
    if (!CArrayEnsureFreeCapacity(array, 1))
        return NULL;

    array->count++;
    return CArrayAccess(array, array->count - 1);
}

bool CArrayStackPop(DynamicCArray *array)
{
    if (array->count > 0)
    {
        array->count--;
        return true;
    }
    return false;
}

void *CArrayInsert(DynamicCArray *array, size_t index, size_t count)
{
    if (index > array->count)
        return NULL;

    if (!CArrayEnsureFreeCapacity(array, count))
        return NULL;

    void *shiftPrev = array->elements + (index * array->element_stride);
    void *shiftNext = shiftPrev + array->element_stride * count;
    size_t shiftLen = array->count - index + count;

    memmove(shiftNext, shiftPrev, shiftLen);

    array->count += count;
    return shiftPrev;
}

bool CArrayRemove(DynamicCArray *array, size_t index, size_t count)
{
    if (index + count - 1 >= array->count)
        return false;

    if (array->elements == NULL)
        return false; // prevent UB in memmove

    void *shiftNext = array->elements + (index * array->element_stride);
    void *shiftPrev = shiftNext + array->element_stride * count;
    size_t shiftLen = array->count - index;

    memmove(shiftNext, shiftPrev, shiftLen);

    array->count -= count;
    return true;
}

bool CArrayMirror(DynamicCArray *destination, DynamicCArray *source)
{
    if (destination->element_stride != source->element_stride)
        return false;

    if (!CArrayEnsureCapacity(destination, source->count))
        return false;

    destination->count = source->count;

    if (destination->count == 0)
        return true;

    if (destination->elements == NULL || source->elements == NULL)
        return false; // prevent UB in memcpy

    memcpy(destination->elements,
           source->elements,
           source->element_stride * destination->count);

    return true;
}
