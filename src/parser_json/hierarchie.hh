#pragma once

#include <string>
#include <vector>

/**
 * Enumeration for identified a type of one element
 *  - PLA = A place
 *  - HIE = A hierarchy
 */
enum EltType
{ HIE,PLA};

/**
 * \class Interface of an element
 * One element is identified by its type
 */
class ItfElement
{
  public:
    /**
     * Identifiant universel de la place, afin de maximiser le partage SDD
     */
    unsigned int level;
    /**
     * Forcer la destruction des classes héritéres
     */
    virtual ~ItfElement()
    {
    }
    /**
     * \return Retourne le type de la classe
     */
    virtual EltType getType() = 0;
    /**
     * Afficher un élément
     * \param cpt : Nombre d'espace à afficher
     */
    virtual void print(int cpt) = 0;
};

/**
 * \class Element can be a PName (Place type)
 */
class PName : public ItfElement
{
  public:
    /**
     * Name of the place
     */
    std::string place;
    /**
     * Destructor
     */
    virtual ~PName()
    {
    }
    /**
     * \return Return the type of the class
     */
    virtual EltType getType();
    /**
     * Print element
     * \param cpt : Number of space to print
     */
    virtual void print(int cpt);
};

/**
 * \class One element can be a hierarchy
 */
class Hierarchie : public ItfElement
{
  public:
    /**
     * Vector of elements, can be a
     * 	- Name place
     *  - New hierarchy
     */
  typedef std::vector<ItfElement *> elts_t;
  typedef elts_t::const_iterator elts_it;
  elts_t elts;

  elts_it begin() const { return elts.begin() ; }
  elts_it end() const { return elts.end() ; }
    /**
     * Counter of level
     */
    int nblevel;
    /**
     * Counter of elements in the vector
     */
    int nbplace;

    /**
     * Default constructor
     */
    Hierarchie();

    /**
     * Destructor :
     * Delete all element in the vector of element
     */
    virtual ~Hierarchie();

    /**
     * Add new Place Name
     * \param name Name of the place
     */
    void addPlace(const std::string& name);

    /**
     * Add a new hierarchy
     * \param h The reference of the new hierarchy
     */
    void addHierarchie(Hierarchie& h);

    /**
     * Print the current element
     * \param cpt : Number of space
     */
    virtual void print(int cpt);

    /**
     * \return Return the type of the class
     */
    virtual EltType getType();

};

