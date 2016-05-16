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
