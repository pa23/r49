#include "toxicerror.h"

ToxicError::ToxicError(QString errStr) {

    errMsg = errStr;
}

QString ToxicError::toxicErrMsg() const {

    return errMsg;
}
