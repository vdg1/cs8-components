#include "cs8programcodeviewplugin.h"
#include "cs8ProjectComponent.h"
#include <QtPlugin>
//
cs8ProgramCodeViewPlugin::cs8ProgramCodeViewPlugin( QObject * parent ) 
	: QObject(parent)
{
  initialized = false;
}
//

void cs8ProgramCodeViewPlugin::initialize ( QDesignerFormEditorInterface * /* core */ )
{
  if ( initialized )
    return;
    
  initialized = true;
}
bool cs8ProgramCodeViewPlugin::isInitialized() const
 {
     return initialized;
 }
QString cs8ProgramCodeViewPlugin::name() const
{
  return "cs8ProgramCodeView";
}

QString cs8ProgramCodeViewPlugin::group() const
{
  return "CS8 Widgets";
}

QIcon cs8ProgramCodeViewPlugin::icon() const
{
  return QIcon();
}

QString cs8ProgramCodeViewPlugin::toolTip() const
{
  return "";
}


QString cs8ProgramCodeViewPlugin::whatsThis() const
{
  return "";
}

bool cs8ProgramCodeViewPlugin::isContainer() const
{
  return false;
}

QString cs8ProgramCodeViewPlugin::domXml() const
{
  return "<widget class=\"cs8ProgramCodeView\" name=\"cs8ProgramCodeView\">\n"
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

 QString cs8ProgramCodeViewPlugin::includeFile() const
 {
     return "cs8programcodeview.h";
 } 
 
QWidget * cs8ProgramCodeViewPlugin::createWidget ( QWidget *parent )
{
  return new cs8ProgramCodeView ( parent );
}

	Q_EXPORT_PLUGIN2(progrmadataviewplugin, cs8ProgramCodeViewPlugin)

