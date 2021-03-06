// buildExe.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>



inline void setbyte(BYTE* pBuf, DWORD off, BYTE val) { pBuf[off] = val; }
inline void setword(BYTE* pBuf, DWORD off, WORD val) { *(WORD*)(&pBuf[off]) = val; }
inline void setdword(BYTE* pBuf, DWORD off, DWORD val) { *(DWORD*)(&pBuf[off]) = val; }
inline void setstring(BYTE* pBuf, DWORD off, char* val) { lstrcpy((LPWSTR)&pBuf[off], (LPWSTR)val); }

DWORD BuildExe(BYTE* exe)
{
	// 1. DOS HEADER, 64 bytes
	setstring(exe, 0, (char *) "MZ"); // DOS header signature is 'MZ'
	setdword(exe, 60, 64); // DOS e_lfanew field gives the file offset to the PE header

						   // 2. PE HEADER, at offset DOS.e_lfanew, 24 bytes
	setstring(exe, 64, (char *) "PE"); // PE header signature is 'PE\0\0'
	setword(exe, 68, 0x14C); // PE.Machine = IMAGE_FILE_MACHINE_I386
	setword(exe, 70, 1); // PE.NumberOfSections = 1
	setword(exe, 84, 208); // PE.SizeOfOptionalHeader = offset between the optional header and the section table
	setword(exe, 86, 0x103); // PE.Characteristics = IMAGE_FILE_32BIT_MACHINE | IMAGE_FILE_EXECUTABLE_IMAGE | IMAGE_FILE_RELOCS_STRIPPED

							 // 3. OPTIONAL HEADER, follows PE header, 96 bytes
	setword(exe, 88, 0x10B); // Optional header signature is 10B
	setdword(exe, 104, 4096); // Opt.AddressOfEntryPoint = RVA where code execution should begin
	setdword(exe, 116, 0x400000); // Opt.ImageBase = base address at which to load the program, 0x400000 is standard
	setdword(exe, 120, 4096); // Opt.SectionAlignment = alignment of section in memory at run-time, 4096 is standard
	setdword(exe, 124, 512); // Opt.FileAlignment = alignment of sections in file, 512 is standard
	setword(exe, 136, 4); // Opt.MajorSubsystemVersion = minimum OS version required to run this program
	setdword(exe, 144, 4096 * 2); // Opt.SizeOfImage = total run-time memory size of all sections and headers
	setdword(exe, 148, 512); // Opt.SizeOfHeaders = total file size of header info before the first section
	setword(exe, 156, 3); // Opt.Subsystem = IMAGE_SUBSYSTEM_WINDOWS_CUI, command-line program
	setdword(exe, 180, 14); // Opt.NumberOfRvaAndSizes = number of data directories following

							// 4. DATA DIRECTORIES, follows optional header, 8 bytes per directory 
							// offset and size for each directory is zero

							// 5. SECTION TABLE, follows data directories, 40 bytes
	setstring(exe, 296, (char *) ".text"); // name of 1st section
	setdword(exe, 304, 4); // sectHdr.VirtualSize = size of the section in memory at run-time
	setdword(exe, 308, 4096); // sectHdr.VirtualAddress = RVA for the section
	setdword(exe, 312, 4); // sectHdr.SizeOfRawData = size of the section data in the file
	setdword(exe, 316, 512); // sectHdr.PointerToRawData = file offset of this section's data
	setdword(exe, 332, 0x60000020); // sectHdr.Characteristics = IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_CNT_CODE

									// 6. .TEXT SECTION, at sectHdr.PointerToRawData (aligned to Opt.FileAlignment)
	setbyte(exe, 512, 0x6A); // PUSH
	setbyte(exe, 513, 0x2C); // value to push
	setbyte(exe, 514, 0x58); // POP EAX
	setbyte(exe, 515, 0xC3); // RETN

	return 516; // size of exe
}

int main(int argc, char* argv[])
{
	HANDLE hFile = CreateFile(L"handmade.exe", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);
	BYTE* buf = (BYTE*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 1024);

	DWORD exeSize = BuildExe(buf);

	DWORD numberOfBytesWritten;
	WriteFile(hFile, buf, exeSize, &numberOfBytesWritten, NULL);

	HeapFree(GetProcessHeap(), 0, buf);
	CloseHandle(hFile);
	printf("wrote handmade.exe\n");
	return 0;
}

