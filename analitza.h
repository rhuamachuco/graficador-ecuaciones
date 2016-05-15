#ifndef ANALITZA
#define ANALITZA

#include <iostream>
#include <cmath>
#include <qstring.h>
#include <qlocale.h>
#include <qdom.h>
#include <qfile.h>
#include <qxml.h>
#include <qvaluelist.h>
#include <qtextstream.h>
#include <qmessagebox.h>
#include "qexp.h"
#include "variables.h"

using namespace std;

void print_dom(QDomNode, int ind=0);

class Analitza{
public:

    Analitza(QString path);
    Analitza();
    ~Analitza();

    QDomNode elem;
    static int isOperador(QString);
    int setPath(QString path);
    int setTextMML(QString exp);
    int setText(QString exp);
    QString textMML();
    bool simplify();

    void setVars(Variables v);
    double Calcula();
    QStringList lambda(); //retrieve lambda vars
    double toNum(QDomElement res);

    static bool isNum(QString);
    static QString treu_tags(QString in);
    static QDomElement toCn(double);
    static bool takesQualifiers(QString);
    static QStringList bvar(QDomNode);
    static bool isMathML(QString exp);

    Variables vars;
    QString err;


#endif

