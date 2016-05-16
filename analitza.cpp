#include "analitza.h"

Analitza::Analitza(QString path){
	setPath(path);
	Analitza();
}

Analitza::Analitza(){
	QLocale::setDefault(QLocale::C); err="";
}

Analitza::~Analitza(){}

void Analitza::setVars(Variables v){ vars=v; }

int Analitza::setPath(QString ar)
{
	QDomDocument doc;
	QDomElement docElem;
	QFile file(ar);
	if ( !file.open( IO_ReadOnly ) ){
		err += i18n("Error while parsing: %1<br />\n").arg(ar);
		return -1;
	}
	if ( !doc.setContent( &file ) ) {
		file.close();
		err += i18n("Error while parsing: %1<br/>\n").arg(ar);
		return -2;
	}
	file.close();
	
	docElem = doc.documentElement();
	elem = docElem.firstChild();
	return 0;
}

int Analitza::setTextMML(QString str)
{
	QDomDocument doc;
	
	if ( !doc.setContent(str) ) {
		err += i18n("Error while parsing: %1<br />\n").arg(str);
		return -1;
	}
	elem = doc.documentElement();
	return 0;
}

int Analitza::setText(QString op)
{
	QExp a(op);
	a.parse();
	err="";
	
	if(a.error() != ""){
		err = a.error();
		return -1;	
	}
	mmlexp=a.mathML();
	
	return setTextMML(mmlexp);
}

QString Analitza::textMML()
{
	return mmlexp;
}

bool Analitza::simplify()
{
	/*qDebug("******************");
	print_dom(elem);
	qDebug("******************");
	qDebug("..................");
	print_dom(simp(elem.firstChild()));
	qDebug("..................");
	
	print_dom(elem);*/
	elem.replaceChild(simp(elem.firstChild().cloneNode(true)), elem.firstChild());
// 	print_dom(elem);
	return true;
}
