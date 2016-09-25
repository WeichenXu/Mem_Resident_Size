#ifndef _mem_analysis_sys_
#define _mem_analysis_sys_
// Author: Weichen Xu
// Date: 09/24/2016
// Transplant memory management from .c to .cpp 

#include <iostream>
#include <vector>   // the sequence of mem access
#include <unordered_map>    // maintain the resident memory set

namespace Memory_System{
    // enum class for resident set management algorithm
    enum class resident_set_manage_algo : int {
        PFF,
        VSWS
    };

    class mem_analysis_sys{
    private:
        // constructor and destructor
        mem_analysis_sys();
        ~mem_analysis_sys();

        static mem_analysis_sys* sys_instance_;

        // parameters for mem_analysis_sys
        size_t page_fault_times_{0};

        // elapsed page fault times for VSWS
        size_t elapsed_page_fault_times_{0};
        int last_sampling_time_{0};
        int VSWS_L_{0};
        int VSWS_Q_{0};
        int VSWS_M_{0};

        // reference time of last page fault for PFF
        int last_page_fault_time_{0};
        int PFF_F_{0};

        // nested class, pages in memory
        struct page{
            bool in_memory{false};
            int use_bit{0};
        };

        // resident memory
        std::unordered_map<size_t, page> pages_;
        size_t res_mem_size_{0};
    public:
        // return the reference to the mem_analysis_sys
        static mem_analysis_sys& getMemAnalysisSys();

        // getter and setter for the parameters
        size_t getPageFaultTimes() const { return this->page_fault_times_; }
        size_t getResidentSize() const { return this->res_mem_size_; }

        //----------------Utility function----------------//
        void shrinkResidentSet();
        int memAccess(std::vector<size_t>& memAccessSequence, 
            resident_set_manage_algo algo);
        void reset();
        void setPFFParams(int PFF_F=0);
        void setVSWSParams(int VSWS_L=0, int VSWS_Q=0, int VSWS_M=0);

        //----------------Perform memory resident algorithm----------//
        // PFF algorithm
        int PFF(int curReferenceTime, size_t accessAddress);
        // VSWS algorithm
        int VSWS(int curReferenceTime, size_t accessAddress);
    };
};
#endif
