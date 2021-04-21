#ifndef SHEETLOGGER_INCLUDE
#define SHEETLOGGER_INCLUDE

// Sheet lives here: https://docs.google.com/spreadsheets/d/1s3l9Ynd2_5dL4CWa_6QCBA5sY6KbA35eckuIAtvpB7k/edit#gid=0
// Script lives here: https://script.google.com/u/0/home/projects/1tvJFLoZ03pvSuns1yMmNIlcFfA2VsJvgA9v-KSM4pQuZkNXn6tQ3L-lh/edit
// #define GSCRIPTID in Secrets.h

bool setupSheetLogger();
void logTempHumidityToSheet(float temp, float humidity);

#endif
