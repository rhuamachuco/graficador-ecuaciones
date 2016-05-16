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

bool hasVars(QDomNode n) {
	bool r=false;
	if(n.toElement().tagName()=="ci")
		r = true;
	else if(!n.hasChildNodes())
		r = false;
	else {
		QDomNodeList nl=n.childNodes();
		for(unsigned int i=0; i<nl.length(); i++) {
			if(nl.item(i).isElement())
				r |= hasVars(nl.item(i));
		}
	}
	return r;
}

QDomNode Analitza::simp(QDomNode n){
	QDomNode k;
	
	if(!hasVars(n))
		return toCn(evalua(n));
	
	
	QDomNodeList nl=n.childNodes();
	for(unsigned int i=0; i<nl.length(); i++) {
		k=nl.item(i);
		if(k.isElement() && k.hasChildNodes()){
// 			if(n.replaceChild(simp(k), k).isNull())
// 				qDebug("mec mec");
			n.replaceChild(simp(k), k);
		}
	}
	return n;
}

double Analitza::Calcula() {
// 	QDomNodeList elems = elem.toElement().elementsByTagName(QString("apply"));
// 	return evalua(elems.item(0));
	err="";
	return evalua(elem.firstChild());
}

double Analitza::evalua(QDomNode n){
	QDomNode j, n_ini=n;
	QDomElement e;
	QString operador;
	QValueList<double> nombres;
	double ret=.0;
	int fills=0;
	int sons=0;
	
	while( !n.isNull() ) {
		e = n.toElement();
		
		if(e.tagName() == "apply" || e.tagName() == "lambda"){
			j = e.firstChild();
			nombres.append(evalua(j));
		} else if (e.tagName() == "cn" || (e.tagName() == "ci" && e.attribute("type")!="function") || isNum(e.tagName())) {
			nombres.append(toNum(e));
		} else if (e.tagName() == "declare"){
			j = e.firstChild(); //Should be a <ci>
			QString s = j.toElement().text();
			j = j.nextSibling();
			vars.modifica(s, j.toElement());
			//ret = evalua(j);
		} else if(isOperador(e.tagName())) {
			operador = e.tagName();
			sons=isOperador(operador);
		} else if(e.tagName() == "ci" && e.attribute("type")=="function") {
			operador="ci";
			break;
		} else if (e.tagName() != "bvar")
			err += i18n("The operator <em>%1</em> hasn't been implemented<br />\n").arg(e.tagName());
		if(e.tagName() == "sum" || e.tagName() == "product") //we treat it outside the loop
			break;
		fills++;
		n = n.nextSibling();
	}
	
	if(operador=="sum") ret=sum(n);
	else if(operador=="ci") ret=func(n);
	else if(fills-1==sons || (sons==-1 && fills>=3) || operador.isEmpty() || operador == "minus"){
		QValueList<double>::iterator it = nombres.begin();
		
		ret = *it;
		
		if(fills>2)
			it++;
		for(; it != nombres.end(); ++it)
			ret = opera(ret,*it,operador, fills<=2?1:0);
	} else {
		if(sons==-1)
			err += i18n("The <em>%1</em> operator, should have more than 1 parameter<p />").arg(operador);
		else
			err += i18n("Can't have %1 parameter with <em>%2</em> operator, it should have %3 parameter<p />").arg(fills-1).arg(operador).arg(sons);
	}
	return ret;
}

double Analitza::func(QDomNode n)
{
	double ret=.0;
	bool ex=true;
	QDomNodeList params = n.parentNode().childNodes();
	QString funcname = params.item(0).toElement().text();
	
	QDomElement funcop = vars.value(funcname, &ex);
	if(ex) {
		err += i18n("The function <em>%1</em> doesn't exist<br />\n").arg(funcname);
		return 0;
	}
	
	QStringList var=bvar(funcop);
	for(unsigned int i=0; i<var.count(); i++){
		vars.rename(var[i], QString("%1_").arg(var[i])); //We save the var value
		vars.modifica(var[i], params.item(i+1).toElement());
	}
	
	ret=evalua(funcop);
	
	for(unsigned int i=0; i<var.count(); i++) {
		vars.remove(var[i]);
		vars.rename(QString("%1_").arg(var[i]), var[i]); //We save the var value
	}
	return ret;
}

double Analitza::sum(QDomNode n)
{
	double ret=.0;
	QString var=bvar(n.parentNode())[0];
	vars.rename(var, QString("%1_").arg(var)); //We save the var value
	double ul=toNum(uplimit(n.parentNode()).toElement());
	double dl=toNum(downlimit(n.parentNode()).toElement());
	
	for(double a=dl; a<ul; a+=1.){
		vars.modifica(var, a);
		ret=opera(ret, evalua(first_val(n.parentNode())),"plus",1);
	}
	vars.remove(var);
	vars.rename(QString("%1_").arg(var), var); //We restore the var value
	return ret;
}


double Analitza::opera(double res, double oper, QString op, int minus=0){
	double a=res, b=oper;
	bool boolean=false;
	
	if(op == "plus"){
		a += b;
	}else if(op =="times"){
		a *= b;
	}else if(op=="divide"){
		a /=b;
	}else if(op=="minus"){
		a = minus ? -a : a-b;
	}else if(op=="power"){
		a = pow(a, b);
	}else if(op=="rem"){
		a = (int)a % (int)b;
	}else if(op=="quotient"){
		a = floor(a / b);
	}else if(op=="factorof"){
		a = (((int)a % (int)b)==0) ? 1.0 : 0.0;
		boolean = true;
	}else if(op=="factorial"){
		b = a;
		for(a=1; b>1; b--)
			a*=b;
	} else if(op=="sin"){
		a=sin(a);
	} else if(op=="cos"){
		a=cos(a);
	} else if(op=="tan"){
		a=tan(a);
	} else if(op=="sec"){
		a=1/cos(a);
	} else if(op=="csc"){
		a=1/sin(a);
	} else if(op=="cot"){
		a=1/tan(a);
	} else if(op=="sinh"){
		a=sinh(a);
	} else if(op=="cosh"){
		a=cosh(a);
	} else if(op=="tanh"){
		a=tanh(a);
	} else if(op=="sech"){
		a=1.0/cosh(a);
	} else if(op=="csch"){
		a=1.0/sinh(a);
	} else if(op=="coth"){
		a=cosh(a)/sinh(a);
	} else if(op=="arcsin"){
		a=asin(a);
	} else if(op=="arccos"){
		a=acos(a);
	} else if(op=="arctan"){
		a=acos(a);
	} else if(op=="arccot"){
		a=log(a+pow(a*a+1, 0.5));
	} else if(op=="arccoth"){
		a=0.5*(log(1+1/a)-log(1-1/a));
	} else if(op=="arccosh"){
		a=log(a+sqrt(a-1)*sqrt(a+1));
// 	} else if(op=="arccsc"){
// 	} else if(op=="arccsch"){
// 	} else if(op=="arcsec"){
// 	} else if(op=="arcsech"){
// 	} else if(op=="arcsinh"){
// 	} else if(op=="arctanh"){
	} else if(op=="exp"){
		a=exp(a);
	} else if(op=="ln"){
		a=log(a);
	} else if(op=="log"){
		a=log10(a);
	} else if(op=="abs"){
		a=a>=0. ? a : -a;
	//} else if(op=="conjugate"){
	//} else if(op=="arg"){
	//} else if(op=="real"){
	//} else if(op=="imaginary"){
	} else if(op=="floor"){
		a=floor(a);
	} else if(op=="ceiling" ){
		a=ceil(a);
	} else if(op=="min" ){
		a= a < b? a : b;
	} else if(op=="max" ){
		a= a > b? a : b;
	} else if(op=="gt" ){
		a= a > b? 1.0 : 0.0;
		boolean=true;
	} else if(op=="lt" ){
		a= a < b? 1.0 : 0.0;
		boolean=true;
	} else if(op=="eq" ){
		a= a == b? 1.0 : 0.0;
		boolean=true;
	} else if(op=="approx" ){
		a= abs(a-b)<0.001? 1.0 : 0.0;
		boolean=true;
	} else if(op=="neq" ){
		a= a != b? 1.0 : 0.0;
		boolean=true;
	} else if(op=="geq" ){
		a= a >= b? 1.0 : 0.0;
		boolean=true;
	} else if(op=="leq" ){
		a= a <= b? 1.0 : 0.0;
		boolean=true;
	} else if(op=="and" ){
		a= a && b? 1.0 : 0.0;
		boolean=true;
	} else if(op=="not"){
		a=!a;
		boolean = true;
	} else if(op=="or"){
		a= a || b? 1.0 : 0.0;
		boolean = true;
	} else if(op=="xor"){
		a= (a || b) && !(a&&b)? 1.0 : 0.0;
		boolean = true;
	} else if(op=="implies"){
		a= (a && !b)? 0.0 : 1.0;
		boolean = true;
	} else if(op=="gcd"){ //code by miquel canes :)
		int residu;
		while (b > 0) {
			residu = (int) a % (int) b;
			a = b;
			b = residu;
		}
	} else if(op=="lcm"){ //code by miquel canes :)
		int residu;
		double c=a*b;
		while (b > 0) {
			residu = (int) a % (int) b;
			a = b;
			b = residu;
		}
		a=(int)c/(int)a;
	} else if(op=="root") {
		a = (b==2.0)? sqrt(a) : pow(a, 1.0/b);
	}else {
		if(op!="") err += i18n("The operator <em>%1</em> hasn't been implemented<br/>").arg(op);
	}
	
	return a;
}

