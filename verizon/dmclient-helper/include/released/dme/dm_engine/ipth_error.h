/******************************************************************************
 * Copyright (c) 2003-2010 InnoPath Software, Inc. All rights reserved.
 *
 * Legal Statements
 *
 * THE SOFTWARE AND ANY MATERIALS PROVIDED BY INNOPATH ARE PROVIDED ON AN "AS
 * IS" BASIS WITHOUT ANY REPRESENTATIONS, WARRANTIES OR CONDITIONS OF ANY KIND,
 * WHETHER EXPRESS, IMPLIED, STATUTORY, OUT OF A COURSE OF DEALING OR USAGE,
 * TRADE OR OTHERWISE INCLUDING ANY IMPLIED WARRANTIES OR CONDITIONS OF
 * MERCHANTABILITY, MERCHANTABLE QUALITY, FITNESS FOR ANY PARTICULAR PURPOSE
 * OR USE, TITLE, OR NON-INFRINGEMENT. INNOPATH DOES NOT REPRESENT OR WARRANT
 * THAT THE SOFTWARE OR ANY MATERIALS WILL BE FREE OF DEFECTS, UNINTERRUPTED,
 * ACCURATE, COMPLETE, CURRENT, STABLE, BUG-FREE, ERROR-FREE, OR AVAILABLE AT
 * ANY TIME.
 *
 * TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL INNOPATH BE LIABLE
 * FOR ANY DIRECT, INDIRECT, CONSEQUENTIAL, INCIDENTAL, SPECIAL, RELIANCE,
 * PUNITIVE OR OTHER DAMAGES OR EXPENSES OF ANY KIND, INCLUDING BUT NOT LIMITED
 * TO ANY LOSS OF PROFITS OR REVENUE, LOST SAVINGS, INTERRUPTED BUSINESS, LOST
 * BUSINESS, LOST BUSINESS OPPORTUNITIES, LOST DATA, LOST GOODWILL, LOSS FROM
 * WORK STOPPAGE, COSTS OF OVERHEAD, COSTS OF COVER, LOSS OF ANTICIPATED
 * BENEFITS HEREUNDER, ARISING OUT OF OR RELATED TO THIS AGREEMENT, HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, EVEN IF INNOPATH HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES, AND NOTWITHSTANDING THE FAILURE OF THE
 * ESSENTIAL PURPOSE OF ANY LIMITED REMEDY STATED HEREIN.
 *****************************************************************************/

#ifndef __IPTH_ERROR_H__
#define __IPTH_ERROR_H__

#include "ipth_hal_typedef.h"

/**
 * \file
 *
 * \brief This file includes the Device Management Engine (DME) error code definitions.
 *
 * \par API Name
 *      Error and Status Codes
 */


/**
 * True or false logic value specification.
 *
 * \par Constant Table
 *      True or false constant definitions
 * End Description
 */
#define IPTH_TRUE   1 /**< Logical true */
#define IPTH_FALSE  0 /**< Logical false */

/**
 * OMA status specification.
 *
 * \par Constant Table
 *      OMA status definitions
 * End Description
 */
#define IPTH_OMA_STATUS_200 (200) /**< OK */
#define IPTH_OMA_STATUS_202 (202) /**< Accepted for processing */
#define IPTH_OMA_STATUS_212 (212) /**< Authentication accepted */
#define IPTH_OMA_STATUS_213 (213) /**< Chunked item accepted */
#define IPTH_OMA_STATUS_214 (214) /**< Operation cancelled */
#define IPTH_OMA_STATUS_215 (215) /**< Not executed */
#define IPTH_OMA_STATUS_216 (216) /**< Atomic rollback OK */

#define IPTH_OMA_STATUS_304 (304) /**< Not modified */

#define IPTH_OMA_STATUS_400 (400) /**< Bad request */
#define IPTH_OMA_STATUS_401 (401) /**< Unauthorized */
#define IPTH_OMA_STATUS_403 (403) /**< Forbidden */
#define IPTH_OMA_STATUS_403_UPDATE_DEFERRED (499) /**< Forbidden UPDATE_DEFERRED */
#define IPTH_OMA_STATUS_404 (404) /**< Not found */
#define IPTH_OMA_STATUS_405 (405) /**< Command not allowed */
#define IPTH_OMA_STATUS_406 (406) /**< Optional feature not supported */
#define IPTH_OMA_STATUS_407 (407) /**< Authentication required */
#define IPTH_OMA_STATUS_408 (408) /**< Request timeout */
#define IPTH_OMA_STATUS_412 (412) /**< Incomplete command */
#define IPTH_OMA_STATUS_413 (413) /**< Request entity too large */
#define IPTH_OMA_STATUS_414 (414) /**< URI too long */
#define IPTH_OMA_STATUS_415 (415) /**< Unsupported media type or format */
#define IPTH_OMA_STATUS_416 (416) /**< Request range not satisfied */
#define IPTH_OMA_STATUS_418 (418) /**< Already exists */
#define IPTH_OMA_STATUS_420 (420) /**< Device full */
#define IPTH_OMA_STATUS_424 (424) /**< Size mismatch */
#define IPTH_OMA_STATUS_425 (425) /**< Permission denied */

#define IPTH_OMA_STATUS_500 (500) /**< Command failed */
#define IPTH_OMA_STATUS_507 (507) /**< Atomic failed */
#define IPTH_OMA_STATUS_510 (510) /**< Data store failed */
#define IPTH_OMA_STATUS_516 (516) /**< Atomic rollback failed */
#define IPTH_OMA_STATUS_517 (517) /**< Atomic response too large to fit */

#define IPTH_OMA_STATUS_1200 (1200) /**< EXEC operation succeeded */
#define IPTH_OMA_STATUS_1250 (1250) /**< EXEC operation succeeded with vendor specified result */
#define IPTH_OMA_STATUS_1299 (1299) /**< EXEC operation succeeded with vendor specified result */
#define IPTH_OMA_STATUS_1400 (1400) /**< EXEC operation client error */
#define IPTH_OMA_STATUS_1401 (1401) /**< EXEC operation cancelled by user */
#define IPTH_OMA_STATUS_1450 (1450) /**< EXEC operation client error that is vendor specific */
#define IPTH_OMA_STATUS_1499 (1499) /**< EXEC operation client error that is vendor specific */

/**
 * Mapped OMA status specification for convenience of use.
 *
 * \par Constant Table
 *      Mapped OMA status definitions for convenience of use
 * End Description
 */
#define IPTH_OMA_STATUS_OK                        IPTH_OMA_STATUS_200
#define IPTH_OMA_STATUS_ACCEPTED                IPTH_OMA_STATUS_202
#define IPTH_OMA_STATUS_AUTH_ACCEPTED            IPTH_OMA_STATUS_212
#define IPTH_OMA_STATUS_CHUNKED_ACCEPTED        IPTH_OMA_STATUS_213
#define IPTH_OMA_STATUS_OP_CANCELLED            IPTH_OMA_STATUS_214
#define IPTH_OMA_STATUS_NOT_EXECUTED            IPTH_OMA_STATUS_215
#define IPTH_OMA_STATUS_ATOMIC_RLLBCK_OK        IPTH_OMA_STATUS_216

#define IPTH_OMA_STATUS_NOT_MODIFIED            IPTH_OMA_STATUS_304

#define IPTH_OMA_STATUS_BAD_REQUEST                IPTH_OMA_STATUS_400
#define IPTH_OMA_STATUS_UNAUTHORIZED            IPTH_OMA_STATUS_401
#define IPTH_OMA_STATUS_FORBIDDEN                IPTH_OMA_STATUS_403
#define IPTH_OMA_STATUS_NOT_FOUND                IPTH_OMA_STATUS_404
#define IPTH_OMA_STATUS_NOT_ALLOWED                IPTH_OMA_STATUS_405
#define IPTH_OMA_STATUS_NOT_SUPPORTED            IPTH_OMA_STATUS_406
#define IPTH_OMA_STATUS_AUTH_REQUIRED            IPTH_OMA_STATUS_407
#define IPTH_OMA_STATUS_REQUEST_TIMEOUT            IPTH_OMA_STATUS_408
#define IPTH_OMA_STATUS_INCOMPLETE_COMMAND        IPTH_OMA_STATUS_412
#define IPTH_OMA_STATUS_REQUEST_TOO_LARGE        IPTH_OMA_STATUS_413
#define IPTH_OMA_STATUS_URI_TOO_LONG            IPTH_OMA_STATUS_414
#define IPTH_OMA_STATUS_UNSUPPORTED_FORMAT        IPTH_OMA_STATUS_415
#define IPTH_OMA_STATUS_RQUEST_RANGE_NOT_MET    IPTH_OMA_STATUS_416
#define IPTH_OMA_STATUS_ALREADY_EXISTS            IPTH_OMA_STATUS_418
#define IPTH_OMA_STATUS_DEVICE_FULL                IPTH_OMA_STATUS_420
#define IPTH_OMA_STATUS_SIZE_MISMATCH            IPTH_OMA_STATUS_424
#define IPTH_OMA_STATUS_PERMISSION_DENIED        IPTH_OMA_STATUS_425

#define IPTH_OMA_STATUS_COMMAND_FAILED            IPTH_OMA_STATUS_500
#define IPTH_OMA_STATUS_ATOMIC_FAILED            IPTH_OMA_STATUS_507
#define IPTH_OMA_STATUS_DATA_STORE_FAILURE        IPTH_OMA_STATUS_510
#define IPTH_OMA_STATUS_ATOMIC_RLLBCK_FAILED    IPTH_OMA_STATUS_516
#define IPTH_OMA_STATUS_ATOMIC_RSPNSE_TOO_LARGE IPTH_OMA_STATUS_517

#define IPTH_OMA_STATUS_EXEC_SUCCEEDED          IPTH_OMA_STATUS_1200
#define IPTH_OMA_STATUS_EXEC_SUCCEEDED_VENDOR_FIRST  IPTH_OMA_STATUS_1250
#define IPTH_OMA_STATUS_EXEC_SUCCEEDED_VENDOR_LAST   IPTH_OMA_STATUS_1299
#define IPTH_OMA_STATUS_EXEC_CLIENT_ERROR       IPTH_OMA_STATUS_1400

/**
 * OMA Download Agent (DLA) error codes.
 *
 * \par Constant Table
 *      OMA Download Agent (DLA) error codes
 * End Description
 */
/*communication related*/
#define IPTH_ERR_DLACOM_INIT (101)  /**< Communication initialization failed*/
#define IPTH_ERR_DLACOM_CONNECT	(102)  /**< Failed connecting to server */
#define IPTH_ERR_DLACOM_SETHEAD (103) /**< Failed to set request header*/
#define IPTH_ERR_DLACOM_SETBODY (104) /**< Failed to set request body*/
#define IPTH_ERR_DLACOM_SEND (105) /**< Cannot send request*/
#define IPTH_ERR_DLACOM_CLTSTATUS (106) /**< Server responded with bad status code indicating client error*/
#define IPTH_ERR_DLACOM_SVRSTATUS (107) /**< Server responded with bad status code indicating server error*/
#define IPTH_ERR_DLACOM_RECVHEAD (108) /**< Failed to receive response header*/
#define IPTH_ERR_DLACOM_RECVBODY (109) /**< Failed to receive response body*/
#define IPTH_ERR_DLACOM_STORE (110) /**< Failed to store downloaded content*/
#define IPTH_ERR_DLACOM_INVALIDURL    (111) /**< Invalid URL format */
#define IPTH_ERR_DLACOM_AUTH (112) /**< Authentication failed*/
#define IPTH_ERR_DLACOM_TIMEOUT (113) /**< Server timeout */

/*user related */
#define IPTH_ERR_DLAUSER_DENY (120) /**< User denied download*/
#define IPTH_ERR_DLAUSER_CANCEL1 (121) /**< User cancelled download; response needs to be sent*/
#define IPTH_ERR_DLAUSER_CANCEL2 (122) /**< User cancelled download with no response to server*/

/*dd parse related*/
#define IPTH_ERR_DLADD_PARSE (133) /**< Device Definition (DD) not integrated*/
#define IPTH_ERR_DLADD_INVALID (134) /**< Some Device Definition (DD) content is inconsistent with the OMA DL standard*/
#define IPTH_ERR_DLADD_INVALIDVERSION (135) /**< Invalid Device Definition (DD) version*/

/*device related*/
#define IPTH_ERR_DLADEV_INSUFFICIENTMEMORY (145) /**< Device does not have enough available memory */
#define IPTH_ERR_DLADEV_LACKCAPABILITY (146) /**< Device lacks the capability to receive object*/
#define IPTH_ERR_DLADEV_INSTALLOBJ (147) /**< Failed to install object*/
#define IPTH_ERR_DLADEV_BADRETV (148) /**< Device returned invalid value*/

#define IPTH_ERR_DLACOM_SU_CANCEL_552 (552)
#define IPTH_ERR_DLACOM_SU_CANCEL_553 (553)
#define IPTH_ERR_DLACOM_SU_CANCEL_554 (554)
#endif /* __IPTH_ERROR_H__ */
