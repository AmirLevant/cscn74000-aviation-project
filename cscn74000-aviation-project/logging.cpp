#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <iomanip>

#define LOG_FILE_MAX_SIZE 1048576  // 1MB double check if this is correct sizing
#define LOG_TIMESTAMP_FORMAT "%Y-%m-%d_%H_%M_%S"
#define LOG_DIR "logs/"
#define BUFFER_SIZE 1024
#define PORT 25603 //port#

using namespace std;

//generating a formatted timestamp for the log file
string getCurrentTimestamp() 
{
	time_t now = time(0);
	tm *ltm = localtime(&now);
	char timestamp[80];
	strftime(timestamp, 80, LOG_TIMESTAMP_FORMAT, ltm);
	
	return string(timestamp);
}

//generating a log file name with the current timestamp
string generateLogFileName() 
{
	return LOG_DIR + getCurrentTimestamp() + ".log";
}

void checkLogRollover(ofstream& logFile, string&  logFileName) 
{
	logFile.seekp(0, ios::end);

	if (logFile.tellp() > LOG_FILE_MAX_SIZE) 
	{
		logFile.close();
		logFileName = generateLogFileName();
		logFile.open(logFileName, ios::app);
	}
}

//writing a message to the log file with timestamp
static void writelog(string message, string log_file_name)
{
	ofstream log_file(log_file_name, ios::app);

	//checkLogRollover(logFile, logFileName);
	log_file << getCurrentTimestamp() << " " << message << endl;
	log_file.close();

	ifstream log_file_size(log_file_name, ios::binary | ios::ate);
	if (log_file_size.tellg() > LOG_FILE_MAX_SIZE) 
	{
		log_file_size.close();
		remove(log_file_name.c_str());
	}
}

