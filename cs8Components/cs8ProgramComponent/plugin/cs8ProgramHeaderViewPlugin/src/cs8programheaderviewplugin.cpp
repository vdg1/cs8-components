#include "cs8programheaderviewplugin.h"
#include "cs8ProjectComponent.h"
#include <QtPlugin>
//
cs8ProgramHeaderViewPlugin::cs8ProgramHeaderViewPlugin( QObject * parent ) 
	: QObject(parent)
{
  initialized = false;
}
//

void cs8ProgramHeaderViewPlugin::initialize ( QDesignerFormEditorInterface * /* core */ )
{
  if ( initialized )
    return;
    
  initialized = true;
}
bool cs8ProgramHeaderViewPlugin::isInitialized() const
 {
     return initialized;
 }
QString cs8ProgramHeaderViewPlugin::name() const
{
  return "cs8ProgramHeaderView";
}

QString cs8ProgramHeaderViewPlugin::group() const
{
  return "CS8 Widgets";
}

QIcon cs8ProgramHeaderViewPlugin::icon() const
{
  return QIcon();
}

QString cs8ProgramHeaderViewPlugin::toolTip() const
{
  return "";
}


QString cs8ProgramHeaderViewPlugin::whatsThis() const
{
  return "";
}

bool cs8ProgramHeaderViewPlugin::isContainer() const
{
  return false;
}

QString cs8ProgramHeaderViewPlugin::domXml() const
{
  return "<widget class=\"cs8ProgramHeaderView\" name=\"cs8ProgramHeaderView\">\n"
         " <property name=\"geometry\">\n"
         "  <rect>\n"
         "   <x>0</x>\n"
         "   <y>0</y>\n"
         "   <width>465</width>\n"
         "   <height>270</height>\n"
         "  </rect>\n"
         " </property>\n"
         " <property name=\"toolTip\" >\n"
         "  <string>Val3 Program Header View</string>\n"
         " </property>\n"
         " <property name=\"whatsThis\" >\n"
         "  <string></string>\n"
         " </property>\n"
         "</widget>\n";
}

 QString cs8ProgramHeaderViewPlugin::includeFile() const
 {
     return "cs8programheaderview.h";
 } 
 
QWidget * cs8ProgramHeaderViewPlugin::createWidget ( QWidget *parent )
{
  return new cs8ProgramHeaderView ( parent );
}

	Q_EXPORT_PLUGIN2(progrmaheaderviewplugin, cs8ProgramHeaderViewPlugin)

