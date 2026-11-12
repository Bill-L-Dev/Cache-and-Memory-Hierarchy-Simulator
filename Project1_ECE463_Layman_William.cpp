#include <iostream>
#include "Project1_ECE463_Layman_William.h"
//#include <cstdint>
//#include <vector>
#include <cmath>
#include <iomanip>
//#include <sim.h>

using std::cout;
using std::endl;
using std::vector;

#define ADDRESS_SIZE 32

/* ECE463 Project 1                                                     */
/* Author: William Layman                                               */
/* Abstract: implementation of a flexible cache system                  */
/* File Name: Project1_ECE463_Layman_William.cpp                        */

//Notes:
// - simulator must be capable of modeling one or more instances of CACHE to form an overall memory hierarchy
//      -Therefore needs to be able to have a pointer to a CACHE object and be able to call the functions of the CACHE class
// 
// - CACHE receives a read or write request fom whatever is above it in the memory hierarchy.
//    - The only situation where CACHE must interact with the next level below it is when the read or write request misses in cache
//    - when missed, the cache must allocate the requeseted block so that the read or write can be perfromed
//    - note: good description of the process in project description
//
// - After read or write request, always update LRU properly, as well as the associated dirty bit if the request was a write
//
// - For this project
//    - 463 will study two memory hierarchy configurations, single L1, L1 and L2
//    - all CACHEs will have the same block size
//
// - Traces
//    - Format r|w <hex address> ex: r ffe04540
//
// - Command-line arguments to the simulator
//    - sim <BLOCKSIZE> <L1_SIZE> <L1_ASSOC> <L2_SIZE> <L2_ASSOC> <PREF_N> <PREF_M> <trace_file>
//    - must still parse the prefetching-related arguments, gradescope will always specify "0 0"
//
// - Outputs from Simulator
//    - 1. Memory hierarchy configuration and tracefile name
//    - 2. The final contents of all caches: blocks within a set must be printed in order from most recently used to Least recently used, do not print invalid blocks within a set, If a given set has no valid blocks do not print that set
//    - 3. the following measurements:
//          a. number of L1 reads
//          b. number of L1 read misses
//          c. number of L1 writes
//          d. number of L1 write misses
//          e. L1 miss rate = MR_L1 = (L1 read misses + L1 write misses)/L1 reads + L1 writes)
//          f. number of writebacks from L1 to next level
//          g. number of L1 prefetches (if prefetch unit is enabled)(print 0)
//          h. number of L2 reads that did not originate from L1 to next level, if prefetch unit is enabled
//          i. number of L2 read misses thta did not originate from L1 prefetches, excluding such L2 read misses that hit in the stream buffers if L2 prefetch unit is enabled
//          j. number of L2 reads that originated from L1 prefetches (print 0)
//          k. number of L2 read misses that originated from L1 prefetches (print 0)
//          l. number of L2 writes
//          m. number of L2 write misses, excluding L2 write misses that hit in the stream buffers if L2 prefeth unit is enabled
//          n. L2 miss rate (from sandpoint of stalling the CPU) = MR_L2 = (L2 read misses + L2 write misses)/(L2 reads + L2 writes)
//          o. number of writebacks from L2 to next level
//          p. number of L2 prefetches (if prefetch unit is enabled)(print 0)
//          q. total memory traffic = number of blocks transferred to/from memory




//Class Defining a Cache

//Notes: 
// -
//class CACHE {

    //SIZE: Total bytes of data storage
    //ASSOC: the associativity of the cache/number of ways
    //BLOCKSIZE: the number of byes in a block of data (power of 2 and two)
    // the number of sets is a power of 2 and is equal to SIZE/(ASSOC*BLOCKSIZE)
    // ASSOC neeed not be power of 2
    // #sets = SIZE/(ASSOC*BLOCKSIZE)

    //Replacement Policy: LRU
    //Write Policy: Write Back Write Allocate
    //private:
    //private variables

        // control variables
        //bool VALID_CACHE;

        // Cache params
        //uint32_t SIZE;
        //uint32_t ASSOC;
        //uint32_t BLOCKSIZE;
        //uint32_t NUM_SETS;

        // Address Field Variables
        //uint8_t OFFSET_BITS;
        //uint8_t INDEX_BITS;
        //uint8_t TAG_BITS;

        // cache hierarchy
        //CACHE* NEXT;
        //CACHE* PREV;

        // Data handling struct
        //vector<vector<block>> CACHE_DATA;

    // Private/Helper Functions

        //function that calculate the caches ammount of total sets
        void CACHE::CalculateNumSets(){
            NUM_SETS = SIZE / (ASSOC * BLOCKSIZE);

            return;
        }

        //function that creates the structure for data unique to the cache parameters
        void CACHE::CreateCacheDataStruct(){

            CACHE_DATA.resize(NUM_SETS); 

    //        if (ASSOC > 1){ //check if column resizing is needed
                for(uint32_t i = 0; i < NUM_SETS; i++){
                    CACHE_DATA[i].resize(ASSOC);
                }

                for(uint32_t f = 1; f < ASSOC; f++){ //setting LRU
                    for(uint32_t j = 0; j < NUM_SETS; j++){
                        CACHE_DATA[j][f].LRU = f;
                    }
                }
    //        }

            return;
        }

        //funtion that calculates the number of bits for parsing instruction adresses
        void CACHE::CalculateAddressFieldSizes(){

            OFFSET_BITS = std::log2(BLOCKSIZE);
            INDEX_BITS =  std::log2(NUM_SETS);
            TAG_BITS = ADDRESS_SIZE - (OFFSET_BITS + INDEX_BITS);

            return;
        }

        //function created to search the cache for a given address and return true or false
        bool CACHE::Search(uint32_t addr, uint32_t& tag, uint32_t& index, uint32_t& way){
            index = DetermineIndex(addr);
            tag = DetermineTag(addr);

           // cout << "determined tag: " << std::hex << tag << " determined index: " << index << std::dec << endl;
           // cout << "addr: " << std::hex << addr << std::dec << endl;

            for(uint32_t i = 0; i < ASSOC; i++){
                if(CACHE_DATA[index][i].VALID && CACHE_DATA[index][i].TAG == tag){
                    way = i;
                    return true;
                }
            }
            return false;
        }

        //function to determine the proper index from a given address
        uint32_t CACHE::DetermineIndex(uint32_t addr){
            uint32_t index;
            uint32_t index_mask = (1u << INDEX_BITS) - 1;
            
            index = (addr >> OFFSET_BITS) & index_mask;

            return index;

        }

        //function to determine proper tag from a given address
        uint32_t CACHE::DetermineTag(uint32_t addr){
            uint32_t tag;
            uint32_t tag_mask = (1u << TAG_BITS) - 1;

            tag = (addr >> (OFFSET_BITS + INDEX_BITS)) & tag_mask;

            return tag;

        }

        //function for updating LRU in proper ways of an index
        void CACHE::UpdateLRU(uint32_t index, uint32_t way){

            uint32_t lru_prev_val = CACHE_DATA[index][way].LRU;

            for(uint32_t i = 0; i < ASSOC; i++){
                if(CACHE_DATA[index][i].LRU < lru_prev_val){
                    CACHE_DATA[index][i].LRU++;
                }
            }

            
            CACHE_DATA[index][way].LRU = 0;

            return;

        }

        //simple function for setting dirty bit when writing data
        void CACHE::SetDirtyBit(uint32_t index, uint32_t way){

            CACHE_DATA[index][way].DIRTY = true; //possible put if statement and bool argument to make function flexible (update function desc)


        }

        // function for checking for an invalid block within an index
        bool CACHE::CheckSpace(uint32_t index, uint32_t& way){
            for(uint32_t i = 0; i < ASSOC; i++){
                if(CACHE_DATA[index][i].VALID == false){
                    way = i;
                    return true;
                }
            }
            return false;
        }

        //function used to update a specific block with a new tag within a specific way and index, also calls for update of LRU;
        void CACHE::UpdateBlock(uint32_t index, uint32_t way, uint32_t tag){
            CACHE_DATA[index][way].TAG = tag;

            if(CACHE_DATA[index][way].VALID == false){
                CACHE_DATA[index][way].VALID = true;
            }

            UpdateLRU(index,way);

            return;
        }

        //function for converting a tag and index into a full 32 bit address for translation in a lower level cache
        uint32_t CACHE::ConvertToAddr(uint32_t index, uint32_t tag){
            uint32_t addr = 0;

            tag = tag << (ADDRESS_SIZE - TAG_BITS);
            index = index << OFFSET_BITS;

            addr |= (tag | index);

            return addr;

        }

        //function for determinining the proper block to evict and sending a read request if block is dirty
        void CACHE::EvictBlock(uint32_t index, uint32_t& way){

            uint32_t addr;

            for(uint32_t i = 0; i < ASSOC; i++){
                if (CACHE_DATA[index][i].LRU == (ASSOC - 1)){
                    
                    way = i;
                    
                    if (CACHE_DATA[index][i].DIRTY == true){
                        addr = ConvertToAddr(index, CACHE_DATA[index][i].TAG);
                        if(NEXT != nullptr) NEXT->Write(addr);

                        CACHE_DATA[index][i].DIRTY = false;

                        if(ID==0){
                            OUTPUT->L1NUMWRITEBACKS++;
                            if(NEXT == nullptr) OUTPUT->TOTALMEMTRAFF++;
                        }
                        else{
                            OUTPUT->L2NUMWRITEBACKS++;
                            OUTPUT->TOTALMEMTRAFF++;
                        }

                    }

                    return;

                }
                
            }

            //return;
            cout << "evict block error: something to do with invalid LRU values" << endl;
            for (uint32_t m = 0; m < ASSOC; m++){
                cout << CACHE_DATA[index][m].LRU << endl;;
            }
            

        }

    //public:

        //main functions

        //read initiated to cache; handles hit and miss cases accordingly
        void CACHE::Read(uint32_t addr){
            uint32_t index;
            uint32_t tag;
            uint32_t way;

            if(ID == 0) OUTPUT->L1READS++;
            else OUTPUT->L2READS++;

            bool hit = Search(addr, tag, index, way);

            if(hit){
                UpdateLRU(index, way);
            }
            else{
                bool space_avail = CheckSpace(index, way);

                if(ID==0){
                    OUTPUT->L1READMISSES++;
                    if(NEXT == nullptr) OUTPUT->TOTALMEMTRAFF++;
                }
                else{
                    OUTPUT->L2READMISSES++;
                    OUTPUT->TOTALMEMTRAFF++;

                }

                if(space_avail){
                    if(NEXT != nullptr) NEXT->Read(addr);

                    UpdateBlock(index, way, tag);

                }
                else{
                    EvictBlock(index, way);
                    if(NEXT != nullptr) NEXT->Read(addr);
                    UpdateBlock(index, way, tag);

                }

            }

            return;
        }

        //handles Write requests to respective cache, both hit and miss cases
        void CACHE::Write(uint32_t addr){
            uint32_t index;
            uint32_t tag;
            uint32_t way;

            if(ID == 0) OUTPUT->L1WRITES++;
            else OUTPUT->L2WRITES++;
            
            bool hit = Search(addr, tag, index, way);

            if(hit){
                SetDirtyBit(index, way);
                UpdateLRU(index, way);
            }
            else{
                bool space_avail = CheckSpace(index, way);

                if(ID==0){
                    OUTPUT->L1WRITEMISSES++;
                    if(NEXT == nullptr) OUTPUT->TOTALMEMTRAFF++;
                }
                else{
                    OUTPUT->L2WRITEMISSES++;
                    OUTPUT->TOTALMEMTRAFF++;
                }

                if(space_avail){
                    if(NEXT != nullptr) NEXT->Read(addr);

                    UpdateBlock(index, way, tag);
                    SetDirtyBit(index, way);
                }
                else{
                    EvictBlock(index, way);
                    if(NEXT != nullptr) NEXT->Read(addr);
                    UpdateBlock(index, way, tag);
                    SetDirtyBit(index,way);
                }

            }

            return;

        }

        //helpers

        //function for assigning an output struct;
        void CACHE::AssignOutput(Outputs* output){
            OUTPUT = output;
            if(NEXT != nullptr) NEXT->OUTPUT = output;
        }

        //function for helping me determine proper structure in early stages of coding
        void CACHE::PrintAll(){
            cout << "Cache Parameters:" << endl;
            cout << "SIZE: " << SIZE << endl;
            cout << "ASSOC: " << ASSOC << endl;
            cout << "BLOCKSIZE: " << BLOCKSIZE << endl;
            cout << "NUM_SETS: " << NUM_SETS << endl;

            cout << "Cache Data Structure:" << endl;
            for (uint32_t i = 0; i < NUM_SETS; i++){
                cout << "Set " << i << ": ";
                for (uint32_t j = 0; j < ASSOC; j++){
                    cout << "[VALID: " << CACHE_DATA[i][j].VALID << ", DIRTY: " << CACHE_DATA[i][j].DIRTY << ", TAG: " << CACHE_DATA[i][j].TAG << ", LRU: " << CACHE_DATA[i][j].LRU << "] ";
                }
                cout << endl;
            }
            
        } 
        
        void CACHE::PrintData(){
            cout << "===== L1 contents =====" << endl;
            for(uint32_t i = 0; i < NUM_SETS; i++){
                cout << "set " << i << ": ";

                
                for(uint32_t f = 0; f < ASSOC; f++){
                    for(uint32_t  g = 0; g < ASSOC; g++){
                        if(CACHE_DATA[i][g].LRU == f && CACHE_DATA[i][g].VALID == true){
                            cout << std::hex << CACHE_DATA[i][g].TAG << std::dec << " ";
                            //cout << "LRU: " << CACHE_DATA[i][g].LRU << " "; //debugging

                            if(CACHE_DATA[i][g].DIRTY == true) cout << "D ";
                            else cout << "  ";

                            break;
                        }
                    }

                }


                cout << endl;
            }


            if(NEXT != nullptr){
                cout << endl;
                cout << "===== L2 contents =====" << endl;
                for(uint32_t i = 0; i < NEXT->NUM_SETS; i++){
                    cout << "set " << i << ": ";



                    for(uint32_t f = 0; f < NEXT->ASSOC; f++){
                        for(uint32_t  g = 0; g < NEXT->ASSOC; g++){
                            if(NEXT->CACHE_DATA[i][g].LRU == f && NEXT->CACHE_DATA[i][g].VALID == true){
                                cout << std::hex << NEXT->CACHE_DATA[i][g].TAG << std::dec << " ";
                               // cout << "LRU: " << NEXT->CACHE_DATA[i][g].LRU << " "; //debugging

                                if(NEXT->CACHE_DATA[i][g].DIRTY == true) cout << "D ";
                                else cout << "  ";

                                break;
                            }
                        }
                    }
                    cout << endl;
                }                

            }
        }

        void CACHE::PrintMeasurements(){
            OUTPUT->L1MISSRATE = static_cast<double>(OUTPUT->L1READMISSES + OUTPUT->L1WRITEMISSES)/(OUTPUT->L1READS + OUTPUT->L1WRITES);
            OUTPUT->L2MISSRATE = static_cast<double>(OUTPUT->L2READMISSES)/(OUTPUT->L1READMISSES + OUTPUT->L1WRITEMISSES);

            if(std::isnan(OUTPUT->L2MISSRATE)){
                OUTPUT->L2MISSRATE = 0;
            }

            cout << endl;
            cout << "===== Measurements =====" << endl;
            cout << "a. L1 reads: " << OUTPUT->L1READS << endl;
            cout << "b. L1 read misses: " << OUTPUT->L1READMISSES << endl;
            cout << "c. L1 writes: " << OUTPUT->L1WRITES << endl;
            cout << "d. L1 write misses: " << OUTPUT->L1WRITEMISSES << endl;
            cout << "e. L1 miss rate: " << std::fixed << std::setprecision(4) << OUTPUT->L1MISSRATE << endl;
            cout << "f. L1 writebacks: " << OUTPUT->L1NUMWRITEBACKS << endl;
            cout << "g. L1 prefetches: " << OUTPUT->NUML1PREFETCHES << endl;
            cout << "h. L2 reads (demand): " << OUTPUT->L2READS << endl;
            cout << "i. L2 read misses (demand): " << OUTPUT->L2READMISSES << endl;
            cout << "j. L2 reads (prefetch): " << OUTPUT->L2READORIGPREFETCH << endl;
            cout << "k. L2 read misses (prefetch): " << OUTPUT->L2READMISSESORIGPREFETCH << endl;
            cout << "l. L2 writes: " << OUTPUT->L2WRITES << endl;
            cout << "m. L2 write misses: " << OUTPUT->L2WRITEMISSES << endl;
            cout << "n. L2 miss rate: " << std::fixed << std::setprecision(4) << OUTPUT->L2MISSRATE << endl;
            cout << "o. L2 writebacks: " << OUTPUT->L2NUMWRITEBACKS << endl;
            cout << "p. L2 prefetches: " << OUTPUT->L2NUMPREFETCHES << endl;
            cout << "q. memory traffic: " << OUTPUT->TOTALMEMTRAFF << endl;

        }

        //default constructor
        CACHE::CACHE(){
            SIZE = ASSOC = BLOCKSIZE = NUM_SETS = 0;
            NEXT = nullptr;
            VALID_CACHE = false;
        }

        //constructor
        CACHE::CACHE(cache_param param, CACHE* prev) :  SIZE(param.SIZE), ASSOC(param.ASSOC), BLOCKSIZE(param.BLOCKSIZE)
        {
            CalculateNumSets();
            CalculateAddressFieldSizes();
            CreateCacheDataStruct();

            PREV = nullptr;
            NEXT = nullptr;

            ID = 0;

            if (prev != nullptr){ 
                PREV = prev; 
                ID = 1;
                PREV->NEXT = this;
            }
            
            VALID_CACHE = true;
        }

        //deconstructor
        CACHE::~CACHE(){

        }




//};


//void main(){
//
//    cache_param test;
//    test.BLOCKSIZE = 16;
//    test.SIZE = 64;
//    test.ASSOC = 2;
//
//    CACHE* head = new CACHE(test);
//    head->PrintAll();
//}