#include <iostream>
#include <fstream>

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>


using namespace std;

#include "device.h"
#include "backlight.h"

#include <string.h>


backlight::backlight(char *path)
{
	min_level = 0;
	max_level = 0;
	start_level = 0;
	strncpy(sysfs_path, path, sizeof(sysfs_path));
}

void backlight::start_measurement(void)
{
	char filename[4096];
	ifstream file;

	sprintf(filename, "%s/max_brightness", sysfs_path);
	file.open(filename, ios::in);
	if (file) {
		file >> max_level;
	}
	file.close();

	sprintf(filename, "%s/actual_brightness", sysfs_path);
	file.open(filename, ios::in);
	if (file) {
		file >> start_level;
	}
	file.close();
}

void backlight::end_measurement(void)
{
	char filename[4096];
	ifstream file;

	sprintf(filename, "%s/actual_brightness", sysfs_path);
	file.open(filename, ios::in);
	if (file) {
		file >> end_level;
	}
	file.close();
}


double backlight::utilization(void)
{
	double p;

	p = 100.0 * (end_level + start_level) / 2 / max_level;
	return p;
}

void create_all_backlights(void)
{
	struct dirent *entry;
	DIR *dir;
	char filename[4096];
	
	dir = opendir("/sys/class/backlight/");
	if (!dir)
		return;
	while (1) {
		class backlight *bl;
		entry = readdir(dir);
		if (!entry)
			break;
		if (entry->d_name[0] == '.')
			continue;
		sprintf(filename, "/sys/class/backlight/%s", entry->d_name);
		bl = new class backlight(filename);
		all_devices.push_back(bl);
	}
	closedir(dir);

}
