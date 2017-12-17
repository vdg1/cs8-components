#include "cs8parameter.h"
//
cs8Parameter::cs8Parameter(QDomElement & element, const QString & description) :
        cs8Variable(element, description) {
    setGlobal(false);
}
//

