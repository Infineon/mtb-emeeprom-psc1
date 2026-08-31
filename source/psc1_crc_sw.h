/******************************************************************************
 * File Name: psc1_crc_sw.h
 *
 * Description: Header file for CRC s/w implementation in PSC1 MCUs.
 * It contains the necessary macro and structure definitions to using CRC s/w along with
 * API prototypes.
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

#ifndef CRC_SW_H
#define CRC_SW_H

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * HEADER FILES
 ****************************************************************************/
#include <cy_common.h>

/**
 * @ingroup CRC_SW_publicparam
 * @{
 */
/****************************************************************************
 * MACROS
 ***************************************************************************/
#define CRC_SW_ZERO       (0U)
#define CRC_SW_ONE        (1U)
#define CRC_SW_EIGHT      (8U)
#define CRC_SW_SIXTEEN    (16U)
#define CRC_SW_TWENTYFOUR (24U)
#define CRC_SW_THIRTYTWO  (32U)
/***************************************************************************
 * ENUMS
 **************************************************************************/
/**
 * @ingroup CRC_SW_enumerations
 * @{
 */
/**
 * @brief This enumeration holds the CRC initialization status.
 */
typedef enum CRC_SW_STATUS
{
  CRC_SW_STATUS_SUCCESS = 0U,   /**< APP initialization is successful */
  CRC_SW_STATUS_FAILURE = 1U    /**< APP initialization is failure */
} CRC_SW_STATUS_t;

/**
 * @}
 */

/**************************************************************************
 * DATA STRUCTURES
 **************************************************************************/
 /**
  * @ingroup CRC_SW_datastructures
  * @{
  */

/**
 * @brief This structure holds all the static configuration parameters for CRC.
 */
typedef struct CRC_SW_CONFIG
{
  uint32_t crc_width;                                   /*!< CRC Width */
  uint32_t crc_poly;                                    /*!< Polynomial */
  uint32_t crc_initval;                                 /*!< Initial value */
  uint32_t output_inversion;                            /*!< Output inversion */

  bool input_reflection;                                /*!< Input reflection */
  bool output_reflection;                               /*!< Output reflection */
} CRC_SW_CONFIG_t;

/**
 * @brief Handler structure with pointers to dynamic and static parameters.
 */
typedef struct CRC_SW
{
  CRC_SW_CONFIG_t * const config;                        /*!< CRC_SW configuration pointer*/
  uint32_t crc_runningval;                               /*!< Running CRC value */
  uint32_t msb_mask;                                     /*!< Flag for masking the MSB */
  uint32_t crc_mask;                                     /*!< Flag for masking the CRC */
  uint32_t crc_shift;                                    /*!< Flag for shifting the CRC */
  uint32_t *crctable;                                    /*!< Pointer for CRC table */
  uint32_t tableoffset;                                  /*!< Offset is mapped with CRC width */
} CRC_SW_t;

/**
 * @}
 */

/**
 * @ingroup CRC_SW_apidoc
 * @{
 */
/*************************************************************************
 * API PROTOTYPES
 *************************************************************************/
/**
 * @brief Get CRC_SW APP version.
 * @return \a DAVE_APP_VERSION_t APP version information (major, minor and patch number)
 *
 * \par<b>Description: </b><br>
 * The function can be used to check application software compatibility with a
 * specific version of the APP.
 *
 * @code
 * #include <DAVE.h>
 *
 * int main(void)
 * {
 *    DAVE_APP_VERSION_t version;
 *    DAVE_Init();
 *    version = CRC_SW_GetAppVersion();
 *    if(version.major != 4U)
 *    {
 *    }
 *    while(1)
 *    {}
 *    return 0;
 * }
 * @endcode<BR> </p>
 */

/**
 * @brief Initializes CRC_SW handle.
 * @param handle Constant pointer to CRC structure of type @ref CRC_SW_t
 *
 * \par<b>Description:</b><br>
 * Calculates the MSB mask, CRC Mask and CRC shift values from polynomial width.
 * These values are used to generate the run time CRC table. Once these
 * parameters are set in this function, the user is not supposed to change them
 * in subsequent calls of CRC_SW APIs.
 * @return @ref CRC_SW_STATUS_t
 * <BR><P ALIGN="LEFT"><B>Example:</B>
 * Pre-requisite: Instantiate CRC_SW APP and generate code before using below
 * code snippet.
 * @code
 * #include <DAVE.h>
 *
 * int main(void)
 * {
 *   DAVE_Init();  // CRC_SW_Init(&CRC_SW_0) is called inside DAVE_Init
 *   while(1)
 *   {}
 *   return 0;
 * }
 * @endcode<BR> </p>
 */
CRC_SW_STATUS_t CRC_SW_Init(CRC_SW_t *const handle);

/**
 * @brief Calculates CRC on a block of input data using table driven algorithm
 * and returns running CRC value before reflection and inversion.
 * @param  handle Constant pointer to CRC structure of type @ref CRC_SW_t
 * @param  bufferPtr  Source of data
 * @param  length Length of bufferptr
 * @return void \n
 * <BR><P ALIGN="LEFT"><B>Example:</B>
 * Pre-requisite: Instantiate CRC_SW APP and generate code before using below
 * code snippet.
 * @code
 * #include <DAVE.h>
 * #include <string.h>
 *
 * int8_t CRC_SW_Data[] = "Lorem ipsum dolor sit amet";
 *
 * int main(void)
 * {
 *   DAVE_Init();    // CRC_SW_Init(&CRC_SW_0) is called inside DAVE_Init
 *   CRC_SW_CalculateCRC(&CRC_SW_0,CRC_SW_Data,strlen((const char*)CRC_SW_Data));
 *   while(1)
 *   {}
 *   return 0;
 * }
 * @endcode<BR> </p>
 */
void CRC_SW_CalculateCRC(CRC_SW_t *const handle, void * bufferptr, uint32_t length);

/**
 * @brief Get final CRC result by doing reflection(if selected) and inversion.
 * @param handle Constant pointer to CRC structure of type @ref CRC_SW_t
 * @return uint32_t CRC result
 * <BR><P ALIGN="LEFT"><B>Example:</B>
 * Pre-requisite: Instantiate CRC_SW APP and generate code before using below
 * code snippet.
 * @code
 * #include <DAVE.h>
 * #include <string.h>
 *
 * int8_t CRC_SW_Data[] = "Lorem ipsum dolor sit amet";
 *
 * int main(void)
 * {
 *   uint32_t CRCResult = 0;
 *   DAVE_Init(); // CRC_SW_Init(&CRC_SW_0) is called inside DAVE_Init
 *   CRC_SW_CalculateCRC(&CRC_SW_0,CRC_SW_Data,strlen((const char*)CRC_SW_Data));
 *   CRCResult = CRC_SW_GetCRCResult(&CRC_SW_0);
 *   while(1)
 *   {}
 *   return 0;
 * }
 * @endcode</p>
 */
uint32_t CRC_SW_GetCRCResult(CRC_SW_t *const handle);

/**
 * @}
 */
#include "psc1_crc_sw_extern.h"

#ifdef __cplusplus
}
#endif
#endif /* CRC_SW_H */
