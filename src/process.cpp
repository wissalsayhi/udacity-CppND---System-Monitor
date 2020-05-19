#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid){
    pid_ =pid ;
    cpu_utilization_ =Process::CpuUtilization();  
    
}
// DONE: Return this process's ID
int Process::Pid() { return pid_; }

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() { 
long active_jiffies = LinuxParser::ActiveJiffies(pid_);
    long system_jiffies = LinuxParser::ActiveJiffies();

    long delta_a = active_jiffies - active_jiffies_;
    long delta_s = system_jiffies - system_jiffies_;

    active_jiffies_ = active_jiffies;
    system_jiffies_ = system_jiffies;

    cpuUtil_ = (float) delta_a / (float) delta_s; 

    return cpuUtil_;
}

// DONE: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// DONE: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid_); }

// DONE: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// DONE: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const{ 
    return cpu_utilization_ > a.cpu_utilization_ ;
     } 
