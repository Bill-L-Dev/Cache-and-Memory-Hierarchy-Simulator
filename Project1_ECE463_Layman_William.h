#pragma once

#include "sim.h"
#include <cstdint>
#include <vector>

class CACHE {
    private:

    bool VALID_CACHE;
    uint32_t SIZE;
    uint32_t ASSOC;
    uint32_t BLOCKSIZE;
    uint32_t NUM_SETS;
    uint32_t OFFSET_BITS;
    uint32_t INDEX_BITS;
    uint32_t TAG_BITS;
    CACHE* NEXT;
    CACHE* PREV;
    std::vector<std::vector<block>> CACHE_DATA;

    Outputs* OUTPUT;
    uint8_t ID;

    void CalculateNumSets();
    void CreateCacheDataStruct();
    void CalculateAddressFieldSizes();
    bool Search(uint32_t addr, uint32_t& tag, uint32_t& index, uint32_t& way);
    uint32_t DetermineIndex(uint32_t addr);
    uint32_t DetermineTag(uint32_t addr);
    void UpdateLRU(uint32_t index, uint32_t way);
    void SetDirtyBit(uint32_t index, uint32_t way);
    bool CheckSpace(uint32_t index, uint32_t& way);
    void UpdateBlock(uint32_t index, uint32_t way, uint32_t tag);
    uint32_t ConvertToAddr(uint32_t index, uint32_t tag);
    void EvictBlock(uint32_t index, uint32_t& way);

    
    




    public:
    CACHE();
    CACHE(cache_param param, CACHE* prev = nullptr);
    ~CACHE();

    void Read(uint32_t addr);
    void Write(uint32_t addr);

    void AssignOutput(Outputs* OUTPUT);

    void PrintAll();
    void PrintData();
    void PrintMeasurements();
};