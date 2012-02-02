
#include "cs8modbusplugin.h"
#include "cs8ModbusComponent.h"
#include <QtPlugin>
//
cs8ModbusPlugin::cs8ModbusPlugin ( QObject * parent  )
    : QObject ( parent )
{
  initialized = false;
}
//

void cs8ModbusPlugin::initialize ( QDesignerFormEditorInterface * /* core */ )
{
  if ( initialized )
    return;
    
  initialized = true;
}
bool cs8ModbusPlugin::isInitialized() const
 {
     return initialized;
 }
QString cs8ModbusPlugin::name() const
{
  return "cs8ModbusWidget";
}

QString cs8ModbusPlugin::group() const
{
  return "CS8 Widgets";
}

QIcon cs8ModbusPlugin::icon() const
{
  return QIcon();
}

QString cs8ModbusPlugin::toolTip() const
{
  return "";
}


QString cs8ModbusPlugin::whatsThis() const
{
  return "";
}

bool cs8ModbusPlugin::isContainer() const
{
  return false;
}

QString cs8ModbusPlugin::domXml() const
{
  return "<widget class=\"cs8ModbusWidget\" name=\"cs8ModbusWidget\">\n"
         " <property name=\"geometry\">\n"
         "  <rect>\n"
         "   <x>0</x>\n"
         "   <y>0</y>\n"
         "   <width>465</width>\n"
         "   <height>270</height>\n"
         "  </rect>\n"
         " </property>\n"
         " <property name=\"toolTip\" >\n"
         "  <string>Wago Coupler Settings</string>\n"
         " </property>\n"
         " <property name=\"whatsThis\" >\n"
         "  <string></string>\n"
         " </property>\n"
         "</widget>\n";
}

 QString cs8ModbusPlugin::includeFile() const
 {
     return "cs8ModbusComponent.h";
 } 
 
QWidget * cs8ModbusPlugin::createWidget ( QWidget *parent )
{
  return new cs8ModbusWidget ( parent );
}

Q_EXPORT_PLUGIN2(modbuswidgetplugin, cs8ModbusPlugin)