#include "mem_manage.h"

// Implementation for the mem_manage
// Author: Weichen
// Date:09/24/2016

namespace Memory_System{
    // initialize static member for mem_analysis_sys
    mem_analysis_sys* mem_analysis_sys::sys_instance_ = nullptr;
    
    // constructors and destructor sfor mem_analysis_sys
    mem_analysis_sys& mem_analysis_sys::getMemAnalysisSys()
    {
        if (sys_instance_ == nullptr){
            sys_instance_ = new mem_analysis_sys();
        }
        return *sys_instance_;
    }
    mem_analysis_sys::mem_analysis_sys()
    {
    }
    mem_analysis_sys::~mem_analysis_sys(){
        pages_.clear();
        if (sys_instance_ != nullptr){
            delete sys_instance_;
        }
    }

    // reset the parameters
    void mem_analysis_sys::reset(){
        page_fault_times_ = 0;
        elapsed_page_fault_times_ = 0;
        last_sampling_time_ = 0;
        last_page_fault_time_ = 0;
        pages_.clear();
        res_mem_size_ = 0;
    }

    void mem_analysis_sys::setPFFParams(int PFF_F){
        this->PFF_F_ = PFF_F;
    }

    void mem_analysis_sys::setVSWSParams(int VSWS_L, int VSWS_Q, int VSWS_M){
        this->VSWS_L_ = VSWS_L;
        this->VSWS_Q_ = VSWS_Q;
        this->VSWS_M_ = VSWS_M;
    }
    
    // shrink resident set
    // 1. discard the pages whose use-bit == 0
    // 2. set remaining pages use-bit == 0
    void mem_analysis_sys::shrinkResidentSet(){
        for (auto &&it:pages_){
            if (it.second.use_bit){
                it.second.use_bit = 0;
            }
            else{
                if (it.second.in_memory){
                    --res_mem_size_;
                    it.second.in_memory = false;
                }
            }
        }
    }

    // simuate the memory access
    // @memAccessSequence: the sequence of mem access by memory address
    // @algo: the algorithm for resident_set_manage
    // return the page fault number in total
    int mem_analysis_sys::memAccess(std::vector<size_t>& memAccessSequence, 
        resident_set_manage_algo algo){
    
        // reset the temp variables;
        reset();

        int curReferenceTime = 0;
        for (const auto memAddr:memAccessSequence){
            switch(algo){
                case (resident_set_manage_algo::PFF):
                    page_fault_times_ += PFF(curReferenceTime++, memAddr);
                    break;
                case (resident_set_manage_algo::VSWS):
                    page_fault_times_ += VSWS(curReferenceTime++, memAddr);
                default:
                    break;
            }
        }
        return page_fault_times_;
    }


    // simulate resident set management by PFF algorithm
    // @ accessAddress: memory page address currently accessing
    // @ curReferenceTime: the reference time when accessing this page
    // return 0 if no page fault happens, 1 if happens
    int mem_analysis_sys::PFF(int curReferenceTime, size_t accessAddress){
        // 1. find whether accessing page is in the memory pool
        auto &&it = pages_.find(accessAddress);
        // 2. if the page is currently in the resident set
        if (it !=  pages_.end()){
            it->second.use_bit = 1;
            if (it->second.in_memory){
                return 0;
            }
            it->second.in_memory = true;
        }
        // 3. add the new page if not in the page pool
        else{
            page cur;
            cur.in_memory = true;
            cur.use_bit = 1;
            ++res_mem_size_;
            pages_[accessAddress] = cur;
        }
        // 4. calculate the refernce gap between two page fault
        int pffGap = curReferenceTime - last_page_fault_time_;
        if (pffGap > PFF_F_){
            shrinkResidentSet();
            // update the last_page_fault_time
            last_page_fault_time_ = curReferenceTime;
        }
        return 1;
    } 

    // simulate resident set management by VSWS algorithm
    // @curReferenceTime: the current reference time
    // @accessAddress: the memory page it currently access
    // return 0 if no page fault occurs, return 1 otherwise
    int mem_analysis_sys::VSWS(int curReferenceTime, size_t accessAddress){
        int occur_page_fault = 0;
        int elapsed_time = curReferenceTime - last_sampling_time_;
        // 1. find whether accessing page is in the memory pool
        auto &&it =  pages_.find(accessAddress);
        // 2. if the page is currently in resident set
        if (it != pages_.end()){
            it->second.use_bit = 1;
            if (!it->second.in_memory){
                occur_page_fault = 1;
                it->second.in_memory = true;
            }
        }
        // 3. add the new if not in the page pool
        else{
            page cur;
            cur.in_memory = true;
            cur.use_bit = 1;
            ++res_mem_size_;
            pages_[accessAddress] = cur;
            occur_page_fault = 1;
        }
        elapsed_page_fault_times_ += occur_page_fault;
        // 4. if elapsed time since last page fault reaches L
        //    or elapsed page fault reaches Q and time reaches M
        //    Suspand and shrink resident size
        if ( elapsed_time >= VSWS_L_
          || ( elapsed_page_fault_times_ >= VSWS_Q_ && elapsed_time >= VSWS_M_) ){
            shrinkResidentSet();
            last_sampling_time_ = curReferenceTime;
            elapsed_page_fault_times_ = 0;
        }
        return occur_page_fault;
    }
}
