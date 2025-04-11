namespace mgt {

using byte = u8;
using word_be_t = be u16;
using word_le_t = le u16;

enum FileType : byte {
    Erased = 0,
    ZXBASIC = 1,        // 'BAS'
    ZXDATA_ARRAY = 2,   // 'D.ARRAY'
    ZXSTR_ARRAY = 3,    // '$.ARRAY'
    ZXCode = 4,         // 'CDE'
    ZXScreen = 7,       // 'SCREEN$'
    BASIC = 16,         // 'BAS'
    DIM = 17,           // 'D'
    STR = 18,           // '$'
    Code = 19,          // 'C'
    Screen = 20         // 'SCREEN$'
};

enum Autorun : byte {
    Autorun = 0,
    None = 255
};

bitfield Page {
    page : 5;
    padding : 3;
};

struct sector_address_map
{
    byte bytes[195];
};

struct file_type_info_array {
    byte type;                      // File [sic] type [str: 40-41, dim: 68-69]
    char name[10];                  // Name [variable name]
};

struct file_type_info_basic {
    Page progPageLen;
    word_be_t progLen;              // Program length excluding variables
    Page progPageLenNumVar;
    word_be_t progLenNumVar;        // Program length including numeric variables
    Page progPageLenAll;
    word_be_t progLenAll;           // Program length inclusive
};

struct file_type_info_code {
};
    
struct file_type_info_screen {
    byte screenMode;                // Screen mode
};
    
union file_type_info
{
    file_type_info_array array;
    file_type_info_basic basic;
    file_type_info_code code;
    file_type_info_screen screen;
    byte bytes[11];
};

struct autorun_basic {
    Autorun autorun;
    word_le_t lineNumber;
};

struct autorun_code {
    Page page;                  // Page ?
    word_le_t offset;           // Start address
};

union autorun
{
    autorun_basic basic;
    autorun_code code;
    byte bytes[3];
};

bitfield FileTypeAndStatus {
    FileType fileType : 6; 
    bool protected : 1;
    bool hidden : 1;  
};

struct DirectoryEntry {
    FileTypeAndStatus;              // Statuc and file type
    char name[10];                  // File name
    word_be_t sectorsUsed;          // 11-12
    byte trackNo;                   // 13
    byte sectorNo;                  // 14      
    sector_address_map sectorAddressMap;
    byte future_and_past[10];       // 210-219 - MGT FUTURE AND PAST (10 bytes)
    byte flags;                     // 220     - Flags (MGT use only)
    file_type_info fileTypeInfo;    // 221-231 - File type information
    byte spare4[4];                 // 232-235 - Spare 4 bytes (reserved)
    Page startPageNo;               // 236     - Start page number
    word_le_t pageOffset;           // 237-238 - Page offset
    byte numPages;                  // 239     - Number of pages in length
    word_le_t moduloLength;         // 240-241 - Length of file, MOD 16384
    autorun executionAddress;       // 242-244 - Execution address if code file or line number if an auto running basic file
    byte spare8[9];                 // 244-253 - Spare 8 [sic] bytes [actually 9 bytes]
    byte future_use[2];             // 254-255 - For future use by MGT only
};

struct LogicalSector
{
    byte data[510];
    byte nextTrack;
    byte nextSector;
};

union Sector
{
    LogicalSector logical;
    byte raw[512];
};

struct Track
{
    Sector sectors[10];
};

struct LogicalDirectory 
{
    DirectoryEntry track1[20];
    Track; // track80padding;
    DirectoryEntry track2[20];
    Track; // track81padding;
    DirectoryEntry track3[20];
    Track; // track82padding;
    DirectoryEntry track4[20];
};

union MgtDiskImage
{
    LogicalDirectory directory;
    Track interleavedTracks[160];
};

}

mgt::MgtDiskImage diskImage @ 0x00;