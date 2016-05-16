Variables::Variables(){
// 	afegeix("x", 33.0f);
	modifica("true", 1.);
	modifica("false", 0.);
	modifica("pi", 3.141592653);
	modifica("e", 2.718281828);
	modifica("euler", 0.5772156649);
}

inline bool Variables::add(QString id, QDomElement val){
	return modifica(id,val);
}

bool Variables::remove(QString id){
	bool ex=false;
	QValueList<struct VARIABLE>::iterator it;
	struct VARIABLE aux;
	
	for(it = vars.begin(); it != vars.end(); ++it){ 
		aux = *it;
		if(aux.nom == id){
			vars.remove(it);
			ex=true;
			break;
		}
	}
	return ex;
}

bool Variables::modifica(QString id, double new_val)
{
	QDomDocument a;
	QDomElement e=a.createElement("cn");
	e.appendChild(a.createTextNode(QString("%1").arg(new_val, 0, 'g', 16)));
	
	return modifica(id,e);
}

bool Variables::modifica(QString id, int new_val)
{
	QDomDocument a;
	QDomElement e=a.createElement("cn");
	e.appendChild(a.createTextNode(QString("%1").arg(new_val)));
	
	return modifica(id,e);
}

bool Variables::modifica(QString id, QDomElement new_val)
{
	bool ex=false;
		
	QValueList<struct VARIABLE>::iterator it;
	for(it = vars.begin(); !ex && it != vars.end(); ++it) {
		if((*it).nom == id){
			(*it).valor=new_val;
			ex=true;
		}
	}
	if(!ex){
		(*it).nom=id;
		(*it).valor=new_val;
		vars.prepend(*it);
	}
	return ex;
}

QDomElement* Variables::find(QString id)
{
	QDomElement* ret=NULL;
	bool ex=false;
		
	QValueList<struct VARIABLE>::iterator it;
	for(it = vars.begin(); !ex && it != vars.end(); ++it){ 
		if((*it).nom == id){
			ret=&(*it).valor;
			ex=true;
		}
	}
	return ret;
}

QDomElement Variables::value(QString id, bool *ex){
	QValueList<struct VARIABLE>::iterator it;
	
	for(it = vars.begin(); it != vars.end(); ++it){
		if((*it).nom.stripWhiteSpace() == id.stripWhiteSpace()){
			if(ex!=NULL) *ex=false;
			return (*it).valor;
		}
	}
	//error
	QDomDocument a; QString err("<cn \"constant\"> &NaN; </cn>");
	a.setContent(err);
	if(ex!=NULL) *ex=true;
	return a.documentElement();
}

QStringList Variables::getNoms(){
	struct VARIABLE aux;
	QStringList out;
	QValueList<struct VARIABLE>::iterator it;
	for(it = vars.begin(); it != vars.end(); ++it){ 
		aux = *it;
		out << aux.nom;
	}
	return out;
}

bool Variables::isvar(QDomElement e){
	if(e.tagName()=="ci"){
// 		struct VARIABLE aux;
		QStringList out;
		QValueList<struct VARIABLE>::iterator it;
		for(it = vars.begin(); it != vars.end(); ++it){ 
			if((*it).nom==e.text())
				return true;
		}
	}
	return false;
}

bool Variables::rename(QString id, QString newId){
	bool ex=false;
	struct VARIABLE aux;
	QValueList<struct VARIABLE>::iterator it;
	
	for(it = vars.begin(); !ex && it != vars.end(); ++it){ 
		aux = *it;
		if(aux.nom == id.stripWhiteSpace()){
			ex=true;
			(*it).nom=newId;
		}
	}
	return ex;
}

