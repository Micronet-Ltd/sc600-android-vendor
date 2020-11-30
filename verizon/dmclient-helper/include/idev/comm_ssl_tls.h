/****************************************************************************************
*                                                                                       *
* The file (comm_ssl_tls.h) declares a security layer for InnoPath's MDMC           *
*                                                                                       *
* Copyright 2003-2009 InnoPath Software, Inc. All rights reserved.                      *
*                                                                                       *
* Legal Statements                                                                      *
*                                                                                       *
* THE SOFTWARE AND ANY MATERIALS PROVIDED BY INNOPATH ARE PROVIDED ON AN "AS IS" BASIS  *
* WITHOUT ANY REPRESENTATIONS, WARRANTIES OR CONDITIONS OF ANY KIND, WHETHER EXPRESS,   *
* IMPLIED, STATUTORY, OUT OF A COURSE OF DEALING OR USAGE, TRADE OR OTHERWISE INCLUDING *
* ANY IMPLIED WARRANTIES OR CONDITIONS OF MERCHANTABILITY, MERCHANTABLE QUALITY,        *
* FITNESS FOR ANY PARTICULAR PURPOSE OR USE, TITLE, OR NON-INFRINGEMENT. INNOPATH DOES  *
* NOT REPRESENT OR WARRANT THAT THE SOFTWARE OR ANY MATERIALS WILL BE FREE OF DEFECTS,  *
* UNINTERRUPTED, ACCURATE, COMPLETE, CURRENT, STABLE, BUG-FREE, ERROR-FREE, OR          *
* AVAILABLE AT ANY TIME.                                                                *
*                                                                                       *
* TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL INNOPATH BE LIABLE FOR ANY  *
* DIRECT, INDIRECT, CONSEQUENTIAL, INCIDENTAL, SPECIAL, RELIANCE, PUNITIVE OR OTHER     *
* DAMAGES OR EXPENSES OF ANY KIND, INCLUDING BUT NOT LIMITED TO ANY LOSS OF PROFITS OR  *
* REVENUE, LOST SAVINGS, INTERRUPTED BUSINESS, LOST BUSINESS, LOST BUSINESS             *
* OPPORTUNITIES, LOST DATA, LOST GOODWILL, LOSS FROM WORK STOPPAGE, COSTS OF OVERHEAD,  *
* COSTS OF COVER, LOSS OF ANTICIPATED BENEFITS HEREUNDER, ARISING OUT OF OR RELATED TO  *
* THIS AGREEMENT, HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, EVEN IF INNOPATH HAS   *
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, AND NOTWITHSTANDING THE FAILURE OF   *
* THE ESSENTIAL PURPOSE OF ANY LIMITED REMEDY STATED HEREIN.                            *
*                                                                                       *
****************************************************************************************/



#include "openssl/ssl.h"
#include "openssl/err.h"
#include "openssl/crypto.h"
#include "openssl/x509.h"
#include "openssl/pem.h"


#define IP_SSL_RSA_RC4_128_SHA                 0x0000    /* OpenSSL: RC4-SHA */
#define IP_SSL_RSA_3DES_EDE_CBC_SHA            0x0001    /* OpenSSL: DES-CBC3-SHA */
#define IP_TLS_RSA_AES_128_CBC_SHA1            0x0002    /* OpenSSL: AES128-SHA */
#define IP_TLS_RSA_3DES_EDE_CBC_SHA            0x0003    /* OpenSSL: DES-CBC3-SHA */
#define IP_TLS_RSA_RC4_128_CBC_SHA             0x0004    /* OpenSSL: RC4-SHA */

#define CIPHER_LIST                            "ALL:!ADH:!LOW:!EXP:!MD5"
#define STRENGTH                               ":@STRENGTH"
#define SSL_RSA_RC4_128_SHA                    ":RC4-SHA"
#define SSL_RSA_3DES_EDE_CBC_SHA               ":DES-CBC3-SHA"
#define TLS_RSA_AES_128_CBC_SHA1               ":AES128-SHA"
#define TLS_RSA_3DES_EDE_CBC_SHA               ":DES-CBC3-SHA"
#define TLS_RSA_RC4_128_CBC_SHA                ":RC4-SHA"

#define TLS_RSA_AES_SHA      ":AES128-SHA256:AES256-SHA256:AES256-SHA"
#define TLS_DHE_RSA_AES_SHA  ":DHE-RSA-AES128-SHA256:DHE-RSA-AES128-SHA:DHE-RSA-AES256-SHA256:DHE-RSA-AES256-SHA"

/* Protocol ID */
#define IP_NON_SECURE           0x00
#define IP_TLS_SSL              0x01
#define IP_SSL                  0x02
#define IP_TLS                  0x03

typedef struct ip_tlsSSLContext {
    int proto;         /* protocol */

    SSL_CTX *ctx;           /* SSL context */
    SSL     *ssl;           /* SSL connection */
    X509    *server_cert;   /* X.509 server certificate */
    X509    *client_cert;   /* X.509 client certificate */

    int status;        /* SSL status for conn/read/write etc. */
} ip_tlsSSLContext_s;


/*
 * Initialize SSL/TLS context.
 * @param none
 */
ip_tlsSSLContext_s * ipth_comm_secureInitContext( void );

/*
 * un-initialize SSL/TLS context.
 * @param  none
 */
void ipth_comm_secureDeleteContext( ip_tlsSSLContext_s *);
