#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>

typedef struct {
    uint8_t status;
    uint8_t first_chs[3];
    uint8_t partition_type;
    uint8_t last_chs[3];
    uint32_t lba;
    uint32_t sector_count;
} PartitionEntry;

int main(int argc, char **argv)
{
    char buf[512];

    int fd = open(argv[1], O_RDONLY);
	if(-1 == file_from_redirection)
	{
		printf(">>>> can't open file %s\n",argv[1]);
		return -1;
	}

	/* Read MBR */
    int num_read = read(fd, buf, 512);
	if(num_read <= 0)
	{
		printf(">>>> can't read file %s\n",argv[1]);
		return -1;
	}

    if (buf[510] == 0x55 && buf[511] == 0xAA)
	{
        // its MBR
    }
	else 
	{
        printf("its not MBR\n");
		return 0;
    }
	
	/* Point at entry table */
    PartitionEntry *table_entry_ptr = (PartitionEntry *) &buf[446];

    printf("%-5s %-10s %-10s %-10s %-10s %-10s %-10s %-10s\n", "Device",
           "Boot", "Start", "End", "Sectors", "Size", "Id", "Type");

	/******* print primary partition entries ********/
    for (int i = 0; i < 4; i++) 
	{
		if(table_entry_ptr[i].lba == 0)
		{
			/* there are only i-1 primary partitions */
			break;
		}
		printf("%s%-5d %-10c %-10u %-10u %-10u %uG      %-10X\n",
		argv[1],
		i + 1,
		table_entry_ptr[i].status == 0x80 ? '*' : ' ',
		table_entry_ptr[i].lba,
		table_entry_ptr[i].lba + table_entry_ptr[i].sector_count - 1,
		table_entry_ptr[i].sector_count,
		(uint32_t) (((uint64_t) table_entry_ptr[i].sector_count *
				512) / (1024 * 1024 * 1024)),
		table_entry_ptr[i].partition_type);
    }

	for (int i = 0; i < 4; i++)
	{
		/* if extended partition found */
		if (table_entry_ptr[i].partition_type == 0x05 ) 
		{
			// Read logical partition table
            lseek(fd, table_entry_ptr[i].lba * 512, SEEK_SET);
            num_read = read(fd, buf, 512);
			if(num_read <= 0)
			{
				printf(">>>> can't logical partition table\n");
				return -1;
			}
			
			if (buf[510] == 0x55 && buf[511] == 0xAA)
			{
				// its MBR
			}
			else 
			{
				printf("its not MBR\n");
				return 0;
			}
			
			/* Point at entry table of extended partition */
            table_entry_ptr = (PartitionEntry *) &buf[446]; 
			
			/******* print logical partition entries ********/
			for (int i = 0;/* i < 4 */; i++) 
			{
				if(table_entry_ptr[i].lba == 0)
				{
					/* there are only i-1 logical partitions */
					break;
				}
				printf("%s%-5d %-10c %-10u %-10u %-10u %uG      %-10X\n",
				argv[1],
				i + 5, // 4 primary partitions + i logical partitions
				table_entry_ptr[i].status == 0x80 ? '*' : ' ',
				table_entry_ptr[i].lba,
				table_entry_ptr[i].lba + table_entry_ptr[i].sector_count - 1,
				table_entry_ptr[i].sector_count,
				(uint32_t)(((uint64_t)table_entry_ptr[i].sector_count * 512) / (1024 * 1024 * 1024)),
				table_entry_ptr[i].partition_type);	   
			}
			break; /* only one extended partition */
		}
	}
    return 0;
}
