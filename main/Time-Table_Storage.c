#include <stdio.h>
#include "esp_spiffs.h"
#include "esp_log.h"
#include <string.h>

// Function to get class name based on day and time
const char* getClassFromTimetable(const char* day, const char* time) {
    static char className[50] = {0}; 


    FILE *fp = fopen("/storage/time-table.csv", "r");
    if (!fp) {
        printf("Can't open timetable file\n");
        return "Error: File not found";
    }

    char line[256];
    int rowIndex = 0;
    int dayRowIndex = -1;
    int timeColIndex = -1; 

    
    while (fgets(line, sizeof(line), fp) && (dayRowIndex == -1 || timeColIndex == -1)) {

        line[strcspn(line, "\n")] = 0;


        char *token;
        char *rest = strdup(line);
        char *temp = rest; 
        int colIndex = 0;

        if (rowIndex == 0) {

            while ((token = strtok_r(rest, ",", &rest))) {
                if (colIndex > 0 && strcmp(token, time) == 0) {
                    timeColIndex = colIndex;
                    break;
                }
                colIndex++;
            }
        } else {

            token = strtok_r(rest, ",", &rest);
            if (token && strcmp(token, day) == 0) {
                dayRowIndex = rowIndex;
            }
        }

        free(temp);
        rowIndex++;
    }


    fseek(fp, 0, SEEK_SET);

    if (dayRowIndex != -1 && timeColIndex != -1) {
        rowIndex = 0;


        while (fgets(line, sizeof(line), fp)) {
            if (rowIndex == dayRowIndex) {

                line[strcspn(line, "\n")] = 0;

                char *token;
                char *rest = strdup(line);
                char *temp = rest;
                int colIndex = 0;

                while ((token = strtok_r(rest, ",", &rest))) {
                    if (colIndex == timeColIndex) {

                        strncpy(className, token, sizeof(className) - 1);
                        className[sizeof(className) - 1] = '\0';
                        free(temp);
                        fclose(fp);
                        return className;
                    }
                    colIndex++;
                }

                free(temp);
                break;
            }
            rowIndex++;
        }
    }

    fclose(fp);


    sprintf(className, "Not found (Day: %s, Time: %s)", day, time);
    return className;
}

static const char* TAG = "FileSystem"; 
void  app_main(void)
{	//to mount the partition to vfs and to check whether the spiffs partition is initialized properly
	esp_vfs_spiffs_conf_t config = {
		.base_path = "/storage",
		.partition_label = NULL,
		.max_files = 5,
		.format_if_mount_failed = true,
	};
	size_t total = 0, used = 0;
	esp_err_t result =   esp_vfs_spiffs_register(&config);
	result = esp_spiffs_info(config.partition_label, &total, &used);

	if(result != ESP_OK){
		ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(result));
	}
	else{
		ESP_LOGI(TAG, "PARTITION SIZE: total: %d, used: %d", total, used);

	}

	const char* day = "MON";
	const char* time = "9:40";
	const char* current_class = getClassFromTimetable(day,time);
	printf("Current Class: %s", current_class);


}



	 

	


					




