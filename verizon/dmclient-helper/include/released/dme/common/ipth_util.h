/******************************************************************************
 * ipth_util.h - defines the public interface of client 6.x utility library
 *
 * Copyright (c) 2003-2007 InnoPath Software, Inc. All rights reserved.
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

#ifndef __IPTH_UTIL_H__
#define __IPTH_UTIL_H__

#include <ipth_config.h>
#include <dm_engine/ipth_mtree_node.h>

#ifdef	__cplusplus
extern "C" {
#endif

#ifndef MAX
#define MAX(a,b)    (((a)>(b))?(a):(b))
#endif

#ifndef MIN
#define MIN(a,b)    (((a)<(b))?(a):(b))
#endif

    /**
     * Validate the format of the URI
     *
     * \param uri The uri to be checked
     * \return 1 if the format is correct or 0 if incorrect.
     */
    int ipth_UtilValidateURIFormat( const char* uri);

    /**
     * Validate the format of the URL
     *
     * \param uri The uri to be checked
     * \return 1 if the format is correct or 0 if incorrect.
     */
    int ipth_UtilValidateURLFormat(const char* url);

    /**
     * Build a file name from a store identifier or from another file name.
     * If the name has the same suffix or no suffix, the new suffix will
     * be concatenated to the name. Otherwise the suffix will be replaced.
     * The new name is guaranteed to be different from the old one.
     * The caller must release the memory, allocated for the new name.
     *
     * \param name A store identifier or a file name
     * \param suffix The suffinx of the new file.
     * \return Returns the newly created file name or NULL.
     */
    char *ipth_UtilBuildFileName(const char *name, const char *suffix);

    /**
     * Return the parent from the given URI
     *
     * \param uri The input uri.
     * \param parent The parent extracted from the uri.
     */
    void ipth_UtilGetParentUri( const char *uri, char *parent);

    /**
     * Create child's URI from a parent and name. The caller must free the
     * allocated memory.
     *
     * \param parent The parent's URI.
     * \param child The child's name.
     * \return The child's URI or NULL.
     */
    char *ipth_UtilCreateChildURI(const char *parent, const char *child);

    /**
     * Return next child's name froma a list of children separated by '/'.
     * Tp be thread-safe the function saves the current search context in an
     * output argument.
     *
     * \param children The list of children separated by forward slashes.
     * \param context The place where the function saves the current search context.
     * \return The child's name or NULL if no more children.
     */
    char * ipth_UtilNextChild(char * children, char **context);

    /**
     * Return node's name from node's URI or NULL.
     *
     * \param uri Node's URI.
     * \return The node's name or NULL if no more children.
     */
    char *ipth_UtilGetNodeNameFromURI(const char *uri);



    /**
     * \brief Initialize a NodeInfo struct with proper initial values.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param[in] nInfo A pointer to the NodeInfo structure to be initialized.
     */
    int ipth_InitializeNodeInfo(ipth_NodeInfo *nInfo);

    /**
     * \brief Free the storage area pointed by member pointer fields.
     * Note we only free data fields marked by propMask.
     * Further we don't free the NodeInfo structure itself.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * After the call the node is re-initialized and ready for new usage.
     *
     * \param[in] nInfo A pointer to the NodeInfo structure to be released.
     */
    void ipth_FreeNodeInfo(ipth_NodeInfo *nInfo);


#ifdef	__cplusplus
}
#endif
#endif /* __IPTH_UTIL_H__ */
