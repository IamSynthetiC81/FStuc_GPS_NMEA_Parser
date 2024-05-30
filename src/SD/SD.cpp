#include "SD.h"

#ifdef ENABLE_ERROR_HANDLING
	ErrorRegister ERROR_REGISTER_SD = ErrorRegister();
#endif  // SD_ERROR_H

#include "../general/general.h"

SD::SD(const char* name) {
	Packet nullpackt = Packet();
	for (int i = 0; i < FIFO_DIM; i++) {
		fifo[i] = nullpackt;
	}
	fifoHead = 0;

	strcpy(LOG_FILE_NAME, name);
	if (!createCsvFile(LOG_FILE_NAME)) {
		ERROR(F("Failed to create CSV file"));
		#if ENABLE_ERROR_HANDLING
			ERROR_REGISTER_SD.SET(SD_ERROR_CREATE_CSV_FILE);
		#endif
	}
}

SD::~SD() {
	if (csvFile.isOpen()) {
		csvFile.close();
	}
}

bool SD::logData(Packet data){
	if (fifoHead == FIFO_DIM){
		Write(fifo, FIFO_DIM);
		fifoHead = 0;
	}
	
	fifo[fifoHead++] = data;
}

//------------------------------------------------------------------------------
void SD::Write(Packet *data, unsigned int size) {
  if (!csvFile.isOpen()) {
		ERROR(F("No current CSV file"));
		return;
	}

	int len = 0;

	char *buffer = (char *)malloc(1 * sizeof(char));
	for (int i = 0; i < size; i++) {
		char tmp[124];
		len += data[i].toChar(tmp);

		buffer = (char *)realloc(buffer, len * sizeof(char));
		strcat(buffer, tmp);
		strcat(buffer, "\r\n");

		if (len > 512) {
			ERROR(F("Buffer overflow"));
			#if ENABLE_ERROR_HANDLING
				ERROR_REGISTER_SD.SET(SD_ERROR_BUFFER_OVERFLOW);
			#endif
			return;
		}


	}
}
//------------------------------------------------------------------------------
bool SD::createCsvFile(const char* name) {
	log(F("\t|---Creating file : "));
  char csvName[NAME_DIM];
	strcpy(csvName, name);
	log(csvName);

	if(!csvFile.open(csvName, O_CREAT | O_WRITE | O_EXCL)){
		ERROR("open csv failed");
		#if ENABLE_ERROR_HANDLING
			ERROR_REGISTER_SD.SET(SD_ERROR_CREATE_CSV_FILE);
		#endif
		return EXIT_FAILURE;
	}
  
	logln(F("\t\tPASS"));
  return EXIT_SUCCESS;
}