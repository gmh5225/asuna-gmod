#include "memory.h"

void BytePatch(PVOID source, BYTE newValue)
{
    DWORD originalProtection;
    VirtualProtect(source, sizeof(PVOID), PAGE_EXECUTE_READWRITE, &originalProtection);
    *(BYTE*)(source) = newValue;
    VirtualProtect(source, sizeof(PVOID), originalProtection, &originalProtection);
}

void RestoreVMTHook(PVOID** src, PVOID dst, int index)
{
    PVOID* VMT = *src;
    PVOID ret = (VMT[index]);

    DWORD originalProtection;

    VirtualProtect(&VMT[index], sizeof(PVOID), PAGE_EXECUTE_READWRITE, &originalProtection);
    VMT[index] = dst;
    VirtualProtect(&VMT[index], sizeof(PVOID), originalProtection, &originalProtection);
}

std::vector<HookData> VMTHooks;
void RestoreVMTHooks()
{
    for (int i = 0; i < VMTHooks.size(); i++)
    {
        auto currentData = VMTHooks[i];
        RestoreVMTHook(currentData.src, currentData.dst, currentData.index);
    }
}

static auto GenerateBadCharTable(std::string_view pattern) noexcept
{
    std::array<std::size_t, 256> table{};

    auto lastWildcard = pattern.rfind('?');
    if (lastWildcard == std::string_view::npos)
        lastWildcard = 0;

    const auto defaultShift = (std::max)(std::size_t(1), pattern.length() - 1 - lastWildcard);
    table.fill(defaultShift);

    for (auto i = lastWildcard; i < pattern.length() - 1; ++i)
        table[static_cast<std::uint8_t>(pattern[i])] = pattern.length() - 1 - i;

    return table;
}

const char* FindPattern(const char* moduleName, std::string_view pattern, std::string patternName) noexcept
{
    PVOID moduleBase = 0;
    std::size_t moduleSize = 0;

    if (HMODULE handle = GetModuleHandleA(moduleName))
    {
        if (MODULEINFO moduleInfo; GetModuleInformation(GetCurrentProcess(), handle, &moduleInfo, sizeof(moduleInfo)))
        {
            moduleBase = moduleInfo.lpBaseOfDll;
            moduleSize = moduleInfo.SizeOfImage;
        }
    }

    if (moduleBase && moduleSize) 
    {
        int lastIdx = pattern.length() - 1;
        const auto badCharTable = GenerateBadCharTable(pattern);

        auto start = static_cast<const char*>(moduleBase);
        const auto end = start + moduleSize - pattern.length();

        while (start <= end) {
            int i = lastIdx;

            while (i >= 0 && (pattern[i] == '?' || start[i] == pattern[i]))
            {
                --i;
            }

            if (i < 0)
            {
                return start;
            }

            start += badCharTable[static_cast<std::uint8_t>(start[lastIdx])];
        }
    }
 
    return 0;
}

char* GetRealFromRelative(char* address, int offset, int instructionSize, bool isRelative)
{
    #ifdef _WIN64
        isRelative = true;
    #endif

    char* instruction = address + offset;
    if (!isRelative)
    {
        return *(char**)(instruction);
    }

    int relativeAddress = *(int*)(instruction);
    char* realAddress = address + instructionSize + relativeAddress;

    return realAddress;
}

std::string RandomString(int length)
{
    std::string output;
    std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
   
    for (int i = 0; i < length; i++)
        output += alphabet[rand() % (alphabet.length() - 1)];
    
    return output;
}