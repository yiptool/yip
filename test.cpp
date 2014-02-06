#include "xcode_project.h"
#include <cstdio>
#include <cstring>
#include <cerrno>

int main()
{
	XCodeProject project;

	project.setUniqueID("A38BD24118A3044000AD1F3F");
	project.setOrganizationName("FriedCroc");

	XCodeGroup * mainGroup = project.addGroup();
	project.setMainGroup(mainGroup);
	mainGroup->setUniqueID("A38BD24018A3044000AD1F3F");

	std::string data = project.toString();
	FILE * f = fopen("project.pbxproj", "wb");
	if (!f)
	{
		fprintf(stderr, "Unable to create file 'project.pbxproj': %s\n", strerror(errno));
		return 1;
	}
	fwrite(data.c_str(), 1, data.length(), f);
	fclose(f);

	return 0;
}
