#include "cs8wagoplugin.h"
#include "cs8ModbusComponent.h"

#include <QtPlugin>
//
cs8WagoPlugin::cs8WagoPlugin ( QObject * parent  )
    : QObject ( parent )
{
  initialized = false;
}
//

void cs8WagoPlugin::initialize ( QDesignerFormEditorInterface * /* core */ )
{
  if ( initialized )
    return;
    
  initialized = true;
}
bool cs8WagoPlugin::isInitialized() const
 {
     return initialized;
 }
QString cs8WagoPlugin::name() const
{
  return "cs8WagoNodeWidget";
}

QString cs8WagoPlugin::group() const
{
  return "CS8 Widgets";
}

QIcon cs8WagoPlugin::icon() const
{
  return QIcon();
}

QString cs8WagoPlugin::toolTip() const
{
  return "";
}


QString cs8WagoPlugin::whatsThis() const
{
  return "";
}

bool cs8WagoPlugin::isContainer() const
{
  return false;
}

QString cs8WagoPlugin::domXml() const
{
  return "<widget class=\"cs8WagoNodeWidget\" name=\"cs8WagoNodeWidget\">\n"
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

 QString cs8WagoPlugin::includeFile() const
 {
     return "cs8ModbusComponent.h";
 } 
 
QWidget * cs8WagoPlugin::createWidget ( QWidget *parent )
{
  return new cs8WagoNodeWidget ( parent );
}

Q_EXPORT_PLUGIN2(modbuswidgetplugin, cs8WagoPlugin)
