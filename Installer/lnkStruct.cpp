#include "lnkStruct.h"
#include <any>
#include <locale> 
#include <codecvt>

FAT_DATE TimeToFat(FILETIME& time) {
    SYSTEMTIME sTime;
    FileTimeToSystemTime(&time, &sTime);
    FAT_DATE date = { 0 };
    date.date += (sTime.wYear - 1980) << 9;
    date.date += (sTime.wMonth) << 5;
    date.date += (sTime.wDay);
    date.time += sTime.wSecond;
    date.time += (sTime.wMinute) >> 5;
    date.time += (sTime.wHour) >> 11;
    return date;
}

int LinkTargetIDList::Write(std::ofstream& ifs)
{
    Size += sizeof(TerminalID);
    ifs.write((char*)&Size, sizeof(Size));
    for (; !shellItem.empty();) {
        (shellItem.top())->Write(ifs);
        shellItem.pop();
    }
    ifs.write((char*)&TerminalID, sizeof(TerminalID));
    return 0;
}

int LinkTargetIDList::add(LnkObject* shellItem)
{
    this->shellItem.push(shellItem);
    Size += shellItem->Size;
    return 0;
}

VoluteShellItem::VoluteShellItem(BYTE Indicator, const char* name):Indicator(Indicator),name(name){
    Size = 0x19;
}

int VoluteShellItem::Write(std::ofstream& ifs)
{
    int s = Size;
    s -= sizeof(Size);
    s -= sizeof(Indicator);
    ifs.write((char*)&Size, sizeof(Size));
    ifs.write((char*)&Indicator, sizeof(Indicator));
    for(int i=0; *(name+i)!='\0'; i++,s--)
        ifs.write((char*)(name + i), sizeof(char));

    for(;s>0;s--)
    ifs.write((char*)("\0"), sizeof(char));
    return 0;
}

FileEntryItem::FileEntryItem(BYTE I, DWORD F, WIN32_FILE_ATTRIBUTE_DATA l, WORD a, std::string P):Indicator(I),
FileSize(F), lastModify(TimeToFat(l.ftLastWriteTime)), atrribute(a), PrimaryName(P),Padding(0), ex04(TimeToFat(l.ftCreationTime), TimeToFat(l.ftLastAccessTime), P) {
    Size = 0;
    Size += sizeof(Indicator);
    Size += sizeof(Padding);
    Size += sizeof(FileSize);
    Size += sizeof(lastModify);
    Size += sizeof(atrribute);
    Size += PrimaryName.length();
    if (PrimaryName.length() % 2)
        Size ++;
    Size += ex04.Size;
    ex04.offset = Size / 4;
}


int FileEntryItem::Write(std::ofstream& ifs)
{
    ifs.write((char*)&Size, sizeof(Size));
    ifs.write((char*)&Indicator, sizeof(Indicator));
    ifs.write((char*)&Padding, sizeof(Padding));
    ifs.write((char*)&FileSize, sizeof(FileSize));
    ifs.write((char*)&lastModify, sizeof(lastModify));
    ifs.write((char*)&atrribute, sizeof(atrribute));
    const char* p = PrimaryName.c_str();
    for (int i = 0; *(p + i) != '\0'; i++) {
        ifs.write((char*)(p + i), sizeof(char));
    }
    const char null = '\0';
    ifs.write((char*)&null, sizeof(char));
    if (PrimaryName.length() % 2)
        ifs.write((char*)&null, sizeof(char));

    ex04.Write(ifs);
    return 0;
}


Beef0004::Beef0004(FAT_DATE c, FAT_DATE l, std::string n) :createTime(c), lastAccess(l){
    Size = 0;
    longStringName = n;
    longStringSize = 0;
    Size += 50+ longStringName.length()*2;

}
int Beef0004::Write(std::ofstream& ifs)
{
    ifs.write((char*)&Size, sizeof(Size));
    ifs.write((char*)&exVirsion, sizeof(exVirsion));
    ifs.write((char*)&signature, sizeof(signature));
    ifs.write((char*)&createTime, sizeof(createTime));
    ifs.write((char*)&lastAccess, sizeof(lastAccess));
    ifs.write((char*)&Virsion, sizeof(Virsion));
    ifs.write((char*)&padding, sizeof(padding));
    ifs.write((char*)&NTFSfile, sizeof(NTFSfile));
    ifs.write((char*)&padding2, sizeof(padding2));
    ifs.write((char*)&longStringSize, sizeof(longStringSize));
    ifs.write((char*)&padding3, sizeof(padding3));
    ifs.write((char*)&unknown, sizeof(unknown));
    const char* n = longStringName.c_str();
    for (int i = 0; *(n + i) != '\0'; i++) {
        char16_t char16 = *(n + i);
        ifs.write((char*)&(char16), sizeof(char16_t));
    }
    int null = 0;
    ifs.write((char*)&null, sizeof(char16_t));
    ifs.write((char*)&offset, sizeof(offset));
    return 0;
}

RootFolderShellItem::RootFolderShellItem(){

    std::any t=CLSIDFromString((LPCOLESTR)L"{20d04fe0-3aea-1069-a2d8-08002b30309d}", &(ShellFolder));
    Size = 20;
}

int RootFolderShellItem::Write(std::ofstream& ifs)
{
    ifs.write((char*)&Size, sizeof(Size));
    ifs.write((char*)&Indicator, sizeof(Indicator));
    ifs.write((char*)&SortIndex, sizeof(SortIndex));
    ifs.write((char*)&ShellFolder, sizeof(ShellFolder));
    return 0;
}

int VolumeID::Write(std::ofstream& ifs)
{
    ifs.write((char*)&DSize, sizeof(DWORD));
    ifs.write((char*)&DriveType, sizeof(DWORD));
    ifs.write((char*)&DriveSerialNumber, sizeof(DWORD));
    ifs.write((char*)&VolumeLabelOffset, sizeof(DWORD));

    const char* n = data.c_str();
    for (int i = 0; *(n + i) != '\0'; i++) {
        char letter = *(n + i);
        ifs.write((char*)&(letter), sizeof(letter));
    }
    const char null = '\0';
    ifs.write((char*)&(null), sizeof(char));
    return 0;
}

VolumeID::VolumeID():DriveType(DRIVE_FIXED),VolumeLabelOffset(0x10)
{
    char name[MAX_PATH];
    GetVolumeInformationA("C:\\", name, MAX_PATH, &DriveSerialNumber,0,0,0,0);
    data = std::string(name);
    if (data == "") {
        data = '\0';
    }
    DSize = 16 + data.length();
}

int LinkInfo::Write(std::ofstream& ifs){
    ifs.write((char*)&DSize, sizeof(DWORD));
    ifs.write((char*)&LinkInfoHeaderSize, sizeof(DWORD));
    ifs.write((char*)&LinkInfoFlags, sizeof(DWORD));
    ifs.write((char*)&VolumeIDOffset, sizeof(DWORD));
    ifs.write((char*)&LocalBasePathOffset, sizeof(DWORD));
    ifs.write((char*)&CommonNetworkRelativeLinkOffset, sizeof(DWORD));
    ifs.write((char*)&CommonPathSuffixOffset, sizeof(DWORD));
    vol.Write(ifs);
    const wchar_t* n = LocalBasePath.c_str();
    for (int i = 0; *(n + i) != '\0'; i++) {
        wchar_t letter = *(n + i);
        ifs.write((char*)&(letter), sizeof(char));
    }
    const char null = '\0';
    ifs.write((char*)&(null), sizeof(char));
    ifs.write((char*)&(CommonPathSuffix), sizeof(CommonPathSuffix));
    return 0;
}

LinkInfo::LinkInfo(std::wstring path):LinkInfoHeaderSize(0x0000001C), vol(),LinkInfoFlags(1), VolumeIDOffset(0x1C),
CommonNetworkRelativeLinkOffset(0), LocalBasePath(path), CommonPathSuffix('\0')
{
    LocalBasePathOffset = (0x1C + vol.DSize);
    DSize = 0;
    DSize += 28+ LocalBasePath.length() + 2+vol.DSize;
    CommonPathSuffixOffset = 0x2D + LocalBasePath.length() + 1;
}

StringData::StringData(std::u16string str):str(str)
{
    CountCharacters = str.length();
}

int StringData::Write(std::ofstream& ifs)
{
    ifs.write((char*)&CountCharacters, sizeof(CountCharacters));
    ifs.write((char*)str.c_str(), str.length()*2);
    return 0;
}
