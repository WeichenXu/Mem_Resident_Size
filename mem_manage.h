#ifndef _mem_analysis_sys_
#define _mem_analysis_sys_
// Author: Weichen Xu
// Date: 09/24/2016
// Transplant memory management from .c to .cpp 

#include <iostream>
#include <std.h>
#include <unordered_map>

namespace Memory_System{
    // enum class for resident set management algorithm
    enum class resident_set_manage_algo : int {
        PFF,
        VSWS
    };

    class mem_analysis_sys{
    public:
        // return the reference to the mem_analysis_sys
        static mem_analysis_sys& getMemAnalysisSys();

        // getter and setter for the parameters
        size_t getPageNum() const { return this->page_num_; }
        size_t getMemAccTimes() const { return this->mem_acc_times_; }
        size_t getPageFaultTimes() const { return this->page_fault_times_; }
        size_t getResidentSize() const { return this->res_mem_size_; }

        //----------------Utility function----------------//
        void shrinkResidentSize();
        void memAccess();    

        //----------------Perform memory resident algorithm----------//
        // PFF algorithm
        void PFF(int curReferenceTime, size_t accessAddress);
        // VSWS algorithm
        void VSWS(int curReferenceTime, size_t accessAddress);

    private:
        // constructor and destructor
        mem_analysis_sys(int mem_access_times = 0);
        ~mem_analysis_sys();

        static mem_analysis_sys* sys_instance_;
        // use pimpl pattern

        parameters* params_(nullptr);
        // parameters for mem_analysis_sys
        size_t page_num_(0);
        size_t mem_acc_times_(0);
        size_t page_fault_times_(0);
        // elapsed page fault times for VSWS
        size_t elapsed_page_fault_times_(0);
        int last_sampling_time_(0);
        // reference time of last page fault for PFF
        int last_page_fault_time_(0);

        // nested class, pages in memory
        struct page{
            bool in_memory(false);
            int use_bit(0);
        }

        // resident memory
        std::unordered_map<page> pages_;
        size_t res_mem_size_(0);
    };
}
#endif
