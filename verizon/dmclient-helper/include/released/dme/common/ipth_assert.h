/******************************************************************************
 * Check for errors that should not happen. (Assertion)
 *
 * Copyright (c) 2003-2007 InnoPath Software, Inc. All rights reserved.
 *
 * Author(s):   Jun Sun <jsun@innopath.com>
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

#ifndef __IPTH_IPTH_ASSERT_H__
#define __IPTH_IPTH_ASSERT_H__

/*
 * NOTE :
 *
 * Rules about using assertion:
 *
 *  . NEVER replace an error checking with an assertion.
 *  . Assertion can only be used for the case that is logically and
 *    provably impossible error.  In general, this kind of errors can
 *    only happen due to memory corruption or hardware errors.
 *  . Use assertion liberally (subject to above restrictions).
 */

/*
 * Assertion can be turned off, use native platform assert or be redirected
 * to logger.
 */

#include <ipth_config.h>

#if defined(IPTH_CONFIG_ASSERT_OFF)

/* Assert a statement is true */
#define	ipth_Assert(expr)

/* Verify is similar to assert, except expr is evaluated even when
 * assertion is turned off. Use example, ipth_Verify(GetInt(), != 0);
 */
#define ipth_Verify(expr, test) (expr)

#elif defined(IPTH_CONFIG_ASSERT_NATIVE)

/* ipth_Assert and ipth_Verify() are defined in the platform specific ipth_hal_config.h */

#elif defined(IPTH_CONFIG_ASSERT_LOGGER)

#include <dm_engine/ipth_logger.h>

#define ipth_Assert(expr) \
	if (!(expr)) \
		ipth_Log(IPTH_LOG_PANIC, "Assert failure at %s:%d, %s", \
		__FILE__, __LINE__, #expr);

#define ipth_Verify(expr, test) \
	if (!(expr test)) \
		ipth_Log(IPTH_LOG_PANIC, "Assert failure at %s:%d, %s%s", \
		__FILE__, __LINE__, #expr, #test);

#else

#error "IPTH_CONFIG_ASSERT_xxx needs to be defined."

#endif

#endif /* __IPTH_IPTH_ASSERT_H__ */

