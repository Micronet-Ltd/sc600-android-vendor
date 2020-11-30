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
#ifndef __IPTH_PLUGIN_ALERT_H__
#define __IPTH_PLUGIN_ALERT_H__

/** \file
 *
 * \brief This file includes the interfaces to construct a generic alert.
 *
 * \par API Name
 *      ActiveCare Client Foundation: Server Notification Processing
 */


/*=================================================================================================
                                         INCLUDE FILES
==================================================================================================*/

#include "ipth_hal_config.h"
#include "dm_engine/ipth_dme_types.h"
#include "dm_engine/ipth_error.h"


#ifdef __cplusplus
extern "C" {
#endif


    /*==================================================================================================
                                     STRUCTURES AND OTHER TYPEDEFS
    ==================================================================================================*/

    /**
     * \brief Specifies item for generic alert
     */
    typedef struct {
        int dataSize;           /**< Size of data. Use 0 if no data is present. */
        void * data;            /**< Data field. Can be NULL if not present. */
        ipth_Format metaFormat; /**< Data format. */
        char * metaType;        /**< One of the standard meta types. Can be NULL.*/
        ipth_MetaMark metaMark; /**< One of the standard meta marks as defined by ipth_MetaMark.*/
        char * srcUri;          /**< Source URI. Can be NULL.*/
        char * targetUri;       /**< Target URI. Can be NULL.*/
    } ipth_AlertItem;

    /**
     * \brief Specifies list of items for generic alert.
     */
    struct _ipth_AlertItemList {
        ipth_AlertItem item;               /**< Item. */
        struct _ipth_AlertItemList * next; /**< Pointer to next element. */
    };

    typedef struct _ipth_AlertItemList ipth_AlertItemList;

    /**
     * \brief Specifies generic alerts.
     */
    typedef struct _ipth_GenericAlert {
        int code;                      /**< Alert code. */
        char * correlator;             /**< Correlator supplied in the Exec command. */
        ipth_AlertItemList * itemList; /**< List of alert items. */
    }  ipth_GenericAlert;

    /**
     * \brief Specifies generic alert with server acknowledgement status.
     */
    typedef struct _ipth_GenericAlertStatus {
        ipth_GenericAlert alert; /**< Original alert sent to the server. */
        int status;				 /**< Status received from server acknowledgement. */
    }  ipth_GenericAlertStatus;


    /*==================================================================================================
                                     FUNCTION PROTOTYPES
    ==================================================================================================*/


    /**
     * Free alert item.
     *
     * \param [in] item Pointer to ipth_AlertItem structure.
     */
    void ipth_FreeAlertItem(ipth_AlertItem * item);

    /**
     * Free general alert.
     *
     * \param [in] aList Pointer to ipth_GenericAlert structure.
     */
    void ipth_FreeGenericAlert(ipth_GenericAlert * aList);

    /**
     * Set alert.
     *
     * \param [in] aList Pointer to ipth_GenericAlert structure.
     * \param [in] code Alert code.
     * \param [in] correlator Pointer to correlator supplied in the Exec command.
     *
     * \retval IPTH_OK
     * \retval IPTH_ERR
     */
    int ipth_SetAlert(ipth_GenericAlert * aList, int code, const char * correlator);

    /**
     * Add alert item.
     *
     * \param [in] aList Pointer to ipth_GenericAlert structure.
     * \param [in] dataSize Size of data to be added.
     * \param [in] data Pointer of void type.
     * \param [in] metaFormat Structure of ipth_Format.
     * \param [in] metaType Pointer to constant char.
     * \param [in] metaMark Structure of ipth_MetaMark.
     * \param [in] srcUri Pointer to source URI.
     * \param [in] targetUri Pointer to target URI.
     *
     * \retval IPTH_OK
     * \retval IPTH_ERR
     */
    int ipth_AddAlertItem(ipth_GenericAlert * aList,
                          unsigned int dataSize,
                          const void *data,
                          ipth_Format metaFormat,
                          const char * metaType,
                          ipth_MetaMark metaMark,
                          const char * srcUri,
                          const char * targetUri);

#ifdef __cplusplus
}
#endif

#endif /* __IPTH_PLUGIN_ALERT_H__ */
