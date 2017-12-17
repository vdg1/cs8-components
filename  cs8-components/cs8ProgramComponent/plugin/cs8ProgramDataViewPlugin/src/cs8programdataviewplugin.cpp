#include "cs8programdataviewplugin.h"
#include "cs8ProjectComponent.h"
#include <QtPlugin>
//
cs8ProgramDataViewPlugin::cs8ProgramDataViewPlugin( QObject * parent ) 
	: QObject(parent)
{
  initialized = false;
}
//

void cs8ProgramDataViewPlugin::initialize ( QDesignerFormEditorInterface * /* core */ )
{
  if ( initialized )
    return;
    
  initialized = true;
}
bool cs8ProgramDataViewPlugin::isInitialized() const
 {
     return initialized;
 }
QString cs8ProgramDataViewPlugin::name() const
{
  return "cs8ProgramDataView";
}

QString cs8ProgramDataViewPlugin::group() const
{
  return "CS8 Widgets";
}

QIcon cs8ProgramDataViewPlugin::icon() const
{
  return QIcon();
}

QString cs8ProgramDataViewPlugin::toolTip() const
{
  return "";
}


QString cs8ProgramDataViewPlugin::whatsThis() const
{
  return "";
}

bool cs8ProgramDataViewPlugin::isContainer() const
{
  return false;
}

QString cs8ProgramDataViewPlugin::domXml() const
{
  return "<widget class=\"cs8ProgramDataView\" name=\"cs8ProgramDataView\">\n"
         " <property name=\"geometry\">\n"
         "  <rect>\n"
         "   <x>0</x>\n"
         "   <y>0</y>\n"
         "   <width>465</width>\n"
         "   <height>270</height>\n"
         "  </rect>\n"
         " </property>\n"
         " <property name=\"toolTip\" >\n"
         "  <string>Val3 Program Data View</string>\n"
         " </property>\n"
         " <property name=\"whatsThis\" >\n"
         "  <string></string>\n"
         " </property>\n"
         "</widget>\n";
}

 QString cs8ProgramDataViewPlugin::includeFile() const
 {
     return "cs8programdataview.h";
 } 
 
QWidget * cs8ProgramDataViewPlugin::createWidget ( QWidget *parent )
{
  return new cs8ProgramDataView ( parent );
}

	Q_EXPORT_PLUGIN2(progrmadataviewplugin, cs8ProgramDataViewPlugin)

