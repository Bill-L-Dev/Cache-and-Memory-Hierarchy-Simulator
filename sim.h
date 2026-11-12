#ifndef SIM_CACHE_H
#define SIM_CACHE_H

#pragma once
#include <cstdint>

typedef 
struct {
   uint32_t BLOCKSIZE;
   uint32_t L1_SIZE;
   uint32_t L1_ASSOC;
   uint32_t L2_SIZE;
   uint32_t L2_ASSOC;
   uint32_t PREF_N;
   uint32_t PREF_M;
} cache_params_t;

typedef
struct {
   uint32_t BLOCKSIZE;
   uint32_t SIZE;
   uint32_t ASSOC;
} cache_param;

struct block{
   bool VALID = 0;
   bool DIRTY = 0;
   uint32_t LRU = 0;
   uint32_t TAG = 0;
};

struct Outputs{
   uint32_t L1READS = 0;
   uint32_t L1READMISSES = 0;
   uint32_t L1WRITES = 0;
   uint32_t L1WRITEMISSES = 0;
   double L1MISSRATE = 0;
   uint32_t L1NUMWRITEBACKS = 0;
   uint32_t NUML1PREFETCHES = 0;
   uint32_t L2READS = 0;
   uint32_t L2READMISSES = 0;
   uint32_t L2READORIGPREFETCH = 0;
   uint32_t L2READMISSESORIGPREFETCH = 0;
   uint32_t L2WRITES = 0;
   uint32_t L2WRITEMISSES = 0;
   double L2MISSRATE = 0;
   uint32_t L2NUMWRITEBACKS = 0;
   uint32_t L2NUMPREFETCHES = 0;
   uint32_t TOTALMEMTRAFF = 0;
};

// Put additional data structures here as per your requirement.

#endif
