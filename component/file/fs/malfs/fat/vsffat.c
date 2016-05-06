/***************************************************************************
 *   Copyright (C) 2009 - 2010 by Simon Qian <SimonQian@SimonQian.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <ctype.h>
#include "vsf.h"
#include "vsffat.h"

// Refer to:
// 1. "Microsoft Extensible Firmware Initiative FAT32 File System Specification"

PACKED_HEAD struct PACKED_MID fatfs_bpb_t
{
	uint16_t BytsPerSec;
	uint8_t SecPerClus;
	uint16_t RsvdSecCnt;
	uint8_t NumFATs;
	uint16_t RootEntCnt;
	uint16_t TotSec16;
	uint8_t Media;
	uint16_t FATSz16;
	uint16_t SecPerTrk;
	uint16_t NumHeads;
	uint32_t HiddSec;
	uint32_t TotSec32;
}; PACKED_TAIL

PACKED_HEAD struct PACKED_MID fatfs_ebpb_t
{
	uint8_t DrvNo;
	uint8_t Reserved;
	uint8_t BootSig;
	uint32_t VolID;
	uint8_t VolLab[11];
	uint8_t FilSysType[8];
}; PACKED_TAIL

PACKED_HEAD struct PACKED_MID fatfs_dbr_t
{
	uint8_t jmp[3];
	uint8_t OEM[8];
	// bpb All0 for exFAT
	struct fatfs_bpb_t bpb;
	union
	{
		struct
		{
			PACKED_HEAD struct PACKED_MID
			{
				uint32_t FATSz32;
				uint16_t ExtFlags;
				uint16_t FSVer;
				uint32_t RootClus;
				uint16_t FSInfo;
				uint16_t BkBootSec;
				uint8_t Reserved[12];
			} bpb; PACKED_TAIL
			struct fatfs_ebpb_t ebpb;
			uint8_t Bootstrap[420];
		} fat32;
		struct
		{
			struct fatfs_ebpb_t ebpb;
			uint8_t Bootstrap[448];
		} fat1216;
		struct
		{
			uint8_t Reserved_All0[28];
			PACKED_HEAD struct PACKED_MID
			{
				uint64_t SecStart;
				uint64_t SecCount;
				uint32_t FATSecStart;
				uint32_t FATSecCount;
				uint32_t ClusSecStart;
				uint32_t ClusSecCount;
				uint32_t RootClus;
				uint32_t VolSerial;
				PACKED_HEAD struct PACKED_MID
				{
					uint8_t Minor;
					uint8_t Major;
				} Ver; PACKED_TAIL
				uint16_t VolState;
				uint8_t SecBits;
				uint8_t SPCBits;
				uint8_t NumFATs;
				uint8_t DrvNo;
				uint8_t AllocPercnet;
				uint8_t Reserved_All0[397];
			} bpb; PACKED_TAIL
		} exfat;
	};
	uint16_t Magic;
}; PACKED_TAIL

PACKED_HEAD struct PACKED_MID fatfs_dentry_t
{
	union
	{
		PACKED_HEAD struct PACKED_MID
		{
			char Name[11];
			uint8_t Attr;
			uint8_t LCase;
			uint8_t CrtTimeTenth;
			uint16_t CrtTime;
			uint16_t CrtData;
			uint16_t LstAccData;
			uint16_t FstClusHI;
			uint16_t WrtTime;
			uint16_t WrtData;
			uint16_t FstClusLO;
			uint32_t FileSize;
		} fat;
		PACKED_HEAD struct PACKED_MID
		{
			union
			{
				
				PACKED_HEAD struct PACKED_MID
				{
					uint8_t Type;
					uint8_t NumExt;
					uint16_t Chksum;
					uint16_t Attr;
					uint16_t Reserved1;
					uint16_t CrtTime;
					uint16_t CrtData;
					uint16_t WrtTime;
					uint16_t WrtData;
					uint16_t AccTime;
					uint16_t AddData;
					uint8_t CrtTimeMs;
					uint8_t WrtTimeMs;
					uint8_t AccTimeMs;
					uint8_t Reserved2[9];
				} file;
				PACKED_HEAD struct PACKED_MID
				{
					uint8_t Type;
					uint8_t Flag;
					uint16_t Uni[15];
				} fname;
				PACKED_HEAD struct PACKED_MID
				{
					uint8_t Type;
					uint8_t Reserved1[3];
					uint32_t Chksum;
					uint8_t Reserved2[12];
					uint32_t FstClu;
					uint64_t Size;
				} casetbl;
				PACKED_HEAD struct PACKED_MID
				{
					uint8_t Order;
					uint16_t Uni0[5];
					uint8_t Attr;
					uint8_t SysID;
					uint8_t Chksum;
					uint16_t Uni5[6];
					uint16_t FstClus;
					uint16_t Uni11[2];
				} edir;
				PACKED_HEAD struct PACKED_MID
				{
					uint8_t Type;
					uint8_t Flag;
					uint8_t Reserved1;
					uint8_t NameLen;
					uint16_t NameHash;
					uint16_t Reserved2;
					uint64_t ValidSize;
					uint32_t Reserved3;
					uint32_t FstClus;
					uint64_t Size;
				} stream;
				PACKED_HEAD struct PACKED_MID
				{
					uint8_t Type;
					uint8_t Flag;
					uint8_t Reserved[18];
					uint32_t FstClus;
					uint64_t Size;
				} bmap;
				PACKED_HEAD struct PACKED_MID
				{
					uint8_t Type;
					uint8_t LblLen;
					uint16_t Uni[11];
					uint8_t Reserved[8];
				} vol;
			};
		} exfat;
	};
}; PACKED_TAIL

static const uint8_t vsffat_FAT_bitsize[] = {0, 12, 16, 32, 32};

static uint32_t vsffat_clus2sec(struct vsffat_t *fat, uint32_t cluster)
{
	cluster -= fat->root_cluster ? fat->root_cluster : 2;
	return fat->database + (cluster >> fat->clustersize_bits);
}

bool vsffat_is_LFN(char *name)
{
	char *ext = NULL;
	bool has_lower = false, has_upper = false;
	uint32_t i, name_len = 0, ext_len = 0;

	if (name != NULL)
	{
		name_len = strlen(name);
		ext = vsfile_getfileext(name);
	}
	if (ext != NULL)
	{
		ext_len = strlen(ext);
		name_len -= ext_len + 1;	// 1 more byte for dot
	}
	if ((name_len > 8) || (ext_len > 3))
	{
		return true;
	}

	for (i = 0; name[i] != '\0'; i++)
	{
		if (islower((int)name[i]))
		{
			has_lower = true;
		}
		if (isupper((int)name[i]))
		{
			has_upper = true;
		}
	}
	return has_lower && has_upper;
}

static vsf_err_t vsffat_get_FATentry(struct vsfsm_pt_t *pt, vsfsm_evt_t evt,
										uint32_t cluster, uint32_t *entry)
{
	struct vsffat_t *fat = (struct vsffat_t *)pt->user_data;
	struct vsf_malfs_t *malfs = &fat->malfs;
	uint8_t fatbit = vsffat_FAT_bitsize[(int)fat->type];
	uint32_t startbit = cluster * fatbit;
	uint32_t sectorbit = 1 << (fat->sectorsize_bits + 3);
	uint32_t startbit_sec = startbit & (sectorbit - 1);

	vsfsm_pt_begin(pt);

	if (cluster < 2)
		return VSFERR_FAIL;

	// for FAT12, one FAT entry MAY be divided into 2 FAT sectors
	fat->readbit = 0;
	while (fat->readbit < fatbit)
	{
		startbit = fat->fatbase + (startbit >> (fat->sectorsize_bits + 3));
		startbit += fat->readbit ? 1 : 0;
		if (fat->cur_fatsector != startbit)
		{
			fat->cur_fatsector = startbit;
			vsf_malfs_read(malfs, startbit, malfs->sector_buffer, 1);
			vsfsm_pt_wait(pt);
			if (VSF_MALFS_EVT_IOFAIL == evt)
			{
				fat->cur_fatsector = 0;
				return VSFERR_FAIL;
			}
		}

		if (fat->readbit)
		{
			*entry |= GET_LE_U32(malfs->sector_buffer) << fat->readbit;
			*entry &= (1 << fatbit) - 1;
			break;
		}
		else
		{
			fat->readbit += min(fatbit, sectorbit - startbit_sec);
			*entry = GET_LE_U32(&malfs->sector_buffer[startbit_sec >> 3]);
			*entry = (*entry >> (startbit & 7)) & ((1 << fat->readbit) - 1);
		}
	}

	vsfsm_pt_end(pt);
	return VSFERR_NONE;
}

// if *cluster != 0, then allocate chain from *cluster for data in size
// if *cluster == 0, then allocate new chain for data in size
static vsf_err_t vsffat_alloc_cluschain(struct vsfsm_pt_t *pt, vsfsm_evt_t evt,
										uint32_t size, uint32_t *cluster)
{
	// TODO: implement this for addfile
	return VSFERR_NOT_SUPPORT;
}

// true for data entry and last cluster entry
static bool vsffat_FATentry_valid(struct vsffat_t *fat, uint32_t cluster)
{
	uint8_t fatbit = vsffat_FAT_bitsize[(int)fat->type];
	uint32_t mask = (1 << fatbit) - 1;

	cluster &= (32 == fatbit) ? 0x0FFFFFFF : mask;
	return (cluster >= 2) && (cluster <= mask);
}

// true for last cluster entry
static bool vsffat_FATentry_EOF(struct vsffat_t *fat, uint32_t cluster)
{
	uint8_t fatbit = vsffat_FAT_bitsize[(int)fat->type];
	uint32_t mask = (1 << fatbit) - 1;

	cluster &= (32 == fatbit) ? 0x0FFFFFFF : mask;
	return (cluster >= (mask - 8)) && (cluster <= mask);
}

static vsf_err_t vsffat_parse_dbr(struct vsffat_t *fat, struct fatfs_dbr_t *dbr)
{
	uint16_t BytsPerSec;
	uint32_t tmp32;
	uint8_t *ptr = (uint8_t *)dbr;
	int i;

	if (dbr->Magic != SYS_TO_BE_U16(0x55AA))
		return VSFERR_FAIL;

	for (i = 0; i < 53; i++) if (*ptr++) break;
	if (i < 53)
	{
		// normal FAT12, FAT16, FAT32
		uint32_t sectornum, clusternum;

		BytsPerSec = LE_TO_SYS_U16(dbr->bpb.BytsPerSec);
		fat->sectorsize_bits = BytsPerSec & (BytsPerSec - 1);
		tmp32 = dbr->bpb.SecPerClus;
		fat->clustersize_bits = tmp32 & (tmp32 - 1);
		fat->reserved_size = LE_TO_SYS_U16(dbr->bpb.RsvdSecCnt);
		fat->fatnum = dbr->bpb.NumFATs;
		sectornum = LE_TO_SYS_U16(dbr->bpb.TotSec16);
		if (!sectornum)
			sectornum = LE_TO_SYS_U32(dbr->bpb.TotSec32);
		fat->fatsize = dbr->bpb.FATSz16 ? LE_TO_SYS_U16(dbr->bpb.FATSz16) :
										LE_TO_SYS_U32(dbr->fat32.bpb.FATSz32);

		// BytsPerSec MUST the same as mal blocksize, and MUST following value:
		// 		512, 1024, 2048, 4096
		// SecPerClus MUST be power of 2
		// RsvdSecCnt CANNOT be 0
		// NumFATs CANNOT be 0
		if (((BytsPerSec != fat->malfs.malstream.mal->cap.block_size) ||
				((BytsPerSec != 512) && (BytsPerSec != 1024) &&
					(BytsPerSec != 2048) && (BytsPerSec != 4096))) ||
			(!dbr->bpb.SecPerClus || !fat->clustersize_bits) ||
			!fat->reserved_size || !fat->fatnum || !sectornum || !fat->fatsize)
		{
			return VSFERR_FAIL;
		}

		tmp32 = fat->rootentry = LE_TO_SYS_U16(dbr->bpb.RootEntCnt);
		if (tmp32)
			tmp32 = ((tmp32 >> 5) + BytsPerSec - 1) / BytsPerSec;
		fat->rootsize = tmp32;
		// calculate base
		fat->fatbase = fat->reserved_size;
		fat->rootbase = fat->fatbase + fat->fatnum * fat->fatsize;
		fat->database = fat->rootbase + fat->rootsize;
		// calculate cluster number: note that cluster starts from root_cluster
		clusternum = (sectornum - fat->reserved_size) >> fat->clustersize_bits;

		// for FAT32 RootEntCnt MUST be 0
		if (!fat->rootentry)
		{
			// FAT32
			fat->type = VSFFAT_FAT32;

			// for FAT32, TotSec16 and FATSz16 MUST be 0
			if (dbr->bpb.FATSz16 || dbr->bpb.TotSec16)
				return VSFERR_FAIL;

			fat->fsinfo = LE_TO_SYS_U16(dbr->fat32.bpb.FSInfo);

			// RootClus CANNOT be less than 2
			fat->root_cluster = LE_TO_SYS_U32(dbr->fat32.bpb.RootClus);
			if (fat->root_cluster < 2)
			{
				return VSFERR_FAIL;
			}

			fat->clusternum = fat->root_cluster + clusternum;
		}
		else
		{
			// FAT12 or FAT16
			fat->type = (clusternum < 4085) ? VSFFAT_FAT12 : VSFFAT_FAT16;

			// root has no cluster
			fat->root_cluster = 0;
			fat->clusternum = clusternum + 2;
		}
	}
	else
	{
		// bpb all 0, exFAT
		fat->type = VSFFAT_EXFAT;

		fat->sectorsize_bits = dbr->exfat.bpb.SecBits;
		fat->clustersize_bits = dbr->exfat.bpb.SPCBits;
		fat->reserved_size = 0;
		fat->fatnum = dbr->exfat.bpb.NumFATs;
		fat->fatsize = LE_TO_SYS_U32(dbr->exfat.bpb.FATSecCount);
		fat->rootentry = 0;
		fat->rootsize = 0;
		fat->fatbase = LE_TO_SYS_U32(dbr->exfat.bpb.FATSecStart);
		fat->rootbase = LE_TO_SYS_U32(dbr->exfat.bpb.ClusSecStart);
		fat->database = fat->rootbase;
		fat->root_cluster = LE_TO_SYS_U32(dbr->exfat.bpb.RootClus);
		fat->clusternum = LE_TO_SYS_U32(dbr->exfat.bpb.ClusSecCount) + 2;

		// SecBits CANNOT be smaller than 9, which is 512 byte
		// RootClus CANNOT be less than 2
		if ((fat->sectorsize_bits < 9) || (fat->root_cluster < 2))
		{
			return VSFERR_FAIL;
		}
	}

	return VSFERR_NONE;
}

vsf_err_t vsffat_mount(struct vsfsm_pt_t *pt, vsfsm_evt_t evt,
								struct vsfile_t *dir)
{
	struct vsffat_t *fat = (struct vsffat_t *)pt->user_data;
	struct vsf_malfs_t *malfs = &fat->malfs;
	struct fatfs_dentry_t *dentry;
	vsf_err_t err = VSFERR_NONE;

	vsfsm_pt_begin(pt);

	fat->cur_fatsector = 0;

	err = vsf_malfs_init(malfs);
	if (err) goto fail;

	if (vsfsm_crit_enter(&malfs->crit, pt->sm))
	{
		vsfsm_pt_wfe(pt, VSF_MALFS_EVT_CRIT);
	}
	malfs->notifier_sm = pt->sm;

	vsf_malfs_read(malfs, 0, malfs->sector_buffer, 1);
	vsfsm_pt_wait(pt);
	if (VSF_MALFS_EVT_IOFAIL == evt)
	{
		goto fail_crit;
	}

	// parse DBR
	fat->type = VSFFAT_NONE;
	err = vsffat_parse_dbr(fat, (struct fatfs_dbr_t *)malfs->sector_buffer);
	if (err)
	{
		goto fail_crit;
	}

	vsf_malfs_read(malfs, fat->rootbase, malfs->sector_buffer, 1);
	vsfsm_pt_wait(pt);
	vsfsm_crit_leave(&malfs->crit);
	if (VSF_MALFS_EVT_IOFAIL == evt)
	{
		goto fail;
	}

	// check volume_id
	malfs->volume_name = NULL;
	dentry = (struct fatfs_dentry_t *)malfs->sector_buffer;
	if (VSFFAT_EXFAT == fat->type)
	{
		// TODO: parse VolID for EXFAT
	}
	else
	{
		if (VSFILE_ATTR_VOLUMID == dentry->fat.Attr)
		{
			malfs->volume_name = vsf_bufmgr_malloc(12);
			if (malfs->volume_name != NULL)
			{
				int i;

				malfs->volume_name[11] = '\0';
				memcpy(malfs->volume_name, dentry->fat.Name, 11);
				for (i = 10; i >= 0; i--)
				{
					if (malfs->volume_name[i] != ' ')
					{
						break;
					}
					malfs->volume_name[i] = '\0';
				}
			}
		}
	}

	// initialize root
	fat->root.file.name = malfs->volume_name;
	fat->root.file.size = 0;
	fat->root.file.attr = VSFILE_ATTR_DIRECTORY;
	fat->root.file.op = (struct vsfile_fsop_t *)&vsffat_op;
	fat->root.file.parent = NULL;
	fat->root.fat = fat;
	fat->root.first_cluster = fat->root_cluster;

	vsfsm_pt_end(pt);
	return err;
fail_crit:
	vsfsm_crit_leave(&malfs->crit);
fail:
	vsf_malfs_fini(malfs);
	return err;
}

vsf_err_t vsffat_unmount(struct vsfsm_pt_t *pt, vsfsm_evt_t evt,
								struct vsfile_t *dir)
{
	struct vsffat_t *fat = (struct vsffat_t *)pt->user_data;
	vsf_malfs_fini(&fat->malfs);
	return VSFERR_NONE;
}

vsf_err_t vsffat_addfile(struct vsfsm_pt_t *pt, vsfsm_evt_t evt,
					struct vsfile_t *dir, char *name, enum vsfile_attr_t attr)
{
	return VSFERR_NOT_SUPPORT;
}

vsf_err_t vsffat_removefile(struct vsfsm_pt_t *pt, vsfsm_evt_t evt,
					struct vsfile_t *dir, char *name)
{
	return VSFERR_NOT_SUPPORT;
}

static vsf_err_t vsffat_getchild(struct vsfsm_pt_t *pt,
					vsfsm_evt_t evt, struct vsfile_t *dir,
					struct vsfile_t **file, char *name, uint32_t idx)
{
	struct vsffat_t *fat = (struct vsffat_t *)pt->user_data;
	struct vsf_malfs_t *malfs = &fat->malfs;
	struct vsfile_fatfile_t *fatdir;
	struct fatfs_dentry_t *dentry;
	vsf_err_t err = VSFERR_NONE;

	if (NULL == dir)
	{
		dir = (struct vsfile_t *)&((struct vsffat_t *)pt->user_data)->root;
	}
	fatdir = (struct vsfile_fatfile_t *)dir;

	vsfsm_pt_begin(pt);

	if (vsfsm_crit_enter(&malfs->crit, pt->sm))
	{
		vsfsm_pt_wfe(pt, VSF_MALFS_EVT_CRIT);
	}
	malfs->notifier_sm = pt->sm;

	fat->cur_cluster = fatdir->first_cluster;
	if (!fat->cur_cluster)
	{
		// it MUST be root for FAT12 and FAT16
		if ((fat->type != VSFFAT_FAT12) && (fat->type != VSFFAT_FAT16))
		{
			err = VSFERR_FAIL;
			goto exit;
		}
		fat->cur_sector = fat->rootbase;
	}
	else
	{
		fat->cur_sector = vsffat_clus2sec(fat, fat->cur_cluster);
	}

	while (1)
	{
		fat->cur_fatsector = 0;
		vsf_malfs_read(malfs, fat->cur_sector, malfs->sector_buffer, 1);
		vsfsm_pt_wait(pt);
		if (VSF_MALFS_EVT_IOFAIL == evt)
		{
			err = VSFERR_FAIL;
			break;
		}

		dentry = (struct fatfs_dentry_t *)malfs->sector_buffer;
		// TODO: try to search file equal to name or idx
		// TODO: if found, allocate and initialize file structure

		if ((!fat->cur_cluster && (fat->cur_sector < fat->rootsize)) ||
			(fat->cur_sector & ((1 << fat->clustersize_bits) - 1)))
		{
			// not found in current sector, find next sector
			fat->cur_sector++;
		}
		else
		{
			// not found in current cluster, find next cluster if exists
			fat->caller_pt.sm = pt->sm;
			fat->caller_pt.user_data = fat;
			fat->caller_pt.state = 0;
			vsfsm_pt_entry(pt);
			err = vsffat_get_FATentry(&fat->caller_pt, evt,
									fat->cur_cluster, &fat->cur_cluster);
			if (err > 0) return err; else if (err < 0) break;

			if (!vsffat_FATentry_valid(fat, fat->cur_cluster) ||
				vsffat_FATentry_EOF(fat, fat->cur_cluster))
			{
				err = VSFERR_NOT_AVAILABLE;
				break;
			}

			// remove MSB 4-bit for 32-bit FAT entry
			fat->cur_cluster &= 0x0FFFFFFF;
			fat->cur_sector = vsffat_clus2sec(fat, fat->cur_cluster);
		}
	}
exit:
	vsfsm_crit_leave(&malfs->crit);
	vsfsm_pt_end(pt);
	return err;
}

vsf_err_t vsffat_getchild_byname(struct vsfsm_pt_t *pt,
					vsfsm_evt_t evt, struct vsfile_t *dir, char *name,
					struct vsfile_t **file)
{
	return vsffat_getchild(pt, evt, dir, file, name, 0);
}

vsf_err_t vsffat_getchild_byidx(struct vsfsm_pt_t *pt,
					vsfsm_evt_t evt, struct vsfile_t *dir, uint32_t idx,
					struct vsfile_t **file)
{
	return vsffat_getchild(pt, evt, dir, file, NULL, idx);
}

vsf_err_t vsffat_close(struct vsfsm_pt_t *pt, vsfsm_evt_t evt,
					struct vsfile_t *file)
{
	if (file->name != NULL)
	{
		vsf_bufmgr_free(file->name);
		file->name = NULL;
	}
	vsf_bufmgr_free(file);
	return VSFERR_NONE;
}

vsf_err_t vsffat_read(struct vsfsm_pt_t *pt, vsfsm_evt_t evt,
					struct vsfile_t *file, uint64_t offset,
					uint32_t size, uint8_t *buff, uint32_t *rsize)
{
	struct vsfile_fatfile_t *fatfile = (struct vsfile_fatfile_t *)file;
	struct vsffat_t *fat = fatfile->fat;
	struct vsf_malfs_t *malfs = &fat->malfs;
	uint32_t clustersize = 1 << (fat->clustersize_bits + fat->sectorsize_bits);
	uint8_t *pbuf;
	vsf_err_t err = VSFERR_NONE;

	vsfsm_pt_begin(pt);

	if (vsfsm_crit_enter(&malfs->crit, pt->sm))
	{
		vsfsm_pt_wfe(pt, VSF_MALFS_EVT_CRIT);
	}
	malfs->notifier_sm = pt->sm;

	// locate the first cluster for access
	fat->cur_cluster = fatfile->first_cluster;
	fat->cur_offset = 0;
	while ((fat->cur_offset + clustersize) <= offset)
	{
		fat->caller_pt.sm = pt->sm;
		fat->caller_pt.user_data = fat;
		fat->caller_pt.state = 0;
		vsfsm_pt_entry(pt);
		err = vsffat_get_FATentry(&fat->caller_pt, evt,
							fat->cur_cluster, &fat->cur_cluster);
		if (err > 0) return err; else if (err < 0)
		{
			err = VSFERR_FAIL;
			goto exit;
		}
		if (!vsffat_FATentry_valid(fat, fat->cur_cluster))
		{
			err = VSFERR_FAIL;
			goto exit;
		}

		// remove MSB 4-bit for 32-bit FAT entry
		fat->cur_cluster &= 0x0FFFFFFF;
		fat->cur_offset += clustersize;
		continue;
	}

	fat->cur_size = 0;
	fat->remain_size = size;
	fat->cur_sector = vsffat_clus2sec(fat, fat->cur_cluster);
	fat->cur_sector += (offset & (clustersize - 1)) >> fat->sectorsize_bits;
	fat->cur_offset = offset & ~((1 << fat->sectorsize_bits) - 1); 
	while (fat->remain_size)
	{
		if (fat->cur_offset != offset)
		{
			// read first non-page-aligned data
			fat->cur_run_size = offset & ((1 << fat->sectorsize_bits) - 1);
			fat->cur_run_sector = 1;
			pbuf = malfs->sector_buffer;
			fat->cur_fatsector = 0;
		}
		else if (fat->remain_size < (1 << fat->sectorsize_bits))
		{
			// read last non-page-aligned data
			fat->cur_run_size = fat->remain_size;
			fat->cur_run_sector = 1;
			pbuf = malfs->sector_buffer;
			fat->cur_fatsector = 0;
		}
		else
		{
			// read page-aligned data in cluster
			// get remain sector in clusrer
			fat->cur_run_sector =
						fat->cur_sector & ((1 << fat->clustersize_bits) - 1);
			fat->cur_run_sector = min(fat->cur_run_sector,
						fat->remain_size >> fat->sectorsize_bits);
			fat->cur_run_size = fat->cur_run_sector << fat->sectorsize_bits;
			pbuf = buff + fat->cur_size;
		}
		vsf_malfs_read(malfs, fat->cur_sector, pbuf, fat->cur_run_sector);
		vsfsm_pt_wait(pt);
		if (VSF_MALFS_EVT_IOFAIL == evt)
		{
			err = VSFERR_FAIL;
			goto exit;
		}

		if (fat->cur_offset != offset)
		{
			uint8_t *src = malfs->sector_buffer;
			src += (1 << fat->sectorsize_bits) - fat->cur_run_size;
			memcpy(buff, src, fat->cur_run_size);
			fat->cur_offset += fat->cur_run_size;
		}
		else if (fat->remain_size < (1 << fat->sectorsize_bits))
		{
			uint8_t *dst = buff + fat->cur_size;
			memcpy(dst, malfs->sector_buffer, fat->cur_run_size);
		}
		fat->cur_size += fat->cur_run_size;
		fat->remain_size -= fat->cur_run_size;

		// get next cluster if necessary
		if (fat->remain_size &&
			!(fat->cur_sector & ((1 << fat->clustersize_bits) - 1)))
		{
			fat->caller_pt.sm = pt->sm;
			fat->caller_pt.user_data = fat;
			fat->caller_pt.state = 0;
			vsfsm_pt_entry(pt);
			err = vsffat_get_FATentry(&fat->caller_pt, evt,
							fat->cur_cluster, &fat->cur_cluster);
			if (err > 0) return err; else if (err < 0)
			{
				err = VSFERR_FAIL;
				goto exit;
			}
			if (!vsffat_FATentry_valid(fat, fat->cur_cluster))
			{
				err = VSFERR_FAIL;
				goto exit;
			}

			// remove MSB 4-bit for 32-bit FAT entry
			fat->cur_cluster &= 0x0FFFFFFF;
		}
	}

exit:
	vsfsm_crit_leave(&malfs->crit);
	vsfsm_pt_end(pt);
	return err;
}

vsf_err_t vsffat_write(struct vsfsm_pt_t *pt, vsfsm_evt_t evt,
					struct vsfile_t *file, uint64_t offset,
					uint32_t size, uint8_t *buff, uint32_t *wsize)
{
	return VSFERR_NOT_SUPPORT;
}

#ifndef VSFCFG_STANDALONE_MODULE
const struct vsfile_fsop_t vsffat_op =
{
	// mount / unmount
	.mount = vsffat_mount,
	.unmount = vsffat_unmount,
	// f_op
	.f_op.close = vsffat_close,
	.f_op.read = vsffat_read,
	.f_op.write = vsffat_write,
	// d_op
	.d_op.addfile = vsffat_addfile,
	.d_op.removefile = vsffat_removefile,
	.d_op.getchild_byname = vsffat_getchild_byname,
	.d_op.getchild_byidx = vsffat_getchild_byidx,
};
#endif
