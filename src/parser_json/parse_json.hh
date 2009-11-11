#pragma once
/**
 * \author HONG Silien
 * \version 1.0
 * \date 03/04/2009
 * \brief Module de définition du parseur d'un fichier JSON.
 * Le format JSON permet de rendre configurable l'état initial d'un SDD selon un fichier de configuration au format JSON
 * ou l'utilisateur va décrire l'ordre de la hiérarchie de chaque place.
 * Ici nous utilisons un parseur BOOST JSON de John W. Wilkinson (http://www.codeproject.com/KB/recipes/JSON_Spirit.aspx)
 */

// json spirit version 3.00
#include <cassert>
#include <algorithm>
#include <fstream>
#include <boost/bind.hpp>
#include "json_spirit/json_spirit.h"
#include "hierarchie.hh"
#include "JSException.hh"


/**
 * \param filename : Nom du fichier de configuration JSON à parser
 * \param h1 : Renvoie la configuration de la hiérarchie
 */
void json_parse(const std::string& filename,Hierarchie& h1);
