#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  float MemUt = 0 , MemTotal, MemFree; 
  string line ,key, value, kb; 
  string FilePath = kProcDirectory+kMeminfoFilename ; 
  std::ifstream filestream(FilePath); 
  if(filestream.is_open()){

    while(std::getline(filestream, line)){
      std::replace(line.begin(), line.end(), ':', ' '); 
      std::istringstream linestream(line);
      while(linestream >> key >>value >> kb )
      {
        if (key == "MemTotal"){MemTotal = std::stof(value);}
        else if (key == "MemFree"){MemFree = std::stof(value);}
      }

    }
  }
  

  MemUt = (MemTotal - MemFree)/MemTotal ; 
  
  return MemUt; 
  }

// DONE: Read and return the system uptime
long LinuxParser::UpTime() { 
  long uptime_long; 
  string uptime, idletime , line; 
  std::ifstream filestream (kProcDirectory + kUptimeFilename); 
  if (filestream.is_open()){
    std::getline(filestream, line); 
    std::istringstream linestream (line); 
    linestream >> uptime >> idletime ;
    uptime_long = std::stol(uptime);
  }
  return uptime_long ; 
 }

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  string line, key;
  int value;
  long jiffies = 0 ; 
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open()) {
    std::getline(filestream, line); 
    std::istringstream linestream(line); 
    for (int i = 0; i<9; i++){
      if (i == 0){linestream >> key ;}
      else {
        linestream >> value ; 
        jiffies += value; 
      }
    }
  } 
  return jiffies ; 

 }

// DONE: Read and return the number of active jiffies for a PID

long LinuxParser::ActiveJiffies(int pid) { 
    string value, line ;
  long clock; 
  vector<string> stat_list ; 
  std::ifstream filestream (kProcDirectory + std::to_string(pid) + kStatFilename);
  if( filestream.is_open()){
    std::getline(filestream , line); 
    std::istringstream linestream(line); 
    while(linestream >> value){stat_list.push_back(value);}

  }
  long int utime{std::stol(stat_list[13])};
  long int stime{std::stol(stat_list[14])};
  long int cutime{std::stol(stat_list[15])};
  long int cstime{std::stol(stat_list[16])};

  clock= utime + stime + cutime +cstime ; 
  return clock; 
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {return Jiffies() - IdleJiffies(); }

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  string line, key;
  long idlejeffies = 0 ; 
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open())
  {
    std::getline(filestream, line); 
    std::istringstream linestream(line); 
    int value ;
    for(int i = 0; i<6 ; i++){
      if (i == 0) {linestream >> key;}
      else if (i < 3) {linestream >> value; idlejeffies += value; }
      else {linestream >> value;}
      
    }
    
  }
    return idlejeffies; 
 }

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  string value, key, line ; 
  vector<string> jiffies_list; 
  std::ifstream filestream(kProcDirectory + kStatFilename ); 
  if (filestream.is_open())
  {
    std::getline(filestream, line); 
    std::istringstream linestream(line); 
    while(linestream >> key ) { 
      if (key != "cpu"){jiffies_list.push_back(key);}
    }
  }
  return jiffies_list; 

 }
 // Cpu utilizationfor specific process
 float LinuxParser::CpuUtilProcess(int pid){  

   string line;
  vector<string> columns;
  string column;
  float util{0.0};
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if(stream.is_open()) {
    getline(stream, line);
    std::istringstream linestream(line);
    while(linestream.good()) {
      getline(linestream, column, ' ');
      columns.push_back(column);
    }
    //totalTime = utime + stime
    // with child processes totalTime += cutime + cstime
    int totalProcessTicks = stoi(columns[13]) + stoi(columns[14]) + stoi(columns[15]) + stoi(columns[16]);
    float totalProcessTime = totalProcessTicks / (float)sysconf(_SC_CLK_TCK);
    long totalSeconds = UpTime() - UpTime(pid);
    util = totalSeconds != 0 ? (totalProcessTime/(float)totalSeconds) : 0.0;
  }
  return util;

 }

// DONE: Read and return the total number of processes


int LinuxParser::TotalProcesses() { 
  std::ifstream filestream(kProcDirectory + kStatFilename);
   long totalProcesses = 0;
  if (filestream.is_open()) {
      std::string line;
      bool processNumberFound = false;
      
      while (std::getline(filestream, line) && !processNumberFound) {
        std::istringstream linestream(line);
        std::string key;
        linestream >> key;
        if (key == "processes")
        {          
            linestream >> totalProcesses;
            processNumberFound = true;
        }
      }

  }
  return totalProcesses; 
  }

  // DONE : Read and return the number of running processes
  
int LinuxParser::RunningProcesses() { 
  std::ifstream filestream(kProcDirectory + kStatFilename);
   long runningProcesses = 0;
  if (filestream.is_open()) {
      std::string line;
      bool processNumberFound = false;
      
      while (std::getline(filestream, line) && !processNumberFound) {
        std::istringstream linestream(line);
        std::string key;
        linestream >> key;
        if (key == "procs_running")
        {          
            linestream >> runningProcesses;
            processNumberFound = true;
        }
      }

  }
  return runningProcesses; 
   }


// DONE: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string cmd ; 
  string kPidDirectory = "/" + std::to_string(pid); 
  std::ifstream filestream (kProcDirectory + kPidDirectory +kCmdlineFilename); 
  if (filestream.is_open())
  {
    std::getline(filestream, cmd); 

  }
  return cmd; 
  
 }

// DONE: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  string line, key; 
  long ram ; 
  string kPidDirectory = "/" + std::to_string(pid);
  std::ifstream filestream (kProcDirectory + kPidDirectory +kStatusFilename ); 
  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      std::istringstream linestream(line); 
      linestream >> key ; 
      if (key == "VmSize:"){linestream >> ram; break ; }
    }
    
  }
  return std::to_string(ram/1000);
  
 }

// DONE: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  string line, key, uid; 
  string kPidDirectory = "/" + std::to_string(pid);
  std::ifstream filestream (kProcDirectory + kPidDirectory +kStatusFilename ); 
  if (filestream.is_open()){
    std::getline(filestream, line); 
    std::istringstream linestream(line);
    while (linestream >>key)
    {
      if (key == "Uid:"){linestream >> uid; break ; }
    }
    
  }
  return uid ; 
 }

// DONE: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string line, user, uid, var; 
  std::ifstream filestream (kPasswordPath);
  if(filestream.is_open()){
    while(std::getline(filestream, line)){
      std::replace(line.begin(), line.end(), ':',  ' ');
      std::istringstream linestream(line);
      linestream >>user >> var >> uid ; 
      if(uid == Uid(pid)){break; } 
    }
  }
  return user; 
 }

// DONE: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  string line, var; 
  long start_time = 0, uptime ; 
  vector<string> stat_list; 
  string kPidDirectory = "/" + std::to_string(pid);
  std::ifstream filestream (kProcDirectory + kPidDirectory +kStatFilename ); 
  if (filestream.is_open()){
    std::getline(filestream, line); 
    std::istringstream linestream(line); 
    while (linestream >> var){stat_list.push_back(var);}
    
  }
  start_time = std::stol(stat_list[21]) / sysconf(_SC_CLK_TCK);
  uptime = LinuxParser::UpTime() - start_time; 
  return uptime; 
 }
