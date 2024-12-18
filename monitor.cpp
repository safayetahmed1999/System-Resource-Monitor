#include <stdio.h>
#include <windows.h>
#include "OpenHardwareMonitorLib.h"  // Include OpenHardwareMonitor header

// Function to get CPU usage
void get_cpu_usage() {
    static FILETIME prev_idle_time = {0, 0};
    static FILETIME prev_kernel_time = {0, 0};
    static FILETIME prev_user_time = {0, 0};

    FILETIME idle_time, kernel_time, user_time;

    if (GetSystemTimes(&idle_time, &kernel_time, &user_time)) {
        ULONGLONG idle = ((ULONGLONG)&idle_time - (ULONGLONG)&prev_idle_time);
        ULONGLONG kernel = ((ULONGLONG)&kernel_time - (ULONGLONG)&prev_kernel_time);
        ULONGLONG user = ((ULONGLONG)&user_time - (ULONGLONG)&prev_user_time);

        ULONGLONG total = kernel + user;

        if (total > 0) {
            printf("CPU Usage: %.2f%%\n", (1.0 - (double)idle / total) * 100.0);
        }

        prev_idle_time = idle_time;
        prev_kernel_time = kernel_time;
        prev_user_time = user_time;
    } else {
        printf("Error retrieving CPU usage.\n");
    }
}

// Function to get memory usage
void get_memory_usage() {
    MEMORYSTATUSEX mem_status;
    mem_status.dwLength = sizeof(mem_status);

    if (GlobalMemoryStatusEx(&mem_status)) {
        printf("Memory Usage: %.2f%%\n", mem_status.dwMemoryLoad);
    } else {
        printf("Error retrieving memory usage.\n");
    }
}

// Function to get disk usage
void get_disk_usage() {
    ULARGE_INTEGER free_bytes_available, total_bytes, total_free_bytes;

    if (GetDiskFreeSpaceEx("C:\\", &free_bytes_available, &total_bytes, &total_free_bytes)) {
        double used_space = (double)(total_bytes.QuadPart - total_free_bytes.QuadPart);
        double total_space = (double)total_bytes.QuadPart;
        printf("Disk Usage (C:\\): %.2f%% used\n", (used_space / total_space) * 100.0);
    } else {
        printf("Error retrieving disk usage.\n");
    }
}

// Function to get CPU temperature using OpenHardwareMonitor
void get_cpu_temperature() {
    Computer^ computer = gcnew Computer();
    computer->CPUEnabled = true;  // Enable CPU sensor

    computer->Open();

    for (int i = 0; i < computer->Hardware->Count; i++) {
        if (computer->Hardware[i]->HardwareType == HardwareType::CPU) {
            for (int j = 0; j < computer->Hardware[i]->Sensors->Count; j++) {
                if (computer->Hardware[i]->Sensors[j]->SensorType == SensorType::Temperature) {
                    printf("CPU Temperature: %.2f°C\n", computer->Hardware[i]->Sensors[j]->Value);
                }
            }
        }
    }

    computer->Close();
}

int main() {
    while (1) {
        printf("\n--- System Resource Monitor ---\n");
        get_cpu_usage();
        get_memory_usage();
        get_disk_usage();
        get_cpu_temperature();  // Call the temperature function
        printf("---------------------------------\n");

        Sleep(5000); // Refresh every 5 seconds
    }

    return 0;
}
