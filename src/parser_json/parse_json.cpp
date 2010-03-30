
/**
 * \author HONG Silien
 * \version 1.0
 * \date 03/04/2009
 * \brief Module de définition du parseur d'un fichier JSON.
 * Le format JSON permet de rendre configurable l'état initial d'un SDD selon un fichier de configuration au format JSON
 * ou l'utilisateur va décrire l'ordre de la hiérarchie de chaque place.
 * Ici nous utilisons un parseur BOOST JSON de John W. Wilkinson (http://www.codeproject.com/KB/recipes/JSON_Spirit.aspx)
 */

#include "parse_json.hh"

#include <sstream>

namespace json {

void add_pair( json_spirit::Object& obj, const std::string& name, const json_spirit::Value& value )
{
    obj.push_back( json_spirit::Pair( name, value ) );
}


json_spirit::Object& add_child_obj( json_spirit::Object& parent, const std::string& name )
{
    parent.push_back( json_spirit::Pair( name, json_spirit::Object() ) );

    return parent.back().value_.get_obj();
}

json_spirit::Object& add_child_obj( json_spirit::Array& parent )
{
    parent.push_back( json_spirit::Object() );

    return parent.back().get_obj();
}

json_spirit::Array& add_array( json_spirit::Object& obj, const std::string& name )
{
    obj.push_back( json_spirit::Pair( name, json_spirit::Array() ) );

    return obj.back().value_.get_array();
}

json_spirit::Array& add_array( json_spirit::Array& parent, const std::string& name )
{
	json_spirit::Object& child =  add_child_obj(parent);
    return add_array(child,name);
}


void __writes(json_spirit::Array& addr_array,Hierarchie& addrs)
{
	/* Adding a new tab places */
    for( std::vector<ItfElement *>::iterator i = addrs.elts.begin()
    		; i != addrs.elts.end()
    		; ++i )
    {
    	if((*i)->getType() == PLA)
    	{
    		const PName& p = dynamic_cast<PName&> (**i);
    		json_spirit::Object& addr_obj( add_child_obj( addr_array ) );
	    	add_pair( addr_obj, "name", p.place );
    	}
    	else if ((*i)->getType() == HIE)
    	{
    		Hierarchie& hh = dynamic_cast<Hierarchie&> (**i);
    		json_spirit::Array& h2( add_array( addr_array, "Hierarchie" ) );
    		__writes(h2,hh);
    	}
    }
}


void writes( const char* file_name, Hierarchie& order )
{
	json_spirit::Object root_obj;

    json_spirit::Array& h1( add_array( root_obj, "ROOT" ) );
    __writes(h1,order);

    std::ofstream os( file_name );

    json_spirit::write_formatted( root_obj, os );

    os.close();
}

void __reads(json_spirit::Value& obj,Hierarchie& hh,int i)
{
	if(obj.type() == json_spirit::str_type)
	{
		//std::cout << "[" << i << "] " << obj.get_str() << std::endl;
		hh.addPlace(obj.get_str());
	}
	else if(obj.type() == json_spirit::array_type)
	{
		Hierarchie* h = new Hierarchie();
		hh.addHierarchie((*h));
		++i;
		for(json_spirit::Array::iterator iter = obj.get_array().begin()
				; iter != obj.get_array().end()
				; ++iter)
		{
			__reads(*iter,*h,i);
		}
	}
}

void reads(const std::string & file_name,Hierarchie& hh)
{
    std::ifstream is( file_name.c_str() );
    if(!is){
      std::stringstream tt;
         tt << "Error when opening JSON file configuration " << file_name << std::endl;
         throw JSException(json_file_unknow
           ,"parse_json.cpp"
           ,"N/A"
           ,"reads"
           ,tt.str());
    }
    json_spirit::Value value;
    read( is, value );
    json_spirit::Array root_obj( value.get_array());  // a 'value' read from a stream could be an JSON array or object
    for(json_spirit::Array::iterator iter = root_obj.begin()
    				; iter != root_obj.end()
    				; ++iter)
	{
		__reads(*iter,hh,0);
	}

}


/**
 * \param filename : Nom du fichier de configuration JSON à parser
 * \param h1 : Renvoie la configuration de la hiérarchie
 */
void json_parse(const std::string& filename,Hierarchie& h1)
{
    reads( filename , h1);
    //h1.inverted_identification();
    //std::cout << "Parsing JSON File " << filename << " :" << std::endl;
    //h1.print(0);
}


}
