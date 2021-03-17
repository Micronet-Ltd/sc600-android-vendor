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

#ifndef __IPTH_HAL_HTTP_H__
#define __IPTH_HAL_HTTP_H__

/**
 * \file
 *
 * \brief This file includes the HTTP interface functions.
 *
 * \par API Name
 *      Device Manufacturer: HTTP Management
 */

#include <ipth_hal_config.h>

IP_START_EXTERN_C

/**
 * HTTP header constant defines for content encoding of plain text and
 * authentication of HMAC.
 *
 * \par Constant Table
 *      HTTP header constant definitions
 * End Description
 */
#define IPTH_HTTP_HD_CONTENTENCODING_PLAINTEXT    "Plain-text/xml-URL encoding" /**< Content encoding of type plain text */
#define IPTH_HTTP_HD_AUTHHMAC                     "x-syncml-hmac" /**< Authentication of type HMAC */

/**
 * HTTP status codes. For details, refer to
 * http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html.
 *
 * \par Constant Table
 *      HTTP status code definitions
 * End Description
 */
/* Informational 1xx */
#define IPTH_HTTP_STSCODE_CONTINUE            100 /**< Informational 1xx status code "Continue" */
#define IPTH_HTTP_STSCODE_SWPROT              101 /**< Informational 1xx status code "Switch Protocol" */

/* Successful 2xx */
#define IPTH_HTTP_STSCODE_OK                  200 /**< Successful 2xx status code "OK" */
#define IPTH_HTTP_STSCODE_CREATED             201 /**< Successful 2xx status code "Created" */
#define IPTH_HTTP_STSCODE_ACCEPTED            202 /**< Successful 2xx status code "Accepted" */
#define IPTH_HTTP_STSCODE_NONAUTH             203 /**< Successful 2xx status code "Non-Authoritative Information" */
#define IPTH_HTTP_STSCODE_NOCONTENT           204 /**< Successful 2xx status code "No Content" */
#define IPTH_HTTP_STSCODE_RSTCONTENT          205 /**< Successful 2xx status code "Reset Content" */
#define IPTH_HTTP_STSCODE_PARTIALCONTENT      206 /**< Successful 2xx status code "Partial Content" */

/* Redirection 3xx */
#define IPTH_HTTP_STSCODE_MULTICHOICES        300 /**< Redirection 3xx status code "Multiple Choices" */
#define IPTH_HTTP_STSCODE_MOVED               301 /**< Redirection 3xx status code "Moved Permanently" */
#define IPTH_HTTP_STSCODE_FOUND               302 /**< Redirection 3xx status code "Found" */
#define IPTH_HTTP_STSCODE_SEEOTHER            303 /**< Redirection 3xx status code "See Other" */
#define IPTH_HTTP_STSCODE_NOTMODIFIED         304 /**< Redirection 3xx status code "Not Modified" */
#define IPTH_HTTP_STSCODE_USEPROXY            305 /**< Redirection 3xx status code "Use Proxy" */
#define IPTH_HTTP_STSCODE_PROXY_REDIRECT      306 /**< Redirection 3xx status code "Proxy Redirect" */
#define IPTH_HTTP_STSCODE_TEMPREDIRECT        307 /**< Redirection 3xx status code "Temporary Redirect" */

/* Client Error 4xx */
#define IPTH_HTTP_STSCODE_BADREQUEST          400 /**< Client Error 4xx status code "Bad Request" */
#define IPTH_HTTP_STSCODE_UNAUTHORIZED        401 /**< Client Error 4xx status code "Unauthorized" */
#define IPTH_HTTP_STSCODE_FORBIDDEN           403 /**< Client Error 4xx status code "Forbidden" */
#define IPTH_HTTP_STSCODE_NOTFOUND            404 /**< Client Error 4xx status code "Not Found" */
#define IPTH_HTTP_STSCODE_NOTALLOW            405 /**< Client Error 4xx status code "Method Not Allowed" */
#define IPTH_HTTP_STSCODE_NOTACCEPTABLE       406 /**< Client Error 4xx status code "Not Acceptable" */
#define IPTH_HTTP_STSCODE_REQAUTH             407 /**< Client Error 4xx status code "Proxy Authentication Required" */
#define IPTH_HTTP_STSCODE_TIMEOUT             408 /**< Client Error 4xx status code "Request Timeout" */
#define IPTH_HTTP_STSCODE_CONFLICT            409 /**< Client Error 4xx status code "Conflict" */
#define IPTH_HTTP_STSCODE_GONE                410 /**< Client Error 4xx status code "Gone" */
#define IPTH_HTTP_STSCODE_REQLEN              411 /**< Client Error 4xx status code "Length Required" */
#define IPTH_HTTP_STSCODE_PRECONDITIONFAIL    412 /**< Client Error 4xx status code "Precondition Failed" */
#define IPTH_HTTP_STSCODE_ENTITYTOOLARGE      413 /**< Client Error 4xx status code "Request Entity Too Large" */
#define IPTH_HTTP_STSCODE_URITOOLONG          414 /**< Client Error 4xx status code "Request-URI Too Long" */
#define IPTH_HTTP_STSCODE_BADTYPE             415 /**< Client Error 4xx status code "Unsupported Media Type" */
#define IPTH_HTTP_STSCODE_BADRANGE            416 /**< Client Error 4xx status code "Requested Range Not Satisfiable" */
#define IPTH_HTTP_STSCODE_BADEXPECT           417 /**< Client Error 4xx status code "Expectation Failed" */

/* Server Error 5xx */
#define IPTH_HTTP_STSCODE_SERVERERR           500 /**< Server Error 5xx status code "Internal Server Error" */
#define IPTH_HTTP_STSCODE_NOTIMPL             501 /**< Server Error 5xx status code "Not Implemented" */
#define IPTH_HTTP_STSCODE_BADGATEWAY          502 /**< Server Error 5xx status code "Bad Gateway" */
#define IPTH_HTTP_STSCODE_UNAVAILSERVICE      503 /**< Server Error 5xx status code "Service Unavailable" */
#define IPTH_HTTP_STSCODE_GATEWAYTIMEOUT      504 /**< Server Error 5xx status code "Gateway Timeout" */
#define IPTH_HTTP_STSCODE_BADVERSION          505 /**< Server Error 5xx status code "HTTP Version Not Supported" */


/**
 * HTTP header name-value pairs. For details, refer to
 * http://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html.
 *
 * \par Constant Table
 *      HTTP header name and value pairs definitions
 * End Description
 */
#define IPTH_HTTP_HD_CONTENTTYPE              "Content-Type" /**< The media type of the entity-body sent to the recipient */
#define IPTH_HTTP_HD_CONTENTLENGTH            "Content-Length" /**< The size of the entity-body */
#define IPTH_HTTP_HD_ACCEPT                   "Accept" /**< Specifying certain media types which are acceptable for the response */
#define IPTH_HTTP_HD_CONTENTENCODING          "Content-Encoding" /**< A modifier to the media-type */
#define IPTH_HTTP_HD_USERAGENT                "User-Agent" /**< information about the user agent originating the request */
#define IPTH_HTTP_HD_CONNECTION               "Connection" /**< Allowing the sender to specify options that are desired for that particular connection and must not be communicated by proxies over further connections */
#define IPTH_HTTP_HD_CONNECTION_PERSISTENT    "Keep-Alive" /**< Keeping connection persistent */
#define IPTH_HTTP_HD_CONNECTION_CLOSE         "close" /**< Closing the connection */
#define IPTH_HTTP_HD_LASTMODIFIED             "Last-Modified" /**< The date and time at which the origin server believes the variant was last modified */
#define IPTH_HTTP_HD_IFRANGE                  "If-Range" /**< Allowing a client to "short-circuit" the second request */
#define IPTH_HTTP_HD_RANGE                    "Range" /**< The sequence of bytes in the entity-body */
#define IPTH_HTTP_HD_AUTH                     "Authorization" /**< A user agent that wishes to authenticate itself with a server */
#define IPTH_HTTP_HD_ETAG                     "ETag" /**< The current value of the entity tag for the requested variant */
#define IPTH_HTTP_HD_CONTENTRANGE             "Content-Range" /**< Where in the full entity-body the partial body should be applied */
#define IPTH_HTTP_HD_WWWAUTH                  "WWW-Authenticate" /**< MUST be included in 401 (Unauthorized) response messages */
#define IPTH_HTTP_HD_PROXYAUTHENTICATION      "Proxy-Authentication" /**< MUST be included as part of a 407 (Proxy Authentication Required) response */
#define IPTH_HTTP_HD_PROXYAUTH                "Proxy-Authorization" /**< Allowing the client to identify itself (or its user) to a proxy which requires authentication */
#define IPTH_HTTP_HD_TRANSFERENCODING         "Transfer-Encoding" /**< Indicating what (if any) type of transformation has been applied to the message body in order to safely transfer it between the sender and the recipient */
#define IPTH_HTTP_HD_CHUNKED                  "chunked" /**< Modifying the body of a message in order to transfer it as a series of chunks, each with its own size indicator, followed by an OPTIONAL trailer containing entity-header fields */


/**
 * \brief HTTP versions
 */
typedef enum ipth_HALHttpVersion {
    IPTH_HTTP_VERSION_09 = 0,	/**< Version 0.9, deprecated */
    IPTH_HTTP_VERSION_10, /**< Version 1.0 */
    IPTH_HTTP_VERSION_11, /**< Version 1.1 */
    IPTH_HTTP_VERSION_UNKNOWN /**< Version unknown */
} ipth_HALHttpVersion;


/**
 * \brief HTTP methods that indicate the purpose of a request. For details, see RFC 2616.
 */
typedef enum ipth_HALHttpMethod {
    IPTH_HTTP_METHOD_OPTIONS = 0, /**< Options */
    IPTH_HTTP_METHOD_GET, /**< Get */
    IPTH_HTTP_METHOD_HEAD, /**< Head */
    IPTH_HTTP_METHOD_POST, /**< Post */
    IPTH_HTTP_METHOD_PUT, /**< Put */
    IPTH_HTTP_METHOD_DELETE, /**< Delete */
    IPTH_HTTP_METHOD_TRACE, /**< Trace */
    IPTH_HTTP_METHOD_CONNECT, /**< Connect */
    IPTH_HTTP_METHOD_UNKNOWN /**< Unknown */
} ipth_HALHttpMethod;

/**
 * Maximum length of MIME type, which should be consistent with the type length
 * as a short string defined in XML schema on OMA-Download-OTA-V1_0 specification.
 *
 * \par Constant Table
 *      Maximum length of MIME type definition
 * End Description
 */
#define IPTH_MAX_LEN_MIMETYPE	(40) /**< Maximum length of MIME type */


/**
 * \brief HTTP connection parameter structure.
 */
typedef struct {
    const char *proxyAddrList; /**< Pointer to a null-terminated string that specifies the name of the proxy server(s) to use */
    const char *proxyAuthType; /**< Proxy Authorization  type -> Basic, Digest */
    const char *proxyAuthName; /**< Proxy Authorization Name */
    const char *proxyAuthSecret; /**< Proxy Authorization Secret */
} ipth_HALHttpConnParam;


/*
 * HTTP Interface prototypes
 */


/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Sets HTTP timeout values. All timeout values are in milliseconds.
 *
 * \param[in] httpHandle	HTTP context handle
 * \param[in] connTimeout	Timeout value for connection request
 * \param[in] sendTimeout	Timeout value for sending the request
 * \param[in] recvTimeout	Timeout value for receiving the response
 *****************************************************************************/
IP_DLL_EXPORT
int ipth_HALHttpSetTimeout(void *httpHandle,
                           int connTimeout,
                           int sendTimeout,
                           int recvTimeout);

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Sets an HTTP protocol version.
 *
 * \param [in] httpHandle	HTTP context handle
 * \param [in] version		HTTP protocol version
 * \return error code.
 *****************************************************************************/
IP_DLL_EXPORT
int ipth_HALHttpSetVersion(void *httpHandle, ipth_HALHttpVersion version);

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Sets an HTTP method.
 *
 * \param [in] httpHandle	HTTP context handle
 * \param[in]  method		HTTP method
 *****************************************************************************/
IP_DLL_EXPORT
int ipth_HALHttpSetMethod(void *httpHandle, ipth_HALHttpMethod method);


/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Sets or adds an HTTP request header key-value pair.
 *
 * \param [in] httpHandle	HTTP context handle
 * \param[in]  key			HTTP header key
 * \param [in] value		HTTP header value
 *****************************************************************************/
IP_DLL_EXPORT
int ipth_HALHttpSetRequestHeader(void *httpHandle,
                                 const char *key,
                                 const char *value);

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Sets an HTTP request body.
 *
 * \param httpHandle	HTTP context handle
 * \param body		    HTTP request body
 * \param len			HTTP request body length
 *****************************************************************************/
IP_DLL_EXPORT
int ipth_HALHttpSetRequestBody(void *httpHandle,
                               unsigned char *body,
                               unsigned int len);


/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Sends an HTTP request
 *
 * \param [in] httpHandle	HTTP context handle
 * \param [in] url			Uniform resource locator
 *
 * \retval IPTH_OK Operation completed successfully
 * \retval IPTH_ERR_IO_PENDING An asynchronous request started that notifies
 *                      completion via the provided callback. The function
 *                      may start an asynchronous operation if the callback
 *                      function is set only, otherwise it will fail.
 * \return any other code indicates operation failure
*****************************************************************************/
IP_DLL_EXPORT
int ipth_HALHttpSendRequest(void *httpHandle, const char *url);


/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Gets an HTTP response body
 *
 * \param [in] httpHandle	HTTP context handle
 * \param [out] buf			Buffer to store the response body
 *                      If the operation triggers asynchronous processing, then
 *                      the caller (DME) ensures this pointer is valid until the callback is called.
 * \param[in]  bytes_to_read	Number of bytes to read.
 * \param[out]  bytes_read	A pointer to the variable that receives the number
 *						of bytes read
 *                      In the case of asynchronous processing, the caller (DME) ensures
 *                      this pointer remains valid until the callback is called.
 * \retval IPTH_OK Operation completed successfully
 * \retval IPTH_ERR_IO_PENDING An asynchronous request started that notifies
 *                      completion via the provided callback. The function
 *                      may start an asynchronous operation if the callback
 *                      function is set only, otherwise it will fail.
 * \return any other code indicates operation failure
*****************************************************************************/
IP_DLL_EXPORT
int ipth_HALHttpGetResponseBody(void *httpHandle,
                                unsigned char *buf,
                                unsigned long bytes_to_read,
                                unsigned long *bytes_read);

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Gets an HTTP response status code
 *
 * \param [in] httpHandle	HTTP context handle
 * \return status code.
 *****************************************************************************/
IP_DLL_EXPORT
int ipth_HALHttpGetResponseStatusCode(void *httpHandle);


/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Gets an HTTP response header value
 *
 * \param [in] httpHandle	HTTP context handle
 * \param [in] key			Key
 * \return value string or NULL for error.
 *****************************************************************************/
IP_DLL_EXPORT
const char *ipth_HALHttpGetResponseHeaderValue(void *httpHandle, const char *key);


/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Creates an HTTP handle
 * \param[in] connParam Pointer to ipth_HALHttpConnParam that contains information about
 *		     the HTTP proxy connection settings. If the connParam parameters are NULL,
 *           then the connection is handled as a direct connection.
 * \param[in] pcb Pointer to callback that can be used to indicate completion of asynchronous
 *                operations, or NULL if DME does not support asynchronous HTTP request processing.
 * \return HTTP handle or NULL for error.
*****************************************************************************/
IP_DLL_EXPORT
void *ipth_HALHttpInitialize( ipth_HALHttpConnParam *connParam,ipth_HALCallback_s *pcb);

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Closes connection with the server.
 *
 * Note: The API may perform operations asynchronously only if a callback is
 *       provided to the ipth_HALHttpInitialize function when the HTTP handle is created.
 *
 * \param[in] httpHandle	HTTP context handle
 * \retval IPTH_OK Operation completed successfully
 * \retval IPTH_ERR_IO_PENDING An asynchronous request started that notifies
 *                      completion via the provided callback.
 * \return Any other code indicates operation failure
 */
IP_DLL_EXPORT
int ipth_HALHttpTerminate( void *httpHandle);

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Releases an HTTP handle and any associated resources.
 *
 * \param[in]  httpHandle	HTTP context handle
 *****************************************************************************/
IP_DLL_EXPORT
void ipth_HALHttpRelease( void *httpHandle);

IP_END_EXTERN_C

#endif /* __IPTH_HAL_HTTP_H__ */
