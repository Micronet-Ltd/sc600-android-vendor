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

#ifndef IPTH_HAL_COMM_H
#define IPTH_HAL_COMM_H

/**
 * \file
 *
 * \brief This file includes the communication device interfaces used by the Device
 * Management (DM) client.
 *
 * \par API Name
 *      Device Manufacturer: Communication
 */

#include <ipth_hal_config.h>

/**
 * Defines URL scheme for communication channel
 */
typedef enum ipth_urlScheme {
    URL_SCHEME_UNKNOWN = -1, /**< Unknown URL scheme */
    URL_SCHEME_HTTP = 0, /**< HTTP URL scheme */
    URL_SCHEME_HTTPS = 1 /**< HTTPS URL scheme */
}
ipth_urlScheme_e;

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Creates a communication channel.
     *
     * \param[in] pcb Pointer to HAL callback function
     *
     * \return  Communication channel handle, or NULL if failed
     */
    void *ipth_HALCommCreate( ipth_HALCallback_s *pcb);

    /**
     * Releases any resources associated with the
     * communication channel referenced by a handle. For example, it closes
     * the non-blocking TCP/IP socket of the connection handler referenced by the h parameter.
     *
     * \param[in] h	Connection handler returned by the ipth_HALCommCreate function
     */
    void ipth_HALCommRelease( void *h);

    /**
     * Establishes a connection to the server using the specified IP address,
     * port number, and connection handle. If the DME is running in preemptive mode, this function is blocking.
     * If the DME is running in cooperative mode, the return value IPTH_ERR_IO_PENDING is returned to
     * indicate that an asynchronous request was started. Use the callback function returned by ipth_HALCommCreate
     * to query the status of the asynchronous request.
     * The function fails if the connection is not established within the specified timeout.
     *
     * \param[in] h       Connection handle to the communication resource
     * \param[in] scheme  HTTP or HTTPS URL scheme
     * \param[in] addr    IP address or host name of the server
     * \param[in] PortNum TCP port number of the server
     * \param[in] TimeOut Timeout value (in milliseconds) for establishing a connection.
     * \retval IPTH_OK		Connection successful
     * \retval IPTH_ERR_IO_PENDING  Operation incomplete
     * \retval IPTH_ERR		Connection failure due to timeout
     */
    int ipth_HALCommConnect( void *h, ipth_urlScheme_e scheme, const char *addr, unsigned short PortNum, unsigned long TimeOut);

    /**
     * Shuts down a communication channel with the server.
     * ipth_HALCommConnect can be called again to reopen a connection with the server.
     *
     * \param[in] h Connection handle to the communication resource
     *
     * \retval IPTH_OK
     * \retval IPTH_ERR				Error occurred when shutting down the communication channel
     */
    int ipth_HALCommShutdown( void *h);

    /**
     * Sends number of bytes in the data buffer to the server using the specified connection handler.
     * If the DME is running in cooperative mode, the return value IPTH_ERR_IO_PENDING is returned to
     * indicate that an asynchronous request was started. Use the callback function returned by ipth_HALCommCreate
     * to query the status of the asynchronous request.
     * The function fails if the connection is not established within the specified timeout.
     *
     * \param[in] h Connection handler to the communication resource
     *
     * \param[in] sendBuf Data buffer containing the data to be sent to the server.
     *                    The device API should maintain the buffer associated with the
     *                    handler.
     * \param[in] sendLen Length (in bytes) of 'sendBuf'.
     * \param[in] timeOut Timeout value (in milliseconds) for establishing a connection.
     * \retval IPTH_OK			        All data successfully sent
     * \retval IPTH_ERR_IO_PENDING  Operation incomplete
     * \retval IPTH_ERR				      Send failure due to timeout
     */
    int ipth_HALCommSend( void *h, char *sendBuf, unsigned long sendLen, unsigned long timeOut);

    /**
     * Receives the specified number of bytes in the receive buffer from the server
     * using the specified connection handler.
     * If the DME is running in cooperative mode, the return value IPTH_ERR_IO_PENDING is returned to
     * indicate that an asynchronous request was started. Use the callback function returned by ipth_HALCommCreate
     * to query the status of the asynchronous request.
     * The function fails if the connection is not established within the specified timeout.
     *
     * \param[in] h Connection handler to the communication resource
     *
     * \param[out] recvBuf Data buffer used to store the received data.
     *                     The device API should maintain the buffer associated
     *                     with the handler.
     *
     * \param[in,out] recvLen On input, contains size (in bytes) of the receiving data buffer
     *                        (maximum bytes the caller expects to receive).
     *                        On output, set to the number of bytes received
     *
     * \param[in] timeout Timeout value (in milliseconds) for establishing a connection.
     *
     * \retval IPTH_OK			    All data successfully received
     * \retval IPTH_ERR_IO_PENDING  Operation incomplete
     * \retval IPTH_ERR				Receive failure due to timeout
     */
    int ipth_HALCommRecv( void *h, char* recvBuf, unsigned long *recvLen, unsigned long timeout);

#ifdef __cplusplus
}
#endif

#endif /* IPTH_HAL_COMM_H */
