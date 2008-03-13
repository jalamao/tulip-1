//-*-c++-*-
/**
 Author: Morgan Mathiaut and Samuel Carruesco, Mickael Melas, Laurent Peyronnet, Michael Roche, Sabrina Wons
 Email : mathiaut@labri.fr
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by  
 the Free Software Foundation; either version 2 of the License, or     
 (at your option) any later version.
*/

#ifndef _Tulip_PLUGINSINFOWIDGET_H_
#define _Tulip_PLUGINSINFOWIDGET_H_


#include <iostream>
#include <QtGui/QTextBrowser>
#include <QtXml/QDomDocument>
#include <QtCore/QFile>

#include "ResponseTreatment.h"
#include "PluginInfo.h"

class QDomElement;

namespace tlp {

  static std::string htmlBegin = "<html><body><h3><center>Plugin informations</h3><br>";
  static std::string htmlEnd = "</html></body>";

  class TLP_PLUGINSMANAGER_SCOPE PluginsInfoWidget : public QTextBrowser{
    
    Q_OBJECT
    
  protected:
    
    void initPIW();
    void addPluginInfoXML(std::string xml); 
    void addPluginDocXML(std::string xml);
    void formatInfo(const std::string &name,const std::string &author,const std::string &type,const std::string &date,const std::string &info,const std::string &version,const std::vector<std::string> &dependencies);

    std::string pluginInfoXML;
    std::string pluginDocXML;
    
    
  public : 
    
    friend class GetPluginInfoTreatment;
    friend class GetPluginDocTreatment;
    
    PluginsInfoWidget(QWidget *parent=0);
    void setPluginInfo(const PluginInfo *pluginInfo, std::string addrWS);
    
    
  private:
    
    std::string formatXMLDoc(QDomElement elt);
    
  };
  
  
  class TLP_PLUGINSMANAGER_SCOPE GetPluginInfoTreatment : public ResponseTreatment {
    
  private:
    PluginsInfoWidget *piw;
    
  public :
    
    GetPluginInfoTreatment(PluginsInfoWidget *p):piw(p){
    }
    
    void operator()(const std::string &data){
      piw->addPluginInfoXML(data); 
      piw->setText((htmlBegin+piw->pluginInfoXML+htmlEnd).c_str());
    }
    
  };

  class TLP_PLUGINSMANAGER_SCOPE GetPluginDocTreatment : public ResponseTreatment {
    
  private:
    PluginsInfoWidget *piw;
    
  public :
    
    GetPluginDocTreatment(PluginsInfoWidget *p):piw(p){
    }
    
    void operator()(const std::string &data){
      piw->addPluginDocXML(data);  
      piw->setText((htmlBegin+piw->pluginInfoXML+piw->pluginDocXML+htmlEnd).c_str());
    }
    
  };

}

#endif //_PLUGINSINFOWIDGET_H_
