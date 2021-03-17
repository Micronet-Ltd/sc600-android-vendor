#ifndef _IDEV_MTD_H
#define _IDEV_MTD_H

#include <sys/types.h>  // for size_t, etc.

typedef struct MtdPartition MtdPartition;
typedef struct MtdReadContext MtdReadContext;
typedef struct MtdWriteContext MtdWriteContext;


int idev_init_mtd(ip_char *partitionName);
ip_int32 idev_mtd_scanPartitions(void);
const MtdPartition *idev_mtd_findPartition_byName(ip_char *name);
int idev_mtd_partitionInfo(const MtdPartition *partition,
                           ip_int32 *total_size, ip_int32 *erase_size, ip_int32 *write_size);


ip_int16 idev_mtd_writeBlock(ip_uint16 romNo, ip_uint16 blockNo, const char *blockData);
ip_int16 idev_mtd_readBlock(ip_uint16 romNo, ip_uint16 blockNo, char *blockData);
ip_int16 idev_mtd_eraseBlock(ip_uint16 romNo, ip_uint16 blockNo);


MtdWriteContext *idev_mtd_writePartition(const MtdPartition *partition);
MtdReadContext *idev_mtd_readPartition(const MtdPartition *partition);
int idev_mtd_writeClose(MtdWriteContext *ctx);
void idev_mtd_readClose(MtdReadContext *ctx);


ip_int32 idev_mtd_write(MtdWriteContext *ctx, const char *data, ip_uint16 offset);
ip_int32 idev_mtd_read(const MtdPartition *partition, int fd, char *data, ip_uint16 offset);
int idev_mtd_erase(MtdWriteContext *ctx, int blockNo);


#endif //_IDEV_MTD_H

