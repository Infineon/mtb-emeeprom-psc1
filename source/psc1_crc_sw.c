/******************************************************************************
 * File Name: psc1_crc_sw.c
 *
 * Description: This file has CRC Algorithm for PSC1 MCUs.
 *
 * Related Document: See README.md
 *
 ******************************************************************************
 *
 * Copyright (c) 2016-2026, Infineon Technologies AG or an affiliate of
 * Infineon Technologies AG.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *****************************************************************************/

/****************************************************************************************
 * HEADER FILES
 ***************************************************************************************/
#include "psc1_crc_sw.h"

/***************************************************************************************
 * MACROS
 **************************************************************************************/

/**************************************************************************************
 * LOCAL DATA
 *************************************************************************************/

/*************************************************************************************
 * LOCAL ROUTINES
 ************************************************************************************/
static uint32_t CRC_SW_lReflect(uint32_t data, uint32_t length);


static void CRC_SW_lCalculateCRC_psc1_refin(CRC_SW_t *const handle, void * bufferptr, uint32_t length);


/************************************************************************************
 * API IMPLEMENTATION
 ************************************************************************************/
/*
 * This function reverses all bits of data
 */
static uint32_t CRC_SW_lReflect(uint32_t data, uint32_t length)
{
  uint32_t count;
  uint32_t retval;

  retval = data & CRC_SW_ONE;

  for (count = CRC_SW_ONE; count < length ; count++)
  {
    data >>= CRC_SW_ONE;
    retval = (retval << CRC_SW_ONE) | (data & CRC_SW_ONE);
  }

  return retval;
}

/*
 * This function initializes CRC_SW handle.
 *
 */
CRC_SW_STATUS_t CRC_SW_Init(CRC_SW_t *const handle)
{
  CRC_SW_STATUS_t status;

  if (handle != NULL)
  {
    /* calculate MSB mask, CRC Mask and shift from polynomial width */
    handle->msb_mask = (uint32_t)(CRC_SW_ONE << (handle->config->crc_width - CRC_SW_ONE));

    handle->crc_mask = (uint32_t)((handle->msb_mask - CRC_SW_ONE) | (handle->msb_mask));

    handle->crc_shift = CRC_SW_ZERO;

    handle->config->output_reflection &= handle->crc_mask;

    status = CRC_SW_STATUS_SUCCESS;
  }
  else
  {
    status = CRC_SW_STATUS_FAILURE;
  }

  return (status);
}

/*
 * This function calculates CRC on a block of data.
 */
void CRC_SW_CalculateCRC(CRC_SW_t *const handle, void * bufferptr, uint32_t length)
{
  CY_ASSERT(handle != NULL);
  CY_ASSERT(bufferptr != NULL);

  CRC_SW_lCalculateCRC_psc1_refin(handle, bufferptr, length);
}

/*
 * This function calculates CRC when input reflection is enabled for psc1.
 */
static void CRC_SW_lCalculateCRC_psc1_refin(CRC_SW_t *const handle, void * bufferptr, uint32_t length)
{
  uint32_t crctable;
  uint32_t tableindex;
  uint32_t tableoffs;
  uint32_t data;
  uint32_t temp1;
  uint32_t shiftvalue;
  uint8_t *buffer;
  uint8_t *lookuptable;

  /* Load the initial CRC value as running value for CRC */
  handle->crc_runningval = handle->config->crc_initval;

  buffer = (uint8_t *)bufferptr;

  data = CRC_SW_ZERO;
  shiftvalue = (uint32_t)handle->crc_shift;
  crctable = (uint32_t)handle->crctable;
  tableoffs = (uint32_t)handle->tableoffset;

  while (length--)
  {
    temp1 = (uint32_t) *buffer;

    /* if input reflection is set */
    if (handle->config->input_reflection == true)
    {
      tableindex = ((handle->crc_runningval >> shiftvalue) ^ temp1) &
                    (uint32_t)0xff;
      lookuptable = (uint8_t *)(crctable + tableindex * tableoffs);
    }
    else
    {
      tableindex = ((handle->crc_runningval >>
                    ((handle->config->crc_width - CRC_SW_EIGHT) + shiftvalue)) ^ temp1) & (uint32_t)0xff;
      lookuptable = (uint8_t *)(crctable + tableindex * tableoffs);
    }
    /* load the lookup table value based on CRC width */
    if (handle->config->crc_width == CRC_SW_EIGHT)
    {
      data = *lookuptable;
    }
    else if (handle->config->crc_width == CRC_SW_SIXTEEN)
    {
      data = (*lookuptable | (*(lookuptable+1) << CRC_SW_EIGHT));
    }
    else if (handle->config->crc_width == CRC_SW_THIRTYTWO)
    {
      data = (*lookuptable | (*(lookuptable+1) << CRC_SW_EIGHT) | *(lookuptable+2) << CRC_SW_SIXTEEN |
              *(lookuptable+3) << CRC_SW_TWENTYFOUR);
    }

    /* if input reflection is set */
    if (handle->config->input_reflection == true)
    {
      handle->crc_runningval = (data ^ (handle->crc_runningval >> CRC_SW_EIGHT)) & (handle->crc_mask << shiftvalue);
    }
    else
    {
      handle->crc_runningval = (data ^ (handle->crc_runningval << (CRC_SW_EIGHT - handle->crc_shift))) & \
                               ((handle->crc_mask) << (handle->crc_shift));
    }

    buffer++;
  } /*End of "while (Length--)"*/
}

/**
 * This function returns the CRC value for the already calculated CRC by doing
 * reflection(if selected) and inversion.
 **/
uint32_t CRC_SW_GetCRCResult(CRC_SW_t *const handle)
{
  uint32_t result;

  if (handle != NULL)
  {
    handle->crc_runningval &= (handle->crc_mask << handle->crc_shift);
    handle->crc_runningval >>= handle->crc_shift;

    /* Do not reflect the bytes if input reflection and output reflection are set to true. Otherwise reflect the bytes*/
    if (handle->config->input_reflection != handle->config->output_reflection)
    {
      handle->crc_runningval = CRC_SW_lReflect(handle->crc_runningval, handle->config->crc_width);
    }

    handle->crc_runningval ^= handle->config->output_inversion;
    handle->crc_runningval &= handle->crc_mask;

    result = handle->crc_runningval;
  }
  else
  {
    result = CRC_SW_ZERO;
  }
  return (result);
}
