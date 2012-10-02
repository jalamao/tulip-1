#ifndef %ProjectName:u%_H
#define %ProjectName:u%_H

#include <iostream>
#include <tulip/TulipPluginHeaders.h>

class %ProjectName:c%: public tlp::ExportModule {
public:
  PLUGININFORMATIONS("%PluginName%", "%Author%", "%Date%", "%Informations%", "%Version%", "%Group%")
%ProjectName:
  c%(tlp::PluginContext* context);
~%ProjectName:
  c%();
  bool exportGraph(std::ostream & fileOut);
  std::string fileExtension() const;
};

#endif // %ProjectName:u%_H

