#define ExternalObjectsArray 38
#define FirstLinkIndex 0
#define NextLinkIndex 0
#define SchedulerAssociation 3
#define ValueIndex 1
#define ProcessListsIndex 0
#define ActiveProcessIndex 1
#include "nopsys.h"
#include "ints.h"
#include "stdio.h"
#include "libc.h"

/**
 *  Basic stuff related to memory management (paging)
**/

extern ulong tabs;

// we add 1024 extra so we can get aligned to 1024 blocks
// returns a pointer to an array of 1024 page directory entries

long* page_directory()
{
	static long pd[1024+1024];
	return (long*)((long)(pd+1023) & 0xfffff000);
}

// returns a pointer to an array of 1024 page tables, of 1024 entries each.
// 1024 pages per table * 1024 tables = 1 M entries
// 4 KB per page * 1 M pages = 4 GB
long* page_tables_start()
{
	static long pt[1024*1024+1024];
	return (long*)((long)(pt+1023) & 0xfffff000);
}

void generate_empty_page_directory()
{
	//set each entry to not present
	int i = 0;
	long *next_page_table = page_tables_start();
	for(i = 0; i < 1024; i++, next_page_table += 1024)
	{
		//attribute: supervisor level, read/write, present.
		page_directory()[i] = (long)next_page_table | 3;
	}
	
	next_page_table = page_tables_start();
	//~ next_page_table += 1024 * 10;
	//~ 
	//~ for(i = 0x10; i < 1020; i++, next_page_table += 1024)
	//~ {
		//~ //attribute: supervisor level, read/write, present.
		//~ page_directory()[i] = (long)next_page_table;
	//~ }
}


void generate_empty_page_tables()
{
	// holds the physical address where we want to start mapping these pages to.
	// in this case, we want to map these pages to the very beginning of memory.
	unsigned int address = 0; 
	unsigned int i;
	 
	//we will fill all 1024*1024 entries, mapping 4 gigabytes
	for(i = 0; i < 1024*1024; i++)
	{
		page_tables_start()[i] = address | 3; // attributes: supervisor level, read/write, present.
		address = address + 4096; //advance the address to the next page boundary
	}
}

void set_table_read_only(unsigned int *pageTable, int firstIndex, int lastIndex)
{
	int i;
	//printf("");
	for (i = firstIndex; i <= lastIndex; i++)
		pageTable[i] = pageTable[i] & 0xFFFFFFFD;
	
}

void set_table_read_write(unsigned int *pageTable, int firstIndex, int lastIndex)
{
	int i;
	//printf("");
	for (i = firstIndex; i <= lastIndex; i++)
		pageTable[i] = pageTable[i] | 0x00000002;
	
}

void make_read_only(ulong from, ulong to){
	printf_tab("Read only from: %d to: %d\n",from,to);
	unsigned int *pageDirectory;

	__asm volatile("movl %%cr3, %0": "=a" (pageDirectory));

	int i;

	unsigned int firstDirIndex   = from / (1024 * 1024 * 4);
	unsigned int lastDirIndex    = to   / (1024 * 1024 * 4);
	unsigned int firstTableIndex = (from & 0x003FF000) >> 12;
	unsigned int lastTableIndex  = (to   & 0x003FF000) >> 12;
	
	printf_tab("Page Directory: %d\n,First table: %d, last table: %d,\n first entry in first table: %d, last entry in last table: %d\n",
				pageDirectory, firstDirIndex, lastDirIndex, firstTableIndex, lastTableIndex);
	
	for (i = firstDirIndex; i <= lastDirIndex; i++)
	{
		unsigned int firstIndex = 0;
		unsigned int lastIndex = 1023;
		if (i == firstDirIndex)
			firstIndex = firstTableIndex;
		
		if (i == lastDirIndex)
			lastIndex = lastTableIndex;
			
		set_table_read_only((unsigned int*)(pageDirectory[i] & 0xFFFFF000), firstIndex, lastIndex);
	}
	
}

void make_read_write(){
	unsigned int *pageDirectory;

	__asm volatile("movl %%cr3, %0": "=a" (pageDirectory));

	int i;
	int from = 0;
	int to = 300000000;
	unsigned int firstDirIndex   = from / (1024 * 1024 * 4);
	unsigned int lastDirIndex    = to   / (1024 * 1024 * 4);
	unsigned int firstTableIndex = (from & 0x003FF000) >> 12;
	unsigned int lastTableIndex  = (to   & 0x003FF000) >> 12;
	
	printf("Page Directory: %p\n,First table: %d, last table: %d,\n first entry in first table: %d, last entry in last table: %d\n",
				pageDirectory, firstDirIndex, lastDirIndex, firstTableIndex, lastTableIndex);
	
	for (i = firstDirIndex; i <= lastDirIndex; i++)
	{
		unsigned int firstIndex = 0;
		unsigned int lastIndex = 1023;
		if (i == firstDirIndex)
			firstIndex = firstTableIndex;
		
		if (i == lastDirIndex)
			lastIndex = lastTableIndex;
			
		set_table_read_write((unsigned int*)(pageDirectory[i] & 0xFFFFF000), firstIndex, lastIndex);
	}
	
}

void enable_paging_in_hardware()
{
	unsigned int cr0;
	long *pd = page_directory();
	
	__asm volatile("xchg %%bx, %%bx" ::: "ebx");
	
	//moves pd (which is a pointer) into the cr3 register.
	__asm volatile("mov %0, %%cr3":: "b"(pd));
	
	//reads cr0, switches the "paging enable" bit, and writes it back.
	
	__asm volatile("mov %%cr0, %0": "=b"(cr0));
	cr0 |= 0x80000000;
	__asm volatile("mov %0, %%cr0":: "b"(cr0));
}


void enable_paging()
{
	generate_empty_page_tables();
	generate_empty_page_directory();
	enable_paging_in_hardware();
	
	__asm volatile("xchg %%bx, %%bx" ::: "ebx"); // This is only qemu debugging stuff...
}



void paging_handle_fault(uint32_t error_code)
{
/* ////// 	if ((error_code & 1) == 1) // Protection page fault
	{
		if ((computer->in_page_fault > 1) || (computer->in_gc)){
			printf_tab("PageFaultISR: Manually saved pages: %d of %d \n",computer->snapshot.pagesSaved,computer->snapshot.pagesToSave);
			printf_tab("PageFaultISR: Entered native page-fault for: %d\n",virtual_address_failure);
///////			saveSnapshotPage(virtual_address_failure);
		} else {
///////			VmStatus status;
			semaphore_signal_with_index(irq_semaphores[15]);
			printf_tab("PageFaultISR: Entered page-fault callback for:%d\n",virtual_address_failure);
///////			saveStatus(&status);
			computer->page_fault_handler(virtual_address_failure);
///////			releaseStatus(&status);
		}
	} else {
		// page not present
		semaphore_signal_with_index(irq_semaphores[15]);
		printf_tab("PageFaultISR: Inside a not present page fault");
		computer->page_fault_handler(virtual_address_failure);
	}
*/ /////////////////
}


