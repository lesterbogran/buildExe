# buildExe
Handmade PE Executable File

The executable is built from six data structures, which are numbered in the code’s comments. The cross-references in these structures are sometimes specified as offsets within the file, and sometimes as relative virtual addresses or RVAs. File offsets reflect the executable as it exists on disk, while RVAs reflect how it’s loaded in memory at run-time. An RVA is a run-time offset from the executable’s base address in memory. Getting these two confused will lead to problems!

DOS Header – The only fields that must be filled are the ‘MZ’ signature at the beginning and the e_lfanew parameter at the end (unless you’re actually writing a DOS program). e_lfanew gives the offset to the PE header, which in this case follows immediately after.

PE Header – The true PE header doesn’t contain much, because all the good stuff is in the optional header. The PE header specifies 1 section (the single .text section with the code to return 44), and 208 bytes combined size for the next two sections.

Optional Header – The optional header is only optional if you don’t care whether the program works. Some noteworthy values:

SectionAlignment – Each section of the executable (.text, .data, etc) must be alignment to this boundary in memory at run-time. The standard is 4096 or 4K, the size of a single page of virtual memory.
AddressOfEntryPoint – Program execution will begin at this memory offset from the base address. Because the section alignment is 4096, the program’s single .text section will be loaded at offset 4096, and execution should begin at the first byte of that section.
FileAlignment – Similar to section alignment, but for the file on disk instead of the program in memory. The standard is 512 bytes, the size of a single disk sector.
SizeOfHeaders – This isn’t really the combined size of all the headers, but rather the file offset to the first section’s data. Normally that’s the same as the combined size of all headers plus any necessary padding.
Data Directories – A typical executable would store offsets and sizes for its data directories here, the number of which is given in the optional header. Data directories are used to specify the program’s imports and exports, references to debug symbols, and other useful things. Manually constructing an import data directory is a bit complicated, so I didn’t do it. That’s why the program just returns 44 instead of doing something more interesting that would have required Win32 DLL imports. Handmade.exe does not have any data directories at all.

If you’re wondering why there are 14 data directories each with zero offset and size, instead of just specifying zero data directories, that’s a small mystery. According to tutorials I read, some parts of the OS will attempt to find info in data directories even if the number of data directories is zero. So the only safe way to have an empty data directory is to have a full table of offsets and sizes, all set to zero. However, I found other examples that did specify zero data directories and that reportedly worked fine. I didn’t look into the question any further, since it turned out not to matter anyway.

Section Table – For each section, there’s an entry here in the section table. Handmade.exe only has a single .text section, so there’s just one table entry. It gives the section size as 4 bytes, which is all that’s needed for the “return 44” code. The section will be loaded in memory at RVA 4096, which is also the program’s entry point.

Section Data – Finally comes the actual data of the .text section, which is x86 machine code. This is the meat of the program. The section data must be aligned to 512 bytes, so there’s some padding between the section table and start of the section data.
