#ifndef TOXICERROR_H
#define TOXICERROR_H

#include <QString>

class ToxicError {

public:

    ToxicError(QString);
    QString toxicErrMsg() const;

private:

    QString errMsg;

};

#endif // TOXICERROR_H
