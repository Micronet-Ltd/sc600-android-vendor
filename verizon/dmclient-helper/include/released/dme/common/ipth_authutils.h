/******************************************************************************
 * ipthauthutils.h - provides b64 and md5 utilities interface.
 *
 * Copyright (c) 2003-2007 InnoPath Software, Inc. All rights reserved.
 *
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

#ifndef __IPTH_AUTHUTILS_H__
#define __IPTH_AUTHUTILS_H__

#ifdef	__cplusplus
extern "C" {
#endif

#include <ipth_hal_config.h>

#define MD5_DIGEST_LEN		16		/* MD5 digest length: 128 bits */
#define SHA1_DIGEST_LEN     20      /* SHA-1 digest length */

    /* MD5 context. */
    typedef struct {
        unsigned int state[4];        /* state (ABCD) */
        unsigned int count[2];        /* number of bits, modulo 2^64 (lsb first) */
        unsigned char buffer[64]; /* input buffer */
    } ipth_MD5_CTX;

    /**
     * Calculate the length of the message in B64 format.
     * \param inLen Length of the original message.
     * \return The length of the message in B64 format.
     */
    int ipth_GetB64EncodeLen(int inLen);

    /**
     * Encode a message in B64 format.
     * \param indata Original message.
     * \param inlen  Size of the original message.
     * \return Length of the message in B64 format.
     */
    void ipth_B64Encode( char *indata, size_t inlen, char *outdata);

    /**
     * Get the decoded size of a B64 encoded message.
     * \param msg	B64 encoded message.
     * \param inlen	Size of the B64 encoded message.
     * \return The length of the decoded message.
     */
    size_t ipth_GetB64DecodeLen(const char* msg, size_t inLen);

    /**
     * Decode a B64 encoded message.
     * \param indata	B64 encoded message.
     * \param inlen		Size of the B64 encoded message.
     * \outdata			The ecoded message.
     */
    void ipth_B64Decode(const char *indata, size_t inlen, char *outdata);

    /**
     * Initialize MD5 algorith with the given context.
     * \param context	MD5 context.
     */
    void ipth_MD5Init(ipth_MD5_CTX *context);

    /**
     * MD5 block update operation. Continues an MD5 message-digest
     * operation, processing another message block, and updating the
     * context.
     * \param context	MD5 context.
     * \param input		Message block
     * \param inputLen	Size of the message block.
     */
    void ipth_MD5Update (ipth_MD5_CTX *context, const unsigned char *input,
                         size_t inputLen);

    /**
     * MD5 finalization. Ends an MD5 message-digest operation, writing the
     * the message digest and zeroizing the context.
     * \param digest	Calculated digest.
     * \param context	MD5 context.
     */
    void ipth_MD5Final(unsigned char digest[16], ipth_MD5_CTX *context);

    /**
     * SHA-1 digest calculation.
     * \param text	input message to calculate SHA-1 digest.
     * \param textLen	length of the input message.
     * \param key   key used in this calculation.
     * \param keyLen length of the key.
     * \hmac output buffer for the SHA-1 digest. The caller provides this buffer.
     */
    void ipth_HmacSha1(const unsigned char *text,
                       size_t textLen,
                       const unsigned char *key,
                       size_t keyLen,
                       unsigned char *hmac);

#ifdef	__cplusplus
}
#endif

#endif /* __IPTH_AUTHUTILS_H__ */
