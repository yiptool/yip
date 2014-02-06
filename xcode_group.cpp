#include "xcode_group.h"
#include <sstream>

XCodeGroup::XCodeGroup()
	: XCodeObject("PBXGroup"),
	  m_SourceTree("<group>")
{
}

XCodeGroup::~XCodeGroup()
{
}

std::string XCodeGroup::toString() const
{
	std::stringstream ss;
	ss << "\t\t" << uniqueID().toString() << " = {\n";
	ss << "\t\t\tisa = " << className() << ";\n";
	ss << "\t\t\tchildren = (\n";
//				A38BD25218A3044000AD1F3F /* PRODPROD */,
//				A38BD26B18A3044000AD1F3F /* PRODPRODTests */,
//				A38BD24B18A3044000AD1F3F /* Frameworks */,
//				A38BD24A18A3044000AD1F3F /* Products */,
	ss << "\t\t\t);\n";
	if (m_Name.length() > 0)
		ss << "\t\t\tname = " << stringLiteral(m_Name) << ";\n";
	if (m_Path.length() > 0)
		ss << "\t\t\tpath = " << stringLiteral(m_Path) << ";\n";
	ss << "\t\t\tsourceTree = " << stringLiteral(m_SourceTree) << ";\n";
	ss << "\t\t};\n";
	return ss.str();
}
