#ifndef VARIABLES_H
#define VARIABLES_H
#include <qvaluelist.h>
#include <qstringlist.h>
#include <qdom.h>

struct VARIABLE{
    QString nom;
    QDomElement valor;
};

class Variables{
    private:
        QValueList<struct VARIABLE> vars;
    public:
        Variables();
        bool add(QString id, QDomElement val);
        bool remove(QString id);
        bool modifica(QString id, QDomElement new_val);
        bool modifica(QString id, double new_val);
        bool modifica(QString id, int new_val);
        bool rename(QString id, QString newId);
        QDomElement* find(QString var);
        QDomElement value(QString id, bool *ex=NULL);
        QStringList getNoms();
        bool isvar(QDomElement e);
};

#endif
